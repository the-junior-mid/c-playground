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

#define SENTINEL_PTR 0xDEADBEEF

int test_hm_new() {
    hashmap hm;
    hm_new(&hm);
    ASSERT_MSG(hm.slots == NULL, "hashmap initialized slots with non-null value");

    return 0;
}

int test_hn_insert() {
    hashmap hm;
    hm_new(&hm);
    ASSERT_MSG(hm.slots == NULL, "hashmap initialized slots with non-null value");

    // Fill the map, but do not cause a reallocation
    for (size_t key = 0; key < 7; key++) {
        char *value = "abcde";
        void *out = (void *)SENTINEL_PTR;
        assert(hm_insert(&hm, &key, V_INT, &value, V_STR, &out) == 0);

        size_t hash = hm_hash((void *)&key, V_INT);
        size_t bin = hm_get_bin(&hm, hash);
        hm_slot slot = *_hm_get_slot(&hm, bin);
        ASSERT_MSG(slot.hash == hash, "expected %lu for bin %lu, found %lu", hash, bin, slot.hash);
        ASSERT_MSG(slot.key.as.i == key, "expected %d, found %d", key, slot.key.as.i);
        ASSERT_MSG(strcmp(slot.val.as.s, value) == 0, "expected '%s', found '%s'", value, slot.val.as.s);
        ASSERT_MSG(out == (void *)SENTINEL_PTR, "expected '%x', found %p", SENTINEL_PTR, out);
        printf("hm0[key %lu (%lu, %lu)] = %p\n", key, hash, bin, slot.val.as.s);
    }
    printf("\n");

    // force reallocation
    for (size_t key = 7; key < 14; key++) {
        char *value = "fghij";
        void *out = (void *)SENTINEL_PTR;
        assert(hm_insert(&hm, &key, V_INT, &value, V_STR, &out) == 0);

        // TODO: Find a better way to perform these checks since they do not account for collision corrections
        size_t hash = hm_hash((void *)&key, V_INT);
        size_t bin = hm_get_bin(&hm, hash);
        hm_slot slot = *_hm_get_slot(&hm, bin);
        ASSERT_MSG(slot.hash == hash, "expected %lu for bin %lu, found %lu", hash, bin, slot.hash);
        ASSERT_MSG(slot.key.as.i == key, "expected %d, found %d", key, slot.key.as.i);
        ASSERT_MSG(strcmp(slot.val.as.s, value) == 0, "expected '%s', found '%s'", value, slot.val.as.s);
        ASSERT_MSG(out == (void *)SENTINEL_PTR, "expected '%x', found %p", SENTINEL_PTR, out);
        printf("hm1[key %lu (%lu, %lu)] = %p\n", key, hash, bin, slot.val.as.s);
    }
    printf("\n");

    // overwrite existing keys
    for (size_t key = 0; key < 7; key++) {
        char *value = "abcde";
        void *out = (void *)SENTINEL_PTR;
        assert(hm_insert(&hm, &key, V_INT, &value, V_STR, &out) == 0);

        size_t hash = hm_hash((void *)&key, V_INT);
        size_t bin = hm_get_bin(&hm, hash);
        hm_slot slot = *_hm_get_slot(&hm, bin);
        ASSERT_MSG(slot.hash == hash, "expected %lu for bin %lu, found %lu", hash, bin, slot.hash);
        ASSERT_MSG(slot.key.as.i == key, "expected %d, found %d", key, slot.key.as.i);
        ASSERT_MSG(strcmp(slot.val.as.s, value) == 0, "expected '%s', found '%s'", value, slot.val.as.s);
        ASSERT_MSG(out == slot.val.as.s, "expected '%p', found %p", slot.val.as.s, out);
        printf("hm2[key %lu (%lu, %lu)] = %p\n", key, hash, bin, slot.val.as.s);
    }

    return 0;
}

int test_hm_contains_key() {
    hashmap hm;
    hm_new(&hm);
    ASSERT_MSG(hm.slots == NULL, "hashmap initialized slots with non-null value");

    int key = 10;
    char *value = "abcde";
    void *out;
    assert(hm_insert(&hm, &key, V_INT, &value, V_STR, &out) == 0);

    ASSERT_MSG(hm_contains_key(&hm, &key, V_INT) == true, "the key '%d' was not found in the hash map", key);

    return 0;
}

int main()
{
    TEST_STATUS(test_hm_new());
    TEST_STATUS(test_hn_insert());
    TEST_STATUS(test_hm_contains_key());

    return 0;
}