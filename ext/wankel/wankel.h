#include <ruby.h>
#include <ruby/encoding.h>
#include <yajl/yajl_common.h>
#include <yajl/yajl_parse.h>

#define READ_BUFSIZE 8092
#define WRITE_BUFSIZE 8092

void Init_wankel();

static VALUE wankel_parser_initialize(VALUE);

static ID intern_io_read, intern_on_null, intern_on_boolean, intern_on_integer,
          intern_on_double, intern_on_number, intern_on_string, intern_on_start_map,
          intern_on_map_key, intern_on_end_map, intern_on_start_array, intern_on_end_array;

static VALUE m_wankel, e_parseError, e_encodeError, c_Parser;

typedef struct {
    yajl_handle handle;
    int alloced;
} wankel_parser;

// Callbacks =================================================================
static int wankel_parser_callback_on_null(void *ctx);
static int wankel_parser_callback_on_boolean(void *ctx, int boolVal);
static int wankel_parser_callback_on_integer(void *ctx, long long integerVal);
static int wankel_parser_callback_on_double(void *ctx, double doubleVal);
static int wankel_parser_callback_on_number(void *ctx, const char * numberVal, size_t numberLen);
static int wankel_parser_callback_on_string(void *ctx, const char * stringVal, size_t stringLen);
static int wankel_parser_callback_on_start_map(void *ctx);
static int wankel_parser_callback_on_map_key(void *ctx, const unsigned char * key, size_t keyLen);
static int wankel_parser_callback_on_end_map(void *ctx);
static int wankel_parser_callback_on_start_array(void *ctx);
static int wankel_parser_callback_on_end_array(void *ctx);

// Ruby GC ===================================================================
static VALUE wankel_parser_alloc(VALUE);
//void wankel_parser_mark(void *);
static void wankel_parser_free(void * parser);

// Memory funcs ==============================================================
static void* handle_malloc(void *, size_t);
static void* handle_realloc(void *ctx, void *ptr, size_t size);
static void  handle_free(void *, void *);
