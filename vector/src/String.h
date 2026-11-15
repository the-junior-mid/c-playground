#include "vector.h"  


typedef enum {
    STR_OK = 0,
    STR_ERR_NULL,
    STR_ERR_EMPTY,
    STR_ERR_RANGE,
    STR_ERR_NOMEM,
    STR_ERR_INVALID_SIZE,
} string_status;

char * string_strerror(int err) {
    switch (err) {
        case 1:
            return "output parameter cannot be null";
        case 2:
            return "String is empty";
        case 3:
            return "invalid index";
        case 4:
            return "out of memory";
        case 5:
            return "provided item is not size of char";
        default:
            return "unknown error";
    }
}

// A utf-8 string, just a special case of a vector struct
typedef struct {
    vector v;
} String;

size_t get_str_len(const char *str) {
    size_t len = 0;
    char c = str[0];
    while (c != '\0') {
        len++;
        c = str[len];
    }

    return len;
}

size_t string_len(const String *s) {
    return s->v.len;
}

// Create an empty String. The `str` must be null terminated or null.
// If `str` is not null then the size of `str` is calculated and set as the String's
// capacity. Otherwise, the Strings length is set to 0 and no allocation is done.
int string_new(String *s, char *str) {
    size_t len = (str == NULL) ? 0 : get_str_len(str);
    if (vec_new(&s->v, len, sizeof(char)) < 0) return STR_ERR_NOMEM;
    if ((len > 0) && (vec_extend_with_array(&s->v, str, len) < 0)) return STR_ERR_NOMEM;

    return STR_OK;
}

// Push a string into the current string
int string_push_str(String *s, char *str) {
    size_t len = get_str_len(str);
    int ret = vec_extend_with_array(&s->v, str, len);
    if (ret < 0) return ret;

    return STR_OK;
}

// Push a character into the current string
int string_push(String *s, char c) {
    int ret = vec_push(&s->v, &c);
    if (ret < 0) return ret;

    return STR_OK;
}

// Remove the last char in `s` and place it in `out` if `out` is not null.
int string_pop(String *s, char *out) {
    return vec_pop(&s->v, out);
}

// Remove the char at index and place it in `out` if it is not null.
int string_remove(String *s, size_t index, char *out) {
    return vec_remove(&s->v, index, out);
}

// Insert the given char at the given index.
int string_insert(String *s, size_t index, char *c) {
    return vec_insert(&s->v, index, c);
}

int string_get(String *s, size_t index, char *out) {
    return vec_get(&s->v, index, &out);
}

void string_destroy(String *s) {
    vec_destroy(&s->v);
}