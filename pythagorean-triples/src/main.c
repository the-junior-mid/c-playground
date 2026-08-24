/**
 * 
 * Pythagorean Triples Checker
        Allows the user to input the sides of any triangle in any order.
        Return whether the triangle is a Pythagorean Triple or not.
        Loop the program so the user can use it more than once without having to restart the program.

 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>
#include <string.h>


char *trim(char *str) {
    // Remove leading whitespace
    while (isspace((unsigned char)*str)) str++;

    // If we reached the end of the string then it was all whitespace or empty to begin with
    if (*str == '\0') return str;

    // Remove trailing whitespace
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end --;
    end[1] = '\0';

    return str;
}

int get_max(int list[], int len, int *index) {
    int max = list[0];
    for (int i = 0; i < len; i++) {
        if (list[i] > max) {
            max = list[i];
            *index = i;
        }
    }

    return max;
}


bool is_pythagorean_triple(char buff[]) {
    const char delims[] = " ";
    // First token
    char *token = strtok(buff, delims);

    // Loop through remaining items
    int vals[3] = {0, 0, 0};
    int i = 0;
    while (token != NULL || i == 2) {
        char *end_ptr;
        int val = strtol(trim(token), &end_ptr, 10);
        if (*end_ptr != '\0' || end_ptr == token) {
            fprintf(stderr, "Error: invalid int '%s'\n", token);
            exit(1);
        }
        vals[i] = val;
        token = strtok(NULL, delims);
        i++;
    }

    // Find the hypotenous
    int len = sizeof(vals) / sizeof(vals[0]);
    int index;
    int h = get_max(vals, len, &index);

    // Get the sum of squares for other two values
    int squares = 0;
    for (int i = 0; i < len; i++) {
        if (i != index)  {
            squares += pow(vals[i], 2);
        }
    }

    if (squares == pow(h, 2)) {
        printf("%d == %d^2\n", squares, h);
        return true;
    } else {
        printf("%d != %d^2\n", squares, h);
        return false;
    }
}


int main() {
    bool run = true;

    while (run) {
        char buff[64];
        printf("Input triangle lengths (q/Q to quit): ");
        fgets(buff, sizeof(buff), stdin);
        if (buff[0] == 'q' || buff[0] == 'Q') {
            break;
        }

        if (is_pythagorean_triple(buff)) {
            printf("Is pythagorean triple\n");
        } else {
            printf("Is NOT pythagorean triple\n");
        }
    }

    return 0;
}