#include <stdio.h>
#include <stdint.h>
#include "prime.h"

int main() {
    // User supplied number
    uint64_t num;
    printf("Enter a number to check if it is prime: ");
    
    /**
     * scanf() is a bad way to do this since we do not check if the provided
     * number is within the allowed bounds
     */
    scanf("%llu", &num);

    printf("Checking if %llu is prime...\n", num);

    if (is_prime(num)) {
        printf("%llu is prime", num);
    } else {
        printf("%llu is NOT prime", num);
    }

    return 0;
}