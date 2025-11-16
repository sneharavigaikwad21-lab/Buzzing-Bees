#ifndef AGNI_COMMON_H
#define AGNI_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

// ============================================================================
// ERROR CODES
// ============================================================================
#define AGNI_OK                    0
#define AGNI_ERROR_NULL_POINTER   -1
#define AGNI_ERROR_INVALID_INPUT  -2
#define AGNI_ERROR_ALLOCATION     -3
#define AGNI_ERROR_TIMEOUT        -4
#define AGNI_ERROR_RUNTIME        -5

// ============================================================================
// LOGGING MACROS
// ============================================================================
#define LOG_INFO(fmt, ...) \
    do { printf("[INFO] " fmt "\n", ##__VA_ARGS__); fflush(stdout); } while(0)

#define LOG_WARN(fmt, ...) \
    do { printf("[WARN] " fmt "\n", ##__VA_ARGS__); fflush(stdout); } while(0)

#define LOG_ERROR(fmt, ...) \
    do { fprintf(stderr, "[ERROR] " fmt "\n", ##__VA_ARGS__); fflush(stderr); } while(0)

#define LOG_DEBUG(fmt, ...) \
    do { printf("[DEBUG] " fmt "\n", ##__VA_ARGS__); fflush(stdout); } while(0)

// ============================================================================
// MEMORY UTILITIES
// ============================================================================
#define SAFE_FREE(ptr) \
    do { if (ptr) { free(ptr); ptr = NULL; } } while(0)

#define SAFE_DELETE(ptr) \
    do { if (ptr) { delete (ptr); ptr = NULL; } } while(0)

// ============================================================================
// MATH UTILITIES
// ============================================================================
#define MIN(a, b)       ((a) < (b) ? (a) : (b))
#define MAX(a, b)       ((a) > (b) ? (a) : (b))
#define CLAMP(x, lo, hi) MAX((lo), MIN((x), (hi)))
#define ABS(x)          ((x) < 0 ? -(x) : (x))

// ============================================================================
// PERFORMANCE TARGET CONSTANTS
// ============================================================================
#define INPUT_BUFFER_SIZE      (1024 * 1024)   // 1MB
#define OUTPUT_BUFFER_SIZE     (4 * 1024 * 1024) // 4MB

// ============================================================================
// TARGET PERFORMANCE (AGNI Hardware Targets)
// ============================================================================

// ============================================================================
// TIMING UTILITIES
// ============================================================================
typedef struct {
    clock_t start_time;
    clock_t end_time;
} Timer;

static inline void timer_start(Timer* t) {
    if (t) t->start_time = clock();
}

static inline double timer_elapsed_ms(Timer* t) {
    if (!t) return 0.0;
    t->end_time = clock();
    return ((double)(t->end_time - t->start_time) / CLOCKS_PER_SEC) * 1000.0;
}

#endif // AGNI_COMMON_H