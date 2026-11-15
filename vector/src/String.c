#include "include/Strings.h"


size_t string_len(const String *s) {
    return s->v.len;
}

int string_new(String *s, char *str) {
    size_t len = (str == NULL) ? 0 : get_str_len(str);
    if (vec_new(&s->v, len, sizeof(char)) < 0) return MID_ERR_NOMEM;
    if ((len > 0) && (vec_extend_with_array(&s->v, str, len) < 0)) return MID_ERR_NOMEM;

    return MID_OK;
}

void string_destroy(String *s) {
    vec_destroy(&s->v);
}

int string_push_str(String *s, char *str) {
    size_t len = get_str_len(str);
    int ret = vec_extend_with_array(&s->v, str, len);
    if (ret < 0) return ret;

    return MID_OK;
}

int string_push(String *s, char c) {
    int ret = vec_push(&s->v, &c);
    if (ret < 0) return ret;

    return MID_OK;
}

int string_pop(String *s, char *out) {
    return vec_pop(&s->v, out);
}

int string_remove(String *s, size_t index, char *out) {
    return vec_remove(&s->v, index, out);
}

int string_insert(String *s, size_t index, const char *c) {
    return vec_insert(&s->v, index, c);
}

int string_get(const String *s, size_t index, char *out) {
    return vec_get(&s->v, index, &out);
}