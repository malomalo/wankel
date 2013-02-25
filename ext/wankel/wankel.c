#include "wankel.h"

static VALUE wankel_initialize(int argc, VALUE * argv, VALUE self) {
    VALUE defaults = rb_const_get(c_wankel, intern_DEFAULTS);
    VALUE options, rbufsize;
    wankel_parser * p;
    
    rb_scan_args(argc, argv, "01", &options);
    if(options == Qnil) {
        rb_iv_set(self, "@options", rb_funcall(defaults, intern_clone, 0) );
    } else {
        Check_Type(options, T_HASH);
        rb_iv_set(self, "@options", rb_funcall(defaults, intern_merge, 1, options) );
    }
    options = rb_iv_get(self, "@options");

    Data_Get_Struct(self, wankel_parser, p);
    p->alloc_funcs.malloc = yajl_helper_malloc;
    p->alloc_funcs.realloc = yajl_helper_realloc;
    p->alloc_funcs.free = yajl_helper_free;
    p->h = yajl_alloc(&callbacks, &p->alloc_funcs, (void *)p);
    
    rbufsize = rb_hash_aref(options, sym_read_buffer_size);
    Check_Type(rbufsize, T_FIXNUM);
    p->rbufsize = rbufsize;

    if(rb_hash_aref(options, sym_symbolize_keys) == Qtrue) {
        p->symbolize_keys = 1;
    } else {
        p->symbolize_keys = 0;
    }
    
    if(rb_hash_aref(options, sym_allow_comments) == Qtrue) {
        yajl_config(p->h, yajl_allow_comments, 1);
    } else {
        yajl_config(p->h, yajl_allow_comments, 0);
    }

    if(rb_hash_aref(options, sym_validate_strings) == Qtrue) {
        yajl_config(p->h, yajl_dont_validate_strings, 0);
    } else {
        yajl_config(p->h, yajl_dont_validate_strings, 1);
    }

    if(rb_hash_aref(options, sym_trailing_garbage) == Qtrue) {
        yajl_config(p->h, yajl_allow_trailing_garbage, 1);
    } else {
        yajl_config(p->h, yajl_allow_trailing_garbage, 0);
    }
    
    if(rb_hash_aref(options, sym_multiple_values) == Qtrue) {
        yajl_config(p->h, yajl_allow_multiple_values, 1);
    } else {
        yajl_config(p->h, yajl_allow_multiple_values, 0);
    }
    
    if(rb_hash_aref(options, sym_partial_values) == Qtrue) {
        yajl_config(p->h, yajl_allow_partial_values, 1);
    } else {
        yajl_config(p->h, yajl_allow_partial_values, 0);
    }
    
    return self;
}

static VALUE wankel_parse(int argc, VALUE * argv, VALUE self) {
    const char * cptr;
    unsigned int len;
    yajl_status status;
    wankel_parser * p;
    VALUE input, options, callback;
    rb_scan_args(argc, argv, "11", &input, &callback); // Hack, cuz i'm not sure how to call a method with a block from c
    
    if(callback == Qnil && rb_block_given_p()) {
        callback = rb_block_proc();
    }
        
    Data_Get_Struct(self, wankel_parser, p);
    p->callback = callback;
    p->stack = rb_ary_new();
    p->stack_index = 0;
    if (TYPE(input) == T_STRING) {
        cptr = RSTRING_PTR(input);
        len = RSTRING_LEN(input);
        status = yajl_parse(p->h, (const unsigned char*)cptr, len);
        yajl_helper_check_status(p->h, status, 1, (const unsigned char*)cptr, len);
    } else if (rb_respond_to(input, intern_io_read)) {
        VALUE chunk = rb_str_new(0, NUM2LONG(p->rbufsize));
        while (rb_funcall(input, intern_io_read, 2, p->rbufsize, chunk) != Qnil) {
            cptr = RSTRING_PTR(chunk);
            len = RSTRING_LEN(chunk);
            status = yajl_parse(p->h, (const unsigned char*)cptr, len);
            yajl_helper_check_status(p->h, status, 1, (const unsigned char*)cptr, len);
        }
    } else {
        rb_raise(e_parseError, "input must be a string or an IO");
    }
    
    status = yajl_complete_parse(p->h);
    yajl_helper_check_status(p->h, status, 0, NULL, NULL);

    options = rb_iv_get(self, "@options");
    if(rb_block_given_p()) {
        return Qnil;
    } else if(rb_hash_aref(options, sym_multiple_values) == Qtrue) {
        return p->stack;
    } else {
        return rb_ary_pop(p->stack);
    }
}

// Class Methods =============================================================
static VALUE wankel_class_parse(int argc, VALUE * argv, VALUE klass) {
    VALUE parser, input, options, callback;
    rb_scan_args(argc, argv, "11&", &input, &options, &callback);
    
    parser = rb_funcall(klass, intern_new, 1, options);
    return rb_funcall(parser, intern_parse, 2, input, callback);
}

void Init_wankel() {
    c_wankel = rb_define_class("Wankel", rb_cObject);
    e_parseError = rb_define_class_under(c_wankel, "ParseError", rb_eStandardError);
    e_encodeError = rb_define_class_under(c_wankel, "EncodeError", rb_eStandardError);

    rb_define_alloc_func(c_wankel, wankel_alloc);
    rb_define_method(c_wankel, "initialize", wankel_initialize, -1);
    rb_define_method(c_wankel, "parse", wankel_parse, -1);
    rb_define_singleton_method(c_wankel, "parse", wankel_class_parse, -1);

    intern_io_read = rb_intern("read");
    intern_new = rb_intern("new");
    intern_clone = rb_intern("clone");
    intern_merge = rb_intern("merge");
    intern_parse = rb_intern("parse");
    intern_call = rb_intern("call");
    intern_DEFAULTS = rb_intern("DEFAULTS");
    sym_read_buffer_size = ID2SYM(rb_intern("read_buffer_size"));
    sym_write_buffer_size = ID2SYM(rb_intern("write_buffer_size"));
    sym_symbolize_keys = ID2SYM(rb_intern("symbolize_keys"));
    sym_allow_comments = ID2SYM(rb_intern("allow_comments"));
    sym_validate_strings = ID2SYM(rb_intern("validate_strings"));
    sym_trailing_garbage = ID2SYM(rb_intern("trailing_garbage"));
    sym_multiple_values = ID2SYM(rb_intern("multiple_values"));
    sym_partial_values = ID2SYM(rb_intern("partial_values"));
    
    Init_sax_parser();
}

// Ruby GC ===================================================================
static VALUE wankel_alloc(VALUE klass) {
    wankel_parser * p;
    return Data_Make_Struct(klass, wankel_parser, wankel_mark, wankel_free, p);
}

static void wankel_free(void * handle) {
    wankel_parser * p = handle;
    yajl_free(p->h);
}

static void wankel_mark(void * handle) {
    wankel_parser * p = handle;
    rb_gc_mark(p->stack);
    rb_gc_mark(p->rbufsize);
}

// Parse Builder =============================================================
static wankel_builder_push(void *ctx, VALUE val) {
    int len;
	wankel_parser * p = ctx;
    VALUE lastEntry, hash;
    VALUE klass = rb_funcall(val, rb_intern("class"), 0);
    len = RARRAY_LEN(p->stack);
    
    // rb_funcall(rb_const_get(rb_cObject,rb_intern("Kernel")), rb_intern("puts"), 1, INT2FIX(p->stack_index));
    // rb_funcall(rb_const_get(rb_cObject,rb_intern("Kernel")), rb_intern("puts"), 1, rb_funcall(p->stack, rb_intern("join"), 1, rb_str_new2(",")));
    if (p->stack_index > 0) {
        lastEntry = rb_ary_entry(p->stack, len-1);
        switch (TYPE(lastEntry)) {
            case T_ARRAY:
                rb_ary_push(lastEntry, val);
                if (TYPE(val) == T_HASH || TYPE(val) == T_ARRAY) {
                    rb_ary_push(p->stack, val);
                    p->stack_index++;
                }
                break;
            case T_HASH:
                rb_hash_aset(lastEntry, val, Qnil);
                rb_ary_push(p->stack, val);
                p->stack_index++;
                break;
            case T_STRING:
            case T_SYMBOL:
                hash = rb_ary_entry(p->stack, len-2);
                if (TYPE(hash) == T_HASH) {
                    rb_hash_aset(hash, lastEntry, val);
                    rb_ary_pop(p->stack);
                    p->stack_index--;
                    if (TYPE(val) == T_HASH || TYPE(val) == T_ARRAY) {
                        rb_ary_push(p->stack, val);
                        p->stack_index++;
                    }
                }
                break;
        }
    } else {
        rb_ary_push(p->stack, val);
        p->stack_index++;
    }
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

	if(p->symbolize_keys) {
		wankel_builder_push(ctx, ID2SYM(rb_to_id(str)) );
	} else {
		wankel_builder_push(ctx, str);
	}
	
    return 1;
}
static int wankel_parse_callback_on_map_end(void *ctx) {
    wankel_parser * p = ctx;
    p->stack_index--;
    
    if(p->stack_index > 0) {
        rb_ary_pop(p->stack);
    } else if(p->stack_index == 0 && p->callback != Qnil) {
        rb_funcall(p->callback, intern_call, 1, rb_ary_pop(p->stack));
    }
    
    return 1;
}
static int wankel_parse_callback_on_array_start(void *ctx) {
	wankel_builder_push(ctx, rb_ary_new());
    return 1;
}
static int wankel_parse_callback_on_array_end(void *ctx) {
	wankel_parser * p = ctx;
    p->stack_index--;
    
    if(p->stack_index > 0) {
        rb_ary_pop(p->stack);
    } else if(p->stack_index == 0 && p->callback != Qnil) {
        rb_funcall(p->callback, intern_call, 1, rb_ary_pop(p->stack));
    }

    return 1;
}