#ifndef WANKEL
#define WANKEL

#include <ruby.h>
#include <ruby/encoding.h>
#include <yajl/yajl_common.h>
#include <yajl/yajl_parse.h>

#include "wankel_parser.h"
#include "wankel_sax_parser.h"
#include "yajl_helpers.h"

void Init_wankel();

static VALUE c_wankel, c_wankelParser, e_parseError, e_encodeError;

static ID intern_new, intern_parse;

#endif