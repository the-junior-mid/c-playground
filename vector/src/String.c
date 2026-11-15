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
    printf("String: '%s'\n", (char *)s.v.ptr);
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

int test_string_push_char() {
    String s;
    char *str = "The quick brown fox jumps over the lazy dog!?\n";
    assert(string_new(&s, str) == 0);
    assert(string_len(&s) == get_str_len(str));

    char str_ext = 'Y';

    assert(string_push_char(&s, str_ext) == 0);
    assert(string_len(&s) == get_str_len(str) + 1);

    char *new_str = "The quick brown fox jumps over the lazy dog!?\nY";
    printf("String: %s\n", (char *)s.v.ptr);
    for (size_t i = 0; i < string_len(&s); i++) {
        char c = *(char *)&s.v.ptr[i];
        ASSERT_MSG(c == new_str[i], "expected %c at index %lu, found %c", new_str[i], i, c);
    }

    string_destroy(&s);

    return 0;
}

int main()
{
    TEST_STATUS(test_string_new());
    TEST_STATUS(test_string_null());
    TEST_STATUS(test_string_push_str());
    TEST_STATUS(test_string_push_char());

    return 0;
}