#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "status.h"


#ifndef MID_HASHMAP_H
#define MID_HASHMAP_H

static const unsigned char ZERO_HASH[8]; 


typedef enum {V_INT=1, V_DOUBLE, V_STR, V_PTR} hm_type;

typedef struct {
    hm_type type;
    union {
        int i;
        double d;
        char *s;
        void *p;
    } as;
} hm_value;

typedef struct {
    size_t hash;
    hm_value key;
    hm_value val;
} hm_slot;

/// @brief A simple hash map. No memory is allocated until a key-value pair is inserted.
typedef struct {
    hm_slot *slots;
    size_t entries;
    size_t capacity;
} hashmap;

/// @brief Get the nearest power of two for @p cap.
/// @param cap The value to find the closest power of two.
/// @return `size_t`
size_t hm_get_power_of_two(size_t cap);

hm_slot * _hm_get_slot(const hashmap *hm, size_t index);
size_t hm_hash(void *key, hm_type key_type);

/// @brief Perform the hashing function on the given key.
/// @param[in] key Pointer to the key to be hashed.
/// @return `MID_OK` on success.
size_t hm_get_bin(const hashmap *hm, size_t hash);

/// @brief Initialize a `hashmap` structure.
/// @param[in,out] hm The `hashmap` to modify. Must be initialized.
/// @return `MID_OK` on success.
void hm_new(hashmap *hm);

/// @brief Initialize a `hashmap` with @p capacity.
/// @param[in,out] hm       The `hashmap` to modify. Must be initialized.
/// @param[in]     capacity The minimum capacity @p hm should be created with. Nearest power of two will be used.
/// @param[in]     size     The size of the type
/// @return `MID_OK` on success.
int hm_with_capacity(hashmap *hm, size_t capacity);

/// @brief Return the number of elements in the `hashmap`.
/// @param[in,out] hm The `hashmap` to modify. Must be initialized.
/// @return `size_t`
size_t hm_len(const hashmap *hm);

/// @brief Return a boolean that dictates if the number of elements is zero.
/// @param[in,out] hm The `hashmap` to modify. Must be initialized.
/// @return `bool` 
bool hm_is_empty(const hashmap *hm);

/// @brief Get a pointer to an array of `hashmap`'s keys.
/// @param[in,out] hm  The `hashmap` to modify. Must be initialized.
/// @param[out]    out Pointer for the output to be stored in. Cannot be NULL. 
/// @return `MID_OK` on success.
int hm_keys(const hashmap *hm, void *out);

/// @brief Get a pointer to an array of `hashmap`'s values.
/// @param[in,out] hm  The `hashmap` to modify. Must be initialized.
/// @param[out]    out Pointer for the output to be stored in. Cannot be NULL. 
/// @return `MID_OK` on success.
int hm_values(const hashmap *hm, void *out);

/// @brief Clears out all key-value pairs, retaining any memory allocated for reuse.
/// @param[in,out] hm The `hashmap` to modify. Must be initialized.
/// @return `MID_OK` on success.
int hm_clear(hashmap *hm);

/// @brief Get a reference to the value associtated with @p key.
/// @param[in,out] hm  The `hashmap` to modify. Must be initialized.
/// @param[in]     key Pointer to the key to get the value of.
/// @return `void *` on success.
/// @return `NULL` otherwise.
void * hm_get(const hashmap *hm, void *key, hm_type key_type);

/// @brief Get a reference to the key and value associtated with @p key.
/// @param[in,out] hm  The `hashmap` to modify. Must be initialized.
/// @param[in]     key Pointer to the key to get the value of.
/// @return `MID_OK` on success.
int hm_get_key_value(const hashmap *hm, void *key);

/// @brief Return a bool of whether the key is in the `hashmap`.
/// @param[in,out] hm  The `hashmap` to modify. Must be initialized.
/// @param[in]     key Pointer to the key to get the value of.
/// @return `bool`
bool hm_contains_key(const hashmap *hm, void *key, hm_type key_type);

/// @brief Insert a @p key and @p value pair into the `hashmap`.
/// @param[in,out] hm        The `hashmap` to modify. Must be initialized.
/// @param[in]     key       Pointer to the key.
/// @param[in]     value     Pointer to the value to store.
/// @param[out]    out_value Pointer to store a previous value. If NULL then it is ignored.
/// @return `MID_OK` on success.
int hm_insert(hashmap *hm, void *key, hm_type key_type, void *value, hm_type val_type,  void *out_value);

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
void hm_destroy(hashmap *hm);

#endif