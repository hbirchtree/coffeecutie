#pragma once

#include "common.h"

namespace gl::groups {

// StencilOp
enum class stencil_op : ::libc_types::u32 {
#ifdef GL_DECR
    decr = GL_DECR,
#endif
#ifdef GL_DECR_WRAP
    decr_wrap = GL_DECR_WRAP,
#endif
#ifdef GL_INCR
    incr = GL_INCR,
#endif
#ifdef GL_INCR_WRAP
    incr_wrap = GL_INCR_WRAP,
#endif
#ifdef GL_INVERT
    invert = GL_INVERT,
#endif
#ifdef GL_KEEP
    keep = GL_KEEP,
#endif
#ifdef GL_REPLACE
    replace = GL_REPLACE,
#endif
#ifdef GL_ZERO
    zero = GL_ZERO,
#endif
}; // enum class stencil_op

} // namespace gl::groups
