#include <stdio.h>
#include <assert.h>
#include "hashmap.h"


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

int test_hm_new() {
    hashmap hm;
    hm_new(&hm);
    ASSERT_MSG(hm.ptr == NULL, "hashmap initialized ptr with non-null value");

    return 0;
}

int test_hn_insert() {
    hashmap hm;
    hm_new(&hm);
    ASSERT_MSG(hm.ptr == NULL, "hashmap initialized ptr with non-null value");

    int key = 10;
    char *value = "abcde";
    void *out;
    assert(hm_insert(&hm, &key, &value, V_STR, &out) == 0);

    hm_slot slot = *(hm_slot *)_hm_get_slot(&hm, 0);
    size_t hash = hm_hash((void *)&key, hm.capacity);
    ASSERT_MSG(slot.hash == hash, "expected %lu, found %lu", hash, slot.hash);
    ASSERT_MSG(slot.key.as.i == key, "expected %d, found %lu", key, slot.key.as.i);
    // ASSERT_MSG(strcmp(slot.val.as.s, value) == 0, "expected '%s', found '%s'", value, slot.val.as.s);

    return 0;
}

int test_hm_contains_key() {
    hashmap hm;
    hm_new(&hm);
    ASSERT_MSG(hm.ptr == NULL, "hashmap initialized ptr with non-null value");

    int key = 10;
    char *value = "abcde";
    void *out;
    assert(hm_insert(&hm, &key, &value, V_STR, &out) == 0);

    ASSERT_MSG(hm_contains_key(&hm, &key) == true, "the key '%d' was not found in the hash map", key);

    return 0;
}

int main()
{
    TEST_STATUS(test_hm_new());
    TEST_STATUS(test_hn_insert());
    TEST_STATUS(test_hm_contains_key());

    return 0;
}