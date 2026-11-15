#include "vector.h"  


typedef enum {
    STR_OK = 0,
    STR_ERR_NOMEM,
} string_status;


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

// `str` must be null terminated
int string_new(String *s, char *str) {
    size_t len = (str == NULL) ? 0 : get_str_len(str);
    if (vec_new(&s->v, len, sizeof(char)) < 0) return STR_ERR_NOMEM;

    if (vec_extend_with_array(&s->v, str, len) < 0) {
        return STR_ERR_NOMEM;
    }

    return STR_OK;
}

// Push a string into the current string
int string_push_str(String *s, char *str) {
    size_t len = get_str_len(str);
    if (vec_realloc(&s->v, s->v.len + len) < 0) {
        return STR_ERR_NOMEM;
    }
    if (vec_extend_with_array(&s->v, str, len) < 0) {
        return STR_ERR_NOMEM;
    }

    return STR_OK;
}

// Push a character into the current string
int string_push_char(String *s, char c) {
    if (vec_realloc(&s->v, s->v.len + 1) < 0) {
        return STR_ERR_NOMEM;
    }
    if (vec_push(&s->v, &c) < 0) {
        return STR_ERR_NOMEM;
    }

    return STR_OK;
}

void string_destroy(String *s) {
    vec_destroy(&s->v);
}