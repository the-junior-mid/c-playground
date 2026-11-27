#include <stdlib.h>
#include <assert.h>

#ifndef MID_DYNAMICARRAY_H
#define MID_DYNAMICARRAY_H

#define DA_INIT_CAP 256

#define DA_DECLTYPE_CAST(T)

#ifndef DA_REALLOC
#include <stdlib.h>
#define DA_REALLOC realloc
#endif /* DA_REALLOC */

/// @brief Reserve the @p expected_capacity. Rounds up to the nearest power of 2. If capacity of @p da is 0, then it is set to `DA_INIT_CAP`.
#define da_reserve(da, expected_capacity)                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((expected_capacity) > (da)->capacity) {                                                                    \
            if ((da)->capacity == 0) (da)->capacity = DA_INIT_CAP;                                                     \
            while ((expected_capacity) > (da)->capacity) {                                                             \
                (da)->capacity *= 2;                                                                                   \
            }                                                                                                          \
            (da)->items = DA_DECLTYPE_CAST((da)->items)DA_REALLOC((da)->items, (da)->capacity * sizeof(*(da)->items)); \
            assert((da)->items != NULL && "No memory");                                                                \
        }                                                                                                              \
    } while (0)                                                                                                        \

/// @brief Add an @p item to the end of the @p da. A reallocation is done if needed.
#define da_push(da, item)                  \
    do                                     \
    {                                      \
        da_reserve((da), (da)->len + 1);   \
        (da)->items[(da)->len++] = (item); \
    } while (0)                            \

/// @brief Extend @p da with the given extension, @p ext, and extend by @p ext_len. 
#define da_extend(da, ext, ext_len)                                               \
    do                                                                            \
    {                                                                             \
        da_reserve((da), (da)->len + ext_len);                                    \
        memcpy((da)->items + (da)->len, (ext), (ext_len) * sizeof(*(da)->items)); \
        (da)->len += ext_len;                                                     \
    } while (0)                                                                   \
    
/// @brief Resize @p da to the @p new_size. Leaves memory untouched if no reallocation is needed.
#define da_resize(da, new_size)    \
    do                             \
    {                              \
       da_reserve((da), new_size); \
       (da)->len = (new_size);     \
    } while (0)                    \

/// @brief Return the last item in @p da and decrement the length of @p da.
#define da_pop(da) (da)->items[--(da)->len]

/// @brief Return the first item in @p da.
#define da_first(da) (da)->items[0]

/// @brief Return the last item in @p da.
#define da_last(da) (da)->items[(da)->len - 1]

/// @brief Iterate over each item, @p i, in @p da, where @p i has type @p Type.
#define da_foreach(da, i, Type) for (Type *i = (da)->items; i < (da)->items + (da)->len; ++i)

/// @brief Free the allocated memory for @p da.
#define da_free(da) free((da).items)

#endif