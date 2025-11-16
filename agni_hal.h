#ifndef AGNI_HAL_H
#define AGNI_HAL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// SECTION 1: MEMORY MAP (TRM v3.0, Sec 6.0) - HARD-LOCKED
////////////////////////////////////////////////////////////////////////////////

#define FOREMAN_RAM_BASE       0x00000000UL  // 0x0000_0000 - 0x00FF_FFFF (16MB)
#define WRENCH_L2_BASE         0x20000000UL  // 0x2000_0000 - 0x201F_FFFF (2MB)
#define KEY_L2_BASE            0x30000000UL  // 0x3000_0000 - 0x301F_FFFF (2MB)
#define NOC_BASE               0x40000000UL  // 0x4000_0000 - 0x4000_FFFF (64KB)
#define GLOBAL_DRAM_BASE       0x80000000UL  // 0x8000_0000 - 0x4_7FFF_FFFF (16GB)

#define FOREMAN_RAM_SIZE       0x01000000UL  // 16MB
#define WRENCH_L2_SIZE         0x00200000UL  // 2MB
#define KEY_L2_SIZE            0x00200000UL  // 2MB
#define GLOBAL_DRAM_SIZE       0x400000000UL // 16GB

////////////////////////////////////////////////////////////////////////////////
// SECTION 2: NOC (lotus_noc) REGISTERS - TRM v3.0 Sec 5.0
////////////////////////////////////////////////////////////////////////////////

#define NOC_SEND_CMD           (NOC_BASE + 0x0000UL)  // (W) NocCopyDescriptor*
#define NOC_STATUS             (NOC_BASE + 0x0008UL)  // (R) 0x1=BUSY, 0x0=DONE
#define NOC_IRQ_ENABLE         (NOC_BASE + 0x000CUL)  // (W/R) IRQ enable
#define NOC_IRQ_STATUS         (NOC_BASE + 0x0010UL)  // (R) IRQ pending

typedef struct {
    uint64_t source_addr;
    uint64_t dest_addr;
    uint32_t size_bytes;
    uint32_t _reserved;
} NocCopyDescriptor;

////////////////////////////////////////////////////////////////////////////////
// SECTION 3: WRENCH (Gemmini) RoCC OPCODES - TRM v3.0 Sec 3.0
// funct7 values for custom RISC-V RoCC instructions
////////////////////////////////////////////////////////////////////////////////

#define WRENCH_CONFIG_OPCODE    0b0001011  // agni.wrench.config rs1, rs2
#define WRENCH_LOAD_A_OPCODE    0b0001111  // agni.wrench.load_a rs1
#define WRENCH_LOAD_B_OPCODE    0b0101111  // agni.wrench.load_b rs1
#define WRENCH_EXECUTE_OPCODE   0b1101111  // agni.wrench.execute rs1

// RoCC instruction format (RISC-V custom instruction)
// Encoding: funct7[31:25] rs2[24:20] rs1[19:15] funct3[14:12] rd[11:7] opcode[6:0]
// opcode for RoCC = 0b0001011

#define ENCODE_ROCC_INSTR(funct7, rd, rs1, rs2) \
    (((funct7) << 25) | ((rs2) << 20) | ((rs1) << 15) | ((rd) << 7) | 0b0001011)

////////////////////////////////////////////////////////////////////////////////
// SECTION 4: KEY (RVV 1.0) OPERATIONS
// RISC-V Vector Extension 1.0 - Ratified November 2021
// 1024-bit vector width (within RVV 1.0 spec: 32-2048 bits)
////////////////////////////////////////////////////////////////////////////////

// RVV 1.0 Vector Configuration Register (vtype)
// VTYPE = (vediv << 3) | (vsew << 0), where:
// - vsew = 0 (8-bit), 1 (16-bit), 2 (32-bit), 3 (64-bit)
// - vediv = 0 (no reduction), 1 (divide by 2), 2 (divide by 4), 3 (divide by 8)

#define RVV_VTYPE_FP64          (3 << 0)  // vsew = 3 (64-bit floating point)
#define RVV_VTYPE_LMUL_1        (0 << 5)  // LMUL = 1
#define RVV_VTYPE_LMUL_2        (1 << 5)  // LMUL = 2
#define RVV_VTYPE_LMUL_4        (2 << 5)  // LMUL = 4
#define RVV_VTYPE_LMUL_8        (3 << 5)  // LMUL = 8

// RVV CSR (Control and Status Register) addresses
#define CSR_VTYPE               0x008     // Vector data type register
#define CSR_VL                  0x009     // Vector length register
#define CSR_VLENB               0x008F    // Vector length in bytes

////////////////////////////////////////////////////////////////////////////////
// SECTION 5: FOREMAN (CVA6S+) CPU CONTROL
// Dual-Issue, RV64GC, 1.2GHz (unique identifier)
////////////////////////////////////////////////////////////////////////////////

#define FOREMAN_FREQ_HZ         1200000000UL  // 1.2 GHz (AGNI unique marker)
#define FOREMAN_CPU_ID          0x00000042UL  // "CVA6S+" CPU signature

////////////////////////////////////////////////////////////////////////////////
// SECTION 6: NOC DMA OPERATIONS (Async, Non-Blocking)
// V6 (Dual-Issue) Software DMA Fix - TRM v3.0 Sec 5.0
////////////////////////////////////////////////////////////////////////////////

// Async NOC copy (fire-and-forget) - Thread 1 of dual-issue pair
static inline void agni_hal_noc_copy_async(
    uint64_t src_addr,
    uint64_t dst_addr,
    uint32_t size_bytes
) {
    volatile NocCopyDescriptor* desc =
        (volatile NocCopyDescriptor*)(FOREMAN_RAM_BASE);

    desc->source_addr = src_addr;
    desc->dest_addr = dst_addr;
    desc->size_bytes = size_bytes;
    desc->_reserved = 0;

    // Write to NOC_SEND_CMD (non-blocking, fire-and-forget)
    volatile uint64_t* noc_cmd = (volatile uint64_t*)NOC_SEND_CMD;
    *noc_cmd = (uint64_t)(uintptr_t)desc;
    // V6 Fix: CPU continues immediately (Thread 2 of dual-issue)
}

// Poll NOC transfer status (non-blocking)
static inline bool agni_hal_noc_is_busy(void) {
    volatile uint32_t* status = (volatile uint32_t*)NOC_STATUS;
    return (*status == 0x1);
}

// Wait for NOC completion (blocking)
static inline int agni_hal_noc_wait(uint32_t timeout_ms) {
    uint32_t elapsed = 0;
    while (agni_hal_noc_is_busy() && elapsed < timeout_ms) {
        elapsed++;
        // Sleep 1ms
        for (volatile int i = 0; i < 1000000; i++);
    }
    return agni_hal_noc_is_busy() ? -1 : 0;
}

// Enable NOC IRQ (when transfer completes)
static inline void agni_hal_noc_irq_enable(void) {
    volatile uint32_t* irq_en = (volatile uint32_t*)NOC_IRQ_ENABLE;
    *irq_en = 0x1;
}

// Disable NOC IRQ
static inline void agni_hal_noc_irq_disable(void) {
    volatile uint32_t* irq_en = (volatile uint32_t*)NOC_IRQ_ENABLE;
    *irq_en = 0x0;
}

// Check NOC IRQ pending status
static inline bool agni_hal_noc_irq_pending(void) {
    volatile uint32_t* irq_stat = (volatile uint32_t*)NOC_IRQ_STATUS;
    return (*irq_stat != 0x0);
}

////////////////////////////////////////////////////////////////////////////////
// SECTION 7: WRENCH (Gemmini) OPERATIONS
// Systolic array control via RoCC interface
////////////////////////////////////////////////////////////////////////////////

// Configure Wrench for matrix multiply (MxM)
#define agni_wrench_config(rows, cols) \
    do { \
        register uint64_t _rows asm("a0") = (rows); \
        register uint64_t _cols asm("a1") = (cols); \
        asm volatile ( \
            ".word %0" \
            : \
            : "i" (ENCODE_ROCC_INSTR(WRENCH_CONFIG_OPCODE, 0, 10, 11)) \
        ); \
    } while(0)

// Load matrix A into Wrench L2 (address in rs1)
#define agni_wrench_load_a(src_addr) \
    do { \
        register uint64_t _addr asm("a0") = (src_addr); \
        asm volatile ( \
            ".word %0" \
            : \
            : "i" (ENCODE_ROCC_INSTR(WRENCH_LOAD_A_OPCODE, 0, 10, 0)) \
        ); \
    } while(0)

// Load matrix B into Wrench L2
#define agni_wrench_load_b(src_addr) \
    do { \
        register uint64_t _addr asm("a0") = (src_addr); \
        asm volatile ( \
            ".word %0" \
            : \
            : "i" (ENCODE_ROCC_INSTR(WRENCH_LOAD_B_OPCODE, 0, 10, 0)) \
        ); \
    } while(0)

// Execute Wrench MAC (output address in rs1)
#define agni_wrench_execute(output_addr) \
    do { \
        register uint64_t _out asm("a0") = (output_addr); \
        asm volatile ( \
            ".word %0" \
            : \
            : "i" (ENCODE_ROCC_INSTR(WRENCH_EXECUTE_OPCODE, 0, 10, 0)) \
        ); \
    } while(0)

////////////////////////////////////////////////////////////////////////////////
// SECTION 8: MEMORY UTILITIES
////////////////////////////////////////////////////////////////////////////////

static inline void agni_hal_memcpy_l2(
    uint8_t* dst,
    const uint8_t* src,
    size_t size
) {
    memcpy(dst, src, size);
}

static inline void agni_hal_memset_l2(
    uint8_t* dst,
    int value,
    size_t size
) {
    memset(dst, value, size);
}

// Flush cache line (write-through guarantee)
static inline void agni_hal_flush_cache_line(uint64_t addr) {
    asm volatile("fence rw, rw");  // Memory fence
}

////////////////////////////////////////////////////////////////////////////////
// SECTION 9: BUZZING BEES CONTEXT & ENTRY POINTS
// Placeholder APIs that Layer 1 (Bees) will call
////////////////////////////////////////////////////////////////////////////////

typedef struct {
    uint32_t job_id;
    uint64_t foreman_ram_addr;
    uint32_t foreman_ram_size;
    void (*noc_done_callback)(void);
} BeeContext;

// Initialize Bee execution context
static inline void agni_hal_bee_init(BeeContext* ctx) {
    if (!ctx) return;
    ctx->foreman_ram_addr = FOREMAN_RAM_BASE;
    ctx->foreman_ram_size = FOREMAN_RAM_SIZE;
    ctx->noc_done_callback = NULL;
}

// Submit Wrench job (matrix multiply: C = A * B)
static inline void agni_hal_bee_submit_wrench(
    BeeContext* ctx,
    uint64_t a_addr,
    uint64_t b_addr,
    uint64_t c_addr
) {
    if (!ctx) return;
    agni_wrench_config(8, 8);          // 8x8 systolic
    agni_wrench_load_a(a_addr);
    agni_wrench_load_b(b_addr);
    agni_wrench_execute(c_addr);
}

// Submit Key job (vector operations)
static inline void agni_hal_bee_submit_key(
    BeeContext* ctx,
    uint64_t input_addr,
    uint64_t output_addr
) {
    if (!ctx) return;
    // Key jobs use RVV 1.0 instructions (compiled by MLIR)
}

// Wait for Bee job (blocking)
static inline int agni_hal_bee_wait(
    BeeContext* ctx,
    uint32_t timeout_ms
) {
    return agni_hal_noc_wait(timeout_ms);
}

// Register NOC done callback (called by interrupt handler)
static inline void agni_hal_bee_register_done_callback(
    BeeContext* ctx,
    void (*callback)(void)
) {
    if (ctx) ctx->noc_done_callback = callback;
}

#endif // AGNI_HAL_H