#ifndef AGNI_VECTOR_UTILS_H
#define AGNI_VECTOR_UTILS_H

#include <stddef.h>
#include <stdint.h>
#include "common.h"

// ============================================================================
// SIMD VECTOR ADDITION
// ============================================================================
void simd_add_f64(double* dst, const double* src1, const double* src2, size_t len);

// ============================================================================
// SIMD VECTOR MULTIPLICATION
// ============================================================================
void simd_mul_f64(double* dst, const double* src1, const double* src2, size_t len);

// ============================================================================
// SIMD DOT PRODUCT
// ============================================================================
double simd_dot_f64(const double* src1, const double* src2, size_t len);

// ============================================================================
// SIMD SOFTMAX (Numerically stable)
// ============================================================================
void simd_softmax_f64(double* vec, size_t len);

// ============================================================================
// SIMD ACTIVATION FUNCTIONS
// ============================================================================
void simd_relu_f64(double* dst, const double* src, size_t len);
void simd_gelu_f64(double* dst, const double* src, size_t len);
void simd_tanh_f64(double* dst, const double* src, size_t len);

#endif // AGNI_VECTOR_UTILS_H