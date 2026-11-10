#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

// A Vector that will dynamically resize to fit its contents
typedef struct {
    // The pointer for the vector
    void *ptr;
    // The total capacity of the vector
    size_t cap;
    // The current length of the vector
    size_t len;
} vector;

// Construction of the Vector
int vec_new(size_t cap, vector *v) {
    // Allocate memory for the Vector
    // The returned pointer needs to be generic
    void *ptr = malloc(cap);
    if (ptr == NULL) {
        perror("malloc");
        return -1;
    }
    v->ptr = ptr;
    v->cap = cap;
    v->len = 0;
    return 0;
}

// Free the allocated Vecotr
void vec_free(vector *v) {
    free(v->ptr);
}

// Resize the vector to accomodate the new cap
int vec_realloc(vector *v) {
    // If the vector does not contain any elements, then the size of its items is unknown and cannot be properly realloc'd
    if (v->cap == 0) {
        fprintf(stderr, "ERROR: size of items unknown\n");
        return -1;
    }
    v->cap *= 2;
    void *tmp = realloc(v->ptr,  v->cap * sizeof(v->ptr));
    if (tmp == NULL) return -1;
    v->ptr = tmp;
    return 0;
}

// Add an element to the vector
int vec_push(vector *v, void *item) {
    // Check if the vector has space, if not, then resize it
    if ((v->len >= v->cap) & (vec_realloc(v) < 0)) {
        return -1;
    }
    memcpy(v->ptr + (v->len * sizeof(*item)), item, sizeof(*item));
    return 0;
}