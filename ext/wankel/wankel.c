#include "wankel.h"

static VALUE wankel_parser_initialize(VALUE self) {
    wankel_parser * parser;

    Data_Get_Struct(self, wankel_parser, parser);

    parser->alloced = 0;

    return self;
}

static yajl_callbacks wankel_parser_callbacks(VALUE self) {
    yajl_callbacks callbacks = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

    if(rb_respond_to(self, intern_on_null)) {
        callbacks.yajl_null = wankel_parser_callback_on_null;
    }
    if(rb_respond_to(self, intern_on_boolean)) {
        callbacks.yajl_boolean = wankel_parser_callback_on_boolean;
    }
    if(rb_respond_to(self, intern_on_integer)) {
        callbacks.yajl_integer = wankel_parser_callback_on_integer;
    }
    if(rb_respond_to(self, intern_on_double)) {
        callbacks.yajl_double = wankel_parser_callback_on_double;
    }
    if(rb_respond_to(self, intern_on_number)) {
        callbacks.yajl_number = wankel_parser_callback_on_number;
    }

    return callbacks;
}

static VALUE wankel_parser_parse(int argc, VALUE * argv, VALUE self) {
    yajl_callbacks callbacks;
    yajl_alloc_funcs alloc_funcs;
    yajl_status status;
    wankel_parser * parser;
    VALUE rbufsize, input, blk;
    const char * cptr;
    unsigned int len;

    Data_Get_Struct(self, wankel_parser, parser);

    alloc_funcs.malloc = &handle_malloc;
    alloc_funcs.realloc = &handle_realloc;
    alloc_funcs.free = &handle_free;
    alloc_funcs.ctx = NULL;
    callbacks = wankel_parser_callbacks(self);
    parser->handle = yajl_alloc(&callbacks, &alloc_funcs, (void *)self);
    yajl_config(parser->handle, yajl_allow_comments, 1);

    rb_scan_args(argc, argv, "11", &input, &rbufsize);
    if(NIL_P(rbufsize)) {
        rbufsize = INT2FIX(READ_BUFSIZE);
    } else {
        Check_Type(rbufsize, T_FIXNUM);
    }
    if (TYPE(input) == T_STRING) {
        cptr = RSTRING_PTR(input);
        len = RSTRING_LEN(input);
        status = yajl_parse(parser->handle, (const unsigned char*)cptr, len);
        printf("%d\n", yajl_status_ok);
        printf("%d\n", yajl_status_client_canceled);
        printf("%d\n", yajl_status_error);
        printf("parsed\n");
        yajl_complete_parse(parser->handle);
    } else if (rb_respond_to(input, intern_io_read)) {
        printf("io\n");
    } else {
        rb_raise(e_parseError, "input must be a string or an IO");
    }

    return Qnil;
}


void Init_wankel() {
    m_wankel = rb_define_module("Wankel");

    e_parseError = rb_define_class_under(m_wankel, "ParseError", rb_eStandardError);
    e_encodeError = rb_define_class_under(m_wankel, "EncodeError", rb_eStandardError);

    c_Parser = rb_define_class_under(m_wankel, "SaxParser", rb_cObject);
    rb_define_alloc_func(c_Parser, wankel_parser_alloc);
    rb_define_method(c_Parser, "initialize", wankel_parser_initialize, 0);
    rb_define_method(c_Parser, "parse", wankel_parser_parse, -1);

    intern_io_read = rb_intern("read");
    intern_on_null = rb_intern("on_null");
    intern_on_boolean = rb_intern("on_boolean");
    intern_on_integer = rb_intern("on_integer");
    intern_on_double = rb_intern("on_double");
    intern_on_number = rb_intern("on_number");
    intern_on_string = rb_intern("on_string");
    intern_on_start_map = rb_intern("on_start_map");
    intern_on_map_key = rb_intern("on_map_key");
    intern_on_end_map = rb_intern("on_end_map");
    intern_on_start_array = rb_intern("on_start_array");
    intern_on_end_array = rb_intern("on_end_array");
}

// Callbacks =================================================================

static int wankel_parser_callback_on_null(void *ctx) {
    rb_funcall((VALUE)ctx, intern_on_null, 0);
    return 1;
}

static int wankel_parser_callback_on_boolean(void *ctx, int boolVal) {
    rb_funcall((VALUE)ctx, intern_on_boolean, 1, (boolVal ? Qtrue : Qfalse));
    return 1;
}

static int wankel_parser_callback_on_integer(void *ctx, long long integerVal) {
    rb_funcall((VALUE)ctx, intern_on_integer, 1, LL2NUM(integerVal));
    return 1;
}
static int wankel_parser_callback_on_double(void *ctx, double doubleVal) {
    rb_funcall((VALUE)ctx, intern_on_double, rb_float_new(doubleVal));
    return 1;
}
static int wankel_parser_callback_on_number(void *ctx, const char * numberVal, size_t numberLen) {
    rb_funcall((VALUE)ctx, intern_on_number, 1, rb_str_new(numberVal, numberLen));
    return 1;
}
static int wankel_parser_callback_on_string(void *ctx, const char * stringVal, size_t stringLen) {
    rb_funcall((VALUE)ctx, intern_on_string, 0);
    return 1;
}
static int wankel_parser_callback_on_start_map(void *ctx) {
    rb_funcall((VALUE)ctx, intern_on_start_map, 0);
    return 1;
}
static int wankel_parser_callback_on_map_key(void *ctx, const unsigned char * key, size_t keyLen) {
    rb_funcall((VALUE)ctx, intern_on_map_key, 0);
    return 1;
}
static int wankel_parser_callback_on_end_map(void *ctx) {
    rb_funcall((VALUE)ctx, intern_on_end_map, 0);
    return 1;
}
static int wankel_parser_callback_on_start_array(void *ctx) {
    rb_funcall((VALUE)ctx, intern_on_start_array, 0);
    return 1;
}
static int wankel_parser_callback_on_end_array(void *ctx) {
    rb_funcall((VALUE)ctx, intern_on_end_array, 0);
    return 1;
}

// Ruby GC ===================================================================
static VALUE wankel_parser_alloc(VALUE klass) {
    wankel_parser * parser;
    return Data_Wrap_Struct(klass, 0, wankel_parser_free, parser);
}

// void wankel_parser_mark(void * parser) {
//     wankel_parser * p = parser;
//     if(p) { }
// }

static void wankel_parser_free(void * parser) {
    wankel_parser * p = parser;
    if(p->alloced) {
        yajl_free(p->handle);
    }
}

// Memory funcs ==============================================================
static void * handle_malloc(void *ctx, size_t size) {
    return xmalloc(size);
}
static void * handle_realloc(void *ctx, void *ptr, size_t size) {
    return xrealloc(ptr, size);
}
static void handle_free(void *ctx, void *ptr) {
    return xfree(ptr);
}
