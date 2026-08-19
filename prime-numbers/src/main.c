#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "prime.h"


typedef enum {
    OPT_UNKNOWN,
    OPT_VERBOSE,
    OPT_HELP,
    OPT_NUMBER,
} OptionType;

OptionType parse_opts(const char *arg) {
    // if (strcmp(arg, "-v") == 0 || strcmp(arg, "--verbose") == 0) return OPT_VERBOSE;
    if (strcmp(arg, "-n") == 0 || strcmp(arg, "--number") == 0) return OPT_NUMBER;
    // if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) return OPT_HELP;
    return OPT_UNKNOWN;
}


int main(int argc, char *argv[]) {
    // User supplied number
    uint64_t num = 0;

    // Parse the command line args
    for (int opt = 1; opt < argc && argv[opt][0] == '-'; opt++) {
        // Ignore positional arguments
        if (argv[opt][0] != '-') {
            continue;
        }
        switch (parse_opts(argv[opt])) {
            case OPT_NUMBER: 
                if (opt + 1 < argc) {
                    char *endptr;
                    num = strtoll(argv[opt + 1], &endptr, 10);
                } else {
                    fprintf(stderr, "Error: -n requires an argument\n");
                    exit(1);
                }
                break;
            default:
                printf("Enter a number to check if it is prime: ");
                /**
                 * scanf() is a bad way to do this since we do not check if the provided
                 * number is within the allowed bounds
                 */
                scanf("%llu", &num);
                break;
        }
    }

    if (num < 0) {
        fprintf(stderr, "The given number was outside of acceptable bound (a valid uint64_t)");
        exit(1);
    }

    if (num == 0) {
        printf("Enter a number to check if it is prime: ");
        /**
         * scanf() is a bad way to do this since we do not check if the provided
         * number is within the allowed bounds
         */
        scanf("%llu", &num);
    }

    printf("Checking if %llu is prime...\n", num);

    if (is_prime(num)) {
        printf("%llu is prime", num);
    } else {
        printf("%llu is NOT prime", num);
    }

    return 0;
}