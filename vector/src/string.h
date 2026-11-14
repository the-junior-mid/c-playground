#include "vector.h"  


// A utf-8 string, just a special case of vector
typedef struct {
    vector v;
} string;

// `s_data` must be null terminated
int string_new(char * s_data, string *s) {
    
}