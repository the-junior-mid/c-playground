#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include "bint.h"
#include "dynamic_array.h"
#include "log.h"


void bint_print(const bint *a) {
    size_t n = a->len;
    while (n > 0 && a->items[n - 1] == 0) n--;   // ignore leading zero limbs
    if (n == 0) { puts("0"); return; }

    const uint64_t BASE = 10000000000000000000ULL;   // 10^19, largest power of 10 in a uint64_t

    uint64_t *tmp    = malloc(n * sizeof *tmp);        // work on a copy, the division destroys it
    uint64_t *chunks = malloc(2 * n * sizeof *chunks); // a limb holds 64 bits, a chunk ~63.1, so 2n is plenty
    if (!tmp || !chunks) { free(tmp); free(chunks); return; }
    memcpy(tmp, a->items, n * sizeof *tmp);

    size_t nchunks = 0;
    while (n > 0) {
        uint64_t rem = 0;
        for (size_t i = n; i-- > 0; ) {              // top limb down
            __uint128_t cur = ((__uint128_t)rem << 64) | tmp[i];
            tmp[i] = (uint64_t)(cur / BASE);
            rem    = (uint64_t)(cur % BASE);
        }
        chunks[nchunks++] = rem;
        while (n > 0 && tmp[n - 1] == 0) n--;        // shrink as the number gets smaller
    }

    printf("%" PRIu64, chunks[nchunks - 1]);          // most significant chunk: no padding
    for (size_t i = nchunks - 1; i-- > 0; )
        printf("%019" PRIu64, chunks[i]);              // the rest: zero-padded to 19 digits
    putchar('\n');

    free(tmp);
    free(chunks);
}

/// @brief Copy contents of @p b into @p a.
/// @param a 
/// @param b 
/// @return 
void bint_copy(bint *a, bint b) {
    a->len = 0;
    da_foreach(&b, i, uint64_t) {
        da_push(a, *i);
    }
}

int bint_add(bint *a, bint b) {
    __uint128_t c;
    size_t min_len = b.len <= a->len ? b.len : a->len;
    uint64_t carry = 0;
    // Add up to matching length
    for (size_t i = 0; i < min_len; i++) {
        c = (__uint128_t)a->items[i] + b.items[i] + carry;
        carry = (uint64_t)(c / BINT_BASE);
        a->items[i] = (uint64_t)(c % BINT_BASE);
    }

    // Add remaining length
    if (b.len > min_len) {
        for (size_t i = min_len; i < b.len; i++) {
            c = (__uint128_t)b.items[i] + carry;
            carry = (uint64_t)(c / BINT_BASE);
            da_push(a, (uint64_t)(c % BINT_BASE));
        }
    } else{
        for (size_t i = min_len; i < a->len && carry; i++) {
            c = (__uint128_t)a->items[i] + carry;
            carry = (uint64_t)(c / BINT_BASE);
            a->items[i] = (uint64_t)c;
        }
    }

    // Add carry
    if (carry) {
        da_push(a, carry);
    }
}

int bint_sub(bint *a, bint b) {

}

int bint_mult(bint *a, bint b) {

}

int bint_div(bint *a, bint b) {

}

bool bint_eq(bint *a, bint b) {
    if (a->len != b.len) return false;
    for (size_t i = 0; i < a->len; i++) {
        if (a->items[i] != b.items[i]) return false;
    }

    return true;
}