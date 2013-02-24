#include "wankel.h"
#include "yajl_helpers.h"

// Yajl Helpers ==============================================================
void yajl_helper_check_status(yajl_handle handle, yajl_status status, int verbose, const unsigned char * jsonText, size_t jsonTextLength) {
    if(status != yajl_status_ok) {
        unsigned char * str = yajl_get_error(handle, verbose, jsonText, jsonTextLength);
        yajl_free_error(handle, str);
        rb_raise(rb_const_get(rb_const_get(rb_cObject, rb_intern("Wankel")), rb_intern("ParseError")), (const char*) str);
    }
}


// Memory funcs
void * yajl_helper_malloc(void *ctx, size_t size) {
    return xmalloc(size);
}
void * yajl_helper_realloc(void *ctx, void *ptr, size_t size) {
    return xrealloc(ptr, size);
}
void yajl_helper_free(void *ctx, void *ptr) {
    return xfree(ptr);
}

// cant get this to work yet
// yajl_alloc_funcs* yajl_helper_alloc_funcs() {
//     static yajl_alloc_funcs alloc_funcs = {
// 		yajl_helper_malloc,
// 		yajl_helper_realloc,
// 		yajl_helper_free,
// 		NULL
// 	};
// 	
// 	return &alloc_funcs;
// };