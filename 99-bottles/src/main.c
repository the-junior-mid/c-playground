#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


int main() {
    for (int i = 100; i > 0; i--) {
        if (i > 1) {
            printf("%d bottles of beer on the wall\n%d bottles of beer\ntake one down, pass it around\n", i, i);
            if (i == 2) {
                printf("%d bottle of beer on the wall\n", i - 1);
            } else {
                printf("%d bottles of beer on the wall\n", i - 1);
            }
        } else {
            printf("%d bottle of beer on the wall\n%d bottle of beer\ntake one down, pass it around\nno more bottles of beer on the wall\n", i, i);
        }
    }
    
    return 0;
}