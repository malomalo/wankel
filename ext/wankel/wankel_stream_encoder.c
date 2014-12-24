#include "wankel_stream_encoder.h"

typedef struct {
    yajl_gen g;
	VALUE output;
	int write_buffer_size;
} wankel_encoder;

static VALUE wankelStreamEncoder_initialize(int argc, VALUE * argv, VALUE self);
static VALUE wankelStreamEncoder_number(VALUE self, VALUE number);
static VALUE wankelStreamEncoder_string(VALUE self, VALUE string);
static VALUE wankelStreamEncoder_null(VALUE self);
static VALUE wankelStreamEncoder_boolean(VALUE self, VALUE b);
static VALUE wankelStreamEncoder_map_open(VALUE self);
static VALUE wankelStreamEncoder_map_close(VALUE self);
static VALUE wankelStreamEncoder_array_open(VALUE self);
static VALUE wankelStreamEncoder_array_close(VALUE self);
static VALUE wankelStreamEncoder_flush(VALUE self);
static void wankelStreamEncoder_write_buffer(wankel_encoder * p);
static VALUE wankel_stream_encoder_alloc(VALUE klass);
static void wankel_stream_encoder_free(void * handle);
static void wankel_stream_encoder_mark(void * handle);

static VALUE c_wankel, c_wankelStreamEncoder, e_encodeError;

static ID intern_to_s, intern_keys, intern_io_write, intern_to_json, intern_clone, intern_merge, intern_DEFAULTS;

static ID sym_beautify, sym_indent_string, sym_validate_utf8, sym_escape_solidus;

/*
 * Document-method: new
 *
 * call-seq: new([options])
 *
 * +:beautify+ generate indented (beautiful) output. Default `false`.
 *
 * +:indent_string+ Set an indent string which is used when yajl_gen_beautify
 *                  is enabled. Maybe something like \\t or some number of
 *                  spaces. The default is four spaces ' '.
 *
 * +:validate_utf8+ Normally the generator does not validate that strings you
 *                  pass to it are valid UTF8. Enabling this option will cause
 *                  it to do so.
 *
 * +:escape_solidus+ the forward solidus (slash or '/' in human) is not required
 *                   to be escaped in json text. By default, YAJL will not escape
 *                   it in the iterest of saving bytes. Setting this flag will
 *                   cause YAJL to always escape '/' in generated JSON strings.
 */
static VALUE wankelStreamEncoder_initialize(int argc, VALUE * argv, VALUE self) {
    VALUE defaults = rb_const_get(c_wankel, intern_DEFAULTS);
    VALUE io, options;
	wankel_encoder * p;
    yajl_alloc_funcs alloc_funcs;
	
    rb_scan_args(argc, argv, "11", &io, &options);

    if(options == Qnil) {
        rb_iv_set(self, "@options", rb_funcall(defaults, intern_clone, 0) );
    } else {
        Check_Type(options, T_HASH);
        rb_iv_set(self, "@options", rb_funcall(defaults, intern_merge, 1, options) );
    }
	options = rb_iv_get(self, "@options");
	
	if (!rb_respond_to(io, intern_io_write)) {
		rb_raise(e_encodeError, "output must be a an IO");
	}
    Data_Get_Struct(self, wankel_encoder, p);
	p->output = io;

	alloc_funcs.malloc = yajl_helper_malloc;
	alloc_funcs.realloc = yajl_helper_realloc;
	alloc_funcs.free = yajl_helper_free;
	p->g = yajl_gen_alloc(&alloc_funcs);
	yajl_gen_configure(p->g, options);

	p->write_buffer_size = FIX2INT(rb_hash_aref(options, ID2SYM(rb_intern("write_buffer_size"))));
		
    return self;
}

static VALUE wankelStreamEncoder_change_io(VALUE self, VALUE io) {
    wankel_encoder * p;
    
    if (!rb_respond_to(io, intern_io_write)) {
        rb_raise(e_encodeError, "output must be a an IO");
    }

    Data_Get_Struct(self, wankel_encoder, p);
        
    wankelStreamEncoder_flush(self);
    p->output = io;
    
    return Qnil;
}

static VALUE wankelStreamEncoder_number(VALUE self, VALUE number) {
    size_t len;
    const char * cptr;
	wankel_encoder * p;
	yajl_gen_status status;
	VALUE str = rb_funcall(number, intern_to_s, 0);

    Data_Get_Struct(self, wankel_encoder, p);
    cptr = RSTRING_PTR(str);
    len = RSTRING_LEN(str);
	
    if (memcmp(cptr, "NaN", 3) == 0 || memcmp(cptr, "Infinity", 8) == 0 || memcmp(cptr, "-Infinity", 9) == 0) {
        rb_raise(e_encodeError, "'%s' is an invalid number", cptr);
    }
	
    status = yajl_gen_number(p->g, cptr, len);
	yajl_helper_check_gen_status(status);

	wankelStreamEncoder_write_buffer(p);
	
	return Qnil;
}

static VALUE wankelStreamEncoder_string(VALUE self, VALUE string) {
    size_t len;
    const char * cptr;
	wankel_encoder * p;
	yajl_gen_status status;
	
	Check_Type(string, T_STRING);
	
    Data_Get_Struct(self, wankel_encoder, p);
    cptr = RSTRING_PTR(string);
    len = RSTRING_LEN(string);
	
    status = yajl_gen_string(p->g, (const unsigned char *)cptr, len);
	yajl_helper_check_gen_status(status);

	wankelStreamEncoder_write_buffer(p);
	
	return Qnil;
}

static VALUE wankelStreamEncoder_null(VALUE self) {
	wankel_encoder * p;
	yajl_gen_status status;
    Data_Get_Struct(self, wankel_encoder, p);
	
    status = yajl_gen_null(p->g);
	yajl_helper_check_gen_status(status);
	
	wankelStreamEncoder_write_buffer(p);
	
	return Qnil;
}

static VALUE wankelStreamEncoder_boolean(VALUE self, VALUE b) {
	wankel_encoder * p;
	yajl_gen_status status;
    Data_Get_Struct(self, wankel_encoder, p);

	status = yajl_gen_bool(p->g, RTEST(b));
	yajl_helper_check_gen_status(status);
	
	wankelStreamEncoder_write_buffer(p);
	
	return Qnil;
}

static VALUE wankelStreamEncoder_map_open(VALUE self) {
	wankel_encoder * p;
	yajl_gen_status status;
    Data_Get_Struct(self, wankel_encoder, p);
	
    status = yajl_gen_map_open(p->g);
	yajl_helper_check_gen_status(status);
	
	wankelStreamEncoder_write_buffer(p);
	
	return Qnil;
}

static VALUE wankelStreamEncoder_map_close(VALUE self) {
	wankel_encoder * p;
	yajl_gen_status status;
    Data_Get_Struct(self, wankel_encoder, p);
	
    status = yajl_gen_map_close(p->g);
	yajl_helper_check_gen_status(status);
	
	wankelStreamEncoder_write_buffer(p);
	
	return Qnil;
}

static VALUE wankelStreamEncoder_array_open(VALUE self) {
	wankel_encoder * p;
	yajl_gen_status status;
    Data_Get_Struct(self, wankel_encoder, p);
	
    status = yajl_gen_array_open(p->g);
	yajl_helper_check_gen_status(status);
	
	wankelStreamEncoder_write_buffer(p);
	
	return Qnil;
}

static VALUE wankelStreamEncoder_array_close(VALUE self) {
	wankel_encoder * p;
	yajl_gen_status status;
    Data_Get_Struct(self, wankel_encoder, p);
	
    status = yajl_gen_array_close(p->g);
	yajl_helper_check_gen_status(status);
	
	wankelStreamEncoder_write_buffer(p);
	
	return Qnil;
}

static VALUE wankelStreamEncoder_flush(VALUE self) {
    size_t len;
	VALUE rbBuffer;
	wankel_encoder * p;
	yajl_gen_status status;
    const unsigned char * buffer;
    Data_Get_Struct(self, wankel_encoder, p);
	
	status = yajl_gen_get_buf(p->g, &buffer, &len);
	yajl_helper_check_gen_status(status);
	
	rbBuffer = rb_str_new((const char *)buffer, len);
	rb_enc_associate(rbBuffer, rb_utf8_encoding());
	rb_io_write(p->output, rbBuffer);
	yajl_gen_clear(p->g);

	return Qnil;
}

void wankelStreamEncoder_write_buffer(wankel_encoder * p) {
	VALUE rbBuffer;
    yajl_gen_status status;
    const unsigned char * buffer;
    size_t len;
	
	status = yajl_gen_get_buf(p->g, &buffer, &len);
	yajl_helper_check_gen_status(status);
	
	if (len >= (size_t)p->write_buffer_size) {
		rbBuffer = rb_str_new((const char *)buffer, len);
		rb_enc_associate(rbBuffer, rb_utf8_encoding());
		rb_io_write(p->output, rbBuffer);
		yajl_gen_clear(p->g);
	}
}

void Init_wankel_stream_encoder() {
    c_wankel = rb_const_get(rb_cObject, rb_intern("Wankel"));
    c_wankelStreamEncoder = rb_define_class_under(c_wankel, "StreamEncoder", rb_cObject);
    e_encodeError = rb_const_get(c_wankel, rb_intern("EncodeError"));

	rb_define_alloc_func(c_wankelStreamEncoder, wankel_stream_encoder_alloc);
    rb_define_method(c_wankelStreamEncoder, "initialize", wankelStreamEncoder_initialize, -1);
    rb_define_method(c_wankelStreamEncoder, "number", wankelStreamEncoder_number, 1);
    rb_define_method(c_wankelStreamEncoder, "string", wankelStreamEncoder_string, 1);
    rb_define_method(c_wankelStreamEncoder, "null", wankelStreamEncoder_null, 0);
    rb_define_method(c_wankelStreamEncoder, "boolean", wankelStreamEncoder_boolean, 1);
    rb_define_method(c_wankelStreamEncoder, "map_open", wankelStreamEncoder_map_open, 0);
    rb_define_method(c_wankelStreamEncoder, "map_close", wankelStreamEncoder_map_close, 0);
    rb_define_method(c_wankelStreamEncoder, "array_open", wankelStreamEncoder_array_open, 0);
    rb_define_method(c_wankelStreamEncoder, "array_close", wankelStreamEncoder_array_close, 0);
    rb_define_method(c_wankelStreamEncoder, "flush", wankelStreamEncoder_flush, 0);
    rb_define_method(c_wankelStreamEncoder, "output=", wankelStreamEncoder_change_io, 1);

    
	intern_to_s = rb_intern("to_s");
	intern_io_write = rb_intern("write");
	intern_to_json = rb_intern("to_json");
	intern_keys = rb_intern("keys");
    intern_clone = rb_intern("clone");
    intern_merge = rb_intern("merge");
    intern_DEFAULTS = rb_intern("DEFAULTS");
    sym_beautify = ID2SYM(rb_intern("beautify"));
    sym_indent_string = ID2SYM(rb_intern("indent_string"));
    sym_validate_utf8 = ID2SYM(rb_intern("validate_utf8"));
    sym_escape_solidus = ID2SYM(rb_intern("escape_solidus"));
}

// Ruby GC ===================================================================
static VALUE wankel_stream_encoder_alloc(VALUE klass) {
	VALUE self;
    wankel_encoder * p;
	self = Data_Make_Struct(klass, wankel_encoder, wankel_stream_encoder_mark, wankel_stream_encoder_free, p);
	p->g = 0;
	return self;
}

static void wankel_stream_encoder_free(void * handle) {
    wankel_encoder * p = handle;
	if (p->g){
		yajl_gen_free(p->g);
	}
}

static void wankel_stream_encoder_mark(void * handle) {
    wankel_encoder * p = handle;
    rb_gc_mark(p->output);
}