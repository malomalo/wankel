#ifndef YAJL_HELPERS
#define YAJL_HELPERS

#include <ruby.h>
#include <yajl/yajl_common.h>
#include <yajl/yajl_parse.h>
#include <yajl/yajl_gen.h>

// Yajl Helpers ==============================================================
void Init_yajl_helpers();
void yajl_helper_check_status(yajl_handle handle, yajl_status status, int verbose, const unsigned char * jsonText, size_t jsonTextLength);
void yajl_helper_check_gen_status(yajl_gen_status status);

// Memory funcs
void* yajl_helper_malloc(void *ctx, size_t);
void* yajl_helper_realloc(void *ctx, void *ptr, size_t size);
void  yajl_helper_free(void *ctx, void *ptr);
// static yajl_alloc_funcs* yajl_helper_alloc_funcs();

void yajl_configure(yajl_handle handle, VALUE options);
void yajl_gen_configure(yajl_gen g, VALUE options);

#endif