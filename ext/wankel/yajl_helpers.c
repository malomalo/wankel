#include "yajl_helpers.h"

// Yajl Helpers ==============================================================
void yajl_helper_check_status(yajl_handle handle, yajl_status status, int verbose, const unsigned char * jsonText, size_t jsonTextLength) {
    if(status != yajl_status_ok) {
        unsigned char * str = yajl_get_error(handle, verbose, jsonText, jsonTextLength);
        VALUE errorObj = rb_exc_new2(e_parseError, (const char*) str);
        yajl_free_error(handle, str);
        rb_exc_raise(errorObj);
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