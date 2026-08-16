#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "prime.h"

int is_prime(uint64_t num) {
    /**
     * Search limit is sqrt(num). To see why, consider num = 10,
     * then we can have 1 * 10, 2 * 5, 5 * 2, and 10 * 1, notice we have 
     * repeating factors, and to avoid testing these repeated factors we can 
     * ignore any past sqrt(num). So in the case of 10, we can ingore anything 
     * over sqrt(10) ~= 3.16
     */
    float limit = sqrt(num);
    printf("The search limit will be: %.3f\n", limit);

    uint64_t i;
    for (i = 2; i <= limit; i++) {
        if (num % i == 0) {
            printf("Prime factor found: %llu\n", i);
            return 0;
        }
    }

    return 1;
}