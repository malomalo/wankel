#ifndef WANKEL_ENCODER
#define WANKEL_ENCODER

#include <ruby.h>
#include <ruby/encoding.h>
#include <yajl/yajl_common.h>
#include <yajl/yajl_gen.h>

#include "yajl_helpers.h"

ID Init_wankel_encoder();

static VALUE c_wankel, c_wankelEncoder, e_parseError, e_encodeError;

static ID intern_to_s, intern_keys, intern_io_write, intern_to_json, intern_clone, intern_merge, intern_DEFAULTS;

static ID sym_beautify, sym_indent_string, sym_validate_utf8, sym_escape_solidus;

void wankelEncoder_flush(yajl_gen g, VALUE io, int write_buffer_size);
void yajl_encode_part(yajl_gen g, VALUE obj, VALUE io, int write_buffer_size);
void wankelEncoder_flush(yajl_gen g, VALUE io, int write_buffer_size);

#endif