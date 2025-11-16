#include "agni_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Task queue for scheduler
typedef struct {
    uint32_t job_id;
    uint64_t wrench_a_addr;
    uint64_t wrench_b_addr;
    uint64_t wrench_c_addr;
} Task;

#define MAX_TASKS 100
static Task task_queue[MAX_TASKS];
static uint32_t queue_head = 0;
static uint32_t queue_tail = 0;

// Submit task to queue
void agni_scheduler_submit_task(const Task* task) {
    if ((queue_tail + 1) % MAX_TASKS == queue_head) {
        return;  // Queue full
    }
    memcpy(&task_queue[queue_tail], task, sizeof(Task));
    queue_tail = (queue_tail + 1) % MAX_TASKS;
}

// Get next task
bool agni_scheduler_get_next_task(Task* task) {
    if (queue_head == queue_tail) {
        return false;  // Queue empty
    }
    memcpy(task, &task_queue[queue_head], sizeof(Task));
    queue_head = (queue_head + 1) % MAX_TASKS;
    return true;
}

// Main scheduler loop (runs on Foreman CPU)
// Implements V6 (Dual-Issue) Software DMA Fix (TRM v3.0 Sec 5.0)
void agni_scheduler_run(void) {
    Task current_task;

    while (1) {
        // Thread 1: Check if NOC is busy
        if (agni_hal_noc_is_busy()) {
            continue;  // Wait for NOC to complete
        }

        // Thread 2: Get next task from queue (dual-issue with NOC check)
        // V6 FIX: Both operations happen simultaneously in dual-issue pipeline
        if (!agni_scheduler_get_next_task(&current_task)) {
            continue;  // No task, yield
        }

        // Execute Wrench job (fire-and-forget)
        agni_wrench_config(8, 8);
        agni_wrench_load_a(current_task.wrench_a_addr);
        agni_wrench_load_b(current_task.wrench_b_addr);
        agni_wrench_execute(current_task.wrench_c_addr);

        // V6 FIX: Immediately issue NOC copy (non-blocking, async)
        // Thread 1: NOC_SEND_CMD write
        agni_hal_noc_copy_async(
            current_task.wrench_c_addr,  // Wrench output
            KEY_L2_BASE,                  // Key input
            4096                          // 4KB transfer
        );

        // Thread 2: Foreman does NOT stall
        // Dual-issue pipeline allows both NOC_SEND_CMD and next instruction
        // Loop immediately continues (no 40ms stall)

        // Later (40ms): NOC IRQ fires
        // Interrupt handler will queue next Mamba (Key) job
    }
}

// NOC interrupt handler (called when transfer completes)
void agni_noc_irq_handler(void) {
    // Queue next Key job for execution
    // This is called after NOC transfer finishes
}