#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// A Vector that will dynamically resize to fit its contents
typedef struct {
    // The pointer for the vector
    void **ptr;
    // The total capacity of the vector
    size_t cap;
    // The size of the elements
    size_t elem_s;
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
int vec_new(size_t cap, vector *v) {
    // Allocate memory for the Vector
    // The returned pointer needs to be generic
    void *ptr = malloc(cap * sizeof(void *));
    if (ptr == NULL) {
        perror("malloc");
        return VEC_ERR_NOMEM;
    }
    memset(ptr, 0, cap * sizeof(void *));
    v->ptr = ptr;
    v->cap = cap;
    v->elem_s = sizeof(void *);
    v->len = 0;
    printf("vector: %p\n", v->ptr);
    return VEC_OK;
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

size_t vec_len(vector *v) {
    return v->len;
}

size_t vec_cap(vector *v) {
    return v->cap;
}

bool vec_is_empty(vector *v) {
    return v->len == 0;
}

int vec_print(vector *v) {
    printf("[");
    size_t i = 1;
    for (void *tmp = v->ptr; tmp != (v->ptr + v->len); tmp++) {
        printf("%p", tmp);
        if (i != v->len) {
            printf(", ");
        }
        i++;
    }
    printf("]\n");
}

// Add an element to the end of the vector
int vec_push(vector *v, void *item) {
    if (v->elem_s != sizeof(item)) {
        return VEC_ERR_INVALID_SIZE;
    }
    // Check if the vector has space, if not, then resize it
    if ((v->len >= v->cap) && (vec_realloc(v, v->cap * 2) < 0)) {
        return VEC_ERR_NOMEM;
    }
    v->ptr[v->len] = item;
    v->len += 1;
    return VEC_OK;
}

// Remove an element from the end of the vector and place it in `out`
int vec_pop(vector *v, void *out) {
    if (v->len == 0) {
        return VEC_ERR_EMPTY;
    }
    memcpy(out, v->ptr + v->len - 1, v->elem_s);

    v->len -= 1;

    return VEC_OK;
}

// Extend the vector with the given vector
int vec_extend(vector *v, vector *ext) {
    // Check the elements are at least the same size
    if (v->elem_s != ext->elem_s) {
        return VEC_ERR_INVALID_SIZE;
    }
    // Check if the ext will fit in the current vector, if not, then realloc to the combined size
    if (v->len + ext->len >= v->cap) {
        if (vec_realloc(v, v->cap + ext->cap) < 0) return VEC_ERR_NOMEM;
    }
    size_t old_len = v->len;
    v->len += ext->len;
    // Place contents of ext into vector
    for (size_t i = old_len; i < v->len; i++) {
        v->ptr[i] = ext->ptr[i - old_len];
    }
    return VEC_OK;
}

// Insert the given item at the specified index, shifting all other elements to the right
int vec_insert(vector *v, size_t index, void *item) {
    if (index >= v->len) {
        return VEC_ERR_RANGE;
    }
    v->len++;
    void *tmp = v->ptr[index];
    v->ptr[index] = item;
    for (size_t i = index + 1; i < v->len; i++) {
        printf("vector: tmp = %d, i = %d\n", tmp, i);
        v->ptr[i] = tmp;
        if (i + 1 < v->len) tmp = v->ptr[i + 1];
    }
    return VEC_OK;
}

// Remove the item at the specified index, shifting all other elements to the left, and placing the value of the removed item in `out`
int vec_remove(vector *v, size_t index, void **out) {
    if (v->len == 0) {
        return VEC_ERR_EMPTY;
    }
    if (index >= v->len) {
        return VEC_ERR_RANGE;
    }
    if (out != NULL) *out = v->ptr[index];
    v->len--;
    for (size_t i = index; i < v->len; i++) {
        v->ptr[i] = v->ptr[i + 1];
    } 
    return VEC_OK;
}

// Get a reference to the item at index
int vec_get(vector *v, size_t index, void **out) {
    if (v->len == 0) {
        return VEC_ERR_EMPTY;
    }
    if (index >= v->len) {
        return VEC_ERR_RANGE;
    }
    if (out == NULL) {
        return VEC_ERR_NULL;
    }

    *out = v->ptr[index];

    return VEC_OK;
}

// Reverse the order of the vector
int vec_reverse(vector *v) {
    void *tmp1;
    void *tmp2;
    // Index from the beginning and end of the vector, once i==k the middle has been reached and can stop
    size_t k = v->len - 1;
    size_t i = 0;
    while (i < k) {
        tmp1 = v->ptr[i];
        tmp2 = v->ptr[k];
        v->ptr[i] = tmp2;
        v->ptr[k] = tmp1;
        k--;
        i++;
    }

    return VEC_OK;
}

// Reverse the order of the vector but use malloc first.
// NOTE: This is a bad method of doing this, the new malloc is not needed since values can be swapped in place
int _vec_reverse_realloc(vector *v) {
    void **v_new = malloc(v->cap * sizeof(v->elem_s));
    if (v_new == NULL) {
        perror("malloc");
        return VEC_ERR_NOMEM;
    }
    ssize_t i = v->len - 1;
    void **tmp = v_new;
    while (i >= 0) {
        tmp = v->ptr[i];
        tmp++;
        i--;
    }

    v->ptr = v_new;

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
    void *tmp1 = v->ptr[index1];
    void *tmp2 = v->ptr[index2];
    v->ptr[index1] = tmp2;
    v->ptr[index2] = tmp1;

    return VEC_OK;
}

// Clear the contents of the vector
void vec_clear(vector *v) {
    v->len = 0;
    memset(v->ptr, 0, v->cap);
}