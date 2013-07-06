#ifndef WANKEL_PARSER
#define WANKEL_PARSER

#include <ruby.h>
#include <ruby/encoding.h>
#include <yajl/yajl_common.h>
#include <yajl/yajl_parse.h>

#include "yajl_helpers.h"

ID Init_wankel_parser();

typedef struct {
    yajl_handle h;
    yajl_alloc_funcs alloc_funcs;
	VALUE stack;
	int stack_index;
	int symbolize_keys;
	VALUE rbufsize;
	VALUE callback;
	VALUE last_entry;
} wankel_parser;
    


#endif