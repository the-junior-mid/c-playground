#include <stdio.h>
#include "bint.h"
#include "dynamic_array.h"


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

// The 1000th Fibonnaci number in bint form
const uint64_t fib_1000[11] = {
    0x0b594dc75cc0604bULL,
    0x4c8e6d5286805fc7ULL,
    0x0930b7e8803c312bULL,
    0x2e3510283c1d60b0ULL,
    0x28068131a3fab0c1ULL,
    0x73062f18ff62b99cULL,
    0xb697ad224d553983ULL,
    0x727a736a7e42ca93ULL,
    0xb0e83d0c699bad1aULL,
    0x5732bb116f2c33baULL,
    0x0021d8cb07b572c2ULL,
};

const uint64_t fib_94[2] = {
    0x11f38ad0840bf6bfULL,
    0x0000000000000001ULL,
};

int test_fibonnaci() {
    bint fib = {0};
    size_t fib_len = 11;
    for (size_t i = 0; i < fib_len; i++) {
        da_push(&fib, fib_1000[i]);
    }
    bint a = {0};
    bint b = {0};
    bint c = {0};

    da_push(&a, 0);
    da_push(&b, 1);

    for (size_t i = 0; i <  1000; i++) {
        printf("F(%lu) ", i);
        bint_print(&a);
        // c = a + b
        bint_copy(&c, a);
        bint_add(&c, b);

        // a = b
        bint_copy(&a, b);

        // b = c
        bint_copy(&b, c);
    }
    assert(a.len == fib.len);
    for (size_t i = 0; i < a.len; i++) {
        ASSERT_MSG(a.items[i] == fib.items[i], "expected %llu for item %lu, found %llu", fib.items[i], i, a.items[i]);
    }

    da_free(a);
    da_free(b);
    da_free(c);

    return MID_OK;
}

int test_bint_add_carry_out_of_top_limb() {
    bint a = {0};
    bint b = {0};
    bint expected = {0};

    da_push(&a, UINT64_MAX);
    assert(a.len == 1);
    da_push(&b, 1);
    assert(b.len == 1);

    da_push(&expected, 0);
    da_push(&expected, 1);

    bint_add(&a, b);

    ASSERT_MSG(a.len == 2, "result bint did not increase in length");
    ASSERT_MSG(bint_eq(&a, expected) == true, "result not expected");

    da_free(a);
    da_free(b);
    da_free(expected);

    return MID_OK;
}

int test_bint_add_carry_ripples_through_a() {
    bint a = {0};
    bint b = {0};
    bint expected = {0};

    for (size_t i = 0; i < 3; i++) {
        da_push(&a, UINT64_MAX);
    }
    assert(a.len == 3);
    da_push(&b, 1);
    assert(b.len == 1);
    da_push(&expected, 0);
    da_push(&expected, 0);
    da_push(&expected, 0);
    da_push(&expected, 1);

    bint_add(&a, b);

    ASSERT_MSG(a.len == 4, "result bint did not increase in length");
    assert(a.len == expected.len);
    for (size_t i = 0; i < a.len; i++) {
        ASSERT_MSG(a.items[i] == expected.items[i], "expected %llu for item %lu, found %llu", expected.items[i], i, a.items[i]);
    }

    da_free(a);
    da_free(b);
    da_free(expected);

    return MID_OK;
}

int test_bint_add_carry_ripples_through_b_tail() {
    bint a = {0};
    bint b = {0};
    bint expected = {0};

    da_push(&a, 1);
    assert(a.len == 1);
    for (size_t i = 0; i < 3; i++) {
        da_push(&b, UINT64_MAX);
    }
    assert(b.len == 3);
    da_push(&expected, 0);
    da_push(&expected, 0);
    da_push(&expected, 0);
    da_push(&expected, 1);

    bint_add(&a, b);

    ASSERT_MSG(a.len == 4, "result bint did not increase in length");
    assert(a.len == expected.len);
    for (size_t i = 0; i < a.len; i++) {
        ASSERT_MSG(a.items[i] == expected.items[i], "expected %llu for item %lu, found %llu", expected.items[i], i, a.items[i]);
    }

    da_free(a);
    da_free(b);
    da_free(expected);

    return MID_OK;
}

int test_bint_add_copies_b_tail_without_carry() {
    bint a = {0};
    bint b = {0};
    bint expected = {0};

    da_push(&a, 1);
    assert(a.len == 1);

    da_push(&b, 13);
    da_push(&b, 1);
    assert(b.len == 2);

    da_push(&expected, 14);
    da_push(&expected, 1);

    bint_add(&a, b);

    ASSERT_MSG(a.len == 2, "result bint did not increase in length");
    assert(a.len == expected.len);
    for (size_t i = 0; i < a.len; i++) {
        ASSERT_MSG(a.items[i] == expected.items[i], "expected %llu for item %lu, found %llu", expected.items[i], i, a.items[i]);
    }

    da_free(a);
    da_free(b);
    da_free(expected);

    return MID_OK;
}

int test_bint_add_max_plus_max() {
    bint a = {0};
    bint b = {0};
    bint expected = {0};

    da_push(&a, UINT64_MAX);
    assert(a.len == 1);
    da_push(&b, UINT64_MAX);
    assert(b.len == 1);

    da_push(&expected, 18446744073709551614LLU);
    da_push(&expected, 1);

    bint_add(&a, b);

    ASSERT_MSG(a.len == 2, "result bint did not increase in length");
    assert(a.len == expected.len);
    for (size_t i = 0; i < a.len; i++) {
        ASSERT_MSG(a.items[i] == expected.items[i], "expected %llu for item %lu, found %llu", expected.items[i], i, a.items[i]);
    }

    da_free(a);
    da_free(b);
    da_free(expected);

    return MID_OK;
}

int test_bint_add_carry_in_and_both_operands_max() {
    bint a = {0};
    bint b = {0};
    bint expected = {0};

    for (size_t i = 0; i < 2; i++) {
        da_push(&a, UINT64_MAX);
    }
    assert(a.len == 2);
    for (size_t i = 0; i < 2; i++) {
        da_push(&b, UINT64_MAX);
    }
    assert(b.len == 2);

    da_push(&expected, 18446744073709551614LLU);
    da_push(&expected, 18446744073709551615LLU);
    da_push(&expected, 1);

    bint_add(&a, b);

    ASSERT_MSG(a.len == 3, "result bint did not increase in length");
    assert(a.len == expected.len);
    for (size_t i = 0; i < a.len; i++) {
        ASSERT_MSG(a.items[i] == expected.items[i], "expected %llu for item %lu, found %llu", expected.items[i], i, a.items[i]);
    }

    da_free(a);
    da_free(b);
    da_free(expected);

    return MID_OK;
}

int test_bint_add_aliased_operands() {
    
}

int test_bint_add_matches_reference_value() {

}

int test_bint_add_zero_plus_zero() {
    bint a = {0};
    bint b = {0};

    da_push(&a, 0);
    assert(a.len == 1);
    da_push(&b, 0);
    assert(b.len == 1);

    bint_add(&a, b);

    ASSERT_MSG(a.len == 1, "result bint did not increase in length");
    ASSERT_MSG(a.items[0] == 0, "0 + 0 != 0");

    da_free(a);
    da_free(b);

    return MID_OK;
}

int test_bint_add_zero_is_identity() {
    bint a = {0};
    bint b = {0};

    // 0 + x
    da_push(&a, 0);
    assert(a.len == 1);
    da_push(&b, 1);
    assert(b.len == 1);

    bint_add(&a, b);

    ASSERT_MSG(a.len == 1, "result bint did not increase in length");
    ASSERT_MSG(a.items[0] == 1, "0 + 1 != 1");

    // x + 0
    bint c = {0};
    bint d = {0};

    da_push(&c, 1);
    assert(c.len == 1);
    da_push(&d, 0);
    assert(d.len == 1);

    bint_add(&c, d);

    ASSERT_MSG(c.len == 1, "result bint did not increase in length");
    ASSERT_MSG(c.items[0] == 1, "1 + 0 != 1");

    da_free(a);
    da_free(b);
    da_free(c);
    da_free(d);

    return MID_OK;
}

int test_bint_add_equal_length_no_carry() {
    bint a = {0};
    bint b = {0};
    bint expected = {0};

    da_push(&a, 35);
    assert(a.len == 1);
    da_push(&b, 345);
    assert(b.len == 1);

    da_push(&expected, 380);

    bint_add(&a, b);

    ASSERT_MSG(a.len == 1, "result bint increased in length");
    assert(a.len == expected.len);
    for (size_t i = 0; i < a.len; i++) {
        ASSERT_MSG(a.items[i] == expected.items[i], "expected %llu for item %lu, found %llu", expected.items[i], i, a.items[i]);
    }

    da_free(a);
    da_free(b);
    da_free(expected);

    return MID_OK;
}

int main(void) {
    TEST_STATUS(test_bint_add_carry_out_of_top_limb());
    TEST_STATUS(test_bint_add_carry_ripples_through_a());
    TEST_STATUS(test_bint_add_carry_ripples_through_b_tail());
    TEST_STATUS(test_bint_add_copies_b_tail_without_carry());
    TEST_STATUS(test_bint_add_max_plus_max());
    TEST_STATUS(test_bint_add_carry_in_and_both_operands_max());
    TEST_STATUS(test_bint_add_aliased_operands());
    TEST_STATUS(test_bint_add_zero_plus_zero());
    TEST_STATUS(test_bint_add_zero_is_identity());
    TEST_STATUS(test_bint_add_equal_length_no_carry());

    TEST_STATUS(test_fibonnaci());

    return 0;
}