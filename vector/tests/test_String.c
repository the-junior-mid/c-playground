#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "String.h"

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

int test_string_new() {
    String s;
    char *str = "Hello, World!\n";
    assert(string_new(&s, str) == 0);
    ASSERT_MSG(string_len(&s) == get_str_len(str), "expected 14, found %lu", string_len(&s));

    string_destroy(&s);

    return 0;
}

int test_string_null() {
    String s;
    char *str = "";
    assert(string_new(&s, str) == 0);
    assert(string_len(&s) == get_str_len(str));

    string_destroy(&s);

    return 0;
}

int test_string_push_str() {
    String s;
    char *str = "";
    assert(string_new(&s, str) == 0);
    assert(string_len(&s) == 0);

    char *str_ext = "The quick brown fox jumps over the lazy dog!?\n";

    assert(string_push_str(&s, str_ext) == 0);
    assert(string_len(&s) == get_str_len(str_ext));

    string_destroy(&s);

    return 0;
}

int test_string_push() {
    String s;
    char *str = "The quick brown fox jumps over the lazy dog!?\n";
    assert(string_new(&s, str) == 0);
    assert(string_len(&s) == get_str_len(str));

    char str_ext = 'Y';

    assert(string_push(&s, str_ext) == 0);
    assert(string_len(&s) == get_str_len(str) + 1);

    char *new_str = "The quick brown fox jumps over the lazy dog!?\nY";
    for (size_t i = 0; i < string_len(&s); i++) {
        char *c_ptr = (char *)&s.v.ptr[i];
        if (c_ptr == NULL) return -1;
        ASSERT_MSG(*c_ptr == new_str[i], "expected %c at index %lu, found %c", new_str[i], i, *c_ptr);
    }

    string_destroy(&s);

    return 0;
}

int test_string_pop() {
    String s;
    char *str = "The quick brown fox jumps over the lazy dog!?\n";
    assert(string_new(&s, str) == 0);
    assert(string_len(&s) == get_str_len(str));

    size_t len = get_str_len(str);
    for (size_t i = 0; i < len; i++) {
        char out;
        assert(string_pop(&s, &out) == 0);
        assert(string_len(&s) == (len - 1) - i);
        char c = str[(len - 1) - i];
        ASSERT_MSG(out == c, "expected '%c' at index %lu, found '%c'", c, i, out);
    }
    ASSERT_MSG(str[len - 1] == '\n', "the original str was mutated");
    
    string_destroy(&s);

    return 0;
}

int test_string_remove() {
    String s;
    char *str = "The quick brown fox jumps over the lazy dog!?\n";
    assert(string_new(&s, str) == 0);
    assert(string_len(&s) == get_str_len(str));

    size_t len = get_str_len(str);
    for (size_t i = 0; i < len; i++) {
        char out;
        assert(string_remove(&s, 0, &out) == 0);
        assert(string_len(&s) == (len - 1) - i);
        char c = str[i];
        ASSERT_MSG(out == c, "expected '%c' at index %lu, found '%c'", c, i, out);
    }
    ASSERT_MSG(str[0] == 'T', "the original str was mutated");

    string_destroy(&s);

    return 0;
}

int test_string_insert() {
    String s;
    char *str = "The quick brown fox jumps over the lazy dog!?\n";
    assert(string_new(&s, str) == 0);
    assert(string_len(&s) == get_str_len(str));

    char c_a = 'A';
    char *str_a = "The quickA brown fox jumps over the lazy dog!?\n";
    assert(string_insert(&s, 9, &c_a) == 0);
    for (size_t i = 0; i < s.v.len; i++) {
        char c = *(char *)_vec_slot(&s.v, i);
        ASSERT_MSG(c == str_a[i], "expected '%c' at index %lu, found '%c'", str_a[i], i, c);
    }

    string_destroy(&s);

    return 0;
}

int main()
{
    TEST_STATUS(test_string_new());
    TEST_STATUS(test_string_null());
    TEST_STATUS(test_string_push_str());
    TEST_STATUS(test_string_push());
    TEST_STATUS(test_string_pop());
    TEST_STATUS(test_string_remove());
    TEST_STATUS(test_string_insert());

    return 0;
}