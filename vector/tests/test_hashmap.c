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

char * gen_str_val(size_t size, size_t offset) {
    char *str = calloc(size, sizeof(char));
    if (!str) return NULL;
    for (size_t i = 0; i < size; i++) {
        str[i] = (char)('a' + (offset * 7 + i) % 26);
    }
    str[size] = '\0';
    return str;
}

typedef struct {
    size_t key;
    char *str;
} pair;

int test_hm_new() {
    hashmap hm;
    hm_new(&hm);
    ASSERT_MSG(hm.slots == NULL, "hashmap initialized slots with non-null value");

    hm_destroy(&hm);

    return 0;
}

int test_hn_insert() {
    // track all key-value pairs
    pair pairs[256] = {0};
    // create the key-value pairs that will be used
    for (size_t i = 0; i < 256; i++) {
        pairs[i].key = (int)i;
        pairs[i].str = gen_str_val(5, i);
    }

    size_t strs_len = 0;
    hashmap hm;
    hm_new(&hm);
    ASSERT_MSG(hm.slots == NULL, "hashmap initialized slots with non-null value");

    // Fill the map, but do not cause a reallocation
    for (size_t key = 0; key < 7; key++) {
        char *val = pairs[key].str;
        assert(hm_insert(&hm, &key, V_INT, &val, V_STR) == 0);
    }

    // check all key/value pairs exist
    for (size_t bin = 0; bin < hm.capacity; bin++) {
        hm_slot *slot = _hm_get_slot(&hm, bin);
        if (slot->hash == 0) continue;
        int key = slot->key.as.i;
        char *value = slot->val.as.s;
        ASSERT_MSG(value == pairs[key].str, "expected %p, found %p", pairs[bin].str, value);
    }

    // force reallocation
    // can't use the same check as before since reallocating can lead to collisions
    for (size_t key = 7; key < 14; key++) {
        char *val = pairs[key].str;
        assert(hm_insert(&hm, &key, V_INT, &val, V_STR) == 0);
    }

    // check all key/value pairs exist
    for (size_t bin = 0; bin < hm.capacity; bin++) {
        hm_slot *slot = _hm_get_slot(&hm, bin);
        if (slot->hash == 0) continue;
        int key = slot->key.as.i;
        char *value = slot->val.as.s;
        ASSERT_MSG(value == pairs[key].str, "expected %p, found %p", pairs[bin].str, value);
    }

    // try to overwrite existing keys, should just trigger an allocation and add more key-value pairs
    for (size_t key = 0; key < 7; key++) {
        char *val = pairs[key].str;
        assert(hm_insert(&hm, &key, V_INT, &val, V_STR) == 0);
    }

    // check all key/value pairs exist
    for (size_t bin = 0; bin < hm.capacity; bin++) {
        hm_slot *slot = _hm_get_slot(&hm, bin);
        if (slot->hash == 0) continue;
        int key = slot->key.as.i;
        char *value = slot->val.as.s;
        ASSERT_MSG(value == pairs[key].str, "expected %p, found %p", pairs[bin].str, value);
    }

    for (size_t i = 0; i < strs_len; i++) {
        free(pairs[i].str);
    }

    hm_destroy(&hm);

    return 0;
}

int test_hm_contains_key() {
    // track all key-value pairs
    pair pairs[256] = {0};
    // create the key-value pairs that will be used
    for (size_t i = 0; i < 256; i++) {
        pairs[i].key = (int)i;
        pairs[i].str = gen_str_val(5, i);
    }

    size_t strs_len = 0;
    hashmap hm;
    hm_new(&hm);
    ASSERT_MSG(hm.slots == NULL, "hashmap initialized slots with non-null value");

    // make sure no keys are found
    for (size_t key = 0; key < 8; key++) {
        ASSERT_MSG(hm_contains_key(&hm, &key, V_INT) == false, "the key '%lu' was found in an empty hash map", key);
    }

    // Fill the map
    for (size_t key = 0; key < 7; key++) {
        char *val = pairs[key].str;
        assert(hm_insert(&hm, &key, V_INT, &val, V_STR) == 0);
    }

    // all keys should be found
    for (size_t key = 0; key < 7; key++) {
        ASSERT_MSG(hm_contains_key(&hm, &key, V_INT) == true, "the key '%lu' was not found in the hash map", key);
    }

    // force a reallocation with new keys
    for (size_t key = 7; key < 14; key++) {
        char *val = pairs[key].str;
        assert(hm_insert(&hm, &key, V_INT, &val, V_STR) == 0);
    }

    // all keys should be found
    for (size_t key = 0; key < 14; key++) {
        ASSERT_MSG(hm_contains_key(&hm, &key, V_INT) == true, "the key '%lu' was not found in the hash map", key);
    }

    // clear the map
    hm_clear(&hm);

    // no values should be found
    for (size_t key = 0; key < 14; key++) {
        ASSERT_MSG(hm_contains_key(&hm, &key, V_INT) == false, "the key '%lu' was found in an empty hash map", key);
    }

    for (size_t i = 0; i < strs_len; i++) {
        free(pairs[i].str);
    }

    hm_destroy(&hm);

    return 0;
}

int main()
{
    TEST_STATUS(test_hm_new());
    TEST_STATUS(test_hn_insert());
    TEST_STATUS(test_hm_contains_key());

    return 0;
}