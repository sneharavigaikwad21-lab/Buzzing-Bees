#ifndef RVV_1024_H
#define RVV_1024_H

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include "common.h"

// ============================================================================
// RVV 1.0 VECTOR TYPE (1024-bit)
// ============================================================================
typedef struct {
    double* data;
    size_t length;
} Vec1024_f64;

// ============================================================================
// VECTOR CREATION AND DESTRUCTION
// ============================================================================
Vec1024_f64* vec1024_create(size_t length);
void vec1024_free(Vec1024_f64* vec);

// ============================================================================
// VECTOR OPERATIONS
// ============================================================================
void vec1024_add(Vec1024_f64* dest, const Vec1024_f64* src1, const Vec1024_f64* src2);
void vec1024_mul(Vec1024_f64* dest, const Vec1024_f64* src1, const Vec1024_f64* src2);
void vec1024_dot(double* result, const Vec1024_f64* src1, const Vec1024_f64* src2);
void vec1024_softmax(Vec1024_f64* vec);
void vec1024_relu(Vec1024_f64* vec);
void vec1024_gelu(Vec1024_f64* vec);

// ============================================================================
// MATRIX OPERATIONS
// ============================================================================
void vec1024_matmul(double* C, const double* A, const double* B,
                    size_t M, size_t N, size_t K);

#endif // RVV_1024_H