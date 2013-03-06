#include "wankel_encoder.h"

static VALUE wankelParser_initialize(int argc, VALUE * argv, VALUE self) {
    VALUE defaults = rb_const_get(c_wankelEncoder, intern_DEFAULTS);
    VALUE options, rbufsize, wbufsize;
    wankel_encoder * e;
    
    rb_scan_args(argc, argv, "01", &options);
    if(options == Qnil) {
        rb_iv_set(self, "@options", rb_funcall(defaults, intern_clone, 0) );
    } else {
        Check_Type(options, T_HASH);
        rb_iv_set(self, "@options", rb_funcall(defaults, intern_merge, 1, options) );
    }
    options = rb_iv_get(self, "@options");

    // Data_Get_Struct(self, wankel_parser, p);
    // p->alloc_funcs.malloc = yajl_helper_malloc;
    // p->alloc_funcs.realloc = yajl_helper_realloc;
    // p->alloc_funcs.free = yajl_helper_free;
    // p->h = yajl_alloc(&callbacks, &p->alloc_funcs, (void *)p);
    // 
    // rbufsize = rb_hash_aref(options, sym_read_buffer_size);
    // Check_Type(rbufsize, T_FIXNUM);
    // p->rbufsize = rbufsize;
    // 
    // if(rb_hash_aref(options, sym_symbolize_keys) == Qtrue) {
    //     p->symbolize_keys = 1;
    // } else {
    //     p->symbolize_keys = 0;
    // }
    // 
    // yajl_configure(p->h, options);
    
    return self;
}

ID Init_wankel_encoder() {
    c_wankel = rb_const_get(rb_cObject, rb_intern("Wankel"));
    c_wankelEncoder = rb_define_class_under(c_wankel, "Encoder", rb_cObject);
    e_parseError = rb_const_get(c_wankel, rb_intern("ParseError"));
    e_encodeError = rb_const_get(c_wankel, rb_intern("EncodeError"));

    rb_define_alloc_func(c_wankelEncoder, wankelEncoder_alloc);
    rb_define_method(c_wankelEncoder, "initialize", wankelEncoder_initialize, -1);
    rb_define_method(c_wankelEncoder, "parse", wankelEncoder_parse, -1);
    
    intern_clone = rb_intern("clone");
    intern_merge = rb_intern("merge");
    // intern_call = rb_intern("call");
    intern_DEFAULTS = rb_intern("DEFAULTS");
    sym_beautify = ID2SYM(rb_intern("beautify"));
    sym_indent_string = ID2SYM(rb_intern("indent_string"));
    sym_validate_utf8 = ID2SYM(rb_intern("validate_utf8"));
    sym_escape_solidus = ID2SYM(rb_intern("escape_solidus"));
    
    return c_wankelEncoder;
}
