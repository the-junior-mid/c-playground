#include "vector.h"

#ifndef MID_STRING_H
#define MID_STRING_H

// A utf-8 string, just a special case of a `vector` struct
typedef struct {
    vector v;
} String;

/// @brief Helper function to get the length of @p str.
/// @param[in] str The string to get the length of. Must be `NULL` terminated.
/// @return `size_t` The length of @p str, not including null terminator.
static inline size_t get_str_len(const char *str) {
    size_t len = 0;
    char c = str[0];
    while (c != '\0') {
        len++;
        c = str[len];
    }

    return len;
}

/// @brief Get the current length of @p s.
/// @param[in,out] s The `String` to modify. Must be initialized.
/// @return `size_t` Current length of @p s, not including null terminator.
size_t string_len(const String *s);

/// @brief Create a `String`. If @p str is `NULL`, then a zero capacity `String` is created; otherwise, @p s will have length of @p str.
/// @param[in,out] s   The `String` to modify. Must be initialized.
/// @param[in]     str The `char *` to copy into @p s. Can be `NULL` to avoid an allocation.
/// @return `MID_OK` on success.
/// @return `MID_ERR_NOMEM` if growing the buffer fails. @p s is uninitialized.
int string_new(String *s, char *str);

/// @brief Free the memory used by the underlying `vector` struct.
/// @param[in,out] s The `String` to modify. Must be initialized.
void string_destroy(String *s);

/// @brief Push a string to @p s. Underlying buffer will be resized if needed.
/// @param[in,out] s   The `String` to modify. Must be initialized.
/// @param[in]     str The `char *` to append.
/// @return `MID_OK` on success.
/// @return `MID_ERR_NOMEM` if growing the buffer fails. @p s is unchanged.
int string_push_str(String *s, char *str);

/// @brief Push a `char` at the end of @p s.
/// @param[in,out] s The `String` to modify. Must be initialized.
/// @param[in]     c The `char` to append.
/// @return `MID_OK` on success.
/// @return `MID_ERR_NOMEM` if growing the buffer fails. @p s is unchanged.
int string_push(String *s, char c);

/// @brief Remove the last `char` in @p s and place it in @p out if it's not `NULL`.
/// @param[in,out] s   The `String` to modify. Must be initialized.
/// @param[out]    out A `char *` where the `char` will be stored. If `NULL`, then is ignored.
/// @return `MID_OK` on success.
/// @return `MID_ERR_EMPTY` if @p s is an empty.
int string_pop(String *s, char *out);

/// @brief Remove the `char` at @p index and place it in @p out, if it's not `NULL`.
/// @param[in,out] s     The `String` to modify. Must be initialized.
/// @param[in]     index Position to remove, must be in the range [0, len].
/// @param[out]    out   A `char *` where the `char` will be stored. If `NULL`, then is ignored.
/// @return `MID_OK` on success.
/// @return `MID_ERR_EMPTY` if @p s is an empty.
/// @return `MID_ERR_RANGE` if @p index is greater than `String` length.
int string_remove(String *s, size_t index, char *out);

/// @brief Insert the given `char` at the given @p index. All `char`s at and after @p index will be shifted right. This may grow the buffer.
/// @param[in,out] s     The `String` to modify. Must be initialized.
/// @param[in]     index Position to insert at, must be in the range [0, len].
/// @param[in]     c     Points to the `char` to insert. Read-only copied into the String. Must not be NULL.
/// @return `MID_OK` on success.
/// @return `MID_ERR_RANGE` if @p index is greater than `String` length.
/// @return `MID_ERR_NOMEM` if growing the buffer fails. @p s is unchanged.
int string_insert(String *s, size_t index, const char *c);

/// @brief  Retrieve the `char` at @p index.
/// @param[in,out] s     The `String` to modify. Must be initialized.
/// @param[in]     index Position of the `char` to get, must be in the range [0, len].
/// @param[out]    out   A `char *` where the `char` will be stored. Must not be NULL.
/// @return `MID_OK` on success.
/// @return `MID_ERR_EMPTY` if @p s is an empty.
/// @return `MID_ERR_RANGE` if @p index is greater than `String` length.
/// @return `MID_ERR_NULL` if @p out is NULL.
int string_get(const String *s, size_t index, char *out);

#endif