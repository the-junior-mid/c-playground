#include <stdbool.h>
#include <math.h>
#include "hashmap.h"


/// @brief Get the nearest power of two for @p cap.
/// @param cap The value to find the closest power of two.
/// @return `size_t`
size_t hm_get_power_of_two(size_t cap) {
    // Get nearest power of 2.
    // 2^n = capacity -> n * ln(2) = ln(capacity) -> n = ln(capacity) / ln(2)
    return ceil(log((double)cap) / log((double)2));
}

/// @brief Perform the hashing function on the given key.
/// @param[in] key Pointer to the key to be hashed.
/// @return `MID_OK` on success.
size_t hm_get_bin(hashmap *hm, void *key) {
    size_t hash = (size_t)key;
    return hash & (hm->capacity - 1);
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
    size_t new_cap = hm->capacity * 2;
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
/// @return `MID_OK` on success.
int hm_get(hashmap *hm, void *key);

/// @brief Get a reference to the key and value associtated with @p key.
/// @param[in,out] hm  The `hashmap` to modify. Must be initialized.
/// @param[in]     key Pointer to the key to get the value of.
/// @return `MID_OK` on success.
int hm_get_key_value(hashmap *hm, void *key);

/// @brief Return a bool of whether the key is in the `hashmap`.
/// @param[in,out] hm  The `hashmap` to modify. Must be initialized.
/// @param[in]     key Pointer to the key to get the value of.
/// @return `bool`
bool hm_contains_key(hashmap *hm, void *key);

/// @brief Insert a @p key and @p value pair into the `hashmap`.
/// @param[in,out] hm        The `hashmap` to modify. Must be initialized.
/// @param[in]     key       Pointer to the key.
/// @param[in]     value     Pointer to the value to store.
/// @param[out]    out_value Pointer to store a previous value. If NULL then it is ignored.
/// @return `MID_OK` on success.
int hm_insert(hashmap *hm, void *key, void *value, hm_type val_type,  void *out_value) {
    // TODO: Determine how to know if capacity is being reached since the number of entries is not 1 to 1 with the capacity.
    if (hm->entries + 1 >= hm->capacity) {
        int ret = hm_realloc(hm);
        if (ret < 0) return ret;
    }
    size_t hash = hm_hash(hm, key);
    hm_slot slot;
    slot.hash = hash;
    slot.val.type = val_type;
    switch (val_type)
    {
        case V_INT: slot.val.as.i = *(int *)value;
        case V_DOUBLE: slot.val.as.d = *(double *)value;
        case V_STR: slot.val.as.s = *(char *)value;
        case V_PTR: slot.val.as.p = value;
    }
    memcpy(hm->ptr, &slot, sizeof(hm_value));
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