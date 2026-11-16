#include "vector.h"


// Construct the vector
int vec_new(vector *v, size_t cap, size_t size) {
    // Allocate memory for the vector
    void *ptr = calloc(cap, size);
    if (ptr == NULL) {
        perror("malloc");
        return MID_ERR_NOMEM;
    }
    v->ptr = ptr;
    v->cap = cap;
    v->elem_size = size;
    v->len = 0;
    return MID_OK;
}

// Get the address for index
void * _vec_slot(const vector *v, size_t index) {
    return (unsigned char *)v->ptr + (index * v->elem_size);
}

// Free the allocated vector memory
void vec_destroy(vector *v) {
    if (v->cap == 0 || v == NULL) {
        return;
    }
    free(v->ptr);
    v->ptr = NULL;
    v->cap = 0;
    v->len = 0;
    v->elem_size = 0;
}

// Resize the vector to accomodate the new cap
int vec_realloc(vector *v, size_t new_size) {
    size_t old_size = v->cap;
    void *tmp = realloc(v->ptr, new_size * v->elem_size);
    if (tmp == NULL) return MID_ERR_NOMEM;
    v->cap = new_size;
    // Zero out any newly added memory
    if (new_size > old_size) {
        memset((unsigned char *)tmp + (old_size * v->elem_size), 0, (new_size - old_size) * v->elem_size);
    }
    v->ptr = tmp;
    return MID_OK;
}

size_t vec_len(const vector *v) {
    return v->len;
}

size_t vec_cap(const vector *v) {
    return v->cap;
}

bool vec_is_empty(const vector *v) {
    return v->len == 0;
}

// Casts each element to an int. Only for debugging tests.
void _vec_print_int(const vector *v) {
    printf("[");
    for (size_t i = 0; i < v->len; i++) {
        printf("%d", *(int *)_vec_slot(v, i));
        if (i + 1 < v->len) {
            printf(", ");
        }
    }
    printf("]\n");
}

// Casts each element to an int *. Only for debugging tests.
void _vec_print_ptr(const vector *v) {
    printf("[");
    for (size_t i = 0; i < v->len; i++) {
        printf("%p", (int *)_vec_slot(v, i));
        if (i + 1 < v->len) {
            printf(", ");
        }
    }
    printf("]\n");
}

// Casts each element to an char *. Only for debugging tests.
void _vec_print_char(const vector *v) {
    printf("[");
    for (size_t i = 0; i < v->len; i++) {
        printf("'%c'", *(char *)_vec_slot(v, i));
        if (i + 1 < v->len) {
            printf(", ");
        }
    }
    printf("]\n");
}

// Add an element to the end of the vector
int vec_push(vector *v, const void *item) {
    // Check if the vector has space, if not, then resize it
    if ((v->cap == 0) && vec_realloc(v, 4) < 0) {
        return MID_ERR_NOMEM;
    } else if ((v->len >= v->cap) && (vec_realloc(v, v->cap * 2) < 0)) {
        return MID_ERR_NOMEM;
    }
    memcpy(_vec_slot(v, v->len), item, v->elem_size);
    v->len += 1;
    return MID_OK;
}

// Remove an element from the end of the vector and place it in `out`
int vec_pop(vector *v, void *out) {
    if (v->len == 0) {
        return MID_ERR_EMPTY;
    }
    if (out != NULL) memcpy(out, _vec_slot(v, v->len - 1), v->elem_size);

    v->len--;

    return MID_OK;
}

// Extend the vector with the given vector
int vec_extend(vector *v, const vector *ext) {
    // Check the elements are at least the same size
    if (v->elem_size != ext->elem_size) {
        return MID_ERR_INVALID_SIZE;
    }
    // Check if the ext will fit in the current vector, if not, then realloc to the combined size
    if ((v->cap == 0) && vec_realloc(v, 4 + ext->len) < 0) {
        return MID_ERR_NOMEM;
    } else if ((v->len + ext->len >= v->cap) && vec_realloc(v, (v->cap + ext->cap) * 2) < 0) {
        return MID_ERR_NOMEM;
    }
    // Place contents of ext into vector
    memcpy(_vec_slot(v, v->len), ext->ptr, ext->len * v->elem_size);
    v->len += ext->len;

    return MID_OK;
}

// Extend the vector with the given array with the same data type. The size of the elements in `ext` must
// match the size of the elements in `v`; otherwise undefined behavior can occur.
int vec_extend_with_array(vector *v, const void *ext, size_t len) {
    // Check if the ext will fit in the current vector, if not, then realloc to the combined size
    if (v->len + len >= v->cap) {
        if (vec_realloc(v, (v->cap + len) * 2) < 0) return MID_ERR_NOMEM;
    }
    // Place contents of ext into vector
    memcpy(_vec_slot(v, v->len), ext, len * v->elem_size);
    v->len += len;

    return MID_OK;
}

// Insert the given item at the specified index, shifting all other elements to the right
int vec_insert(vector *v, size_t index, const void *item) {
    if (index >= v->len) {
        return MID_ERR_RANGE;
    }
    if (v->len + 1 >= v->cap && vec_realloc(v, v->cap * 2)) return MID_ERR_NOMEM;
    void *tmp = _vec_slot(v, index);
    memmove(_vec_slot(v, index + 1), _vec_slot(v, index), (v->len - index) * v->elem_size);
    memcpy(tmp, item, v->elem_size);
    v->len++;

    return MID_OK;
}

// Remove the item at the specified index, shifting all other elements to the left, and placing the value of the removed item in `out`
int vec_remove(vector *v, size_t index, void *out) {
    if (v->len == 0) {
        return MID_ERR_EMPTY;
    }
    if (index >= v->len) {
        return MID_ERR_RANGE;
    }
    if (out != NULL) memcpy(out, _vec_slot(v, index), v->elem_size);
    v->len--;
    size_t count = v->len - index;
    memmove(_vec_slot(v, index), _vec_slot(v, index + 1), count * v->elem_size);
    return MID_OK;
}

// Get a reference to the item at index
int vec_get(const vector *v, size_t index, void *out) {
    if (v->len == 0 || v->cap == 0) return MID_ERR_EMPTY;
    if (index >= v->len) return MID_ERR_RANGE;
    if (out == NULL) return MID_ERR_NULL;

    memcpy(out, _vec_slot(v, index), v->elem_size);

    return MID_OK;
}

// Reverse the order of the vector in place
int vec_reverse(vector *v) {
    void *tmp = malloc(v->elem_size);
    if (tmp == NULL) return MID_ERR_NOMEM;
    // Index from the beginning and end of the vector, once i==k the middle has been reached and can stop
    if (v->len <= 1) {
        return MID_OK;
    }
    size_t k = v->len - 1;
    size_t i = 0;
    while (i < k) {
        memmove(tmp, _vec_slot(v, i), v->elem_size);
        memmove(_vec_slot(v, i), _vec_slot(v, k), v->elem_size);
        memmove(_vec_slot(v, k), tmp, v->elem_size);
        i++;
        k--;
    }

    return MID_OK;
}

// Swap the two values at the given indexes
int vec_swap(vector *v, size_t index1, size_t index2) {
    if (index1 >= v->len) {
        return MID_ERR_RANGE;
    }
    if (index2 >= v->len) {
        return MID_ERR_RANGE;
    }
    void *tmp = malloc(v->elem_size);
    memmove(tmp, _vec_slot(v, index1), v->elem_size);
    memmove(_vec_slot(v, index1), _vec_slot(v, index2), v->elem_size);
    memmove(_vec_slot(v, index2), tmp, v->elem_size);

    return MID_OK;
}

// Set the length of the vector to 0. Everything else is untouched, meaning the underlying data is still in the allocated buffer.
void vec_clear(vector *v) {
    v->len = 0;
}