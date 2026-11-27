#include <stdio.h>
#include "bint.h"
#include "dynamic_array.h"

/*
    Generate Fibonnaci numbers up to specified amount.
*/
int main(void) {
    bint a = {0};
    bint b = {0};
    bint c = {0};

    da_push(&a, 0);
    da_push(&b, 1);

    for (size_t i = 0; i <  1000; i++) {
        bint_print(&a);

        // c = a + b
        bint_copy(&c, a);
        bint_add(&c, b);

        // a = b
        bint_copy(&a, b);

        // b = c
        bint_copy(&b, c);
    }
}