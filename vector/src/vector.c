#include <stdio.h>
#include <stdbool.h>
#include "vector.h"

int main(int argc, char const *argv[])
{
    if (test_empty() != 0) fprintf(stderr, "FAILURE: empty tests failed");

    if (test_one_element() != 0) fprintf(stderr, "FAILURE: one element tests failed");

    if (test_max_capacity() != 0) fprintf(stderr, "FAILURE: max capacity tests failed");

    return 0;
}

int test_empty() {
    vector v;
    if (vec_new(3, sizeof(int), &v) < 0) return -1;
    
    assert(vec_len(&v) == 0);
    assert(vec_is_empty(&v) == true);
    assert(vec_cap(&v) == 3);
    // get should return failure
    assert(vec_get(&v, 0, NULL) == -1);
    // remove should return failure
    assert(vec_remove(&v, 0, NULL) == -1);
    // pop should fail
    assert(vec_pop(&v, NULL) == -1);

    vec_free(&v);
}

int test_one_element() {
    vector v;
    if (vec_new(1, sizeof(int), &v) < 0) return -1;

    assert(vec_is_empty(&v) == false);

    assert(vec_push(&v, 23) == 0);

    int out;
    assert(vec_get(&v, 0, &out) == 0);
    assert(out == 23);
    assert(vec_len(&v) == 1);
    assert(vec_is_empty(&v) == false);

    assert(vec_pop(&v, &out) == 0);
    assert(out == 23);
    assert(vec_len(&v) == 0);
    assert(vec_is_empty(&v) == true);

    assert(vec_push(&v, 27) == 0);
    assert(vec_len(&v) == 1);
    assert(vec_is_empty(&v) == false);

    assert(vec_remove(&v, 0, &out) == 0);
    assert(out == 27);
    assert(vec_len(&v) == 0);
    assert(vec_is_empty(&v) == true);

    vec_free(&v);

    return 0;
}

int test_max_capacity() {
    vector v;
    if (vec_new(5, sizeof(int), &v) < 0) return -1;

    assert(vec_is_empty(&v) == false);

    assert(vec_push(&v, 23) == 0);
    assert(vec_push(&v, 27) == 0);
    assert(vec_push(&v, 31) == 0);
    assert(vec_push(&v, 37) == 0);
    assert(vec_push(&v, 41) == 0);

    assert(vec_cap(&v) == 5);

    int out;
    assert(vec_get(&v, 0, &out) == 0);
    assert(out == 23);
    assert(vec_len(&v) == 5);
    assert(vec_is_empty(&v) == false);
    assert(vec_get(&v, 1, &out) == 0);
    assert(out == 27);
    assert(vec_len(&v) == 5);
    assert(vec_is_empty(&v) == false);
    assert(vec_get(&v, 2, &out) == 0);
    assert(out == 31);
    assert(vec_len(&v) == 5);
    assert(vec_is_empty(&v) == false);
    assert(vec_get(&v, 3, &out) == 0);
    assert(out == 37);
    assert(vec_len(&v) == 5);
    assert(vec_is_empty(&v) == false);
    assert(vec_get(&v, 4, &out) == 0);
    assert(out == 41);
    assert(vec_len(&v) == 5);
    assert(vec_is_empty(&v) == false);

    assert(vec_reverse(&v) == 0);
    assert(vec_get(&v, 0, &out) == 0);
    assert(out == 41);
    assert(vec_len(&v) == 5);
    assert(vec_is_empty(&v) == false);
    assert(vec_get(&v, 1, &out) == 0);
    assert(out == 37);
    assert(vec_len(&v) == 5);
    assert(vec_is_empty(&v) == false);
    assert(vec_get(&v, 2, &out) == 0);
    assert(out == 31);
    assert(vec_len(&v) == 5);
    assert(vec_is_empty(&v) == false);
    assert(vec_get(&v, 3, &out) == 0);
    assert(out == 27);
    assert(vec_len(&v) == 5);
    assert(vec_is_empty(&v) == false);
    assert(vec_get(&v, 4, &out) == 0);
    assert(out == 23);
    assert(vec_len(&v) == 5);
    assert(vec_is_empty(&v) == false);

    assert(vec_reverse(&v) == 0);
    assert(out == 23);
    assert(vec_len(&v) == 5);
    assert(vec_is_empty(&v) == false);
    assert(vec_get(&v, 1, &out) == 0);
    assert(out == 27);
    assert(vec_len(&v) == 5);
    assert(vec_is_empty(&v) == false);
    assert(vec_get(&v, 2, &out) == 0);
    assert(out == 31);
    assert(vec_len(&v) == 5);
    assert(vec_is_empty(&v) == false);
    assert(vec_get(&v, 3, &out) == 0);
    assert(out == 37);
    assert(vec_len(&v) == 5);
    assert(vec_is_empty(&v) == false);
    assert(vec_get(&v, 4, &out) == 0);
    assert(out == 41);
    assert(vec_len(&v) == 5);
    assert(vec_is_empty(&v) == false);

    assert(vec_pop(&v, &out) == 0);
    assert(out == 41);
    assert(vec_len(&v) == 4);
    assert(vec_is_empty(&v) == false);
    assert(vec_pop(&v, &out) == 0);
    assert(out == 37);
    assert(vec_len(&v) == 3);
    assert(vec_is_empty(&v) == false);
    assert(vec_pop(&v, &out) == 0);
    assert(out == 31);
    assert(vec_len(&v) == 2);
    assert(vec_is_empty(&v) == false);
    assert(vec_pop(&v, &out) == 0);
    assert(out == 27);
    assert(vec_len(&v) == 1);
    assert(vec_is_empty(&v) == false);
    assert(vec_pop(&v, &out) == 0);
    assert(out == 23);
    assert(vec_len(&v) == 0);
    assert(vec_is_empty(&v) == true);

    assert(vec_push(&v, 23) == 0);
    assert(vec_push(&v, 27) == 0);
    assert(vec_push(&v, 31) == 0);
    assert(vec_push(&v, 37) == 0);
    assert(vec_push(&v, 41) == 0);

    assert(vec_remove(&v, 0, &out) == 0);
    assert(out == 23);
    assert(vec_len(&v) == 4);
    assert(vec_is_empty(&v) == false);
    assert(vec_remove(&v, 0, &out) == 0);
    assert(out == 27);
    assert(vec_len(&v) == 3);
    assert(vec_is_empty(&v) == false);
    assert(vec_remove(&v, 0, &out) == 0);
    assert(out == 31);
    assert(vec_len(&v) == 2);
    assert(vec_is_empty(&v) == false);
    assert(vec_remove(&v, 0, &out) == 0);
    assert(out == 37);
    assert(vec_len(&v) == 1);
    assert(vec_is_empty(&v) == false);
    assert(vec_remove(&v, 0, &out) == 0);
    assert(out == 41);
    assert(vec_len(&v) == 0);
    assert(vec_is_empty(&v) == true);

    assert(vec_cap(&v) == 5);

    vec_free(&v);

    return 0;
}
