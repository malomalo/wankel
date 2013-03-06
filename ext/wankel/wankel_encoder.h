#ifndef WANKEL_ENCODER
#define WANKEL_ENCODER

#include <ruby.h>
#include <ruby/encoding.h>
#include <yajl/yajl_common.h>
#include <yajl/yajl_gen.h>

#include "yajl_helpers.h"

ID Init_wankel_encoder();

static VALUE c_wankel, c_wankelEncoder, e_parseError, e_encodeError;

static ID intern_clone, intern_merge, intern_DEFAULTS;
   
static ID  sym_beautify, sym_indent_string, sym_validate_utf8, sym_escape_solidus;


#endif