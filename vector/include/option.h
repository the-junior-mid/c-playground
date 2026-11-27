#include <stdbool.h>
#include <stdlib.h>


#ifndef MID_OPTION_H
#define MID_OPTION_H


typedef enum { O_NONE, O_INT, O_DOUBLE, O_STR, O_PTR } opt_type;

/// @brief Mimic the Option<T> type in Rust, but only for some default types.
typedef struct {
    opt_type type;
    union {
        int i;
        double d;
        char *s;
        void *p;
    } content;
} option;

void opt_new(option *opt, void *content, opt_type content_type);

bool opt_is_some(option *opt);

bool opt_is_none(option *opt);

int opt_take(option *opt, void *out, opt_type *out_type);

void opt_replae(option *opt, void *new_content, opt_type new_type, void *out);



#endif