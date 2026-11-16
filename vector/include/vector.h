#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "status.h"


#ifndef MID_VECTOR_H
#define MID_VECTOR_H

// A Vector that will dynamically resize to fit its contents. The data is always 0 initialized.
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

// Construct the vector
int vec_new(vector *v, size_t cap, size_t size);

// Get the address for index
void * _vec_slot(const vector *v, size_t index);

// Free the allocated vector memory
void vec_destroy(vector *v);

// Resize the vector to accomodate the new cap
int vec_realloc(vector *v, size_t new_size);

size_t vec_len(const vector *v);

size_t vec_cap(const vector *v);

bool vec_is_empty(const vector *v);

// Casts each element to an int *. Only for debugging tests.
void _vec_print_ptr(const vector *v);

// Casts each element to an char *. Only for debugging tests.
void _vec_print_char(const vector *v);

// Add an element to the end of the vector
int vec_push(vector *v, const void *item);

// Remove an element from the end of the vector and place it in `out`
int vec_pop(vector *v, void *out);

// Extend the vector with the given vector
int vec_extend(vector *v, const vector *ext);

// Extend the vector with the given array with the same data type. The size of the elements in `ext` must
// match the size of the elements in `v`; otherwise undefined behavior can occur.
int vec_extend_with_array(vector *v, const void *ext, size_t len);

// Insert the given item at the specified index, shifting all other elements to the right
int vec_insert(vector *v, size_t index, const void *item);

// Remove the item at the specified index, shifting all other elements to the left, and placing the value of the removed item in `out`
int vec_remove(vector *v, size_t index, void *out);

// Get a reference to the item at index
int vec_get(const vector *v, size_t index, void *out);

// Reverse the order of the vector in place
int vec_reverse(vector *v);

// Swap the two values at the given indexes
int vec_swap(vector *v, size_t index1, size_t index2);

// Set the length of the vector to 0. Everything else is untouched, meaning the underlying data is still in the allocated buffer.
void vec_clear(vector *v);

#endif