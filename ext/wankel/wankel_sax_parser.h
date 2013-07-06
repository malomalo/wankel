#ifndef WANKEL_SAX_PARSER
#define WANKEL_SAX_PARSER

#include <ruby.h>
#include <ruby/encoding.h>
#include <yajl/yajl_common.h>
#include <yajl/yajl_parse.h>

#include "wankel.h"
#include "yajl_helpers.h"

void Init_wankel_sax_parser();

typedef struct {
    yajl_handle h;
	yajl_callbacks callbacks;
    yajl_alloc_funcs alloc_funcs;
    int symbolize_keys;
    VALUE rbufsize;
} sax_parser;


#endif