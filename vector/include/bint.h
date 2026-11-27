#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "vector.h"

#ifndef MID_BINT_H
#define MID_BINT_H


#define BINT_BASE ((__uint128_t)1 << 64)

/// @brief Store the arbitrarily large integer in an array. Each element is a uint64_t.
typedef struct {
    uint64_t *items;
    size_t capacity;
    size_t len;
} bint;

void bint_print(const bint *a);

void bint_copy(bint *a, bint b);

int bint_add(bint *a, bint b);

int bint_sub(bint *a, bint b);

int bint_mult(bint *a, bint b);

int bint_div(bint *a, bint b);

bool bint_eq(bint *a, bint b);

#endif