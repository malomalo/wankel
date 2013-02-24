#ifndef YAJL_HELPERS
#define YAJL_HELPERS

#include <ruby.h>
#include <yajl/yajl_common.h>
#include <yajl/yajl_parse.h>
	
// Yajl Helpers ==============================================================
void yajl_helper_check_status(yajl_handle handle, yajl_status status, int verbose, const unsigned char * jsonText, size_t jsonTextLength);

// Memory funcs
void* yajl_helper_malloc(void *ctx, size_t);
void* yajl_helper_realloc(void *ctx, void *ptr, size_t size);
void  yajl_helper_free(void *ctx, void *ptr);
// static yajl_alloc_funcs* yajl_helper_alloc_funcs();

#endif