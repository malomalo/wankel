#ifndef WANKEL_ENCODER
#define WANKEL_ENCODER

#include <ruby.h>
#include <ruby/encoding.h>
#include <yajl/yajl_common.h>
#include <yajl/yajl_gen.h>

#include "yajl_helpers.h"

ID Init_wankel_encoder();

void wankelEncoder_flush(yajl_gen g, VALUE io, int write_buffer_size);
void yajl_encode_part(yajl_gen g, VALUE obj, VALUE io, int write_buffer_size);
void wankelEncoder_flush(yajl_gen g, VALUE io, int write_buffer_size);

#endif