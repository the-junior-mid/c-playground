#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// A Vector that will dynamically resize to fit its contents
typedef struct {
    // The pointer for the vector
    int *ptr;
    // The total capacity of the vector
    size_t cap;
    // The size of the elements
    size_t elem_s;
    // The current length of the vector
    size_t len;
} vector;

// Construction of the Vector
int vec_new(size_t cap, size_t s, vector *v) {
    // Allocate memory for the Vector
    // The returned pointer needs to be generic
    void *ptr = malloc(cap * s);
    if (ptr == NULL) {
        perror("malloc");
        return -1;
    }
    memset(ptr, 0, cap * s);
    v->ptr = ptr;
    v->cap = cap;
    v->elem_s = s;
    v->len = 0;
    printf("vector: %p %d\n", v->ptr, v->ptr);
    return 0;
}

// Free the allocated Vecotr
void vec_free(vector *v) {
    free(v->ptr);
}

// Resize the vector to accomodate the new cap
int vec_realloc(vector *v, size_t new_size) {
    // If the vector does not contain any elements, then the size of its items is unknown and cannot be properly realloc'd
    if (v->cap == 0) {
        fprintf(stderr, "ERROR: size of items unknown\n");
        return -1;
    }
    v->cap = new_size;
    void *tmp = realloc(v->ptr,  v->cap * sizeof(v->ptr));
    if (tmp == NULL) return -1;
    v->ptr = tmp;
    return 0;
}

size_t vec_len(vector *v) {
    printf("vector length: %d\n", v->len);
    return v->len;
}

size_t vec_cap(vector *v) {
    printf("vector capacity: %d\n", v->cap);
    return v->cap;
}

bool vec_is_empty(vector *v) {
    return v->len == 0;
}

void vec_print(vector *v) {
    printf("[");
    size_t i = 1;
    for (int *tmp = v->ptr; tmp != (v->ptr + v->len); tmp++) {
        printf("%d", *tmp);
        if (i != v->len) {
            printf(", ");
        }
        i++;
    }
    printf("]\n");
}

// Add an element to the end of the vector
int vec_push(vector *v, int *item) {
    // Check if the vector has space, if not, then resize it
    if ((v->elem_s == sizeof(*item)) && (v->len >= v->cap) && (vec_realloc(v, v->cap * 2) < 0)) {
        return -1;
    }
    memcpy(v->ptr + v->len, item, sizeof(*item));
    v->len += 1;
    return 0;
}

// Remove an element from the end of the vector and place it in `out`
int vec_pop(vector *v, int *out) {
    if (v->len == 0) {
        fprintf(stderr, "ERROR: cannot pop from an empty vector\n");
        return -1;
    }
    memcpy(out, v->ptr + v->len, v->elem_s);

    v->len -= 1;

    return 0;
}

// Extend the vector with the given vector
int vec_extend(vector *v, vector *ext) {
    // Check the elements are at least the same size
    if (v->elem_s != ext->elem_s) {
        fprintf(stderr, "ERROR: extension must hold elements of the same size");
        return -1;
    }
    // Check if the ext will fit in the current vector, if not, then realloc to the combined size
    if (v->len + ext->len >= v->cap) {
        if (vec_realloc(v, v->cap + ext->cap) < 0) return -1;
    }
    size_t old_len = v->len;
    v->len += ext->len;
    // Place contents of ext into vector
    for (size_t i = old_len; i < v->len; i++) {
        v->ptr[i] = ext->ptr[i - old_len];
    }
    return 0;
}

// Insert the given item at the specified index, shifting all other elements to the right
int vec_insert(vector *v, size_t index, int item) {
    if (index >= v->len) {
        fprintf(stderr, "ERROR: index is out of bounds\n");
        return -1;
    }
    v->len++;
    int tmp = v->ptr[index];
    v->ptr[index] = item;
    for (size_t i = index + 1; i < v->len; i++) {
        v->ptr[i] = tmp;
        if (i + 1 < v->len) tmp = v->ptr[i + 1];
    }
    return 0;
}

// Remove the item at the specified index, shifting all other elements to the left, and placing the value of the removed item in `out`
int vec_remove(vector *v, size_t index, int *out) {
    if (index >= v->len) {
        fprintf(stderr, "ERROR: index is out of bounds\n");
        return -1;
    }
    if (out != NULL) *out = v->ptr[index];
    v->len--;
    for (size_t i = index; i < v->len; i++) {
        v->ptr[i] = v->ptr[i + 1];
    } 
    return 0;
}

// Get a reference to the item at index
int vec_get(vector *v, size_t index, int *out) {
    if (index >= v->len) {
        fprintf(stderr, "ERROR: index is out of bounds\n");
        return -1;
    }
    memcpy(out, v->ptr + index, v->elem_s);
    return 0;
}

// Reverse the order of the vector
int vec_reverse(vector *v) {
    int tmp1;
    int tmp2;
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

    return 0;
}

// Reverse the order of the vector but use malloc first.
// NOTE: This is a bad method of doing this, the new malloc is not needed since values can be swapped in place
int _vec_reverse_realloc(vector *v) {
    int *v_new = malloc(v->cap * sizeof(v->elem_s));
    if (v_new == NULL) {
        perror("malloc");
        return -1;
    }
    ssize_t i = v->len - 1;
    int *tmp = v_new;
    while (i >= 0) {
        *tmp = v->ptr[i];
        tmp++;
        i--;
    }

    v->ptr = v_new;

    return 0;
}

// Swap the two values at the given indexes
int vec_swap(vector *v, size_t index1, size_t index2) {
    if (index1 >= v->len) {
        fprintf(stderr, "ERROR: index1 is out of bounds\n");
        return -1;
    }
    if (index2 >= v->len) {
        fprintf(stderr, "ERROR: index2 is out of bounds\n");
        return -1;
    }
    int tmp1 = v->ptr[index1];
    int tmp2 = v->ptr[index2];
    v->ptr[index1] = tmp2;
    v->ptr[index2] = tmp1;

    return 0;
}

// Clear the contents of the vector
void vec_clear(vector *v) {
    v->len = 0;
    memset(v->ptr, 0, v->cap);
}