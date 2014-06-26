#include "wankel_parser.h"

// Callbacks =================================================================
static int wankel_parse_callback_on_null(void *ctx);
static int wankel_parse_callback_on_boolean(void *ctx, int boolVal);
// static int wankel_parse_callback_on_integer(void *ctx, long long integerVal);
// static int wankel_parse_callback_on_double(void *ctx, double doubleVal);
static int wankel_parse_callback_on_number(void *ctx, const char * numberVal, size_t numberLen);
static int wankel_parse_callback_on_string(void *ctx, const unsigned char * stringVal, size_t stringLen);
static int wankel_parse_callback_on_map_start(void *ctx);
static int wankel_parse_callback_on_map_key(void *ctx, const unsigned char * key, size_t keyLen);
static int wankel_parse_callback_on_map_end(void *ctx);
static int wankel_parse_callback_on_array_start(void *ctx);
static int wankel_parse_callback_on_array_end(void *ctx);

static yajl_callbacks callbacks = {
    wankel_parse_callback_on_null,
    wankel_parse_callback_on_boolean,
    NULL,
    NULL,
    wankel_parse_callback_on_number,
    wankel_parse_callback_on_string,
    wankel_parse_callback_on_map_start,
    wankel_parse_callback_on_map_key,
    wankel_parse_callback_on_map_end,
    wankel_parse_callback_on_array_start,
    wankel_parse_callback_on_array_end
};

// Ruby GC ===================================================================
static VALUE wankel_alloc(VALUE klass);
static void wankel_free(void * handle);
static void wankel_mark(void * handle);

static ID intern_io_read, intern_clone, intern_merge, intern_call,
          intern_DEFAULTS, sym_multiple_values;
   
static ID sym_read_buffer_size, sym_symbolize_keys; 

static VALUE c_wankel, c_wankelParser, e_parseError, e_encodeError;

/*
 * Document-method: new
 *
 * call-seq: new([options])
 *
 * +:symbolize_keys+ will turn hash keys into Ruby symbols, defaults to false.
 *                   Default `false`.
 *
 * +:allow_comments+ will ignore javascript style comments in JSON input.
 *                   Default `false`.
 *
 * +:check_utf8+ will verify that all strings in JSON input are valid UTF8
 *               and will emit a parse error if this is not so. This option
 *               makes parsing slightly more expensive (~7% depending on
 *               processor and compiler in use). Default `false`.
 *
 * +:allow_trailing_garbage+ will ensure the entire input text was consumed and
 *                           will raise an error otherwise. Default `false`.
 *
 * +:multiple_values+ allow multiple values to be parsed by a single parser. The
 *                  entire text must be valid JSON, and values can be seperated
 *                  by any kind of whitespace. Default `false`.
 *
 * +:allow_partial_values+ check that the top level value was completely consumed/
 *                       Default `false`.
 *
 * 
 * +:read_buffer_size+ is the size of chunk that will be parsed off the input
 *                     (if it's an IO) for each loop of the parsing process.
 *                     8092 is a good balance between the different types of
 *                     streams (off disk, off a socket, etc...), but this option
 *                     is here so the caller can better tune their parsing depending
 *                     on the type of stream being passed. A larger read buffer
 *                     will perform better for files off disk, where as a smaller
 *                     size may be more efficient for reading off of a socket
 *                     directly.
 */
static VALUE wankelParser_initialize(int argc, VALUE * argv, VALUE self) {
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
    rbufsize = rb_hash_aref(options, sym_read_buffer_size);
    Check_Type(rbufsize, T_FIXNUM);
	
    Data_Get_Struct(self, wankel_parser, p);
    p->alloc_funcs.malloc = yajl_helper_malloc;
    p->alloc_funcs.realloc = yajl_helper_realloc;
    p->alloc_funcs.free = yajl_helper_free;
    p->rbufsize = rbufsize;

    if(rb_hash_aref(options, sym_symbolize_keys) == Qtrue) {
        p->symbolize_keys = 1;
    } else {
        p->symbolize_keys = 0;
    }
    
    return self;
}

/*
 * Document-method: parse
 *
 * call-seq: parse(input[, &block])
 *
 * input can either be a String or and IO Object
 *
 * If a block is passed, it is called when the input is finished parsing. If
 * parsing multiple json values in an input it is called once for each value
 *
 */
static VALUE wankelParser_parse(int argc, VALUE * argv, VALUE self) {
    const char * cptr;
    unsigned int len;
    yajl_status status;
    wankel_parser * p;
    VALUE input, callback;
    VALUE options = rb_iv_get(self, "@options");
    rb_scan_args(argc, argv, "11", &input, &callback); // Hack, cuz i'm not sure how to call a method with a block from c
    
    if(callback == Qnil && rb_block_given_p()) {
        callback = rb_block_proc();
    }
        
    Data_Get_Struct(self, wankel_parser, p);
    p->h = yajl_alloc(&callbacks, &p->alloc_funcs, (void *)p);
    yajl_configure(p->h, options);
    p->callback = callback;
    p->stack = rb_ary_new();
    p->stack_index = 0;
    if (TYPE(input) == T_STRING) {
        cptr = RSTRING_PTR(input);
        len = (unsigned int)RSTRING_LEN(input);
        status = yajl_parse(p->h, (const unsigned char*)cptr, len);
        yajl_helper_check_status(p->h, status, 1, (const unsigned char*)cptr, len);
    } else if (rb_respond_to(input, intern_io_read)) {
        VALUE chunk = rb_str_new(0, NUM2LONG(p->rbufsize));
        while (rb_funcall(input, intern_io_read, 2, p->rbufsize, chunk) != Qnil) {
            cptr = RSTRING_PTR(chunk);
            len = (unsigned int)RSTRING_LEN(chunk);
            status = yajl_parse(p->h, (const unsigned char*)cptr, len);
            yajl_helper_check_status(p->h, status, 1, (const unsigned char*)cptr, len);
        }
    } else {
        rb_raise(e_parseError, "input must be a string or an IO");
    }
    
    status = yajl_complete_parse(p->h);
    if(status != yajl_status_ok) {
        rb_raise(e_parseError, "Error completing parse");
    }
    
    if(rb_block_given_p()) {
        return Qnil;
    } else if(rb_hash_aref(options, sym_multiple_values) == Qtrue) {
        return p->stack;
    } else {
        return rb_ary_pop(p->stack);
    }
}

ID Init_wankel_parser() {
    c_wankel = rb_const_get(rb_cObject, rb_intern("Wankel"));
    c_wankelParser = rb_define_class_under(c_wankel, "Parser", rb_cObject);
    e_parseError = rb_const_get(c_wankel, rb_intern("ParseError"));
    e_encodeError = rb_const_get(c_wankel, rb_intern("EncodeError"));

    rb_define_alloc_func(c_wankelParser, wankel_alloc);
    rb_define_method(c_wankelParser, "initialize", wankelParser_initialize, -1);
    rb_define_method(c_wankelParser, "parse", wankelParser_parse, -1);

    intern_io_read = rb_intern("read");
    intern_clone = rb_intern("clone");
    intern_merge = rb_intern("merge");
    intern_call = rb_intern("call");
    intern_DEFAULTS = rb_intern("DEFAULTS");
    sym_read_buffer_size = ID2SYM(rb_intern("read_buffer_size"));
    sym_symbolize_keys = ID2SYM(rb_intern("symbolize_keys"));
    sym_multiple_values = ID2SYM(rb_intern("multiple_values"));
    
    return c_wankelParser;
}
// Ruby GC ===================================================================
static VALUE wankel_alloc(VALUE klass) {
	VALUE self;
    wankel_parser * p;
	self = Data_Make_Struct(klass, wankel_parser, wankel_mark, wankel_free, p);
	p->h = 0;
	return self;
}

static void wankel_free(void * handle) {
    wankel_parser * p = handle;
	if (p->h){
		yajl_free(p->h);
	}
}

static void wankel_mark(void * handle) {
    wankel_parser * p = handle;
    rb_gc_mark(p->stack);
    rb_gc_mark(p->rbufsize);
}

// Parse Builder =============================================================
static void wankel_builder_push(void *ctx, VALUE val) {
    int len;
    wankel_parser * p = ctx;
    VALUE lastEntry, hash;
    
    if (p->stack_index > 0) {
        len = (int)RARRAY_LEN(p->stack);
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

// static int wankel_parse_callback_on_integer(void *ctx, long long integerVal) {
// 	wankel_builder_push(ctx, LL2NUM(integerVal));
//     return 1;
// }
// static int wankel_parse_callback_on_double(void *ctx, double doubleVal) {
// 	wankel_builder_push(ctx, rb_float_new(doubleVal));
//     return 1;
// }
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
static int wankel_parse_callback_on_string(void *ctx, const unsigned char * stringVal, size_t stringLen) {
	VALUE str = rb_str_new((const char *)stringVal, stringLen);
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
	VALUE str = rb_str_new((const char *)key, keyLen);
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