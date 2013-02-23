#include <ruby.h>
#include <ruby/encoding.h>
#include <yajl/yajl_common.h>
#include <yajl/yajl_parse.h>
#include <stdio.h>

#include "wankel.h"
#include "sax_parser.h"
#include "yajl_helpers.h"

static VALUE wankel_parse(int argc, VALUE * argv, VALUE self) {
	const char * cptr;
	unsigned int len;
	yajl_status status;
	wankel_parser p;
    VALUE rbufsize, input, options, option;
	rb_scan_args(argc, argv, "11", &input, &options);

    p.callbacks = wankel_parse_callbacks();
	p.alloc_funcs.malloc = yajl_helper_malloc;
	p.alloc_funcs.realloc = yajl_helper_realloc;
	p.alloc_funcs.free = yajl_helper_free;
	p.alloc_funcs.ctx = NULL;
	p.builderStack = rb_ary_new();
    p.h = yajl_alloc(&p.callbacks, &p.alloc_funcs, (void *)&p);
	
	if(NIL_P(options)) {
		rbufsize = INT2FIX(READ_BUFSIZE);
		p.symbolizeKeys = 0;
		yajl_config(p.h, yajl_allow_comments, 1);
		yajl_config(p.h, yajl_dont_validate_strings, 1);
		yajl_config(p.h, yajl_allow_trailing_garbage, 0);
		yajl_config(p.h, yajl_allow_multiple_values, 0);
		yajl_config(p.h, yajl_allow_partial_values, 0);
    } else {
		Check_Type(options, T_HASH);
		
		// rbufsize = rb_hash_aref(options, intern_buffer_size);
		// if(RTEST(rbufsize)) {
		// 	Check_Type(rbufsize, T_FIXNUM);
		// 	rbufsize = INT2FIX(READ_BUFSIZE);
		// } else {
			rbufsize = INT2FIX(READ_BUFSIZE);
		// }
		
		option = rb_hash_aref(options, ID2SYM(intern_symbolize_keys));
		if(RTEST(option)) {
			p.symbolizeKeys = 1;
		} else {
			p.symbolizeKeys = 0;
		}
		
		option = rb_hash_aref(options, ID2SYM(intern_allow_comments));
		if(RTEST(option)) {
			yajl_config(p.h, yajl_allow_comments, 1);
		} else {
			yajl_config(p.h, yajl_allow_comments, 0);
		}
		
		option = rb_hash_aref(options, ID2SYM(intern_validate_strings));
		if(RTEST(option)) {
			yajl_config(p.h, yajl_dont_validate_strings, 0);
		} else {
			yajl_config(p.h, yajl_dont_validate_strings, 1);
		}
		
		option = rb_hash_aref(options, ID2SYM(intern_trailing_garbage));
		if(RTEST(option)) {
			yajl_config(p.h, yajl_allow_trailing_garbage, 1);
		} else {
			yajl_config(p.h, yajl_allow_trailing_garbage, 0);
		}		
		
		option = rb_hash_aref(options, ID2SYM(intern_multiple_values));
		if(RTEST(option)) {
			yajl_config(p.h, yajl_allow_multiple_values, 1);
		} else {
			yajl_config(p.h, yajl_allow_multiple_values, 0);
		}
		
		option = rb_hash_aref(options, ID2SYM(intern_partial_values));
		if(RTEST(option)) {
			yajl_config(p.h, yajl_allow_partial_values, 1);
		} else {
			yajl_config(p.h, yajl_allow_partial_values, 0);
		}
	}

    if (TYPE(input) == T_STRING) {
        cptr = RSTRING_PTR(input);
        len = RSTRING_LEN(input);
        status = yajl_parse(p.h, (const unsigned char*)cptr, len);
        yajl_helper_check_status(p.h, status, 1, (const unsigned char*)cptr, len);
    } else if (rb_respond_to(input, intern_io_read)) {
        VALUE chunk = rb_str_new(0, FIX2LONG(rbufsize));
        while (rb_funcall(input, intern_io_read, 2, rbufsize, chunk) != Qnil) {
            cptr = RSTRING_PTR(chunk);
            len = RSTRING_LEN(chunk);
            status = yajl_parse(p.h, (const unsigned char*)cptr, len);
            yajl_helper_check_status(p.h, status, 1, (const unsigned char*)cptr, len);
        }
    } else {
        rb_raise(e_parseError, "input must be a string or an IO");
    }
    status = yajl_complete_parse(p.h);
    yajl_helper_check_status(p.h, status, 0, NULL, NULL);

	return rb_ary_pop(p.builderStack);
}

void Init_wankel() {
    m_wankel = rb_define_module("Wankel");

    e_parseError = rb_define_class_under(m_wankel, "ParseError", rb_eStandardError);
    e_encodeError = rb_define_class_under(m_wankel, "EncodeError", rb_eStandardError);

	rb_define_singleton_method(m_wankel, "parse", wankel_parse, -1);
	
    intern_io_read = rb_intern("read");
    intern_symbolize_keys = rb_intern("symbolize_keys");
    intern_buffer_size = rb_intern("buffer_size");
    intern_allow_comments = rb_intern("allow_comments");
    intern_validate_strings = rb_intern("validate_strings");
    intern_trailing_garbage = rb_intern("trailing_garbage");
    intern_multiple_values = rb_intern("multiple_values");
    intern_partial_values = rb_intern("partial_values");
	
	Init_sax_parser();
}


// Parse Builder =============================================================
static wankel_builder_push(void *ctx, VALUE val) {
    int len;
	wankel_parser * p = ctx;
    VALUE lastEntry, hash;

    len = RARRAY_LEN(p->builderStack);
    if (len > 0) {
        lastEntry = rb_ary_entry(p->builderStack, len-1);
        switch (TYPE(lastEntry)) {
            case T_ARRAY:
                rb_ary_push(lastEntry, val);
                if (TYPE(val) == T_HASH || TYPE(val) == T_ARRAY) {
                    rb_ary_push(p->builderStack, val);
                }
                break;
            case T_HASH:
                rb_hash_aset(lastEntry, val, Qnil);
                rb_ary_push(p->builderStack, val);
                break;
            case T_STRING:
            case T_SYMBOL:
                hash = rb_ary_entry(p->builderStack, len-2);
                if (TYPE(hash) == T_HASH) {
                    rb_hash_aset(hash, lastEntry, val);
                    rb_ary_pop(p->builderStack);
                    if (TYPE(val) == T_HASH || TYPE(val) == T_ARRAY) {
                        rb_ary_push(p->builderStack, val);
                    }
                }
                break;
        }
    } else {
        rb_ary_push(p->builderStack, val);
    }
}

// Parse Callbacks ===========================================================
static yajl_callbacks wankel_parse_callbacks() {
    yajl_callbacks callbacks = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

    callbacks.yajl_null = wankel_parse_callback_on_null;
    callbacks.yajl_boolean = wankel_parse_callback_on_boolean;
    callbacks.yajl_number = wankel_parse_callback_on_number;
    callbacks.yajl_string = wankel_parse_callback_on_string;
    callbacks.yajl_start_map = wankel_parse_callback_on_map_start;
    callbacks.yajl_map_key = wankel_parse_callback_on_map_key;
    callbacks.yajl_end_map = wankel_parse_callback_on_map_end;
    callbacks.yajl_start_array = wankel_parse_callback_on_array_start;
    callbacks.yajl_end_array = wankel_parse_callback_on_array_end;

    return callbacks;
}

static int wankel_parse_callback_on_null(void *ctx) {
	wankel_builder_push(ctx, Qnil);
    return 1;
}

static int wankel_parse_callback_on_boolean(void *ctx, int boolVal) {
	wankel_builder_push(ctx, boolVal ? Qtrue : Qfalse);
    return 1;
}

static int wankel_parse_callback_on_integer(void *ctx, long long integerVal) {
	printf("%d\n", integerVal);
	wankel_builder_push(ctx, LL2NUM(integerVal));
    return 1;
}
static int wankel_parse_callback_on_double(void *ctx, double doubleVal) {
	printf("%d\n", doubleVal);
	wankel_builder_push(ctx, rb_float_new(doubleVal));
    return 1;
}
static int wankel_parse_callback_on_number(void * ctx, const char * numberVal, size_t numberLen){
	char buf[numberLen+1];
    buf[numberLen] = 0;
    memcpy(buf, numberVal, numberLen);

	if (memchr(buf, '.', numberLen) || memchr(buf, 'e', numberLen) || memchr(buf, 'E', numberLen)) {
		wankel_builder_push(ctx, rb_float_new(strtod(buf, NULL)));
	} else {
		wankel_builder_push(ctx, rb_cstr2inum(buf, 10));
	}
	return 1;
}
static int wankel_parse_callback_on_string(void *ctx, const char * stringVal, size_t stringLen) {
	VALUE str = rb_str_new(stringVal, stringLen);
	rb_encoding *default_internal_enc = rb_default_internal_encoding();
    rb_enc_associate(str, rb_utf8_encoding());
    if (default_internal_enc) {
      str = rb_str_export_to_enc(str, default_internal_enc);
    }

	wankel_builder_push(ctx, str);
    return 1;
}
static int wankel_parse_callback_on_map_start(void *ctx) {
	wankel_builder_push(ctx, rb_hash_new());
    return 1;
}
static int wankel_parse_callback_on_map_key(void *ctx, const unsigned char * key, size_t keyLen) {
	wankel_parser * p = ctx;
	rb_encoding *default_internal_enc = rb_default_internal_encoding();
	VALUE str = rb_str_new(key, keyLen);
	rb_enc_associate(str, rb_utf8_encoding());
    if (default_internal_enc) {
      str = rb_str_export_to_enc(str, default_internal_enc);
    }

	if(p->symbolizeKeys) {
		wankel_builder_push(ctx, ID2SYM(rb_to_id(str)) );
	} else {
		wankel_builder_push(ctx, str);
	}
	
    return 1;
}
static int wankel_parse_callback_on_map_end(void *ctx) {
	wankel_parser * p = ctx;
	if(RARRAY_LEN(p->builderStack) > 1) {
		rb_ary_pop(p->builderStack);
	}
    return 1;
}
static int wankel_parse_callback_on_array_start(void *ctx) {
	wankel_builder_push(ctx, rb_ary_new());
    return 1;
}
static int wankel_parse_callback_on_array_end(void *ctx) {
	wankel_parser * p = ctx;
	if(RARRAY_LEN(p->builderStack) > 1) {
		rb_ary_pop(p->builderStack);
	}
    return 1;
}