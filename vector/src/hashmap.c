#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "hashmap.h"

#define PHI 0x9E3779B97F4A7C15;


/// @brief Get the nearest power of two for @p cap.
/// @param cap The value to find the closest power of two.
/// @return `size_t`
size_t hm_get_power_of_two(size_t cap) {
    if (cap <= 0) {
        return 2;
    }
    // Get nearest power of 2.
    // 2^n = capacity -> n * ln(2) = ln(capacity) -> n = ln(capacity) / ln(2)
    return ceil(log((double)cap) / log((double)2));
}

size_t hm_hash(void *key, size_t capacity) {
    size_t x = (size_t)PHI * key;
    return (x >> (64 - capacity));
}

/// @brief Perform the hashing function on the given key.
/// @param[in] key Pointer to the key to be hashed.
/// @return `MID_OK` on success.
size_t hm_get_bin(hashmap *hm, size_t hash) {
    return hash & (hm->capacity - 1);
}

void * _hm_get_slot(hashmap *hm, size_t index) {
    return (unsigned char *)hm->ptr + (index * sizeof(hm_slot));
}

/// @brief Initialize a hashmap structure.
/// @param[in,out] hm The `hashmap` to initialize.
/// @return `MID_OK` on success.
void hm_new(hashmap *hm) {
    hm->ptr = NULL;
    hm->entries = 0;
    hm->capacity = 0;
}

/// @brief Initialize a `hashmap` with @p capacity.
/// @param[in,out] hm       The `hashmap` to modify. Must be initialized.
/// @param[in]     capacity The minimum capacity @p hm should be created with. Nearest power of two will be used.
/// @return `MID_OK` on success.
int hm_with_capacity(hashmap *hm, size_t capacity) {
    size_t cap = hm_get_power_of_two(capacity);
    void *tmp = calloc(cap, sizeof(hm_slot));
    if (tmp == NULL) return MID_ERR_NOMEM;
    hm->ptr = tmp;
    hm->entries = 0;
    hm->capacity = capacity;
}

/// @brief Double the current allocation size of @p hm.
/// @param[in, out] hm      The `hashmap` to modify. Must be initialized.
/// @return 
int hm_realloc(hashmap *hm) {
    size_t new_cap = hm->capacity == 0 ? 8 : hm->capacity * 2;
    void *tmp = realloc(hm->ptr, new_cap);
    if (tmp == NULL) return MID_ERR_NOMEM;
    hm->ptr = tmp;
    hm->capacity = new_cap;

    // TODO: rehash all the maps contents

    return MID_OK;
}

/// @brief Return the number of elements in the `hashmap`.
/// @param[in,out] hm The `hashmap` to modify. Must be initialized.
/// @return `size_t`
size_t hm_len(hashmap *hm) {
    return hm->entries;
}

/// @brief Return a boolean that dictates if the number of elements is zero.
/// @param[in,out] hm The `hashmap` to modify. Must be initialized.
/// @return `bool` 
bool hm_is_empty(hashmap *hm) {
    return hm->entries == 0;
}

/// @brief Get a pointer to an array of `hashmap`'s keys.
/// @param[in,out] hm  The `hashmap` to modify. Must be initialized.
/// @param[out]    out Pointer for the output to be stored in. Cannot be NULL. 
/// @return `MID_OK` on success.
int hm_keys(hashmap *hm, void *out);

/// @brief Get a pointer to an array of `hashmap`'s values.
/// @param[in,out] hm  The `hashmap` to modify. Must be initialized.
/// @param[out]    out Pointer for the output to be stored in. Cannot be NULL. 
/// @return `MID_OK` on success.
int hm_values(hashmap *hm, void *out);

/// @brief Clears out all key-value pairs, retaining any memory allocated for reuse.
/// @param[in,out] hm The `hashmap` to modify. Must be initialized.
/// @return `MID_OK` on success.
int hm_clear(hashmap *hm);

/// @brief Get a reference to the value associtated with @p key.
/// @param[in,out] hm  The `hashmap` to modify. Must be initialized.
/// @param[in]     key Pointer to the key to get the value of.
/// @return `void *` on success.
/// @return `NULL` otherwise.
void * hm_get(hashmap *hm, void *key) {
    if (hm_contains_key(hm, key)) {
        size_t hash = hm_hash(key, hm->capacity);
        return _hm_get_slot(hm, hm_get_bin(hm, hash));
    } else {
        return NULL;
    }
}
/// @brief Get a reference to the key and value associtated with @p key.
/// @param[in,out] hm  The `hashmap` to modify. Must be initialized.
/// @param[in]     key Pointer to the key to get the value of.
/// @return `MID_OK` on success.
int hm_get_key_value(hashmap *hm, void *key) {
    // TODO: lookup the key in the table and then get its corresponding value, then return a read-only pointer to both
    return MID_OK;
}

/// @brief Return a bool of whether the key is in the `hashmap`.
/// @param[in,out] hm  The `hashmap` to modify. Must be initialized.
/// @param[in]     key Pointer to the key to get the value of.
/// @return `bool`
bool hm_contains_key(hashmap *hm, void *key) {
    size_t hash = hm_hash(key, hm->capacity);
    for (size_t i = 0; i < hm->entries; i++) {
        hm_slot slot = *(hm_slot *)_hm_get_slot(hm, i);
        printf("calculated hash %lu == %lu\n", slot.hash, hash);
    }
}

/// @brief Insert a @p key and @p value pair into the `hashmap`.
/// @param[in,out] hm        The `hashmap` to modify. Must be initialized.
/// @param[in]     key       Pointer to the key.
/// @param[in]     value     Pointer to the value to store.
/// @param[out]    out_value Pointer to store a previous value. If NULL then it is ignored.
/// @return `MID_OK` on success.
int hm_insert(hashmap *hm, void *key, void *value, hm_type val_type, void *out_value) {
    if (hm->capacity == 0) {
        int ret = hm_realloc(hm);
        if (ret < 0) return ret;
    }
    if (hm->entries + 1 >= hm->capacity) {
        int ret = hm_realloc(hm);
        if (ret < 0) return ret;
    }
    size_t hash = hm_hash(hm, hm->capacity);
    size_t bin = hm_get_bin(hm, hash);
    hm_slot slot;
    slot.hash = hash;
    slot.val.type = val_type;
    switch (val_type)
    {
        case V_INT: memcpy(&slot.val.as.i, value, sizeof(int));
            break;
        case V_DOUBLE: memcpy(&slot.val.as.d, value, sizeof(double));
            break;
        case V_STR: memcpy(&slot.val.as.s, value, sizeof(char *));
            break;
        case V_PTR: memcpy(&slot.val.as.p, value, sizeof(void *));
            break;
        default: return MID_ERR_TYPE;
    }
    // If the key exists, then set the out_value
    if (hm_contains_key(hm, key) && out_value != NULL) {
        memcpy(out_value, hm_get(hm, key), sizeof(hm_value));
    }
    memcpy(hm->ptr + (bin * sizeof(hm_slot)), &slot, sizeof(hm_slot));
    hm->entries++;

    return MID_OK;
}

/// @brief Remove the @p key from the map and return its value in @p out.
/// @param[in,out]  hm  The `hashmap` to modify. Must be initialized.
/// @param[in]      key Pointer to the key to get the value of.
/// @param[out]     out Pointer where the value will be stored. Cannot be NULL.
/// @return `MID_OK` on success.
int hm_remove(hashmap *hm, void *key, void *out);

/// @brief Remove the @p key from the map and return its key and value in @p out_key and @p out_value, respectively.
/// @param[in,out]  hm  The `hashmap` to modify. Must be initialized.
/// @param[in]      key Pointer to the key to get the value of.
/// @param[out]     out Pointer where the value will be stored. Cannot be NULL.
/// @return `MID_OK` on success.
int hm_remove_entry(hashmap *hm, void *key, void *out_key, void *out_value);

/// @brief Destroy the @p hm by freeing its memory, setting underlying ptr to NULL, then setting entiry count and capacity to zero.
/// @param hm 
void hm_destroy(hashmap *hm) {
    if (hm->ptr == NULL) return;
    free(hm->ptr);
    hm->ptr = NULL;
    hm->capacity =0;
    hm->entries = 0;
}