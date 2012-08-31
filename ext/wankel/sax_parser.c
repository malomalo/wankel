#include <ruby.h>
#include <ruby/encoding.h>
#include <yajl/yajl_common.h>
#include <yajl/yajl_parse.h>

#include "sax_parser.h"
#include "wankel.h"
#include "yajl_helpers.h"

static VALUE sax_parser_initialize(VALUE self) {
    sax_parser * p;

    Data_Get_Struct(self, sax_parser, p);

    p->callbacks = sax_parser_callbacks(self);
	p->alloc_funcs.malloc = yajl_helper_malloc;
	p->alloc_funcs.realloc = yajl_helper_realloc;
	p->alloc_funcs.free = yajl_helper_free;
	p->alloc_funcs.ctx = NULL;
    p->h = yajl_alloc(&p->callbacks, &p->alloc_funcs, (void *)self);
    // yajl_config(p->h, yajl_allow_comments, 1);
	
    return self;
}

static VALUE sax_parser_parse(int argc, VALUE * argv, VALUE self) {
    const char * cptr;
    unsigned int len;
    yajl_status status;
    VALUE rbufsize, input;
    sax_parser * p;

    Data_Get_Struct(self, sax_parser, p);
	
    rb_scan_args(argc, argv, "11", &input, &rbufsize);
    if(NIL_P(rbufsize)) {
        rbufsize = INT2FIX(READ_BUFSIZE);
    } else {
        Check_Type(rbufsize, T_FIXNUM);
    }
    if (TYPE(input) == T_STRING) {
        cptr = RSTRING_PTR(input);
        len = RSTRING_LEN(input);
        status = yajl_parse(p->h, (const unsigned char*)cptr, len);
        yajl_helper_check_status(p->h, status, 1, (const unsigned char*)cptr, len);
    } else if (rb_respond_to(input, intern_io_read)) {
        VALUE chunk = rb_str_new(0, FIX2LONG(rbufsize));
        while (rb_funcall(input, intern_io_read, 2, rbufsize, chunk) != Qnil) {
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

    return Qnil;
}

void Init_sax_parser() {
    m_wankel = rb_const_get(rb_cObject, rb_intern("Wankel"));

    e_parseError = rb_const_get(m_wankel, rb_intern("ParseError"));
    e_encodeError = rb_const_get(m_wankel, rb_intern("EncodeError"));

    c_saxParser = rb_define_class_under(m_wankel, "SaxParser", rb_cObject);
    rb_define_alloc_func(c_saxParser, sax_parser_alloc);
    rb_define_method(c_saxParser, "initialize", sax_parser_initialize, 0);
    rb_define_method(c_saxParser, "parse", sax_parser_parse, -1);

    intern_io_read = rb_intern("read");
    intern_on_null = rb_intern("on_null");
    intern_on_boolean = rb_intern("on_boolean");
    intern_on_integer = rb_intern("on_integer");
    intern_on_double = rb_intern("on_double");
    intern_on_string = rb_intern("on_string");
    intern_on_start_map = rb_intern("on_start_map");
    intern_on_map_key = rb_intern("on_map_key");
    intern_on_end_map = rb_intern("on_end_map");
    intern_on_start_array = rb_intern("on_start_array");
    intern_on_end_array = rb_intern("on_end_array");
}

// Callbacks =================================================================
static yajl_callbacks sax_parser_callbacks(VALUE self) {
    yajl_callbacks callbacks = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

    if(rb_respond_to(self, intern_on_null)) {
        callbacks.yajl_null = sax_parser_callback_on_null;
    }
    if(rb_respond_to(self, intern_on_boolean)) {
        callbacks.yajl_boolean = sax_parser_callback_on_boolean;
    }
    if(rb_respond_to(self, intern_on_integer) || rb_respond_to(self, intern_on_double)) {
        callbacks.yajl_number = sax_parser_callback_on_number;
    }
    if(rb_respond_to(self, intern_on_string)) {
        callbacks.yajl_string = sax_parser_callback_on_string;
    }
    if(rb_respond_to(self, intern_on_start_map)) {
        callbacks.yajl_start_map = sax_parser_callback_on_start_map;
    }
    if(rb_respond_to(self, intern_on_map_key)) {
        callbacks.yajl_map_key = sax_parser_callback_on_map_key;
    }
    if(rb_respond_to(self, intern_on_end_map)) {
        callbacks.yajl_end_map = sax_parser_callback_on_end_map;
    }
    if(rb_respond_to(self, intern_on_start_array)) {
        callbacks.yajl_start_array = sax_parser_callback_on_start_array;
    }
    if(rb_respond_to(self, intern_on_end_array)) {
        callbacks.yajl_end_array = sax_parser_callback_on_end_array;
    }

    return callbacks;
}

static int sax_parser_callback_on_null(void *ctx) {
    rb_funcall((VALUE)ctx, intern_on_null, 0);
    return 1;
}

static int sax_parser_callback_on_boolean(void *ctx, int boolVal) {
    rb_funcall((VALUE)ctx, intern_on_boolean, 1, (boolVal ? Qtrue : Qfalse));
    return 1;
}

static int sax_parser_callback_on_number(void *ctx, const char * numberVal, size_t numberLen) {
	char buf[numberLen+1];
	buf[numberLen] = 0;
	memcpy(buf, numberVal, numberLen);
	VALUE obj = ctx;

	if (memchr(buf, '.', numberLen) || memchr(buf, 'e', numberLen) || memchr(buf, 'E', numberLen)) {
		if (rb_respond_to(obj, intern_on_double)) {
			rb_funcall((VALUE)ctx, intern_on_double, 1, rb_float_new(strtod(buf, NULL)));			
		}
	} else {
		if (rb_respond_to(obj, intern_on_integer)) {
			rb_funcall((VALUE)ctx, intern_on_integer, 1, rb_cstr2inum(buf, 10));
		}
	}
	return 1;
}

static int sax_parser_callback_on_string(void *ctx, const char * stringVal, size_t stringLen) {
    rb_funcall((VALUE)ctx, intern_on_string, 1, rb_str_new(stringVal, stringLen));
    return 1;
}
static int sax_parser_callback_on_start_map(void *ctx) {
    rb_funcall((VALUE)ctx, intern_on_start_map, 0);
    return 1;
}
static int sax_parser_callback_on_map_key(void *ctx, const unsigned char * key, size_t keyLen) {
    rb_funcall((VALUE)ctx, intern_on_map_key, 1, rb_str_new(key, keyLen));
    return 1;
}
static int sax_parser_callback_on_end_map(void *ctx) {
    rb_funcall((VALUE)ctx, intern_on_end_map, 0);
    return 1;
}
static int sax_parser_callback_on_start_array(void *ctx) {
    rb_funcall((VALUE)ctx, intern_on_start_array, 0);
    return 1;
}
static int sax_parser_callback_on_end_array(void *ctx) {
    rb_funcall((VALUE)ctx, intern_on_end_array, 0);
    return 1;
}

// Ruby GC ===================================================================
static VALUE sax_parser_alloc(VALUE klass) {
    sax_parser * p;
    VALUE obj = Data_Make_Struct(klass, sax_parser, 0, sax_parser_free, p);
    return obj;
}

// void sax_parser_mark(void * parser) {
//     sax_parser * p = parser;
//     if(p) { }
// }

static void sax_parser_free(void * handle) {
    sax_parser * p = handle;
	yajl_free(p->h);
}