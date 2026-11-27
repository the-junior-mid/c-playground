#include <stdlib.h>
#include <stdint.h>
#include "bint.h"
#include "dynamic_array.h"
#include "log.h"


void bint_print(bint *a) {
    for (size_t i = 0; i < a->len; i++) {
        if (i == 0) {
            printf("%lu", a->items[a->len - i - 1]);
        } else {
            printf("%018lu", a->items[a->len - i - 1]);
        }
    }
    printf("\n");
}

/// @brief Copy contents of @p b into @p a.
/// @param a 
/// @param b 
/// @return 
int bint_copy(bint *a, bint b) {
    a->len = 0;
    da_foreach(&b, i, uint64_t) {
        da_push(a, *i);
    }
}

int bint_add(bint *a, bint b) {
    size_t len = b.len <= a->len ? b.len : a->len;
    uint64_t carry = 0;
    size_t i;
    // Add up to matching length
    for (i = 0; i < len; i++) {
        uint64_t c = a->items[i] + b.items[i] + carry;
        carry = c / BINT_BASE;
        a->items[i] = c % BINT_BASE;
    }

    // Add remaining length
    if (a->len < b.len) {
        do {
            uint64_t d = b.items[i++] + carry;
            carry = d / BINT_BASE;
            da_push(a, d % BINT_BASE);
        } while (carry > 0);
    } else if (b.len < a->len) {
        do {
            uint64_t d = a->items[i++] + carry;
            carry = d / BINT_BASE;
            da_push(a, d % BINT_BASE);
        } while (carry > 0);
    }

    // Add carry
    if (carry > 0) {
        da_push(a, carry);
    }
}

int bint_sub(bint *a, bint b) {

}

int bint_mult(bint *a, bint b) {

}

int bint_div(bint *a, bint b) {

}