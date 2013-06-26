#include "wankel_encoder.h"
#include <stdio.h>

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
static VALUE wankelEncoder_initialize(int argc, VALUE * argv, VALUE self) {
    VALUE defaults = rb_const_get(c_wankel, intern_DEFAULTS);
    VALUE options;
    
    rb_scan_args(argc, argv, "01", &options);
    if(options == Qnil) {
        rb_iv_set(self, "@options", rb_funcall(defaults, intern_clone, 0) );
    } else {
        Check_Type(options, T_HASH);
        rb_iv_set(self, "@options", rb_funcall(defaults, intern_merge, 1, options) );
    }

    return self;
}

/*
 * Document-method: encode
 *
 * call-seq: encode(obj[, io])
 *
 * +obj+ is the Ruby object to encode to JSON
 *
 * +io+ is an optional IO used to stream the encoded JSON string to. If no io
 *      is specified the resulting JSON string is returned. If io is specified,
 *      this method returns nil
 */
static VALUE wankelEncoder_encode(int argc, VALUE * argv, VALUE self) {
	VALUE obj, io, options;
	yajl_gen g;
	yajl_alloc_funcs alloc_funcs;
	yajl_gen_status status;
	int write_buffer_size;
    const unsigned char * buffer;
    size_t len;

	rb_scan_args(argc, argv, "11", &obj, &io);
	options = rb_iv_get(self, "@options");
	
	alloc_funcs.malloc = yajl_helper_malloc;
	alloc_funcs.realloc = yajl_helper_realloc;
	alloc_funcs.free = yajl_helper_free;
	g = yajl_gen_alloc(&alloc_funcs);
	
	yajl_gen_configure(g, options);
	
	if (io != Qnil && !rb_respond_to(io, intern_io_write)) {
		rb_raise(e_encodeError, "output must be a an IO");
	}
	
	write_buffer_size = FIX2INT(rb_hash_aref(options, ID2SYM(rb_intern("write_buffer_size"))));
	
	yajl_encode_part(g, obj, io, write_buffer_size);
	
	// TODO: add terminator here if desired
	if (io == Qnil) {
		status = yajl_gen_get_buf(g, &buffer, &len);
		yajl_helper_check_gen_status(status);
	    io = rb_str_new((const char *)buffer, len);
		rb_enc_associate(io, rb_utf8_encoding());
		yajl_gen_clear(g);
		yajl_gen_free(g);
		return io;
	} else {
		wankelEncoder_flush(g, io, 1);
		yajl_gen_free(g);
		return Qnil;
	}	
	return self;
}

void wankelEncoder_flush(yajl_gen g, VALUE io, int write_buffer_size) {
	VALUE rbBuffer;
    yajl_gen_status status;
    const unsigned char * buffer;
    size_t len;
	
	if (io != Qnil) {
		status = yajl_gen_get_buf(g, &buffer, &len);
		yajl_helper_check_gen_status(status);
		
		if (len >= (size_t)write_buffer_size) {
			rbBuffer = rb_str_new((const char *)buffer, len);
			rb_enc_associate(rbBuffer, rb_utf8_encoding());
			rb_io_write(io, rbBuffer);
			yajl_gen_clear(g);
		}
	}
}

void yajl_encode_part(yajl_gen g, VALUE obj, VALUE io, int write_buffer_size) {
    size_t len;
	int idx = 0;
    VALUE keys, entry, str;
    const char * cptr;
	yajl_gen_status status;

    switch (TYPE(obj)) {
        case T_HASH:
            status = yajl_gen_map_open(g);
			yajl_helper_check_gen_status(status);
			
            keys = rb_funcall(obj, intern_keys, 0);
            for(idx = 0; idx < RARRAY_LEN(keys); idx++) {
               	entry = rb_ary_entry(keys, idx);
                str = rb_funcall(entry, intern_to_s, 0); /* key must be a string */
                /* the key */
                yajl_encode_part(g, str, io, write_buffer_size);
                /* the value */
                yajl_encode_part(g, rb_hash_aref(obj, entry), io, write_buffer_size);
            }

            status = yajl_gen_map_close(g);
			yajl_helper_check_gen_status(status);
            break;
        case T_ARRAY:
            status = yajl_gen_array_open(g);
			yajl_helper_check_gen_status(status);
			
            for(idx = 0; idx < RARRAY_LEN(obj); idx++) {
                yajl_encode_part(g, rb_ary_entry(obj, idx), io, write_buffer_size);
            }
            status = yajl_gen_array_close(g);
			yajl_helper_check_gen_status(status);
            break;
        case T_NIL:
            status = yajl_gen_null(g);
			yajl_helper_check_gen_status(status);
            break;
        case T_TRUE:
            status = yajl_gen_bool(g, 1);
			yajl_helper_check_gen_status(status);
            break;
        case T_FALSE:
            status = yajl_gen_bool(g, 0);
			yajl_helper_check_gen_status(status);
            break;
        case T_FIXNUM:
        case T_FLOAT:
        case T_BIGNUM:
            str = rb_funcall(obj, intern_to_s, 0);
            cptr = RSTRING_PTR(str);
            len = RSTRING_LEN(str);
            if (memcmp(cptr, "NaN", 3) == 0 || memcmp(cptr, "Infinity", 8) == 0 || memcmp(cptr, "-Infinity", 9) == 0) {
                rb_raise(e_encodeError, "'%s' is an invalid number", cptr);
            }
            status = yajl_gen_number(g, cptr, len);
			yajl_helper_check_gen_status(status);
            break;
        case T_STRING:
            cptr = RSTRING_PTR(obj);
            len = RSTRING_LEN(obj);
            status = yajl_gen_string(g, (const unsigned char *)cptr, len);
			yajl_helper_check_gen_status(status);
            break;
        default:
            if (rb_respond_to(obj, intern_to_json)) {
                str = rb_funcall(obj, intern_to_json, 0);
                Check_Type(str, T_STRING);
                cptr = RSTRING_PTR(str);
                len = RSTRING_LEN(str);
                status = yajl_gen_number(g, cptr, len);
				yajl_helper_check_gen_status(status);
            } else {
                str = rb_funcall(obj, intern_to_s, 0);
                Check_Type(str, T_STRING);
                cptr = RSTRING_PTR(str);
                len = RSTRING_LEN(str);
                status = yajl_gen_string(g, (const unsigned char *)cptr, len);
				yajl_helper_check_gen_status(status);
            }
            break;
    }
	
	wankelEncoder_flush(g, io, write_buffer_size);
}

ID Init_wankel_encoder() {
    c_wankel = rb_const_get(rb_cObject, rb_intern("Wankel"));
    c_wankelEncoder = rb_define_class_under(c_wankel, "Encoder", rb_cObject);
    e_parseError = rb_const_get(c_wankel, rb_intern("ParseError"));
    e_encodeError = rb_const_get(c_wankel, rb_intern("EncodeError"));

    rb_define_method(c_wankelEncoder, "initialize", wankelEncoder_initialize, -1);
    rb_define_method(c_wankelEncoder, "encode", wankelEncoder_encode, -1);
    
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
    
    return c_wankelEncoder;
}