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

void yajl_helper_check_gen_status(yajl_gen_status status) {
	VALUE error = rb_const_get(rb_const_get(rb_cObject, rb_intern("Wankel")), rb_intern("EncodeError"));
	
	if (status == yajl_gen_status_ok || status == yajl_gen_generation_complete) {
		return;
	}
	
	if (status == yajl_gen_keys_must_be_strings) {
        rb_raise(error, "at a point where a map key is generated, a function other than yajl_gen_string was called");
	} else if (status == yajl_max_depth_exceeded) {
        rb_raise(error, "YAJL's maximum generation depth was exceeded. see YAJL_MAX_DEPTH");
	} else if (status == yajl_gen_in_error_state) {
        rb_raise(error, "A generator function (yajl_gen_XXX) was called while in an error state");
	} else if (status == yajl_gen_invalid_number) {
        rb_raise(error, "yajl_gen_double was passed an invalid floating point value (infinity or NaN).");
	} else if (status == yajl_gen_no_buf) {
        rb_raise(error, "A print callback was passed in, so there is no internal buffer to get from");
	} else if (status == yajl_gen_invalid_string) {
        rb_raise(error, "returned from yajl_gen_string() when the yajl_gen_validate_utf8 option is enabled and an invalid was passed by client code.");
	} else {
        rb_raise(error, "unkown yajl_gen_status error");
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

void yajl_gen_configure(yajl_gen g, VALUE options) {
    if(rb_hash_aref(options, ID2SYM(rb_intern("beautify")) ) == Qtrue) {
		yajl_gen_config(g, yajl_gen_beautify, 1);
	} else {
		yajl_gen_config(g, yajl_gen_beautify, 0);
	}
	
 	yajl_gen_config(g, yajl_gen_indent_string, RSTRING_PTR(rb_hash_aref(options, ID2SYM(rb_intern("indent_string")))));
	
    if(rb_hash_aref(options, ID2SYM(rb_intern("validate_utf8")) ) == Qtrue) {
		yajl_gen_config(g, yajl_gen_validate_utf8, 1);
	} else {
		yajl_gen_config(g, yajl_gen_validate_utf8, 0);
	}
	
    if(rb_hash_aref(options, ID2SYM(rb_intern("escape_solidus")) ) == Qtrue) {
		yajl_gen_config(g, yajl_gen_escape_solidus, 1);
	} else {
		yajl_gen_config(g, yajl_gen_escape_solidus, 0);
	}
}