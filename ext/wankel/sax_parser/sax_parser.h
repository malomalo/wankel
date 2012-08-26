#include <ruby.h>
#include <ruby/encoding.h>
#include <yajl/yajl_common.h>
#include <yajl/yajl_parse.h>

#define READ_BUFSIZE 8092
#define WRITE_BUFSIZE 8092

void Init_sax_parser();

static VALUE sax_parser_initialize(VALUE self);

static VALUE m_wankel, e_parseError, e_encodeError, c_saxParser;

static ID intern_io_read, intern_on_null, intern_on_boolean, intern_on_integer,
          intern_on_double, intern_on_number, intern_on_string, intern_on_start_map,
          intern_on_map_key, intern_on_end_map, intern_on_start_array, intern_on_end_array;

typedef struct {
    yajl_handle handle;
    int alloced;
} sax_parser;

// Yajl Helpers ==============================================================
void check_yajl_status(yajl_handle handle, yajl_status status, int verbose, const unsigned char * jsonText, size_t jsonTextLength);

// Callbacks =================================================================
static yajl_callbacks sax_parser_callbacks(VALUE self);
static int sax_parser_callback_on_null(void *ctx);
static int sax_parser_callback_on_boolean(void *ctx, int boolVal);
static int sax_parser_callback_on_integer(void *ctx, long long integerVal);
static int sax_parser_callback_on_double(void *ctx, double doubleVal);
static int sax_parser_callback_on_number(void *ctx, const char * numberVal, size_t numberLen);
static int sax_parser_callback_on_string(void *ctx, const char * stringVal, size_t stringLen);
static int sax_parser_callback_on_start_map(void *ctx);
static int sax_parser_callback_on_map_key(void *ctx, const unsigned char * key, size_t keyLen);
static int sax_parser_callback_on_end_map(void *ctx);
static int sax_parser_callback_on_start_array(void *ctx);
static int sax_parser_callback_on_end_array(void *ctx);

// Ruby GC ===================================================================
static VALUE sax_parser_alloc(VALUE);
//void sax_parser_mark(void *);
static void sax_parser_free(void * parser);

// Memory funcs ==============================================================
static void* handle_malloc(void *, size_t);
static void* handle_realloc(void *ctx, void *ptr, size_t size);
static void  handle_free(void *, void *);