#include "wankel.h"

// Class Methods =============================================================
static VALUE wankel_parse(int argc, VALUE * argv, VALUE klass) {
    VALUE parser, input, options, callback;
    rb_scan_args(argc, argv, "11&", &input, &options, &callback);
    
    parser = rb_funcall(c_wankelParser, intern_new, 1, options);
    return rb_funcall(parser, intern_parse, 2, input, callback);
}

void Init_wankel() {
    c_wankel = rb_define_class("Wankel", rb_cObject);
    e_parseError = rb_define_class_under(c_wankel, "ParseError", rb_eStandardError);
    e_encodeError = rb_define_class_under(c_wankel, "EncodeError", rb_eStandardError);
    
    intern_new = rb_intern("new");
    intern_parse = rb_intern("parse");

    rb_define_singleton_method(c_wankel, "parse", wankel_parse, -1);
    
    c_wankelParser = Init_wankel_parser();
    c_wankelEncoder = Init_wankel_encoder();
    Init_wankel_sax_parser();
}