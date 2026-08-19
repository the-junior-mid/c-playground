#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "prime.h"


typedef enum {
    OPT_UNKNOWN,
    OPT_VERBOSE,
    OPT_HELP,
    OPT_NUMBER,
} OptionType;

OptionType parse_opts(const char *arg) {
    // if (strcmp(arg, "-v") == 0 || strcmp(arg, "--verbose") == 0) return OPT_VERBOSE;
    // if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) return OPT_HELP;
    if (strcmp(arg, "-n") == 0 || strcmp(arg, "--number") == 0) return OPT_NUMBER;
    return OPT_UNKNOWN;
}

int read_input(uint64_t *num) {
    printf("Enter a number to check if it is prime: ");
    /**
     * scanf() is a bad way to do this since we do not check if the provided
     * number is within the allowed bounds
     */
    char *endptr_2;
    // Create a character buffer
    char input[1024];
    // Leave room for trailing null terminator in buffer
    if (scanf("%1023s", input) != 1) {
        fprintf(stderr, "Error: failed to read input\n");
        return 1;
    }
    uint64_t val = strtoull(input, &endptr_2, 10);
    // Check the numbner is valid
    if (*endptr_2 != '\0' || endptr_2 == input) {
        fprintf(stderr, "Error: invalid number '%s'\n", input);
        return 1;
    }

    *num = val;

    return 0;
}


int main(int argc, char *argv[]) {
    // User supplied number
    uint64_t num;
    bool init = false;

    // Parse the command line args
    for (int opt = 1; opt < argc; opt++) {
        // Ignore positional arguments
        if (argv[opt][0] != '-') {
            continue;
        }
        switch (parse_opts(argv[opt])) {
            case OPT_NUMBER: 
                if (opt + 1 < argc) {
                    char *endptr_1;
                    num = strtoull(argv[opt + 1], &endptr_1, 10);
                    // Check the numbner is valid
                    if (*endptr_1 != '\0' || endptr_1 == argv[opt + 1]) {
                        fprintf(stderr, "Error: invalid number '%s'\n", argv[opt + 1]);
                        return 1;
                    }
                    init = true;
                    opt++;
                } else {
                    fprintf(stderr, "Error: -n requires an argument\n");
                    return 1;
                }
                break;
            case OPT_UNKNOWN:
                fprintf(stderr, "Error: unknown argument %s\n", argv[opt]);
                return 1;
                break;
            default:
                break;
        }
    }

    if (!init) {
        if (read_input(&num) != 0) {
            return 1;
        }
    }

    // Can assume init succeeded at this point
    printf("Checking if %llu is prime...\n", num);

    if (is_prime(num)) {
        printf("%llu is prime\n", num);
    } else {
        printf("%llu is NOT prime\n", num);
    }

    return 0;
}