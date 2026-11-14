#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "vector.h"


#define ASSERT_MSG(cond, ...)                                  \
    do {                                                       \
        if (!(cond)) {                                             \
            fprintf(stderr, "%s:%d: %s: assertion '%s' failed: ", \
                __FILE__, __LINE__, __func__, #cond);          \
            fprintf(stderr, __VA_ARGS__);                      \
            fputc('\n', stderr);                               \
            abort();                                           \
        }                                                      \
    } while (0)


int test_empty() {
    printf("test_empty: START\n");
    vector v;
    if (vec_new(&v, 3, sizeof(int)) < 0) return -1;

    ASSERT_MSG(vec_len(&v) == 0, "expected length of 0, found %lu", vec_len(&v));
    ASSERT_MSG(vec_is_empty(&v) == true, "expected vector to be empty");
    ASSERT_MSG(vec_cap(&v) == 3, "expected capacity of 3");
    // get should return failure
    ASSERT_MSG(vec_get(&v, 0, NULL) == VEC_ERR_EMPTY, "expected failure for empty vector");
    // remove should return failure
    ASSERT_MSG(vec_remove(&v, 0, NULL) == VEC_ERR_EMPTY, "expected failure for empty vector");
    // pop should fail
    ASSERT_MSG(vec_pop(&v, NULL) == VEC_ERR_EMPTY, "expected failure for empty vector");
    // swap should fail
    ASSERT_MSG(vec_swap(&v, 0, 1) == VEC_ERR_RANGE, "expected failure for empty vector");

    vec_free(&v);
    printf("test_empty: END\n");

    return 0;
}

int test_one_element() {
    printf("test_one_element: START\n");
    int i0 = 23, i1 = 27;

    vector v;
    if (vec_new(&v, 3, sizeof(int)) != 0) return -1;

    assert(vec_is_empty(&v) == true);

    assert(vec_push(&v, &i0) == 0);

    int out;
    assert(vec_get(&v, 0, &out) == 0);
    assert(out != NULL);
    assert((out == i0));
    assert(vec_len(&v) == 1);
    assert(vec_is_empty(&v) == false);

    assert(vec_pop(&v, &out) == 0);
    assert(out == i0);
    assert(vec_len(&v) == 0);
    assert(vec_is_empty(&v) == true);

    assert(vec_push(&v, &i1) == 0);
    assert(vec_len(&v) == 1);
    assert(vec_is_empty(&v) == false);

    assert(vec_remove(&v, 0, &out) == 0);
    assert(out == i1);
    assert(vec_len(&v) == 0);
    assert(vec_is_empty(&v) == true);

    assert(vec_reverse(&v) == 0);

    vec_free(&v);

    printf("test_one_element: END\n");

    return 0;
}

int test_max_capacity() {
    printf("test_max_capacity: START\n");
    int i0 = 23, i1 = 29, i2 = 31, i3 = 37, i4 = 41;

    vector v;
    if (vec_new(&v, 5, sizeof(int)) < 0) return -1;

    assert(vec_is_empty(&v) == true);

    assert(vec_push(&v, &i0) == 0);
    assert(vec_push(&v, &i1) == 0);
    assert(vec_push(&v, &i2) == 0);
    assert(vec_push(&v, &i3) == 0);
    assert(vec_push(&v, &i4) == 0);

    assert(vec_cap(&v) == 5);

    int out;
    assert(vec_get(&v, 0, &out) == 0);
    assert(out != NULL);
    assert(out == i0);
    assert(vec_len(&v) == 5);
    assert(vec_is_empty(&v) == false);
    assert(vec_get(&v, 1, &out) == 0);
    assert(out == i1);
    assert(vec_len(&v) == 5);
    assert(vec_is_empty(&v) == false);
    assert(vec_get(&v, 2, &out) == 0);
    assert(out == i2);
    assert(vec_len(&v) == 5);
    assert(vec_is_empty(&v) == false);
    assert(vec_get(&v, 3, &out) == 0);
    assert(out == i3);
    assert(vec_len(&v) == 5);
    assert(vec_is_empty(&v) == false);
    assert(vec_get(&v, 4, &out) == 0);
    assert((out == i4));
    assert(vec_len(&v) == 5);
    assert(vec_is_empty(&v) == false);

    assert(vec_reverse(&v) == 0);
    assert(vec_get(&v, 0, &out) == 0);
    assert((out == i4));
    assert(vec_is_empty(&v) == false);
    assert(vec_get(&v, 1, &out) == 0);
    assert((out == i3));
    assert(vec_is_empty(&v) == false);
    assert(vec_get(&v, 2, &out) == 0);
    assert((out == i2));
    assert(vec_is_empty(&v) == false);
    assert(vec_get(&v, 3, &out) == 0);
    assert((out == i1));
    assert(vec_is_empty(&v) == false);
    assert(vec_get(&v, 4, &out) == 0);
    assert((out == i0));
    assert(vec_is_empty(&v) == false);

    assert(vec_reverse(&v) == 0);
    assert((out == i0));
    assert(vec_len(&v) == 5);
    assert(vec_is_empty(&v) == false);
    assert(vec_get(&v, 1, &out) == 0);
    assert((out == i1));
    assert(vec_is_empty(&v) == false);
    assert(vec_get(&v, 2, &out) == 0);
    assert((out == i2));
    assert(vec_is_empty(&v) == false);
    assert(vec_get(&v, 3, &out) == 0);
    assert((out == i3));
    assert(vec_is_empty(&v) == false);
    assert(vec_get(&v, 4, &out) == 0);
    assert((out == i4));
    assert(vec_is_empty(&v) == false);

    assert(vec_pop(&v, &out) == 0);
    assert((out == i4));
    assert(vec_len(&v) == 4);
    assert(vec_is_empty(&v) == false);
    assert(vec_pop(&v, &out) == 0);
    assert((out == i3));
    assert(vec_len(&v) == 3);
    assert(vec_is_empty(&v) == false);
    assert(vec_pop(&v, &out) == 0);
    assert((out == i2));
    assert(vec_len(&v) == 2);
    assert(vec_is_empty(&v) == false);
    assert(vec_pop(&v, &out) == 0);
    assert((out == i1));
    assert(vec_len(&v) == 1);
    assert(vec_is_empty(&v) == false);
    assert(vec_pop(&v, &out) == 0);
    assert((out == i0));
    assert(vec_len(&v) == 0);
    assert(vec_is_empty(&v) == true);

    assert(vec_push(&v, &i0) == 0);
    assert(vec_push(&v, &i1) == 0);
    assert(vec_push(&v, &i2) == 0);
    assert(vec_push(&v, &i3) == 0);
    assert(vec_push(&v, &i4) == 0);
    assert(vec_len(&v) == 5);

    assert(vec_remove(&v, 3, &out) == 0);
    assert((out == i3));
    assert(vec_len(&v) == 4);
    assert(vec_is_empty(&v) == false);
    assert(vec_remove(&v, 1, &out) == 0);
    assert((out == i1));
    assert(vec_len(&v) == 3);
    assert(vec_is_empty(&v) == false);
    assert(vec_remove(&v, 1, &out) == 0);
    assert((out == i2));
    assert(vec_len(&v) == 2);
    assert(vec_is_empty(&v) == false);
    assert(vec_remove(&v, 0, &out) == 0);
    assert((out == i0));
    assert(vec_len(&v) == 1);
    assert(vec_is_empty(&v) == false);
    assert(vec_remove(&v, 0, &out) == 0);
    assert((out == i4));
    assert(vec_len(&v) == 0);
    assert(vec_is_empty(&v) == true);
    assert(vec_cap(&v) == 5);

    assert(vec_push(&v, &i0) == 0);
    assert(vec_push(&v, &i1) == 0);
    assert(vec_push(&v, &i2) == 0);
    assert(vec_push(&v, &i3) == 0);
    assert(vec_push(&v, &i4) == 0);
    assert(vec_len(&v) == 5);

    assert(vec_swap(&v, 0, 4) == 0);
    assert(vec_get(&v, 0, &out) == 0);
    assert(out == i4);
    assert(vec_get(&v, 4, &out) == 0);
    assert(out == i0);

    vec_free(&v);

    printf("test_max_capacity: END\n");

    return 0;
}

int main(int argc, char const *argv[])
{
    if (test_empty() != 0) fprintf(stderr, "FAILURE: empty tests failed\n");

    if (test_one_element() != 0) fprintf(stderr, "FAILURE: one element tests failed\n");

    if (test_max_capacity() != 0) fprintf(stderr, "FAILURE: max capacity tests failed\n");

    return 0;
}