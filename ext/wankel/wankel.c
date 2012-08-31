#include <ruby.h>
#include <ruby/encoding.h>
#include <yajl/yajl_common.h>
#include <yajl/yajl_parse.h>

#include "wankel.h"
#include "sax_parser.h"
#include "yajl_helpers.h"

static VALUE wankel_parse(VALUE klass, VALUE input) {
    const char * cptr;
    unsigned int len;
    yajl_status status;
    VALUE rbufsize;
	wankel_parser p;
	
    p.callbacks = wankel_parse_callbacks();
	p.alloc_funcs.malloc = yajl_helper_malloc;
	p.alloc_funcs.realloc = yajl_helper_realloc;
	p.alloc_funcs.free = yajl_helper_free;
	p.alloc_funcs.ctx = NULL;
	p.builderStack = rb_ary_new();
    p.h = yajl_alloc(&p.callbacks, &p.alloc_funcs, (void *)&p);
    // yajl_config(p->h, yajl_allow_comments, 1);

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

	rb_define_singleton_method(m_wankel, "parse", wankel_parse, 1);
	
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
    callbacks.yajl_start_map = wankel_parse_callback_on_start_map;
    callbacks.yajl_map_key = wankel_parse_callback_on_map_key;
    callbacks.yajl_end_map = wankel_parse_callback_on_end_map;
    callbacks.yajl_start_array = wankel_parse_callback_on_start_array;
    callbacks.yajl_end_array = wankel_parse_callback_on_end_array;

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
	wankel_builder_push(ctx, rb_str_new(stringVal, stringLen));
    return 1;
}
static int wankel_parse_callback_on_start_map(void *ctx) {
	wankel_builder_push(ctx, rb_hash_new());
    return 1;
}
static int wankel_parse_callback_on_map_key(void *ctx, const unsigned char * key, size_t keyLen) {
	VALUE stringEncoded = rb_str_new(key, keyLen);
	rb_enc_associate(stringEncoded, rb_utf8_encoding());
	wankel_builder_push(ctx, ID2SYM(rb_to_id(stringEncoded)) );
    return 1;
}
static int wankel_parse_callback_on_end_map(void *ctx) {
	wankel_parser * p = ctx;
	if(RARRAY_LEN(p->builderStack) > 1) {
		rb_ary_pop(p->builderStack);
	}
    return 1;
}
static int wankel_parse_callback_on_start_array(void *ctx) {
	wankel_builder_push(ctx, rb_ary_new());
    return 1;
}
static int wankel_parse_callback_on_end_array(void *ctx) {
	wankel_parser * p = ctx;
	if(RARRAY_LEN(p->builderStack) > 1) {
		rb_ary_pop(p->builderStack);
	}
    return 1;
}