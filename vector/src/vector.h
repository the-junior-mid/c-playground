#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// A Vector that will dynamically resize to fit its contents
typedef struct {
    // The pointer for the vector
    void *ptr;
    // The total capacity of the vector
    size_t cap;
    // The size of the elements
    size_t elem_size;
    // The current length of the vector
    size_t len;
} vector;

typedef enum {
    VEC_OK = 0,
    VEC_ERR_NULL,
    VEC_ERR_EMPTY,
    VEC_ERR_RANGE,
    VEC_ERR_NOMEM,
    VEC_ERR_UNKNOWN_SIZE,
    VEC_ERR_INVALID_SIZE,
} vec_status;

char * vec_strerror(int err) {
    switch (err)
    {
    case 1:
        "output parameter cannot be null";
        break;
    case 2:
        "vector is empty";
        break;
    case 3:
        "invalid index";
        break;
    case 4:
        "out of memory";
        break;
    case 5:
        "unknown element size";
    case 6:
        "provided item does not match size of elements";
        break;
    default:
        "unknown error";
        break;
    }
}

// Construction of the Vector
int vec_new(vector *v, size_t cap, size_t size) {
    // Allocate memory for the Vector
    // The returned pointer needs to be generic
    void *ptr = calloc(cap, size);
    if (ptr == NULL) {
        perror("malloc");
        return VEC_ERR_NOMEM;
    }
    v->ptr = ptr;
    v->cap = cap;
    v->elem_size = size;
    v->len = 0;
    printf("vector: %p\n", v->ptr);
    return VEC_OK;
}

// Get the address for index
void * _vec_slot(const vector *v, size_t index) {
    return (unsigned char *)v->ptr + (index * v->elem_size);
}

// Free the allocated vector memory
void vec_free(vector *v) {
    free(v->ptr);
}

// Resize the vector to accomodate the new cap
int vec_realloc(vector *v, size_t new_size) {
    // If the vector does not contain any elements, then the size of its items is unknown and cannot be properly realloc'd
    if (v->cap == 0) {
        return VEC_ERR_UNKNOWN_SIZE;
    }
    v->cap = new_size;
    void *tmp = realloc(v->ptr,  v->cap * sizeof(v->ptr));
    if (tmp == NULL) return VEC_ERR_NOMEM;
    v->ptr = tmp;
    return VEC_OK;
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
void _vec_print(const vector *v) {
    printf("[");
    for (size_t i = 0; i < v->len; i++) {
        printf("%d", *(int *)_vec_slot(v, i));
        if (i + 1 < v->len) {
            printf(", ");
        }
    }
    printf("]\n");
}

// Add an element to the end of the vector
int vec_push(vector *v, void *item) {
    // Check if the vector has space, if not, then resize it
    if ((v->len >= v->cap) && (vec_realloc(v, v->cap * 2) < 0)) {
        return VEC_ERR_NOMEM;
    }
    memcpy(_vec_slot(v, v->len), item, v->elem_size);
    v->len += 1;
    return VEC_OK;
}

// Remove an element from the end of the vector and place it in `out`
int vec_pop(vector *v, void *out) {
    if (v->len == 0) {
        return VEC_ERR_EMPTY;
    }
    memcpy(out, _vec_slot(v, v->len - 1), v->elem_size);

    v->len -= 1;

    return VEC_OK;
}

// Extend the vector with the given vector
int vec_extend(vector *v, vector *ext) {
    // Check the elements are at least the same size
    if (v->elem_size != ext->elem_size) {
        return VEC_ERR_INVALID_SIZE;
    }
    // Check if the ext will fit in the current vector, if not, then realloc to the combined size
    if (v->len + ext->len >= v->cap) {
        if (vec_realloc(v, v->cap + ext->cap) < 0) return VEC_ERR_NOMEM;
    }
    v->len += ext->len;
    // Place contents of ext into vector
    memmove(_vec_slot(v, v->len), ext, ext->len * v->elem_size);

    return VEC_OK;
}

// Insert the given item at the specified index, shifting all other elements to the right
int vec_insert(vector *v, size_t index, void *item) {
    if (index >= v->len) {
        return VEC_ERR_RANGE;
    }
    v->len++;
    void *tmp = _vec_slot(v, index);
    memcpy(tmp, item, v->elem_size);
    for (size_t i = index + 1; i < v->len; i++) {
        memmove(_vec_slot(v, i), tmp, v->elem_size);
        if (i + 1 < v->len) tmp = _vec_slot(v, i + 1);
    }
    return VEC_OK;
}

// Remove the item at the specified index, shifting all other elements to the left, and placing the value of the removed item in `out`
int vec_remove(vector *v, size_t index, void *out) {
    if (v->len == 0) {
        return VEC_ERR_EMPTY;
    }
    if (index >= v->len) {
        return VEC_ERR_RANGE;
    }
    if (out != NULL) memcpy(out, _vec_slot(v, index), v->elem_size);
    v->len--;
    size_t count = v->len - index;
    memmove(_vec_slot(v, index), _vec_slot(v, index + 1), count * v->elem_size);
    return VEC_OK;
}

// Get a reference to the item at index
int vec_get(const vector *v, size_t index, void *out) {
    if (v->len == 0) {
        return VEC_ERR_EMPTY;
    }
    if (index >= v->len) {
        return VEC_ERR_RANGE;
    }
    if (out == NULL) {
        return VEC_ERR_NULL;
    }

    memcpy(out, _vec_slot(v, index), v->elem_size);

    return VEC_OK;
}

// Reverse the order of the vector in place
int vec_reverse(vector *v) {
    void *tmp = malloc(v->elem_size);
    if (tmp == NULL) return VEC_ERR_NOMEM;
    // Index from the beginning and end of the vector, once i==k the middle has been reached and can stop
    if (v->len <= 1) {
        return VEC_OK;
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

    return VEC_OK;
}

// Swap the two values at the given indexes
int vec_swap(vector *v, size_t index1, size_t index2) {
    if (index1 >= v->len) {
        return VEC_ERR_RANGE;
    }
    if (index2 >= v->len) {
        return VEC_ERR_RANGE;
    }
    void *tmp = malloc(v->elem_size);
    memmove(tmp, _vec_slot(v, index1), v->elem_size);
    memmove(_vec_slot(v, index1), _vec_slot(v, index2), v->elem_size);
    memmove(_vec_slot(v, index2), tmp, v->elem_size);

    return VEC_OK;
}

// Set the length of the vector to 0. Everything else is untouched, meaning the underlying data is still in the allocated buffer.
void vec_clear(vector *v) {
    v->len = 0;
}