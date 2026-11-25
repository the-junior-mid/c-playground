#include <assert.h>
#include "status.h"
#include "option.h"


void opt_new(option *opt, void *content, opt_type content_type) {
    if (content != NULL) {
        switch (content_type)
        {
            case O_INT: 
                opt->content.i = *(int *)content;
                opt->type = O_INT;
                break;
            case O_DOUBLE: 
                opt->content.d = *(double *)content;
                opt->type = O_DOUBLE;
                break;
            case O_STR: 
                opt->content.s = (char *)content;
                opt->type = O_STR;
                break;
            case O_PTR: 
                opt->content.p = content;
                opt->type = O_PTR;
                break;
            default: // O_NONE and all others leave content uninitialized
                opt->type = O_NONE;
                break;
        }
    } else {
        opt->type = O_NONE;
    }
}

bool opt_is_some(option *opt) {
    return opt->type != O_NONE;
};

bool opt_is_none(option *opt) {
    return opt->type == O_NONE;
}

int opt_take(option *opt, void *out, opt_type *out_type) {
    assert(out != NULL && out_type != NULL);
    *out_type = opt->type;
    switch (opt->type)
    {
        case O_INT:
            *(int *)out = opt->content.i;
            opt->type = O_NONE;
            return MID_OK;
        case O_DOUBLE:
            *(double *)out = opt->content.d;
            opt->type = O_NONE;
            return MID_OK;
        case O_STR:
            out = opt->content.s;
            opt->type = O_NONE;
            return MID_OK;
        case O_PTR:
            out = opt->content.p;
            opt->type = O_NONE;
            return MID_OK;
        default: // O_NONE and any unknown types set `out` to nothing.
            return MID_OK;
    }
}

void opt_replae(option *opt, void *new_content, opt_type new_type, void *out) {
    assert(opt->type != O_NONE);
    assert(opt->type == new_type);

    switch (opt->type)
    {
        case O_INT:
            *(int *)out = opt->content.i;
            opt->content.i = *(int *)new_content;
            break;
        case O_DOUBLE:
            *(double *)out = opt->content.d;
            opt->content.d = *(double *)new_content;
            break;
        case O_STR:
            out = opt->content.s;
            opt->content.s = (char *)new_content;
            break;
        case O_PTR:
            out = opt->content.p;
            opt->content.p = new_content;
            break;
        default: // O_NONE and all unknowns
            break;
    }
}