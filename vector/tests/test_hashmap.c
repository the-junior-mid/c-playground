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
#define MAX_ENTRIES 256

char * gen_str_val(size_t size, size_t offset) {
    char *str = calloc(size + 1, sizeof(char));
    if (!str) return NULL;
    for (size_t i = 0; i < size; i++) {
        str[i] = (char)('a' + (offset * 7 + i) % 26);
    }
    str[size] = '\0';
    return str;
}

size_t get_offset_from_str(char *str) {
    return (15 * (str[0] - 'a') % 26 + 26) % 26;
}

typedef struct {
    size_t key;
    char *str;
} pair;

int test_hm_new() {
    hashmap hm;
    hm_new(&hm);
    ASSERT_MSG(hm.slots == NULL, "hashmap initialized slots with non-null value");
    ASSERT_MSG(hm.entries == 0, "hashmap initialized with non-zero entry count");
    ASSERT_MSG(hm.capacity == 0, "hashmap initialized with non-zero capacity");

    hm_destroy(&hm);

    return 0;
}

int test_hm_insert() {
    // track all key-value pairs
    pair pairs[MAX_ENTRIES] = {0};
    // create the key-value pairs that will be used
    for (size_t i = 0; i < MAX_ENTRIES; i++) {
        pairs[i].key = (int)i;
        pairs[i].str = gen_str_val(5, i);
    }

    hashmap hm;
    hm_new(&hm);
    ASSERT_MSG(hm.slots == NULL, "hashmap initialized slots with non-null value");

    // Fill the map, but do not cause a reallocation
    for (size_t key = 0; key < 7; key++) {
        char *val = pairs[key].str;
        assert(hm_insert(&hm, &key, V_INT, val, V_STR) == 0);
    }

    // check all key/value pairs exist
    for (size_t bin = 0; bin < hm.capacity; bin++) {
        hm_slot *slot = &hm.slots[bin];
        if (slot->key.as.i == 0) continue;
        int key = slot->key.as.i;
        char *value = slot->val.as.s;
        ASSERT_MSG(value == pairs[key].str, "expected %p, found %p", pairs[bin].str, value);
    }

    // force reallocation
    // can't use the same check as before since reallocating can lead to collisions
    for (size_t key = 7; key < 14; key++) {
        char *val = pairs[key].str;
        assert(hm_insert(&hm, &key, V_INT, val, V_STR) == 0);
    }

    // check all key/value pairs exist
    for (size_t bin = 0; bin < hm.capacity; bin++) {
        hm_slot *slot = &hm.slots[bin];
        if (slot->key.as.i == 0) continue;
        int key = slot->key.as.i;
        char *value = slot->val.as.s;
        ASSERT_MSG(value == pairs[key].str, "expected %p, found %p", pairs[bin].str, value);
    }

    // try to overwrite existing keys, should just trigger an allocation and add more key-value pairs
    for (size_t key = 0; key < 7; key++) {
        char *val = pairs[key].str;
        assert(hm_insert(&hm, &key, V_INT, val, V_STR) == 0);
    }

    // check all key/value pairs exist
    for (size_t bin = 0; bin < hm.capacity; bin++) {
        hm_slot *slot = &hm.slots[bin];
        if (slot->key.as.i == 0) continue;
        int key = slot->key.as.i;
        char *value = slot->val.as.s;
        ASSERT_MSG(value == pairs[key].str, "expected %p, found %p", pairs[key].str, value);
    }

    for (size_t i = 0; i < MAX_ENTRIES; i++) {
        free(pairs[i].str);
    }

    hm_destroy(&hm);

    return 0;
}

int test_hm_update() {
    // track all key-value pairs
    pair pairs0[MAX_ENTRIES] = {0};
    // create the key-value pairs that will be used
    for (size_t i = 0; i < MAX_ENTRIES; i++) {
        pairs0[i].key = (int)i;
        pairs0[i].str = gen_str_val(4, i);
    }

    // track all key-value pairs
    pair pairs1[MAX_ENTRIES] = {0};
    // create the key-value pairs that will be used
    for (size_t i = 0; i < MAX_ENTRIES; i++) {
        pairs1[i].key = (int)i;
        pairs1[i].str = gen_str_val(6, i);
    }

    hashmap hm;
    hm_new(&hm);
    ASSERT_MSG(hm.slots == NULL, "hashmap initialized slots with non-null value");

    // Fill the map, but do not cause a reallocation
    for (size_t key = 0; key < 7; key++) {
        char *val = pairs0[key].str;
        assert(hm_insert(&hm, &key, V_INT, val, V_STR) == 0);
    }

    // check all key/value pairs exist
    for (size_t bin = 0; bin < hm.capacity; bin++) {
        hm_slot *slot = &hm.slots[bin];
        if (slot->key.as.i == 0) continue;
        int key = slot->key.as.i;
        char *value = slot->val.as.s;
        ASSERT_MSG(value == pairs0[key].str, "expected %p, found %p", pairs0[key].str, value);
    }

    // update all entries
    for (size_t key = 0; key < 7; key++) {
        char *val = pairs1[key].str;
        hm_value value;
        assert(hm_update(&hm, &key, V_INT, val, V_STR, &value) == 0);
        ASSERT_MSG(value.as.s == pairs0[key].str, "expected %p, found %p", pairs0[key].str, value.as.s);
    }

    // check all key/value pairs exist
    for (size_t bin = 0; bin < hm.capacity; bin++) {
        hm_slot *slot = &hm.slots[bin];
        if (slot->key.type == 0) continue;
        int key = slot->key.as.i;
        char *value = slot->val.as.s;
        ASSERT_MSG(value == pairs1[key].str, "expected '%s' for key %d, found '%s'", pairs1[key].str, key, value);
    }

    // force reallocation
    // can't use the same check as before since reallocating can lead to collisions
    for (size_t key = 7; key < 14; key++) {
        char *val = pairs1[key].str;
        assert(hm_insert(&hm, &key, V_INT, val, V_STR) == 0);
    }

    // check all key/value pairs exist
    for (size_t bin = 0; bin < hm.capacity; bin++) {
        hm_slot *slot = &hm.slots[bin];
        if (slot->key.as.i == 0) continue;
        int key = slot->key.as.i;
        char *value = slot->val.as.s;
        ASSERT_MSG(value == pairs1[key].str, "expected %p, found %p", pairs1[key].str, value);
    }

    for (size_t key = 7; key < 14; key++) {
        char *val = pairs0[key].str;
        hm_value value;
        assert(hm_update(&hm, &key, V_INT, val, V_STR, &value) == 0);
        ASSERT_MSG(value.as.s == pairs1[key].str, "expected %p, found %p", pairs1[key].str, value.as.s);
    }

    for (size_t i = 0; i < MAX_ENTRIES; i++) {
        free(pairs0[i].str);
    }

    for (size_t i = 0; i < MAX_ENTRIES; i++) {
        free(pairs1[i].str);
    }

    hm_destroy(&hm);

    return 0;
}

int test_hm_contains_key() {
    // track all key-value pairs
    pair pairs[MAX_ENTRIES] = {0};
    // create the key-value pairs that will be used
    for (size_t i = 0; i < MAX_ENTRIES; i++) {
        pairs[i].key = (int)i;
        pairs[i].str = gen_str_val(5, i);
    }

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

    for (size_t i = 0; i < MAX_ENTRIES; i++) {
        free(pairs[i].str);
    }

    hm_destroy(&hm);

    return 0;
}

int test_hm_get() {
    // track all key-value pairs
    pair pairs[MAX_ENTRIES] = {0};
    // create the key-value pairs that will be used
    for (size_t i = 0; i < MAX_ENTRIES; i++) {
        pairs[i].key = (int)i;
        pairs[i].str = gen_str_val(5, i);
    }

    hashmap hm;
    hm_new(&hm);
    ASSERT_MSG(hm.slots == NULL, "hashmap initialized slots with non-null value");

    // make sure no keys are found
    for (size_t key = 0; key < 8; key++) {
        hm_value val;
        assert(hm_get(&hm, &key, V_INT, &val) == MID_ERR_EMPTY);
    }

    // Fill the map
    for (size_t key = 0; key < 7; key++) {
        char *val = pairs[key].str;
        assert(hm_insert(&hm, &key, V_INT, val, V_STR) == 0);
    }

    // all values should be found
    for (size_t key = 0; key < 7; key++) {
        hm_value val;
        assert(hm_get(&hm, &key, V_INT, &val) == 0);
        ASSERT_MSG(val.as.s == pairs[key].str, "expected value '%s' for key '%lu', found '%s'", pairs[key].str, key, val.as.s);
    }

    // force a reallocation with new keys
    for (size_t key = 7; key < 14; key++) {
        char *val = pairs[key].str;
        assert(hm_insert(&hm, &key, V_INT, val, V_STR) == 0);
    }

    // all keys should be found
    for (size_t key = 0; key < 14; key++) {
        hm_value val;
        assert(hm_get(&hm, &key, V_INT, &val) == 0);
        ASSERT_MSG(val.as.s == pairs[key].str, "expected value '%s' for key '%lu', found '%s'", pairs[key].str, key, val.as.s);
    }

    // clear the map
    hm_clear(&hm);

    // no values should be found
    for (size_t key = 0; key < 14; key++) {
        hm_value val;
        assert(hm_get(&hm, &key, V_INT, &val) == MID_ERR_NOT_FOUND);
    }

    for (size_t i = 0; i < MAX_ENTRIES; i++) {
        free(pairs[i].str);
    }

    hm_destroy(&hm);

    return 0;
}

int test_hm_get_key_value() {
    // track all key-value pairs
    pair pairs[MAX_ENTRIES] = {0};
    // create the key-value pairs that will be used
    for (size_t i = 0; i < MAX_ENTRIES; i++) {
        pairs[i].key = (int)i;
        pairs[i].str = gen_str_val(5, i);
    }

    hashmap hm;
    hm_new(&hm);
    ASSERT_MSG(hm.slots == NULL, "hashmap initialized slots with non-null value");

    // make sure no keys are found
    for (size_t key = 0; key < 8; key++) {
        hm_value out_key;
        hm_value out_val;
        assert(hm_get_key_value(&hm, &key, V_INT, &out_key, &out_val) == MID_ERR_EMPTY);
    }

    // Fill the map
    for (size_t key = 0; key < 7; key++) {
        char *val = pairs[key].str;
        assert(hm_insert(&hm, &key, V_INT, val, V_STR) == 0);
    }

    // all values should be found
    for (size_t key = 0; key < 7; key++) {
        hm_value out_key;
        hm_value out_val;
        assert(hm_get_key_value(&hm, &key, V_INT, &out_key, &out_val) == 0);
        ASSERT_MSG((size_t)out_key.as.i == key, "expected %lu for key, found %d", key, out_key.as.i);
        ASSERT_MSG(out_val.as.s == pairs[key].str, "expected value '%s' for key '%lu', found '%s'", pairs[key].str, key, out_val.as.s);
    }

    // force a reallocation with new keys
    for (size_t key = 7; key < 14; key++) {
        char *val = pairs[key].str;
        assert(hm_insert(&hm, &key, V_INT, val, V_STR) == 0);
    }

    // all keys should be found
    for (size_t key = 0; key < 14; key++) {
        hm_value out_key;
        hm_value out_val;
        assert(hm_get_key_value(&hm, &key, V_INT, &out_key, &out_val) == 0);
        ASSERT_MSG((size_t)out_key.as.i == key, "expected %lu for key, found %d", key, out_key.as.i);
        ASSERT_MSG(out_val.as.s == pairs[key].str, "expected value '%s' for key '%lu', found '%s'", pairs[key].str, key, out_val.as.s);
    }

    // clear the map
    hm_clear(&hm);

    // no values should be found
    for (size_t key = 0; key < 7; key++) {
        hm_value out_key;
        hm_value out_val;
        assert(hm_get_key_value(&hm, &key, V_INT, &out_key, &out_val) == MID_ERR_NOT_FOUND);
    }

    for (size_t i = 0; i < MAX_ENTRIES; i++) {
        free(pairs[i].str);
    }

    hm_destroy(&hm);

    return 0;
}

int test_hm_remove() {
    // track all key-value pairs
    pair pairs[MAX_ENTRIES] = {0};
    // create the key-value pairs that will be used
    for (size_t i = 0; i < MAX_ENTRIES; i++) {
        pairs[i].key = (int)i;
        pairs[i].str = gen_str_val(5, i);
    }

    hashmap hm;
    hm_new(&hm);
    ASSERT_MSG(hm.slots == NULL, "hashmap initialized slots with non-null value");

    // Fill the map, but do not cause a reallocation
    for (size_t key = 0; key < 7; key++) {
        char *val = pairs[key].str;
        assert(hm_insert(&hm, &key, V_INT, val, V_STR) == 0);
    }

    // check all key/value pairs exist
    for (size_t bin = 0; bin < hm.capacity; bin++) {
        hm_slot *slot = &hm.slots[bin];
        if (slot->key.as.i == 0) continue;
        int key = slot->key.as.i;
        char *value = slot->val.as.s;
        ASSERT_MSG(value == pairs[key].str, "expected %p, found %p", pairs[bin].str, value);
    }

    // remove all entries
    for (size_t key = 0; key < 7; key++) {
        hm_value out_value;
        assert(hm_remove(&hm, &key, V_INT, &out_value) == 0);
        ASSERT_MSG(out_value.as.s == pairs[key].str, "expected '%s' for key %lu, found '%s'", pairs[key].str, key, out_value.as.s);
    }

    // check all key/value pairs no longer exist
    for (size_t bin = 0; bin < hm.capacity; bin++) {
        hm_slot *slot = &hm.slots[bin];
        ASSERT_MSG(slot->key.as.i == 0, "expected 0, found %d", slot->key.as.i);
    }

    for (size_t i = 0; i < MAX_ENTRIES; i++) {
        free(pairs[i].str);
    }

    hm_destroy(&hm);

    return 0;
}

int test_hm_remove_entry() {
    // track all key-value pairs
    pair pairs[MAX_ENTRIES] = {0};
    // create the key-value pairs that will be used
    for (size_t i = 0; i < MAX_ENTRIES; i++) {
        pairs[i].key = (int)i;
        pairs[i].str = gen_str_val(5, i);
    }

    hashmap hm;
    hm_new(&hm);
    ASSERT_MSG(hm.slots == NULL, "hashmap initialized slots with non-null value");

    // Fill the map, but do not cause a reallocation
    for (size_t key = 0; key < 7; key++) {
        char *val = pairs[key].str;
        assert(hm_insert(&hm, &key, V_INT, val, V_STR) == 0);
    }

    // check all key/value pairs exist
    for (size_t bin = 0; bin < hm.capacity; bin++) {
        hm_slot *slot = &hm.slots[bin];
        if (slot->key.as.i == 0) continue;
        int key = slot->key.as.i;
        char *value = slot->val.as.s;
        ASSERT_MSG(value == pairs[key].str, "expected %p, found %p", pairs[bin].str, value);
    }

    // remove all entries
    for (size_t key = 0; key < 7; key++) {
        hm_value out_key;
        hm_value out_value;
        assert(hm_remove_entry(&hm, &key, V_INT, &out_key, &out_value) == 0);
        ASSERT_MSG((size_t)out_key.as.i == key, "expected %lu for key, found %d", key, out_key.as.i);
        ASSERT_MSG(out_value.as.s == pairs[key].str, "expected '%s' for key %lu, found '%s'", pairs[key].str, key, out_value.as.s);
    }

    // check all key/value pairs no longer exist
    for (size_t bin = 0; bin < hm.capacity; bin++) {
        hm_slot *slot = &hm.slots[bin];
        ASSERT_MSG(slot->key.as.i == 0, "expected 0, found %lu", slot->key.as.i);
    }

    for (size_t i = 0; i < MAX_ENTRIES; i++) {
        free(pairs[i].str);
    }

    hm_destroy(&hm);

    return 0;
}

int test_str_keys() {
    typedef struct {
        char *key;
        void *val;
    } pair1;

    // track all key-value pairs
    pair1 pairs[MAX_ENTRIES] = {0};
    // create the key-value pairs that will be used
    for (size_t i = 0; i < MAX_ENTRIES; i++) {
        pairs[i].key = gen_str_val(5, i);
        pairs[i].val = (void *)(SENTINEL_PTR + i);
    }

    hashmap hm;
    hm_new(&hm);
    ASSERT_MSG(hm.slots == NULL, "hashmap initialized slots with non-null value");

    // Fill the map, but do not cause a reallocation
    for (size_t key = 0; key < 7; key++) {
        char *k = pairs[key].key;
        void *val = pairs[key].val;
        assert(hm_insert(&hm, k, V_STR, val, V_PTR) == 0);
    }

    for (size_t bin = 0; bin < hm.capacity; bin++) {
        hm_slot *slot = &hm.slots[bin];
        if (slot->key.type == 0) continue;
        char *key = slot->key.as.s;
        void *value = slot->val.as.p;
        size_t k = get_offset_from_str(key);
        ASSERT_MSG(value == pairs[k].val, "expected %p for key '%s', found %p", pairs[k].val, key, value);
    }

    for (size_t i = 0; i < MAX_ENTRIES; i++) {
        free(pairs[i].key);
    }

    hm_destroy(&hm);

    return 0;
}

int test_basics() {
    hashmap hm;
    hm_new(&hm);
    ASSERT_MSG(hm.slots == NULL, "hashmap initialized slots with non-null value");

    hm_value out;
    ASSERT_MSG(hm_get(&hm, 0, V_INT, &out) == MID_ERR_EMPTY, "empty hashmap produced invalid key");
    ASSERT_MSG(hm_get(&hm, 0, V_DOUBLE, &out) == MID_ERR_EMPTY, "empty hashmap produced invalid key");
    ASSERT_MSG(hm_get(&hm, "test-key", V_STR, &out) == MID_ERR_EMPTY, "empty hashmap produced invalid key");
    ASSERT_MSG(hm_get(&hm, (void *)SENTINEL_PTR, V_PTR, &out) == MID_ERR_EMPTY, "empty hashmap produced invalid key");

    int key1 = 45;
    assert(hm_insert(&hm, &key1, V_INT, "hello, world!", V_STR) == 0);
    ASSERT_MSG(hm.entries == 1, "expected 1 entry count, found %lu", &hm.entries);
    assert(hm_get(&hm, &key1, V_INT, &out) == 0);
    ASSERT_MSG(strcmp(out.as.s, "hello, world!") == 0, "retreived invalid hm_value");

    int key2 = 77;
    assert(hm_insert(&hm, &key2, V_INT, "test-asdf", V_STR) == 0);
    ASSERT_MSG(hm.entries == 2, "expected 1 entry count, found %lu", &hm.entries);
    assert(hm_get(&hm, &key2, V_INT, &out) == 0);
    ASSERT_MSG(strcmp(out.as.s, "test-asdf") == 0, "retreived invalid hm_value");

    assert(hm_get(&hm, &key1, V_INT, &out) == 0);
    ASSERT_MSG(strcmp(out.as.s, "hello, world!") == 0, "retreived invalid hm_value");

    void *out1 = (void *)SENTINEL_PTR;
    int bad_key = 777;
    assert(hm_get(&hm, &bad_key, V_INT, out1) == MID_ERR_NOT_FOUND);
    ASSERT_MSG(out1 == SENTINEL_PTR, "retreived invalid hm_value");

    hm_destroy(&hm);

    return 0;
}

int main()
{
    TEST_STATUS(test_hm_new());
    TEST_STATUS(test_hm_insert());
    TEST_STATUS(test_hm_update());
    TEST_STATUS(test_hm_get());
    TEST_STATUS(test_hm_get_key_value());
    TEST_STATUS(test_hm_contains_key());
    TEST_STATUS(test_hm_remove());
    TEST_STATUS(test_hm_remove_entry());

    TEST_STATUS(test_str_keys());
    TEST_STATUS(test_basics());

    return 0;
}