#ifndef WANKEL_SAX_PARSER
#define WANKEL_SAX_PARSER

void Init_sax_parser();

static VALUE sax_parser_initialize(VALUE self);

static VALUE m_wankel, e_parseError, e_encodeError, c_saxParser;

static ID intern_io_read, intern_on_null, intern_on_boolean, intern_on_integer,
          intern_on_double, intern_on_string, intern_on_start_map, intern_on_map_key,
		  intern_on_end_map, intern_on_start_array, intern_on_end_array;

typedef struct {
    yajl_handle h;
	yajl_callbacks callbacks;
    yajl_alloc_funcs alloc_funcs;
} sax_parser;

// Callbacks =================================================================
static yajl_callbacks sax_parser_callbacks(VALUE self);
static int sax_parser_callback_on_null(void *ctx);
static int sax_parser_callback_on_boolean(void *ctx, int boolVal);
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

#endif