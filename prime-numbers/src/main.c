#include <stdio.h>
#include "prime.h"

main() {
    // User supplied number
    int num;
    printf("Enter a number to check if it is prime: ");
    scanf("%d", &num);

    printf("Checking if %d is prime...\n", &num);

    if (is_prime(num)) {
        printf("%d appears to be prime");
    } else {
        printf("%d appears to NOT be prime");
    }

    return 0;
}