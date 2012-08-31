#ifndef WANKEL
#define WANKEL

#define READ_BUFSIZE 8092
#define WRITE_BUFSIZE 8092

void Init_wankel();

static VALUE m_wankel, e_parseError, e_encodeError;

static VALUE wankel_parse(VALUE klass, VALUE input);

typedef struct {
    yajl_handle h;
	yajl_callbacks callbacks;
    yajl_alloc_funcs alloc_funcs;
	VALUE builderStack;
    // int nestedArrayLevel;
    // int nestedHashLevel;
} wankel_parser;

// Callbacks =================================================================
static yajl_callbacks wankel_parse_callbacks();

static int wankel_parse_callback_on_null(void *ctx);
static int wankel_parse_callback_on_boolean(void *ctx, int boolVal);
static int wankel_parse_callback_on_integer(void *ctx, long long integerVal);
static int wankel_parse_callback_on_double(void *ctx, double doubleVal);
static int wankel_parse_callback_on_number(void *ctx, const char * numberVal, size_t numberLen);
static int wankel_parse_callback_on_string(void *ctx, const char * stringVal, size_t stringLen);
static int wankel_parse_callback_on_start_map(void *ctx);
static int wankel_parse_callback_on_map_key(void *ctx, const unsigned char * key, size_t keyLen);
static int wankel_parse_callback_on_end_map(void *ctx);
static int wankel_parse_callback_on_start_array(void *ctx);
static int wankel_parse_callback_on_end_array(void *ctx);

#endif