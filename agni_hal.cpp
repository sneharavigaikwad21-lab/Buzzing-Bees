#include "agni_hal.h"
#include <stdio.h>

// Global NOC interrupt handler
static void (*g_noc_irq_handler)(void) = NULL;

// Register NOC IRQ handler
void agni_hal_register_noc_irq_handler(void (*handler)(void)) {
    g_noc_irq_handler = handler;
}

// Invoke NOC IRQ handler (called by ISR)
void agni_hal_invoke_noc_irq(void) {
    if (g_noc_irq_handler) {
        g_noc_irq_handler();
    }
}

// Initialize AGNI hardware
void agni_hal_init(void) {
    // Initialize Foreman CPU
    // Initialize Wrench systolic array
    // Initialize Key RVV engines
    // Enable NOC
    agni_hal_noc_irq_enable();
    printf("[HAL] AGNI hardware initialized\n");
}

// Shutdown AGNI hardware
void agni_hal_shutdown(void) {
    agni_hal_noc_irq_disable();
    printf("[HAL] AGNI hardware shutdown\n");
}