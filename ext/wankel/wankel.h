#ifndef WANKEL
#define WANKEL

#include <ruby.h>
#include <ruby/encoding.h>
#include <yajl/yajl_common.h>
#include <yajl/yajl_parse.h>

#include "wankel_parser.h"
#include "wankel_encoder.h"
#include "wankel_sax_parser.h"
#include "wankel_sax_encoder.h"
#include "yajl_helpers.h"

void Init_wankel();

#endif