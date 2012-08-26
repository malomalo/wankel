#include "wankel.h"

static VALUE wankel_parse(VALUE klass, VALUE input) {
	return Qnil;
}

void Init_wankel() {
    m_wankel = rb_define_module("Wankel");

    e_parseError = rb_define_class_under(m_wankel, "ParseError", rb_eStandardError);
    e_encodeError = rb_define_class_under(m_wankel, "EncodeError", rb_eStandardError);

	rb_define_singleton_method(m_wankel, "parse", wankel_parse, 1);
}