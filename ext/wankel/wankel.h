#include <ruby.h>

void Init_wankel();

static VALUE m_wankel, e_parseError, e_encodeError;

static VALUE wankel_parse(VALUE klass, VALUE input);