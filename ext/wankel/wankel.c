#include "wankel.h"

// Class Methods =============================================================
static VALUE wankel_parse(int argc, VALUE * argv, VALUE klass) {
    VALUE parser, input, options, callback;
    rb_scan_args(argc, argv, "11&", &input, &options, &callback);
    
    parser = rb_funcall(c_wankelParser, intern_new, 1, options);
    return rb_funcall(parser, intern_parse, 2, input, callback);
}

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
    
    c_wankelParser = Init_wankel_parser();
    c_wankelEncoder = Init_wankel_encoder();
    Init_wankel_sax_parser();
}