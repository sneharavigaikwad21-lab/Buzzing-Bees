/* PROJECT AGNI LINKER SCRIPT */
/* Memory layout for RISC-V 64-bit AGNI chip */

MEMORY {
    /* FOREMAN_RAM: 0x0000_0000 - 0x00FF_FFFF (16MB) */
    /* Buzzing Bees AI agents run here */
    foreman_ram (rwx)  : ORIGIN = 0x00000000, LENGTH = 16M

    /* WRENCH_L2: 0x2000_0000 - 0x201F_FFFF (2MB) */
    /* Dedicated SRAM for Gemmini systolic array */
    wrench_l2 (rwx)    : ORIGIN = 0x20000000, LENGTH = 2M

    /* KEY_L2: 0x3000_0000 - 0x301F_FFFF (2MB) */
    /* Dedicated SRAM for RVV vector engine */
    key_l2 (rwx)       : ORIGIN = 0x30000000, LENGTH = 2M

    /* NOC_IF: 0x4000_0000 - 0x4000_FFFF (64KB) */
    /* Network-on-Chip interface (memory-mapped registers) */
    noc_if (rw)        : ORIGIN = 0x40000000, LENGTH = 64K

    /* GLOBAL_DRAM: 0x8000_0000 - 0x4_7FFF_FFFF (16GB) */
    /* Main system memory (GDDR6) */
    global_dram (rwx)  : ORIGIN = 0x80000000, LENGTH = 16G
}

SECTIONS {
    /* Code section (Foreman RAM) */
    .text : {
        *(.text.entry)
        *(.text*)
        *(.text.startup)
    } > foreman_ram

    /* Read-only data */
    .rodata : {
        *(.rodata*)
        *(.rodata.cst*)
    } > foreman_ram

    /* Initialized data */
    .data : {
        *(.data*)
        *(.data.rel*)
    } > foreman_ram

    /* Uninitialized data (BSS) */
    .bss : {
        *(.bss*)
        *(COMMON)
    } > foreman_ram

    /* Buzzing Bees AI models (loaded at runtime) */
    .bees_models (NOLOAD) : {
        /* 242MB total (Foreman Bee, Wrench Bee, Agni Bee, Game Bee) */
        __bees_start = .;
        . += 242M;
        __bees_end = .;
    } > foreman_ram

    /* Wrench-specific sections */
    .wrench_sram : {
        *(.wrench*)
    } > wrench_l2

    /* Key-specific sections */
    .key_sram : {
        *(.key*)
    } > key_l2

    /* Heap in DRAM */
    .heap (NOLOAD) : {
        __heap_start = .;
        . += 4G;
        __heap_end = .;
    } > global_dram

    /* Stack in DRAM */
    .stack (NOLOAD) : {
        __stack_start = .;
        . += 2M;
        __stack_end = .;
    } > global_dram
}

/* Export symbols */
__foreman_ram_start = ORIGIN(foreman_ram);
__foreman_ram_end = ORIGIN(foreman_ram) + LENGTH(foreman_ram);
__wrench_l2_start = ORIGIN(wrench_l2);
__wrench_l2_end = ORIGIN(wrench_l2) + LENGTH(wrench_l2);
__key_l2_start = ORIGIN(key_l2);
__key_l2_end = ORIGIN(key_l2) + LENGTH(key_l2);
__global_dram_start = ORIGIN(global_dram);
__global_dram_end = ORIGIN(global_dram) + LENGTH(global_dram);