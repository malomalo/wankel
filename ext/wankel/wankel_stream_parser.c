#include "wankel_stream_parser.h"

static VALUE stream_parser_initialize(int argc, VALUE * argv, VALUE self);

static ID  sym_read_buffer_size, sym_symbolize_keys;

static ID  intern_io_read, intern_merge, intern_clone, intern_DEFAULTS;

static ID  intern_on_null, intern_on_boolean, intern_on_integer,
    intern_on_double, intern_on_string, intern_on_map_start, intern_on_map_key,
    intern_on_map_end, intern_on_array_start, intern_on_array_end;
	
static VALUE c_wankel, c_wankelParser, c_streamParser, e_parseError, e_encodeError;
		   
// Callbacks =================================================================
yajl_callbacks stream_parser_callbacks(VALUE self);
int stream_parser_callback_on_null(void *ctx);
int stream_parser_callback_on_boolean(void *ctx, int boolVal);
int stream_parser_callback_on_number(void *ctx, const char * numberVal, size_t numberLen);
int stream_parser_callback_on_string(void *ctx, const unsigned char * stringVal, size_t stringLen);
int stream_parser_callback_on_map_start(void *ctx);
int stream_parser_callback_on_map_key(void *ctx, const unsigned char * key, size_t keyLen);
int stream_parser_callback_on_map_end(void *ctx);
int stream_parser_callback_on_array_start(void *ctx);
int stream_parser_callback_on_array_end(void *ctx);

// Ruby GC ===================================================================
VALUE stream_parser_alloc(VALUE);
//void stream_parser_mark(void *);
void stream_parser_free(void * parser);

VALUE stream_parser_initialize(int argc, VALUE * argv, VALUE self) {
    VALUE defaults = rb_const_get(c_wankel, intern_DEFAULTS);
    VALUE klass = rb_funcall(self, rb_intern("class"), 0);
    VALUE options, rbufsize;
    stream_parser * p;

    rb_scan_args(argc, argv, "01", &options);
    if (rb_const_defined(klass, intern_DEFAULTS)) { 
        defaults = rb_funcall(defaults, intern_merge, 1, rb_const_get(klass, intern_DEFAULTS));
    }
    if(options == Qnil) {
        rb_iv_set(self, "@options", rb_funcall(defaults, intern_clone, 0) );
    } else {
        Check_Type(options, T_HASH);
        rb_iv_set(self, "@options", rb_funcall(defaults, intern_merge, 1, options) );
    }
    options = rb_iv_get(self, "@options");

    Data_Get_Struct(self, stream_parser, p);
    p->callbacks = stream_parser_callbacks(self);
    p->alloc_funcs.malloc = yajl_helper_malloc;
    p->alloc_funcs.realloc = yajl_helper_realloc;
    p->alloc_funcs.free = yajl_helper_free;
    p->h = yajl_alloc(&p->callbacks, &p->alloc_funcs, (void *)self);
    
    yajl_configure(p->h, options);
        
    rbufsize = rb_hash_aref(options, sym_read_buffer_size);
    Check_Type(rbufsize, T_FIXNUM);
    p->rbufsize = rbufsize;
    
    if(rb_hash_aref(options, sym_symbolize_keys) == Qtrue) {
        p->symbolize_keys = 1;
    } else {
        p->symbolize_keys = 0;
    }
    
    return self;
}

static VALUE stream_parser_parse(int argc, VALUE * argv, VALUE self) {
    const char * cptr;
    size_t len;
    yajl_status status;
    stream_parser * p;
    VALUE input;
    Data_Get_Struct(self, stream_parser, p);
	
    rb_scan_args(argc, argv, "10", &input);
    if (TYPE(input) == T_STRING) {
        cptr = RSTRING_PTR(input);
        len = RSTRING_LEN(input);
        status = yajl_parse(p->h, (const unsigned char*)cptr, len);
        yajl_helper_check_status(p->h, status, 1, (const unsigned char*)cptr, len);
    } else if (rb_respond_to(input, intern_io_read)) {
        VALUE chunk = rb_str_new(0, FIX2LONG(p->rbufsize));
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
    yajl_helper_check_status(p->h, status, 0, NULL, 0);
    
    return Qnil;
}

static VALUE stream_parser_write(VALUE self, VALUE input) {
    const char * cptr;
    size_t len;
    yajl_status status;
    stream_parser * p;
    Data_Get_Struct(self, stream_parser, p);
    
    Check_Type(input, T_STRING);
    cptr = RSTRING_PTR(input);
    len = RSTRING_LEN(input);
    status = yajl_parse(p->h, (const unsigned char*)cptr, len);
    yajl_helper_check_status(p->h, status, 1, (const unsigned char*)cptr, len);
    
    return Qnil;
}

static VALUE stream_parser_complete(VALUE self) {
    yajl_status status;
    stream_parser * p;
    Data_Get_Struct(self, stream_parser, p);
    

    status = yajl_complete_parse(p->h);
    yajl_helper_check_status(p->h, status, 0, NULL, 0);

    return Qnil;
}

void Init_wankel_stream_parser() {
    c_wankel = rb_const_get(rb_cObject, rb_intern("Wankel"));
    c_wankelParser = rb_const_get(c_wankel, rb_intern("Parser"));
    c_streamParser = rb_define_class_under(c_wankel, "StreamParser", rb_cObject);
    e_parseError = rb_const_get(c_wankel, rb_intern("ParseError"));
    e_encodeError = rb_const_get(c_wankel, rb_intern("EncodeError"));
    
    rb_define_alloc_func(c_streamParser, stream_parser_alloc);
    rb_define_method(c_streamParser, "initialize", stream_parser_initialize, -1);
    rb_define_method(c_streamParser, "parse", stream_parser_parse, -1);
    rb_define_method(c_streamParser, "write", stream_parser_write, 1);
    rb_define_method(c_streamParser, "complete", stream_parser_complete, 0);
    
    rb_define_alias(c_streamParser, "<<", "write");
    
    intern_merge = rb_intern("merge");
    intern_clone = rb_intern("clone");
    intern_DEFAULTS = rb_intern("DEFAULTS");
    
    sym_read_buffer_size = ID2SYM(rb_intern("read_buffer_size"));
    sym_symbolize_keys = ID2SYM(rb_intern("symbolize_keys"));
    
    intern_on_null = rb_intern("on_null");
    intern_on_boolean = rb_intern("on_boolean");
    intern_on_integer = rb_intern("on_integer");
    intern_on_double = rb_intern("on_double");
    intern_on_string = rb_intern("on_string");
    intern_on_map_start = rb_intern("on_map_start");
    intern_on_map_key = rb_intern("on_map_key");
    intern_on_map_end = rb_intern("on_map_end");
    intern_on_array_start = rb_intern("on_array_start");
    intern_on_array_end = rb_intern("on_array_end");
}

// Callbacks =================================================================
yajl_callbacks stream_parser_callbacks(VALUE self) {
    yajl_callbacks callbacks = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

    if(rb_respond_to(self, intern_on_null)) {
        callbacks.yajl_null = stream_parser_callback_on_null;
    }
    if(rb_respond_to(self, intern_on_boolean)) {
        callbacks.yajl_boolean = stream_parser_callback_on_boolean;
    }
    if(rb_respond_to(self, intern_on_integer) || rb_respond_to(self, intern_on_double)) {
        callbacks.yajl_number = stream_parser_callback_on_number;
    }
    if(rb_respond_to(self, intern_on_string)) {
        callbacks.yajl_string = stream_parser_callback_on_string;
    }

    if(rb_respond_to(self, intern_on_map_start)) {
        callbacks.yajl_start_map = stream_parser_callback_on_map_start;
    }
    if(rb_respond_to(self, intern_on_map_key)) {
        callbacks.yajl_map_key = stream_parser_callback_on_map_key;
    }
    if(rb_respond_to(self, intern_on_map_end)) {
        callbacks.yajl_end_map = stream_parser_callback_on_map_end;
     }
    if(rb_respond_to(self, intern_on_array_start)) {
        callbacks.yajl_start_array = stream_parser_callback_on_array_start;
    }
    if(rb_respond_to(self, intern_on_array_end)) {
        callbacks.yajl_end_array = stream_parser_callback_on_array_end;
    }

    return callbacks;
}

int stream_parser_callback_on_null(void *ctx) {
    rb_funcall((VALUE)ctx, intern_on_null, 0);
    return 1;
}

int stream_parser_callback_on_boolean(void *ctx, int boolVal) {
    rb_funcall((VALUE)ctx, intern_on_boolean, 1, (boolVal ? Qtrue : Qfalse));
    return 1;
}

int stream_parser_callback_on_number(void *ctx, const char * numberVal, size_t numberLen) {
	char buf[numberLen+1];
	buf[numberLen] = 0;
	memcpy(buf, numberVal, numberLen);
	VALUE obj = (VALUE)ctx;

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

int stream_parser_callback_on_string(void *ctx, const unsigned char * stringVal, size_t stringLen) {
    rb_funcall((VALUE)ctx, intern_on_string, 1, rb_str_new((const char *) stringVal, stringLen));
    return 1;
}
int stream_parser_callback_on_map_start(void *ctx) {
    rb_funcall((VALUE)ctx, intern_on_map_start, 0);
    return 1;
}
int stream_parser_callback_on_map_key(void *ctx, const unsigned char * key, size_t keyLen) {
    rb_funcall((VALUE)ctx, intern_on_map_key, 1, rb_str_new((const char *)key, keyLen));
    return 1;
}
int stream_parser_callback_on_map_end(void *ctx) {
    rb_funcall((VALUE)ctx, intern_on_map_end, 0);
    return 1;
}
int stream_parser_callback_on_array_start(void *ctx) {
    rb_funcall((VALUE)ctx, intern_on_array_start, 0);
    return 1;
}
int stream_parser_callback_on_array_end(void *ctx) {
    rb_funcall((VALUE)ctx, intern_on_array_end, 0);
    return 1;
}

// Ruby GC ===================================================================
VALUE stream_parser_alloc(VALUE klass) {
    stream_parser * p;
    return Data_Make_Struct(klass, stream_parser, 0, stream_parser_free, p);
}

void stream_parser_free(void * handle) {
    stream_parser * p = handle;
	yajl_free(p->h);
}