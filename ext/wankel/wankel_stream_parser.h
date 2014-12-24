#ifndef WANKEL_STREAM_PARSER
#define WANKEL_STREAM_PARSER

#include <ruby.h>
#include <ruby/encoding.h>
#include <yajl/yajl_common.h>
#include <yajl/yajl_parse.h>

#include "wankel.h"
#include "yajl_helpers.h"

void Init_wankel_stream_parser();

typedef struct {
    yajl_handle h;
	yajl_callbacks callbacks;
    yajl_alloc_funcs alloc_funcs;
    int symbolize_keys;
    VALUE rbufsize;
} stream_parser;


#endif