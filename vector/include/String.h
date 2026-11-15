#include "vector.h"

#ifndef MID_STRING_H
#define MID_STRING_H

// A utf-8 string, just a special case of a `vector` struct
typedef struct {
    vector v;
} String;

#endif