#include "vector_utils.h"
#include <math.h>
#include <string.h>

// ============================================================================
// SIMD VECTOR ADDITION
// ============================================================================
void simd_add_f64(double* dst, const double* src1, const double* src2, size_t len) {
    if (!dst || !src1 || !src2) return;

    for (size_t i = 0; i < len; i++) {
        dst[i] = src1[i] + src2[i];
    }
}

// ============================================================================
// SIMD VECTOR MULTIPLICATION
// ============================================================================
void simd_mul_f64(double* dst, const double* src1, const double* src2, size_t len) {
    if (!dst || !src1 || !src2) return;

    for (size_t i = 0; i < len; i++) {
        dst[i] = src1[i] * src2[i];
    }
}

// ============================================================================
// SIMD DOT PRODUCT
// ============================================================================
double simd_dot_f64(const double* src1, const double* src2, size_t len) {
    if (!src1 || !src2) return 0.0;

    double result = 0.0;
    for (size_t i = 0; i < len; i++) {
        result += src1[i] * src2[i];
    }
    return result;
}

// ============================================================================
// SIMD SOFTMAX (Numerically stable via log-sum-exp trick)
// ============================================================================
void simd_softmax_f64(double* vec, size_t len) {
    if (!vec || len == 0) return;

    // BUG FIX: Check for length > 0 before accessing vec[0]
    // Step 1: Find maximum for numerical stability
    double max_val = vec[0];
    for (size_t i = 1; i < len; i++) {
        if (vec[i] > max_val) {
            max_val = vec[i];
        }
    }

    // Step 2: Compute exp(x - max) and sum
    double sum = 0.0;
    for (size_t i = 0; i < len; i++) {
        vec[i] = exp(vec[i] - max_val);
        sum += vec[i];
    }

    // Step 3: Normalize
    // BUG FIX: Check for division by zero
    if (sum > 1e-10) {
        for (size_t i = 0; i < len; i++) {
            vec[i] /= sum;
        }
    }
}

// ============================================================================
// SIMD ReLU ACTIVATION
// ============================================================================
void simd_relu_f64(double* dst, const double* src, size_t len) {
    if (!dst || !src) return;

    for (size_t i = 0; i < len; i++) {
        dst[i] = (src[i] > 0.0) ? src[i] : 0.0;
    }
}

// ============================================================================
// SIMD GELU ACTIVATION (Approximate)
// ============================================================================
void simd_gelu_f64(double* dst, const double* src, size_t len) {
    if (!dst || !src) return;

    // Approximation: GELU(x) â‰ˆ 0.5 * x * (1 + tanh(sqrt(2/Ï€) * (x + 0.044715 * x^3)))
    const double sqrt_2_pi = 0.7978845608028654;  // sqrt(2/Ï€)
    const double coeff = 0.044715;

    for (size_t i = 0; i < len; i++) {
        double x = src[i];
        double x3 = x * x * x;
        double tanh_arg = sqrt_2_pi * (x + coeff * x3);
        double tanh_val = tanh(tanh_arg);
        dst[i] = 0.5 * x * (1.0 + tanh_val);
    }
}

// ============================================================================
// SIMD TANH ACTIVATION
// ============================================================================
void simd_tanh_f64(double* dst, const double* src, size_t len) {
    if (!dst || !src) return;

    for (size_t i = 0; i < len; i++) {
        dst[i] = tanh(src[i]);
    }
}