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
    printf("vector: storing elements of size %d\n", s);
    printf("vector: capacity * size = %d\n", cap * s);
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
int vec_extend(vector *v, vector *extension) {
    return 0;
}

// Insert the given item at the specified index, shifting all other elements to the right
int vec_insert(vector *v, size_t index, int item) {
    if (index >= v->len) {
        fprintf(stderr, "ERROR: index is out of bounds\n");
        return -1;
    }
    return 0;
}

// Remove the item at the specified index, shifting all other elements to the left, and placing the value of the removed item in `out`
int vec_remove(vector *v, size_t index, int *out) {
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
    // TODO: reallocate a new vector and put the items in reverse order inside of it
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
    return 0;
}

// Clear the contents of the vector
void vec_clear(vector *v) {
    v->len = 0;
    memset(v->ptr, 0, v->cap);
}