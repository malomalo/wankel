#include "wankel.h"
#include "yajl_helpers.h"

// Yajl Helpers ==============================================================
void yajl_helper_check_status(yajl_handle handle, yajl_status status, int verbose, const unsigned char * jsonText, size_t jsonTextLength) {
    if(status != yajl_status_ok) {
        unsigned char * str = yajl_get_error(handle, verbose, jsonText, jsonTextLength);
        yajl_free_error(handle, str);
        rb_raise(rb_const_get(rb_const_get(rb_cObject, rb_intern("Wankel")), rb_intern("ParseError")), (const char*) str);
    }
}


// Memory funcs
void * yajl_helper_malloc(void *ctx, size_t size) {
    return xmalloc(size);
}
void * yajl_helper_realloc(void *ctx, void *ptr, size_t size) {
    return xrealloc(ptr, size);
}
void yajl_helper_free(void *ctx, void *ptr) {
    return xfree(ptr);
}

// Configure
void yajl_configure(yajl_handle handle, VALUE options) {
    if(rb_hash_aref(options, ID2SYM(rb_intern("allow_comments")) ) == Qtrue) {
        yajl_config(handle, yajl_allow_comments, 1);
    } else {
        yajl_config(handle, yajl_allow_comments, 0);
    }

    if(rb_hash_aref(options, ID2SYM(rb_intern("validate_strings")) ) == Qtrue) {
        yajl_config(handle, yajl_dont_validate_strings, 0);
    } else {
        yajl_config(handle, yajl_dont_validate_strings, 1);
    }

    if(rb_hash_aref(options, ID2SYM(rb_intern("trailing_garbage")) ) == Qtrue) {
        yajl_config(handle, yajl_allow_trailing_garbage, 1);
    } else {
        yajl_config(handle, yajl_allow_trailing_garbage, 0);
    }

    if(rb_hash_aref(options, ID2SYM(rb_intern("multiple_values")) ) == Qtrue) {
        yajl_config(handle, yajl_allow_multiple_values, 1);
    } else {
        yajl_config(handle, yajl_allow_multiple_values, 0);
    }

    if(rb_hash_aref(options, ID2SYM(rb_intern("partial_values")) ) == Qtrue) {
        yajl_config(handle, yajl_allow_partial_values, 1);
    } else {
        yajl_config(handle, yajl_allow_partial_values, 0);
    }
}