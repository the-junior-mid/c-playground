#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>
#include "hashmap.h"
#include "log.h"

#define FNV_1A_PRIME 0x100000001B3;
#define FNV_1A_OFFSET 0xCBF29CE484222325;


/// @brief Initialize an iterator struct. If the @p hm is reallocated, the iterator is invalid.
/// @param hm 
/// @return 
hm_iter hm_iter_new(const hashmap *hm) {
    hm_iter iter = { .hm = hm, .bin = 0 };
    return iter;
}

/// @brief Iterates over the entries in the `hashmap` for an initialized iterator.
/// @param[in] iter The `hm_iter` to iterate to the next `hm_slot`.
/// @param[out] out Pointer where the next valid `hm_slot` will be stored. Must not be NULL.
/// @return `true` a valid `hm_slot` found and @p out was set to it.
/// @return `false` no valid `hm_slot` found.
bool hm_next(const hm_iter *iter, hm_slot *out) {
    assert(out != NULL);

    if (iter->hm->entries == 0) return false;

    size_t bin = iter->bin;
    if (bin >= iter->hm->capacity) return false;

    // Find the next non-zero slot
    hm_slot *slot = iter->hm->slots + bin;
    while (slot->hash == 0 && bin < iter->hm->capacity) {
        bin++;
        slot = iter->hm->slots + bin;
    }
    if (bin >= iter->hm->capacity) return false;

    out = slot;

    return true;
}

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

/// @brief Use the FNV-1a hashing method on the given key.
/// @param[in] key      Pointer to the key to hash. Must not be NULL.      
/// @param[in] key_type The `hm_type` for @p key.
/// @return `size_t` If the key_type is unknown it is mapped to the default hash value of `FNV_1A_OFFSET`.
size_t hm_hash(void *key, hm_type key_type) {
    size_t h = FNV_1A_OFFSET;
    switch (key_type)
    {
        case V_INT:
            h ^= (size_t)*(int *)key;
            h *= FNV_1A_PRIME;
            break;
        case V_DOUBLE:
            h ^= (size_t)*(double *)key;
            h *= FNV_1A_PRIME;
            break;
        case V_STR:
            char *str = key;
            char c = str[0];
            size_t i = 1;
            while (c != 0)
            {
                h ^= (size_t)c;
                h *= FNV_1A_PRIME;
                c = str[i];
                i++;
            }
        case V_PTR:
            h ^= (size_t)(uintptr_t)key;
            h *= FNV_1A_PRIME;
            break;
        default:
            break;
    }

    return h;
}

/// @brief Perform the hashing function on the given key.
/// @param[in] key Pointer to the key to be hashed.
/// @return `MID_OK` on success.
size_t hm_get_bin(const hashmap *hm, size_t hash) {
    return hash & (hm->capacity - 1);
}

hm_slot * _hm_get_slot(const hashmap *hm, size_t bin) {
    return hm->slots + bin;
}

/// @brief Print the @p hm to `STDERR`.
/// @param hm 
void hm_print(hashmap *hm, size_t m) {
    fprintf(stderr, "hashmap (%lu/%lu) {\n", hm->entries, hm->capacity);
    size_t j = 0;
    for (int bin = 0; (size_t)bin < hm->capacity; bin++) {
        hm_slot *slot = _hm_get_slot(hm, bin);
        if (slot->hash == 0) continue;
        if (j % m == 0) {
            if (j == hm->entries - 1) {
                fprintf(stderr, "    %d: %s\n", slot->key.as.i, slot->val.as.s);
            } else {
                fprintf(stderr, "    %d: %s, ", slot->key.as.i, slot->val.as.s);
            }
        } else if (j % m <= (m - 1)) {
            if (j % m == (m - 1)) {
                fprintf(stderr, "%d: %s,\n", slot->key.as.i, slot->val.as.s);
            } else if (j == hm->entries - 1) {
                fprintf(stderr, "%d: %s\n", slot->key.as.i, slot->val.as.s);
            } else {
                fprintf(stderr, "%d: %s, ", slot->key.as.i, slot->val.as.s); 
            }
        } else {
            fprintf(stderr, "\n");
        }
        j++;
    }
    fprintf(stderr, "}\n");
}

/// @brief Try to find the next empty slot. 
/// @param hm 
/// @param slots 
/// @param bin 
int hm_next_empty_slot(const hashmap *hm, hm_slot *slots, size_t bin, size_t *out_bin) {
    if (out_bin == NULL) return MID_ERR_NULL;

    size_t i = 0;
    while (memcmp(slots + bin, &ZERO_HASH, sizeof(size_t)) != 0) {
        bin += 1;
        if (bin > hm->capacity - 1) bin % hm->capacity;
        i++;
        if (i > hm->capacity) {
            LOG(WARN, "%lu slot(s) full", hm->capacity);
            return MID_ERR_SLOTS;
        }
    }

    *out_bin = bin;

    return MID_OK;
}

/// @brief Try to find the matching slot. 
/// @param hm 
/// @param slots 
/// @param bin 
int hm_matching_slot(const hashmap *hm, size_t bin, size_t hash, size_t *out_bin) {
    if (out_bin == NULL) return MID_ERR_NULL;

    size_t i = 0;
    while (memcmp(hm->slots + bin, &hash, sizeof(size_t)) != 0) {
        bin += 1;
        if (bin > hm->capacity - 1) bin %= hm->capacity;
        i++;
        if (i > hm->capacity) {
            break;
        }
    }

    *out_bin = bin;

    return MID_OK;
}

/// @brief Initialize a hashmap structure.
/// @param[in,out] hm The `hashmap` to initialize.
/// @return `MID_OK` on success.
void hm_new(hashmap *hm) {
    hm->slots = NULL;
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
    hm->slots = tmp;
    hm->entries = 0;
    hm->capacity = capacity;

    return MID_OK;
}

/// @brief Double the current allocation size of @p hm. This triggers a re-mapping due to the size increase.
/// @param[in, out] hm The `hashmap` to modify. Must be initialized.
/// @return `MID_OK` on success.
/// @return `MID_ERR_NOMEM` reallocation failed.
int hm_realloc(hashmap *hm) {
    size_t new_cap = hm->capacity == 0 ? 8 : hm->capacity * 2;
    // realloc would free the in-use mempry before we can properly copy its contents
    hm_slot *tmp = calloc(new_cap, sizeof(hm_slot));
    if (tmp == NULL) return MID_ERR_NOMEM;

    // Iterate through the old memory and move any entries into the new memory
    // TODO: Check for collisions in the new memory. If a slot is taken, then move to the next slot. Keep looking for an empty slot
    // until one is found or the entire hash map has been checked.
    for (size_t i = 0; i < hm->capacity; i++) {
        hm_slot *slot = _hm_get_slot(hm, i);
        if ((slot == NULL) || (slot != NULL && slot->hash == 0)) continue;
        size_t bin = hm_get_bin(hm, slot->hash);
        size_t new_bin;
        int ret = hm_next_empty_slot(hm, tmp, bin, &new_bin);
        if (ret != 0) {
            return ret;
        };
        bin = new_bin;
        tmp[bin] = hm->slots[i];
    }

    free(hm->slots);

    hm->slots = tmp;
    hm->capacity = new_cap;

    return MID_OK;
}

/// @brief Return the number of elements in the `hashmap`.
/// @param[in,out] hm The `hashmap` to modify. Must be initialized.
/// @return `size_t`
size_t hm_len(const hashmap *hm) {
    return hm->entries;
}

/// @brief Return a boolean that dictates if the number of elements is zero.
/// @param[in,out] hm The `hashmap` to modify. Must be initialized.
/// @return `bool` 
bool hm_is_empty(const hashmap *hm) {
    return hm->entries == 0;
}

/// @brief Get a pointer to a NULL terminated array of the `hashmap`'s keys.
/// @param[in,out] hm  The `hashmap` to modify. Must be initialized.
/// @param[out]    out Pointer for the output to be stored in. Cannot be NULL. 
/// @return `MID_OK` on success.
int hm_keys(const hashmap *hm, hm_value *out) {

}

/// @brief Get a pointer to an array of `hashmap`'s values.
/// @param[in,out] hm  The `hashmap` to modify. Must be initialized.
/// @param[out]    out Pointer for the output to be stored in. Cannot be NULL. 
/// @return `MID_OK` on success.
int hm_values(const hashmap *hm, void *out);

/// @brief Clears out all key-value pairs, retaining any memory allocated for reuse.
/// @param[in,out] hm The `hashmap` to modify. Must be initialized.
void hm_clear(hashmap *hm) {
    if (hm->slots == NULL) return;
    memset(hm->slots, 0, hm->capacity * sizeof(hm_slot));
}

/// @brief Get a reference to the `hm_value` associtated with @p key.
/// @param[in,out] hm  The `hashmap` to modify. Must be initialized.
/// @param[in]     key Pointer to the key to get the value of.
/// @return `hm_value *` on success.
/// @return `NULL` otherwise.
int hm_get(const hashmap *hm, void *key, hm_type key_type, hm_value *out_value) {
    if (hm->slots == NULL) return MID_ERR_EMPTY;
    if (key == NULL) return MID_ERR_NULL;

    size_t hash = hm_hash(key, key_type);
    size_t bin = hm_get_bin(hm, hash);
    size_t new_bin;
    int ret = hm_matching_slot(hm, bin, hash, &new_bin);
    if (ret != 0) {
        return ret;
    };
    bin = new_bin;
    hm_slot *slot = _hm_get_slot(hm, bin);
    if (slot == NULL || slot->hash == 0) return MID_ERR_NULL;

    *out_value = slot->val;

    return MID_OK;
}

/// @brief Get a reference to the key and value associtated with @p key.
/// @param[in,out] hm  The `hashmap` to modify. Must be initialized.
/// @param[in]     key Pointer to the key to get the value of.
/// @return `MID_OK` on success.
int hm_get_key_value(const hashmap *hm, void *key, hm_type key_type, hm_value *out_key, hm_value *out_value) {
    if (hm->slots == NULL) return MID_ERR_EMPTY;
    if (key == NULL) return MID_ERR_NULL;

    size_t hash = hm_hash(key, key_type);
    size_t bin = hm_get_bin(hm, hash);
    size_t new_bin;
    int ret = hm_matching_slot(hm, bin, hash, &new_bin);
    if (ret != 0) {
        return ret;
    };
    bin = new_bin;
    hm_slot *slot = _hm_get_slot(hm, bin);
    if (slot == NULL || slot->hash == 0) return MID_ERR_EMPTY;

    *out_key = slot->key;
    *out_value = slot->val;

    return MID_OK;
}

/// @brief Return a bool of whether the key is in the `hashmap`.
/// @param[in,out] hm  The `hashmap` to modify. Must be initialized.
/// @param[in]     key Pointer to the key to get the value of.
/// @return `bool`
bool hm_contains_key(const hashmap *hm, void *key, hm_type key_type) {
    if (hm->slots == NULL) return false;
    if (key == NULL) return false;

    size_t hash = hm_hash(key, key_type);
    size_t bin = hm_get_bin(hm, hash);
    size_t new_bin;
    int ret = hm_matching_slot(hm, bin, hash, &new_bin);
    if (ret != 0) {
        return false;
    };
    bin = new_bin;
    hm_slot *slot = _hm_get_slot(hm, bin);
    if (slot == NULL || slot->hash == 0) return false;
    switch (slot->key.type)
    {
        case V_INT: return slot->key.as.i == *(int *)key;
        case V_DOUBLE: return slot->key.as.d == *(double *)key;
        case V_STR: return slot->key.as.s == (char *)key;
        case V_PTR: return slot->key.as.p == key;
        default: return false;
    }
}

/// @brief Insert a @p key and @p value pair into the `hashmap`.
/// @param[in,out] hm        The `hashmap` to modify. Must be initialized.
/// @param[in]     key       Pointer to the key.
/// @param[in]     value     Pointer to the value to store.
/// @param[out]    out_value Pointer to store a previous value. If NULL then it is ignored.
/// @return `MID_OK` on success.
int hm_insert(hashmap *hm, void *key, hm_type key_type, void *value, hm_type val_type) {
    if ((hm->capacity == 0) || (hm->entries + 1 >= hm->capacity)) {
        int ret = hm_realloc(hm);
        if (ret < 0) return ret;
    }
    size_t hash = hm_hash(key, key_type);
    size_t bin = hm_get_bin(hm, hash);
    size_t new_bin;
    int ret = hm_next_empty_slot(hm, hm->slots, bin, &new_bin);
    if (ret != 0) {
        return ret;
    };
    bin = new_bin;
    hm_slot slot;
    slot.hash = hash;
    slot.key.type = V_INT;
    switch (V_INT)
    {
        case V_INT: memcpy(&slot.key.as.i, key, sizeof(int));
            break;
        case V_DOUBLE: memcpy(&slot.key.as.d, key, sizeof(double));
            break;
        case V_STR: memcpy(&slot.key.as.s, key, sizeof(char *));
            break;
        case V_PTR: memcpy(&slot.key.as.p, key, sizeof(void *));
            break;
        default: return MID_ERR_TYPE;
    }
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

    memcpy(hm->slots + bin, &slot, sizeof(hm_slot));
    hm->entries++;

    return MID_OK;
}

/// @brief Insert a @p key and @p value pair into @p hm and overwrite any existing data.
/// @param[in,out] hm        The `hashmap` to modify. Must be initialized.
/// @param[in]     key       Pointer to the key.
/// @param[in]     value     Pointer to the value to store.
/// @param[out]    out_value Pointer to store a previous value. If NULL then it is ignored.
/// @return `MID_OK` on success.
/// @return `MID_ERR_TYPE` unknown `hm_type` provided.
/// @return `MID_ERR_NOMEM` reallocation failed.
int hm_update(hashmap *hm, void *key, hm_type key_type, void *value, hm_type val_type, hm_value *out_value) {
    if ((hm->capacity == 0) || (hm->entries + 1 >= hm->capacity)) {
        int ret = hm_realloc(hm);
        if (ret < 0) return ret;
    }
    size_t hash = hm_hash(key, key_type);
    size_t bin = hm_get_bin(hm, hash);
    size_t new_bin;
    int ret = hm_matching_slot(hm, bin, hash, &new_bin);
    if (ret != 0) {
        return ret;
    };
    bin = new_bin;
    hm_slot *slot = _hm_get_slot(hm, bin);
    if (out_value != NULL) *out_value = slot->val;
    slot->val.type = val_type;
    switch (val_type)
    {
        case V_INT: memcpy(&slot->val.as.i, value, sizeof(int));
            break;
        case V_DOUBLE: memcpy(&slot->val.as.d, value, sizeof(double));
            break;
        case V_STR: memcpy(&slot->val.as.s, value, sizeof(char *));
            break;
        case V_PTR: memcpy(&slot->val.as.p, value, sizeof(void *));
            break;
        default: return MID_ERR_TYPE;
    }

    memcpy(hm->slots + bin, slot, sizeof(hm_slot));

    return MID_OK;
}

/// @brief Remove the @p key from the map and return its value in @p out_value.
/// @param[in,out]  hm  The `hashmap` to modify. Must be initialized.
/// @param[in]      key Pointer to the key to get the valubine of.
/// @param[out]     out Pointer where the value will be stored. Cannot be NULL.
/// @return `MID_OK` on success.
int hm_remove(hashmap *hm, void *key, hm_type key_type, hm_value *out_value) {
    size_t hash = hm_hash(key, key_type);
    size_t bin = hm_get_bin(hm, hash);
    size_t new_bin;
    int ret = hm_matching_slot(hm, bin, hash, &new_bin);
    if (ret != 0) {
        return ret;
    };
    bin = new_bin;
    hm_slot *slot = _hm_get_slot(hm, bin);
    *out_value = slot->val;
    memset(slot, 0, sizeof(hm_slot));

    return MID_OK;
}

/// @brief Remove the @p key from the map and return its key and value in @p out_key and @p out_value, respectively.
/// @param[in,out]  hm  The `hashmap` to modify. Must be initialized.
/// @param[in]      key Pointer to the key to get the value of.
/// @param[out]     out Pointer where the value will be stored. Cannot be NULL.
/// @return `MID_OK` on success.
int hm_remove_entry(hashmap *hm, void *key, hm_type key_type, hm_value *out_key, hm_value *out_value) {
    size_t hash = hm_hash(key, key_type);
    size_t bin = hm_get_bin(hm, hash);
    size_t new_bin;
    int ret = hm_matching_slot(hm, bin, hash, &new_bin);
    if (ret != 0) {
        return ret;
    };
    bin = new_bin;
    hm_slot *slot = _hm_get_slot(hm, bin);
    *out_key = slot->key;
    *out_value = slot->val;
    memset(slot, 0, sizeof(hm_slot));

    return MID_OK;
}

/// @brief Destroy the @p hm by freeing its memory, setting underlying ptr to NULL, then setting entiry count and capacity to zero.
/// @param hm 
void hm_destroy(hashmap *hm) {
    if (hm->slots == NULL) return;
    free(hm->slots);
    hm->slots = NULL;
    hm->capacity = 0;
    hm->entries = 0;
}