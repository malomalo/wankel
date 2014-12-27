#include "wankel.h"

static ID intern_new, intern_parse, intern_encode;

static VALUE c_wankel, c_wankelParser, c_wankelEncoder, e_parseError, e_encodeError;

/*
 * Document-method: dump
 * call-seq:
 *   dump(obj, io=nil, opts=nil)
 */

/*
 * Document-method: load
 * call-seq:
 *   load(io, opts=nil, &block)
 */

/*
 * Document-method: parse
 * call-seq:
 *   parse(io, opts=nil, &block=nil)
 *
 * [io]     The +IO+ object parse JSON from.
 * [opts]   An optional hash of options to override the default parsing options.
 *          See DEFAULTS.
 * [&block] An optional callback used with the +multiple_values+ option. For
 *          example:
 *              results = []
 *              p = Wankel::Parser.new(:multiple_values => true)
 *              p.parse('[{"abc": 123},{"def": 456}][{"abc": 123},{"def": 456}]') do |data|
 *                result << data
 *              end
 */
static VALUE wankel_parse(int argc, VALUE * argv, VALUE klass) {
    VALUE parser, input, options, callback;
    rb_scan_args(argc, argv, "11&", &input, &options, &callback);
    
    parser = rb_funcall(c_wankelParser, intern_new, 1, options);
    return rb_funcall(parser, intern_parse, 2, input, callback);
}

/* call-seq:
 *   encode(obj, io=nil, opts=nil)
 *
 * Returns the obj encoded as a JSON string.
 * 
 * [obj] The object to be encoded as JSON
 * [io]  Either an +IO+ object to which the encoded JSON is written or the
 *       options (opts) hash.
 * [ops] An optional hash to override the encoding options. See DEFAULTS. If
 *       there is no +IO+ object this is the second argument. If there is an
 *       +IO+ object it is the third argument.
 */
static VALUE wankel_encode(int argc, VALUE * argv, VALUE klass) {
    VALUE encoder, input, output, options;
    rb_scan_args(argc, argv, "12", &input, &output, &options);
	

	if (TYPE(output) == T_HASH) {
		encoder = rb_funcall(c_wankelEncoder, intern_new, 1, output);
		return rb_funcall(encoder, intern_encode, 1, input);
	} else {
		encoder = rb_funcall(c_wankelEncoder, intern_new, 1, options);
		return rb_funcall(encoder, intern_encode, 2, input, output);
	}
}

void Init_wankel() {
    c_wankel = rb_define_class("Wankel", rb_cObject);
    e_parseError = rb_define_class_under(c_wankel, "ParseError", rb_eStandardError);
    e_encodeError = rb_define_class_under(c_wankel, "EncodeError", rb_eStandardError);
    
    intern_new = rb_intern("new");
    intern_parse = rb_intern("parse");
	intern_encode = rb_intern("encode");

    rb_define_singleton_method(c_wankel, "parse", wankel_parse, -1);
    rb_define_singleton_method(c_wankel, "encode", wankel_encode, -1);
	
    VALUE c_wankel_singleton = rb_singleton_class(c_wankel);
    rb_define_alias(c_wankel_singleton, "load", "parse");
    rb_define_alias(c_wankel_singleton, "dump", "encode");

    c_wankelParser = Init_wankel_parser();
    c_wankelEncoder = Init_wankel_encoder();
    Init_wankel_stream_parser();
    Init_wankel_stream_encoder();
    Init_yajl_helpers();
}