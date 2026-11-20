#ifndef MID_OPTION_H
#define MID_OPTION_H

/// @brief The value an `option` can be. Either `some`, indicating there is data to handle, or `none`, or no data to handle.
typedef enum {
    None = 0,
    Some,
} optval;

/// @brief Mimic the Option<T> type in Rust, but only for some default types.
typedef struct {
    optval content;
    union {
        int i;
        double d;
        char *s;
        void *p;
    } as;
} option;

#endif