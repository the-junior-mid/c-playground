#include <stdio.h>
#include <errno.h>
#include "vector.h"

int main(int argc, char const *argv[])
{
    // Create the Vector
    vector v;
    if (vec_new(10, &v) < 0) {
        return -1;
    }

    // Free the Vector
    vec_free(&v);
    return 0;
}
