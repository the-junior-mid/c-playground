#include <stdio.h>
#include <errno.h>
#include "vector.h"

int main(int argc, char const *argv[])
{
    // Create the Vector
    vector v;
    if (vec_new(3, sizeof(int), &v) < 0) return -1;
    vec_print(&v); vec_len(&v); vec_cap(&v);

    int i1 = 45, i2 = 23, i3 = 47;
    if (vec_push(&v, &i1) < 0) return -1;
    vec_len(&v); vec_cap(&v); vec_print(&v);
    
    if (vec_push(&v, &i2) < 0) return -1;
    vec_len(&v); vec_cap(&v); vec_print(&v);
    
    if (vec_push(&v, &i3) < 0) return -1;
    vec_len(&v); vec_cap(&v); vec_print(&v);

    int i;
    if (vec_pop(&v, &i) < 0) return -1;
    vec_len(&v); vec_cap(&v); vec_print(&v);
    printf("i = %d\n", i);

    int i4 = 33;
    if (vec_push(&v, &i4) < 0) return -1;
    vec_len(&v); vec_cap(&v); vec_print(&v);

    int i5 = 93;
    if (vec_push(&v, &i5) < 0) return -1;
    vec_len(&v); vec_cap(&v); vec_print(&v);

    int out;
    if (vec_get(&v, 1, &out) < 0) return -1;
    vec_len(&v); vec_cap(&v); vec_print(&v);
    printf("out = %d\n", out);

    if (vec_reverse(&v) < 0) return -1;
    vec_len(&v); vec_cap(&v); vec_print(&v);

    if (vec_reverse(&v) < 0) return -1;
    vec_len(&v); vec_cap(&v); vec_print(&v);

    if (vec_swap(&v, 1, 2) < 0) return -1;
    vec_len(&v); vec_cap(&v); vec_print(&v);

    int item;
    if (vec_remove(&v, 1, &item) < 0) return -1;
    vec_len(&v); vec_cap(&v); vec_print(&v);

    if (vec_insert(&v, 1, item) < 0) return -1;
    vec_len(&v); vec_cap(&v); vec_print(&v);

    vector v2;
    vec_new(4, sizeof(int), &v2);
    int j1 = 54, j2 = 32;
    if (vec_push(&v2, &j1) < 0) return -1;
    vec_len(&v2); vec_cap(&v2); vec_print(&v2);
    
    if (vec_push(&v2, &j2) < 0) return -1;
    vec_len(&v2); vec_cap(&v2); vec_print(&v2);

    if (vec_extend(&v, &v2) < 0) return -1;
    vec_len(&v); vec_cap(&v); vec_print(&v);

    vec_clear(&v);
    vec_len(&v); vec_cap(&v); vec_print(&v);
    vec_clear(&v2);
    vec_len(&v2); vec_cap(&v2); vec_print(&v2);

    // Free the Vector
    vec_free(&v);
    return 0;
}
