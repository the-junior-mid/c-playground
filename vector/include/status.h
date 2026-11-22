#ifndef MID_STATUS_H
#define MID_STATUS_H

typedef enum {
    MID_OK = 0,
    MID_ERR_NULL,
    MID_ERR_EMPTY,
    MID_ERR_RANGE,
    MID_ERR_NOMEM,
    MID_ERR_INVALID_SIZE,
    MID_ERR_TYPE,
    MID_ERR_SLOTS,
    MID_ERR_NOT_FOUND,
} mid_status;

/// @brief Convert the error number into a string detailing the reason for the error.
/// @param[in] err A value returned by a function in this library.
/// @return `char *`
static inline char * mid_strerror(int err) {
    switch (err) {
        case MID_OK:               return "success";
        case MID_ERR_NULL:         return "output parameter is null";
        case MID_ERR_EMPTY:        return "vector is empty";
        case MID_ERR_RANGE:        return "index out of range";
        case MID_ERR_NOMEM:        return "out of memory";
        case MID_ERR_INVALID_SIZE: return "size is invalid (size does not match)";
        case MID_ERR_TYPE:         return "invalid type for hm_value";
        case MID_ERR_SLOTS:        return "slots full";
        case MID_ERR_NOT_FOUND:    return "not found";
        default:                   return "unknown error";
    }
}

#endif