#ifndef WANKEL_PARSER
#define WANKEL_PARSER

#include <ruby.h>
#include <ruby/encoding.h>
#include <yajl/yajl_common.h>
#include <yajl/yajl_parse.h>

#include "yajl_helpers.h"

ID Init_wankel_parser();

static VALUE c_wankel, c_wankelParser, e_parseError, e_encodeError;

static VALUE wankelParser_parse(int argc, VALUE * argv, VALUE self);

static ID intern_io_read, intern_clone, intern_merge, intern_call,
          intern_DEFAULTS, sym_multiple_values;
   
static ID  sym_read_buffer_size, sym_write_buffer_size, sym_symbolize_keys;

typedef struct {
    yajl_handle h;
    yajl_alloc_funcs alloc_funcs;
    VALUE stack;
    int stack_index;
    int symbolize_keys;
    VALUE rbufsize;
    VALUE callback;
} wankel_parser;

static VALUE wankel_alloc(VALUE klass);
static void wankel_free(void * handle);
static void wankel_mark(void * handle);
    
// Callbacks =================================================================
static int wankel_parse_callback_on_null(void *ctx);
static int wankel_parse_callback_on_boolean(void *ctx, int boolVal);
static int wankel_parse_callback_on_integer(void *ctx, long long integerVal);
static int wankel_parse_callback_on_double(void *ctx, double doubleVal);
static int wankel_parse_callback_on_number(void *ctx, const char * numberVal, size_t numberLen);
static int wankel_parse_callback_on_string(void *ctx, const char * stringVal, size_t stringLen);
static int wankel_parse_callback_on_map_start(void *ctx);
static int wankel_parse_callback_on_map_key(void *ctx, const unsigned char * key, size_t keyLen);
static int wankel_parse_callback_on_map_end(void *ctx);
static int wankel_parse_callback_on_array_start(void *ctx);
static int wankel_parse_callback_on_array_end(void *ctx);

static yajl_callbacks callbacks = {
    wankel_parse_callback_on_null,
    wankel_parse_callback_on_boolean,
    NULL,
    NULL,
    wankel_parse_callback_on_number,
    wankel_parse_callback_on_string,
    wankel_parse_callback_on_map_start,
    wankel_parse_callback_on_map_key,
    wankel_parse_callback_on_map_end,
    wankel_parse_callback_on_array_start,
    wankel_parse_callback_on_array_end
};

#endif