#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "vector.h"


#define ASSERT_MSG(cond, ...)                                     \
    do {                                                          \
        if (!(cond)) {                                            \
            fprintf(stderr, "%s:%d: %s: assertion '%s' failed: ", \
                __FILE__, __LINE__, __func__, #cond);             \
            fprintf(stderr, __VA_ARGS__);                         \
            fputc('\n', stderr);                                  \
            abort();                                              \
        }                                                         \
    } while (0)

#define TEST_STATUS(test)                               \
    do {                                                \
        if (test < 0) {                                 \
            fprintf(stderr, "%s ... FAILED\n", #test);  \
        } else {                                        \
            fprintf(stderr, "%s ... SUCCESS\n", #test); \
        }                                               \
    } while (0)

int test_vec_new() {
    vector v;
    assert(vec_new(&v, 5, sizeof(int)) == 0);
    ASSERT_MSG(v.cap == 5, "expected capacity of 5, found %lu", v.cap);
    ASSERT_MSG(v.len == 0, "expected length of 0, found %lu", v.len);

    for (size_t i = 0; i < v.cap; i++) {
        int *val = _vec_slot(&v, i);
        ASSERT_MSG(*val == 0, "vector was not zero initialized at index %lu", i);
    }

    vec_destroy(&v);

    return 0;
}

int test_vec_push() {
    vector v;
    assert(vec_new(&v, 5, sizeof(int)) == 0);
    ASSERT_MSG(v.cap == 5, "expected capacity of 5, found %lu", v.cap);
    ASSERT_MSG(v.len == 0, "expected length of 0, found %lu", v.len);

    for (size_t i = 0; i < v.len - 2; i++) {
        assert(vec_push(&v, (void *)&i) == 0);
    }
    ASSERT_MSG(v.len == 2, "expected length of 2, found %lu", v.len);
    ASSERT_MSG(v.cap == 5, "expected capacity of 5, found %lu", v.cap);

    // Rest of the array should still be zero
    for (size_t i = 2; i < v.cap; i++) {
        int *val = _vec_slot(&v, i);
        ASSERT_MSG(*val == 0, "expected 0 at index %lu, found %d", i, *val);
    }

    // Push more than the capacity holds to force reallocation
    size_t len = v.len + 5;
    for (size_t i = 2; i < len + 5; i++) {
        assert(vec_push(&v, (void *)&i) == 0);
    }
    ASSERT_MSG(v.len == 12, "expected length of 12, found %lu", v.len);
    ASSERT_MSG(v.cap == 20, "expected capacity of 20, found %lu", v.cap);

    for (size_t i = 1; i < v.cap; i++) {
        int *val = _vec_slot(&v, i);
        int expected = (i > 11) ? 0 : (int)i;
        ASSERT_MSG(*val == expected, "expected %d at index %lu, found %d", expected, i, *val);
    }

    vec_destroy(&v);

    return 0;
}

int test_vec_pop() {
    vector v;
    assert(vec_new(&v, 5, sizeof(int)) == 0);
    ASSERT_MSG(v.cap == 5, "expected capacity of 5, found %lu", v.cap);
    ASSERT_MSG(v.len == 0, "expected length of 0, found %lu", v.len);

    int i0 = 345, i1 = 327, i2 = 918;
    assert(vec_push(&v, (void *)&i0) == 0);
    assert(vec_push(&v, (void *)&i1) == 0);
    assert(vec_push(&v, (void *)&i2) == 0);

    int out;
    assert(vec_pop(&v, &out) == 0);
    ASSERT_MSG(v.len == 2, "expected vector length to decrease");
    ASSERT_MSG(out == i2, "expected %d, found %d", i2, out);
    assert(vec_pop(&v, &out) == 0);
    ASSERT_MSG(v.len == 1, "expected vector length to decrease");
    ASSERT_MSG(out == i1, "expected %d, found %d", i1, out);
    assert(vec_pop(&v, &out) == 0);
    ASSERT_MSG(v.len == 0, "expected vector length to decrease");
    ASSERT_MSG(out == i0, "expected %d, found %d", i0, out);

    vec_destroy(&v);

    return 0;
}

int test_vec_realloc() {
    vector v;
    assert(vec_new(&v, 3, sizeof(int)) == 0);
    ASSERT_MSG(v.cap == 3, "expected capacity of 5, found %lu", v.cap);
    ASSERT_MSG(v.len == 0, "expected length of 0, found %lu", v.len);

    assert(vec_realloc(&v, 10) == 0);
    ASSERT_MSG(v.len == 0, "expected vector length to remain 0, found %lu", v.len);
    ASSERT_MSG(v.cap == 10, "expected vector capacity to increase");

    // Check all data is still zero initialized
    for (size_t i = 0; i < v.cap; i++) {
        int *val = _vec_slot(&v, i);
        ASSERT_MSG(*val == 0, "vector was not zero initialized at index %lu", i);
    }

    // Add elements
    for (int i = 13; i < 20; i++) {
        assert(vec_push(&v, &i) == 0);
    }

    // Realloc again
    assert(vec_realloc(&v, 15) == 0);
    ASSERT_MSG(v.len == 7, "expected vector length of 7, found %lu", v.len);
    ASSERT_MSG(v.cap == 15, "expected vector capacity 15, found %lu", v.cap);

    // Ensure previous data was not overwritten
    for (size_t i = 0; i < v.cap; i++) {
        int *val = _vec_slot(&v, i);
        if (i < 7) {
            ASSERT_MSG(*val == 13 + (int)i, "vector was overwritten at index %lu", i);
        } else {
            ASSERT_MSG(*val == 0, "vector was not zero initialized at index %lu", i);
        }
    }

    vec_destroy(&v);

    return 0;
}

int test_vec_extend() {
    vector v1;
    assert(vec_new(&v1, 20, sizeof(int)) == 0);
    ASSERT_MSG(v1.cap == 20, "expected capacity of 5, found %lu", v1.cap);
    ASSERT_MSG(v1.len == 0, "expected capacity of 0, found %lu", v1.len);

    // Add elements
    for (int i = 0; i < 5; i++) {
        assert(vec_push(&v1, &i) == 0);
    }

    vector v2;
    assert(vec_new(&v2, 5, sizeof(int)) == 0);
    ASSERT_MSG(v2.cap == 5, "expected capacity of 5, found %lu", v1.cap);
    ASSERT_MSG(v2.len == 0, "expected capacity of 0, found %lu", v1.len);

    // Add elements
    for (int i = 5; i < 8; i++) {
        assert(vec_push(&v2, &i) == 0);
    }

    // Extend first vector with second
    assert(vec_extend(&v1, &v2) == 0);
    ASSERT_MSG(v1.cap == 20, "expected v1 capacity to remain unchanged");
    ASSERT_MSG(v1.len == 8, "expected 8, found %lu", v1.len);

    for (size_t i = 0; i < v1.len; i++) {
        int *val = _vec_slot(&v1, i);
        ASSERT_MSG(*val == (int)i, "expected %lu at index %lu, found %d", i, i, *val);
    }

    vector v3;
    assert(vec_new(&v3, 20, sizeof(int)) == 0);
    ASSERT_MSG(v3.cap == 20, "expected capacity of 5, found %lu", v3.cap);
    ASSERT_MSG(v3.len == 0, "expected capacity of 0, found %lu", v3.len);

    // Add elements
    for (int i = 8; i < 25; i++) {
        assert(vec_push(&v3, &i) == 0);
    }

    // Extend first vector with third
    assert(vec_extend(&v1, &v3) == 0);
    ASSERT_MSG(v1.cap >= 40, "expected v1 capacity to increase");
    ASSERT_MSG(v1.len >= 24, "expected 8, found %lu", v1.len);

    for (size_t i = 0; i < v1.len; i++) {
        int *val = _vec_slot(&v1, i);
        ASSERT_MSG(*val == (int)i, "expected %lu at index %lu, found %d", i, i, *val);
    }

    vec_destroy(&v1);
    vec_destroy(&v2);
    vec_destroy(&v3);

    return 0;
}

int test_vec_extend_with_array() {
    vector v1;
    assert(vec_new(&v1, 20, sizeof(int)) == 0);
    ASSERT_MSG(v1.cap == 20, "expected capacity of 5, found %lu", v1.cap);
    ASSERT_MSG(v1.len == 0, "expected capacity of 0, found %lu", v1.len);

    // Add elements
    for (int i = 0; i < 5; i++) {
        assert(vec_push(&v1, &i) == 0);
    }

    int v2[] = {5, 6, 7};
    size_t v2_len = 3;

    // Extend first vector with second
    assert(vec_extend_with_array(&v1, &v2, v2_len) == 0);
    ASSERT_MSG(v1.cap == 20, "expected v1 capacity to remain unchanged");
    ASSERT_MSG(v1.len == 8, "expected 8, found %lu", v1.len);

    for (size_t i = 0; i < v1.len; i++) {
        int *val = _vec_slot(&v1, i);
        ASSERT_MSG(*val == (int)i, "expected %lu at index %lu, found %d", i, i, *val);
    }

    int v3[] = {8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
    size_t v3_len = 17;

    // Extend first vector with third
    assert(vec_extend_with_array(&v1, &v3, v3_len) == 0);
    ASSERT_MSG(v1.cap >= 40, "expected v1 capacity to increase");
    ASSERT_MSG(v1.len >= 24, "expected 8, found %lu", v1.len);

    for (size_t i = 0; i < v1.len; i++) {
        int *val = _vec_slot(&v1, i);
        ASSERT_MSG(*val == (int)i, "expected %lu at index %lu, found %d", i, i, *val);
    }

    vec_destroy(&v1);

    return 0;
}

int test_vec_insert() {
    vector v;
    assert(vec_new(&v, 11, sizeof(int)) == 0);
    ASSERT_MSG(v.cap == 11, "expected capacity of 11, found %lu", v.cap);
    ASSERT_MSG(v.len == 0, "expected length of 0, found %lu", v.len);

    for (size_t i = 0; i < 8; i++) {
        assert(vec_push(&v, &i) == 0);
    }
    ASSERT_MSG(v.len == 8, "expected length of 8, found %lu", v.len);

    // What the vector should look like at each stage
    int vals1[] = {11, 0, 1, 2, 3, 4, 5, 6, 7};
    int vals2[] = {11, 0, 1, 2, 3, 33, 4, 5, 6, 7};
    int vals3[] = {11, 0, 1, 2, 3, 33, 4, 5, 6, 17, 7};

    int inserted_val = 11;
    assert(vec_insert(&v, 0, (void *)&inserted_val) == 0);
    ASSERT_MSG(v.cap == 11, "expected capacity of 11, found %lu", v.cap);
    ASSERT_MSG(v.len == 9, "expected length of 9, found %lu", v.len);
    for (size_t i = 0; i < v.len; i++) {
        int *val = _vec_slot(&v, i);
        int expected = vals1[i];
        ASSERT_MSG(*val == expected, "expected %d, found %d", expected, *val);
    }

    inserted_val = 33;
    assert(vec_insert(&v, 5, (void *)&inserted_val) == 0);
    ASSERT_MSG(v.cap == 11, "expected capacity of 20, found %lu", v.cap);
    ASSERT_MSG(v.len == 10, "expected length of 10, found %lu", v.len);
    for (size_t i = 0; i < v.len; i++) {
        int *val = _vec_slot(&v, i);
        int expected = vals2[i];
        ASSERT_MSG(*val == expected, "expected %d at index %lu, found %d", expected, i, *val);
    }

    // Insert one more to force a reallocation
    inserted_val = 17;
    assert(vec_insert(&v, 9, (void *)&inserted_val) == 0);
    ASSERT_MSG(v.cap == 22, "expected capacity of 20, found %lu", v.cap);
    ASSERT_MSG(v.len == 11, "expected length of 11, found %lu", v.len);
    for (size_t i = 0; i < v.len; i++) {
        int *val = _vec_slot(&v, i);
        int expected = vals3[i];
        ASSERT_MSG(*val == expected, "expected %d, found %d", expected, *val);
    }

    vec_destroy(&v);

    return 0;
}

int test_vec_remove() {
    vector v;
    assert(vec_new(&v, 11, sizeof(int)) == 0);
    ASSERT_MSG(v.cap == 11, "expected capacity of 11, found %lu", v.cap);
    ASSERT_MSG(v.len == 0, "expected length of 0, found %lu", v.len);

    for (size_t i = 0; i < 11; i++) {
        assert(vec_push(&v, &i) == 0);
    }
    ASSERT_MSG(v.len == 11, "expected length of 11, found %lu", v.len);

    // What the vector should look like at each stage
    int vals1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int vals2[] = {1, 2, 3, 4, 5, 7, 8, 9, 10};
    int vals3[] = {1, 2, 3, 4, 5, 7, 8, 9};

    int out;
    assert(vec_remove(&v, 0, (void *)&out) == 0);
    ASSERT_MSG(v.cap == 11, "expected capacity of 11, found %lu", v.cap);
    ASSERT_MSG(v.len == 10, "expected length of 10, found %lu", v.len);
    ASSERT_MSG(out == 0, "expected %d, found %d", 0, out);
    for (size_t i = 0; i < v.len; i++) {
        int *val = _vec_slot(&v, i);
        int expected = vals1[i];
        ASSERT_MSG(*val == expected, "expected %d, found %d", expected, *val);
    }

    assert(vec_remove(&v, 5, (void *)&out) == 0);
    ASSERT_MSG(v.cap == 11, "expected capacity of 20, found %lu", v.cap);
    ASSERT_MSG(v.len == 9, "expected length of 9, found %lu", v.len);
    for (size_t i = 0; i < v.len; i++) {
        int *val = _vec_slot(&v, i);
        int expected = vals2[i];
        ASSERT_MSG(*val == expected, "expected %d at index %lu, found %d", expected, i, *val);
    }

    // Insert one more to force a reallocation
    assert(vec_remove(&v, 8, (void *)&out) == 0);
    ASSERT_MSG(v.cap == 11, "expected capacity of 20, found %lu", v.cap);
    ASSERT_MSG(v.len == 8, "expected length of 8, found %lu", v.len);
    for (size_t i = 0; i < v.len; i++) {
        int *val = _vec_slot(&v, i);
        int expected = vals3[i];
        ASSERT_MSG(*val == expected, "expected %d, found %d", expected, *val);
    }

    vec_destroy(&v);

    return 0;
}

int test_vec_get() {
    vector v;
    assert(vec_new(&v, 11, sizeof(int)) == 0);
    ASSERT_MSG(v.cap == 11, "expected capacity of 11, found %lu", v.cap);
    ASSERT_MSG(v.len == 0, "expected length of 0, found %lu", v.len);

    for (size_t i = 0; i < 11; i++) {
        assert(vec_push(&v, &i) == 0);
    }
    ASSERT_MSG(v.len == 11, "expected length of 11, found %lu", v.len);

    int out;
    for (size_t i = 0; i < v.len; i++) {
        assert(vec_get(&v, i, &out) == 0);
        ASSERT_MSG(out == (int)i, "expected %lu at index %lu, found %d", i, i, out);
    }

    // Try to reach out of bounds
    ASSERT_MSG(vec_get(&v, 12, &out) == MID_ERR_RANGE, "reached outside vector bounds");
    // Make sure the get did not write anything to out
    ASSERT_MSG(out == 10, "expected 10 at index 12, found %d", out);
    // Do not try to write to null
    ASSERT_MSG(vec_get(&v, 0, NULL) == MID_ERR_NULL, "wrote to null");

    vec_destroy(&v);

    return 0;
}

int test_vec_reverse() {
    vector v;
    assert(vec_new(&v, 15, sizeof(int)) == 0);
    ASSERT_MSG(v.cap == 15, "expected capacity of 15, found %lu", v.cap);
    ASSERT_MSG(v.len == 0, "expected length of 0, found %lu", v.len);

    for (size_t i = 0; i < 15; i++) {
        assert(vec_push(&v, &i) == 0);
    }
    ASSERT_MSG(v.len == 15, "expected length of 15, found %lu", v.len);

    assert(vec_reverse(&v) == 0);
    for (size_t i = 0; i < 15; i++) {
        int val = *(int *)_vec_slot(&v, i);
        ASSERT_MSG(val == (int)(14 - i), "expected %lu at index %lu, found %d", 14 - i, i, val);
    }

    // Second reverse should be back to normal
    assert(vec_reverse(&v) == 0);
    for (size_t i = 0; i < 15; i++) {
        int val = *(int *)_vec_slot(&v, i);
        ASSERT_MSG(val == (int)(i), "expected %lu at index %lu, found %d", i, i, val);
    }

    vec_destroy(&v);

    return 0;
}

int test_vec_swap() {
    vector v;
    assert(vec_new(&v, 15, sizeof(int)) == 0);
    ASSERT_MSG(v.cap == 15, "expected capacity of 15, found %lu", v.cap);
    ASSERT_MSG(v.len == 0, "expected length of 0, found %lu", v.len);

    for (size_t i = 0; i < 15; i++) {
        assert(vec_push(&v, &i) == 0);
    }
    ASSERT_MSG(v.len == 15, "expected length of 15, found %lu", v.len);

    // Swap with 1st index out of bounds
    ASSERT_MSG(vec_swap(&v, 16, 0) == MID_ERR_RANGE, "swapped with out of bounds index");
    // Swap with 2nd index out of bounds
    ASSERT_MSG(vec_swap(&v, 0, 16) == MID_ERR_RANGE, "swapped with out of bounds index");
    // Swap with 1st and 2nd indexes out of bounds
    ASSERT_MSG(vec_swap(&v, 17, 16) == MID_ERR_RANGE, "swapped with out of bounds index");

    // The values the vector should have after the swap
    int vals1[] = {14, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 0};
    assert(vec_swap(&v, 0, 14) == 0);
    for (size_t i = 0; i < v.len; i++) {
        int val = *(int *)_vec_slot(&v, i);
        ASSERT_MSG(val == vals1[i], "expected %d at index %lu, found %d", vals1[i], i, val);
    }

    // The values the vector should have after the swap
    int vals2[] = {14, 1, 2, 3, 4, 9, 6, 7, 8, 5, 10, 11, 12, 13, 0};
    assert(vec_swap(&v, 5, 9) == 0);
    for (size_t i = 0; i < v.len; i++) {
        int val = *(int *)_vec_slot(&v, i);
        ASSERT_MSG(val == vals2[i], "expected %d at index %lu, found %d", vals2[i], i, val);
    }

    vec_destroy(&v);

    return 0;
}

int test_vec_clear() {
    vector v;
    assert(vec_new(&v, 15, sizeof(int)) == 0);
    ASSERT_MSG(v.cap == 15, "expected capacity of 15, found %lu", v.cap);
    ASSERT_MSG(v.len == 0, "expected length of 0, found %lu", v.len);

    for (size_t i = 0; i < 15; i++) {
        assert(vec_push(&v, &i) == 0);
    }
    ASSERT_MSG(v.len == 15, "expected length of 15, found %lu", v.len);

    vec_clear(&v);
    ASSERT_MSG(v.cap == 15, "expected capacity of 15, found %lu", v.cap);
    ASSERT_MSG(v.len == 0, "expected length of 0, found %lu", v.len);

    // Buffer contents should be unchanged
    for (size_t i = 0; i < v.cap; i++) {
        int val = *(int *)_vec_slot(&v, i);
        ASSERT_MSG(val == (int)i, "expected %d at index %lu, found %d", (int)i, i, val);
    }

    int out;
    ASSERT_MSG(vec_get(&v, 1, &out) == MID_ERR_EMPTY, "retrieved value even though length is 0");

    vec_destroy(&v);

    return 0;
}

int test_zero_capacity() {
    vector v1;
    if (vec_new(&v1, 0, sizeof(int)) < 0) return -1;
    assert(vec_cap(&v1) == 0);
    assert(vec_len(&v1) == 0);

    // A push should trigger a reallocation
    for (size_t i = 0; i < 3; i++) {
        assert(vec_push(&v1, &i) == 0);
    }
    assert(vec_cap(&v1) == 4);
    assert(vec_len(&v1) == 3);

    vector v2;
    if (vec_new(&v2, 0, sizeof(int)) < 0) return -1;
    assert(vec_cap(&v2) == 0);
    assert(vec_len(&v2) == 0);

    // The vector that will extend v2
    vector v3;
    if (vec_new(&v3, 3, sizeof(int)) < 0) return -1;
    assert(vec_cap(&v3) == 3);
    assert(vec_len(&v3) == 0);
    for (size_t i = 0; i < v3.cap; i++) {
        assert(vec_push(&v3, &i) == 0);
    }
    assert(vec_cap(&v3) == 3);
    assert(vec_len(&v3) == 3);

    // An extend should trigger a reallocation
    assert(vec_extend(&v2, &v3) == 0);
    assert(vec_cap(&v2) == 4 + v3.len);
    assert(vec_len(&v2) == 3);

    vector v4;
    if (vec_new(&v4, 0, sizeof(int)) < 0) return -1;
    assert(vec_cap(&v4) == 0);
    assert(vec_len(&v4) == 0);

    // The array that will extend v4
    int vals[] = {0, 1, 2, 3};
    size_t vals_len = 4;

    // An extend w/ array should trigger a reallocation
    assert(vec_extend_with_array(&v4, &vals, vals_len) == 0);
    assert(vec_cap(&v4) == 4 + vals_len);
    assert(vec_len(&v4) == 4);

    vec_destroy(&v1);
    vec_destroy(&v2);
    vec_destroy(&v3);
    vec_destroy(&v4);

    return 0;
}

int test_empty() {
    vector v;
    if (vec_new(&v, 3, sizeof(int)) < 0) return -1;

    ASSERT_MSG(vec_len(&v) == 0, "expected length of 0, found %lu", vec_len(&v));
    ASSERT_MSG(vec_is_empty(&v) == true, "expected vector to be empty");
    ASSERT_MSG(vec_cap(&v) == 3, "expected capacity of 3");
    int out;
    // get should return failure
    ASSERT_MSG(vec_get(&v, 0, &out) == MID_ERR_EMPTY, "expected failure for empty vector");
    // remove should return failure
    ASSERT_MSG(vec_remove(&v, 0, &out) == MID_ERR_EMPTY, "expected failure for empty vector");
    // pop should fail
    ASSERT_MSG(vec_pop(&v, &out) == MID_ERR_EMPTY, "expected failure for empty vector");
    // swap should fail
    ASSERT_MSG(vec_swap(&v, 0, 1) == MID_ERR_RANGE, "expected failure for empty vector");

    vec_destroy(&v);

    return 0;
}

int test_one_element() {
    int i0 = 23, i1 = 27;

    vector v;
    if (vec_new(&v, 3, sizeof(int)) != 0) return -1;

    ASSERT_MSG(vec_is_empty(&v) == true, "vector created was non-empty");

    assert(vec_push(&v, &i0) == 0);

    int out;
    assert(vec_get(&v, 0, &out) == 0);
    ASSERT_MSG((out == i0), "expected %d, found %d", i0, out);
    ASSERT_MSG(vec_len(&v) == 1, "expected vector length to remain unchanged");

    assert(vec_pop(&v, &out) == 0);
    ASSERT_MSG(out == i0, "expected %d, found %d", i0, out);
    ASSERT_MSG(vec_len(&v) == 0, "expected length to decrease");
    assert(vec_is_empty(&v) == true);

    assert(vec_push(&v, &i1) == 0);
    ASSERT_MSG(vec_len(&v) == 1, "expected vector length to increase");
    assert(vec_is_empty(&v) == false);

    assert(vec_remove(&v, 0, &out) == 0);
    ASSERT_MSG(out == i1, "excpected %d, found %d", i1, out);
    ASSERT_MSG(vec_len(&v) == 0, "expected vector length to decrease");
    assert(vec_is_empty(&v) == true);

    assert(vec_reverse(&v) == 0);

    vec_destroy(&v);

    return 0;
}

int test_max_capacity() {
    vector v;
    if (vec_new(&v, 15, sizeof(int)) < 0) return -1;
    assert(vec_cap(&v) == 15);
    assert(vec_len(&v) == 0);
    assert(vec_is_empty(&v) == true);

    for (size_t i = 0; i < v.cap; i++) {
        assert(vec_push(&v, &i) == 0);
    }
    assert(vec_cap(&v) == 15);
    assert(vec_len(&v) == 15);
    assert(vec_is_empty(&v) == false);  

    for (size_t i = 0; i < v.len; i++) {
        int out;
        assert(vec_get(&v, i, &out) == 0);
        ASSERT_MSG(out == (int)i, "expected %lu at index %lu, found %d", i, i, out);
        assert(vec_len(&v) == 15);  
        assert(vec_cap(&v) == 15);
    }

    assert(vec_reverse(&v) == 0);
    for (size_t i = 0; i < v.len; i++) {
        int out;
        assert(vec_get(&v, i, &out) == 0);
        ASSERT_MSG(out == (int)(14 - i), "expected %lu at index %lu, found %d", 14 - i, i, out);
        assert(vec_len(&v) == 15);
        assert(vec_cap(&v) == 15);
        assert(vec_is_empty(&v) == false);
    }

    assert(vec_reverse(&v) == 0);
    for (size_t i = 0; i < v.len; i++) {
        int out;
        assert(vec_get(&v, i, &out) == 0);
        ASSERT_MSG(out == (int)i, "expected %lu at index %lu, found %d", i, i, out);
        assert(vec_len(&v) == 15);
        assert(vec_cap(&v) == 15);
        assert(vec_is_empty(&v) == false);
    }

    size_t len = v.len;
    for (size_t i = 0; i < len; i++) {
        int out;
        assert(vec_pop(&v, &out) == 0);
        ASSERT_MSG(out == (int)(14 - i), "expected %lu at index %lu, found %d", 14 - i, i, out);
        assert(vec_len(&v) == 14 - i);
        assert(vec_cap(&v) == 15);
        if (i < 14) {
            assert(vec_is_empty(&v) == false);
        } else {
            assert(vec_is_empty(&v) == true);
        }
        
    }

    for (size_t i = 0; i < v.cap; i++) {
        assert(vec_push(&v, &i) == 0);
    }
    assert(vec_cap(&v) == 15);
    assert(vec_len(&v) == 15);
    assert(vec_is_empty(&v) == false); 

    len = v.len;
    for (size_t i = 0; i < len; i++) {
        int out;
        assert(vec_remove(&v, 0, &out) == 0);
        ASSERT_MSG(out == (int)i, "expected %lu at index %lu, found %d", i, i, out);
        assert(vec_len(&v) == 14 - i);
        assert(vec_cap(&v) == 15);
        if (i < 14) {
            assert(vec_is_empty(&v) == false);
        } else {
            assert(vec_is_empty(&v) == true);
        }
        
    }

    for (size_t i = 0; i < v.cap; i++) {
        assert(vec_push(&v, &i) == 0);
    }
    assert(vec_cap(&v) == 15);
    assert(vec_len(&v) == 15);
    assert(vec_is_empty(&v) == false);

    int out;
    assert(vec_swap(&v, 0, 4) == 0);
    assert(vec_get(&v, 0, &out) == 0);
    ASSERT_MSG(out == 4, "expected 0, found %d", out);
    assert(vec_get(&v, 4, &out) == 0);
    ASSERT_MSG(out == 0, "expected 4, found %d", out);

    vec_destroy(&v);

    return 0;
}

int test_vec_ptr() {
    vector v;
    assert(vec_new(&v, 5, sizeof(int *)) == 0);
    ASSERT_MSG(v.cap == 5, "expected capacity of 5, found %lu", v.cap);
    ASSERT_MSG(v.len == 0, "expected length of 0, found %lu", v.len);

    int vals[] = {0, 1, 2, 3, 4};
    int *val_ptrs[] = {&vals[0], &vals[1], &vals[2], &vals[3], &vals[4]};
    for (size_t i = 0; i < v.cap; i++) {
        assert(vec_push(&v, &val_ptrs[i]) == 0);
    }
    ASSERT_MSG(v.len == 5, "expected length of 5, found %lu", v.len);

    for (size_t i = 0; i < v.len; i++) {
        int *out = NULL;
        assert(vec_get(&v, i, &out) == 0);
        ASSERT_MSG(out == val_ptrs[i], "expected %p at index %lu, found %p", val_ptrs[i], i, out);
        assert(vec_len(&v) == 5);  
        assert(vec_cap(&v) == 5);
    }

    assert(vec_reverse(&v) == 0);
    for (size_t i = 0; i < v.len; i++) {
        int *out = NULL;
        assert(vec_get(&v, i, &out) == 0);
        ASSERT_MSG(out == val_ptrs[4 - i], "expected %p at index %lu, found %p", val_ptrs[4 - i], i, out);
        assert(vec_len(&v) == 5);
        assert(vec_cap(&v) == 5);
        assert(vec_is_empty(&v) == false);
    }

    assert(vec_reverse(&v) == 0);
    for (size_t i = 0; i < v.len; i++) {
        int *out = NULL;
        assert(vec_get(&v, i, &out) == 0);
        ASSERT_MSG(out == val_ptrs[i], "expected %p at index %lu, found %p", val_ptrs[i], i, out);
        assert(vec_len(&v) == 5);
        assert(vec_cap(&v) == 5);
        assert(vec_is_empty(&v) == false);
    }

    size_t len = v.len;
    for (size_t i = 0; i < len; i++) {
        int *out = NULL;
        assert(vec_pop(&v, &out) == 0);
        ASSERT_MSG(out == val_ptrs[4 - i],  "expected %p at index %lu, found %p", val_ptrs[4 - i], i, out);
        assert(vec_len(&v) == 4 - i);
        assert(vec_cap(&v) == 5);
        if (i < 4) {
            assert(vec_is_empty(&v) == false);
        } else {
            assert(vec_is_empty(&v) == true);
        }
    }

    for (size_t i = 0; i < v.cap; i++) {
        assert(vec_push(&v, &val_ptrs[i]) == 0);
    }
    assert(vec_cap(&v) == 5);
    assert(vec_len(&v) == 5);
    assert(vec_is_empty(&v) == false); 

    len = v.len;
    for (size_t i = 0; i < len; i++) {
        int *out = NULL;
        assert(vec_remove(&v, 0, &out) == 0);
        ASSERT_MSG(out == val_ptrs[i], "expected %p at index %lu, found %p", val_ptrs[i], i, out);
        assert(vec_len(&v) == 4 - i);
        assert(vec_cap(&v) == 5);
        if (i < 4) {
            assert(vec_is_empty(&v) == false);
        } else {
            assert(vec_is_empty(&v) == true);
        }
    }

    for (size_t i = 0; i < v.cap; i++) {
        assert(vec_push(&v, &val_ptrs[i]) == 0);
    }
    assert(vec_cap(&v) == 5);
    assert(vec_len(&v) == 5);
    assert(vec_is_empty(&v) == false);

    int *out = NULL;
    assert(vec_swap(&v, 0, 4) == 0);
    assert(vec_get(&v, 0, &out) == 0);
    ASSERT_MSG(out == val_ptrs[4], "expected %p, found %p", val_ptrs[4], out);
    assert(vec_get(&v, 4, &out) == 0);
    ASSERT_MSG(out == val_ptrs[0], "expected %p, found %p", val_ptrs[0], out);

    vec_destroy(&v);

    return 0;
}

int test_vec_char() {
    vector v;
    assert(vec_new(&v, 14, sizeof(char)) == 0);
    ASSERT_MSG(v.cap == 14, "expected capacity of 5, found %lu", v.cap);
    ASSERT_MSG(v.len == 0, "expected length of 0, found %lu", v.len);

    char *vals = "Hello, World!\n";
    for (size_t i = 0; i < v.cap; i++) {
        assert(vec_push(&v, &vals[i]) == 0);
    }
    ASSERT_MSG(v.len == 14, "expected length of 14, found %lu", v.len);

    for (size_t i = 0; i < v.len; i++) {
        int *out = NULL;
        assert(vec_get(&v, i, &out) == 0);
        ASSERT_MSG(out == vals[i], "expected %s at index %lu, found %s", vals[i], i, out);
        assert(vec_len(&v) == 14);  
        assert(vec_cap(&v) == 14);
    }

    assert(vec_reverse(&v) == 0);
    for (size_t i = 0; i < v.len; i++) {
        int *out = NULL;
        assert(vec_get(&v, i, &out) == 0);
        ASSERT_MSG(out == vals[13 - i], "expected %s at index %lu, found %s", vals[4 - i], i, out);
        assert(vec_len(&v) == 14);
        assert(vec_cap(&v) == 14);
        assert(vec_is_empty(&v) == false);
    }

    assert(vec_reverse(&v) == 0);
    for (size_t i = 0; i < v.len; i++) {
        int *out = NULL;
        assert(vec_get(&v, i, &out) == 0);
        ASSERT_MSG(out == vals[i], "expected %s at index %lu, found %s", vals[i], i, out);
        assert(vec_len(&v) == 14);
        assert(vec_cap(&v) == 14);
        assert(vec_is_empty(&v) == false);
    }

    size_t len = v.len;
    for (size_t i = 0; i < len; i++) {
        int *out = NULL;
        assert(vec_pop(&v, &out) == 0);
        ASSERT_MSG(out == vals[13 - i],  "expected %s at index %lu, found %s", vals[4 - i], i, out);
        assert(vec_len(&v) == 13 - i);
        assert(vec_cap(&v) == 14);
        if (i < 13) {
            assert(vec_is_empty(&v) == false);
        } else {
            assert(vec_is_empty(&v) == true);
        }
    }

    for (size_t i = 0; i < v.cap; i++) {
        assert(vec_push(&v, &vals[i]) == 0);
    }
    assert(vec_cap(&v) == 14);
    assert(vec_len(&v) == 14);
    assert(vec_is_empty(&v) == false); 

    len = v.len;
    for (size_t i = 0; i < len; i++) {
        int *out = NULL;
        assert(vec_remove(&v, 0, &out) == 0);
        ASSERT_MSG(out == vals[i], "expected %s at index %lu, found %s", vals[i], i, out);
        assert(vec_len(&v) == 13 - i);
        assert(vec_cap(&v) == 14);
        if (i < 13) {
            assert(vec_is_empty(&v) == false);
        } else {
            assert(vec_is_empty(&v) == true);
        }
    }

    for (size_t i = 0; i < v.cap; i++) {
        assert(vec_push(&v, &vals[i]) == 0);
    }
    assert(vec_cap(&v) == 14);
    assert(vec_len(&v) == 14);
    assert(vec_is_empty(&v) == false);

    int *out = NULL;
    assert(vec_swap(&v, 0, 4) == 0);
    assert(vec_get(&v, 0, &out) == 0);
    ASSERT_MSG(out == vals[4], "expected %s, found %s", vals[4], out);
    assert(vec_get(&v, 4, &out) == 0);
    ASSERT_MSG(out == vals[0], "expected %s, found %s", vals[0], out);

    vec_destroy(&v);

    return 0;
}

int main()
{
    TEST_STATUS(test_vec_new());
    TEST_STATUS(test_vec_push());
    TEST_STATUS(test_vec_pop());
    TEST_STATUS(test_vec_realloc());
    TEST_STATUS(test_vec_extend());
    TEST_STATUS(test_vec_extend_with_array());
    TEST_STATUS(test_vec_insert());
    TEST_STATUS(test_vec_remove());
    TEST_STATUS(test_vec_get());
    TEST_STATUS(test_vec_reverse());
    TEST_STATUS(test_vec_swap());
    TEST_STATUS(test_vec_clear());

    TEST_STATUS(test_zero_capacity());
    TEST_STATUS(test_empty());
    TEST_STATUS(test_one_element());
    TEST_STATUS(test_max_capacity());

    TEST_STATUS(test_vec_ptr());
    TEST_STATUS(test_vec_char());

    return 0;
}