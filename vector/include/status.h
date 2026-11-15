#ifndef MID_STATUS_H
#define MID_STATUS_H

typedef enum {
    MID_OK = 0,
    MID_ERR_NULL,
    MID_ERR_EMPTY,
    MID_ERR_RANGE,
    MID_ERR_NOMEM,
    MID_ERR_INVALID_SIZE,
} mid_status;

char * mid_strerror(int err) {
    switch (err) {
        case 1:
            return "output parameter is null";
        case 2:
            return "vector is empty";
        case 3:
            return "index out of range";
        case 4:
            return "out of memory";
        case 5:
            return "size is invalid (size does not match)";
        default:
            return "unknown error";
    }
}

#endif