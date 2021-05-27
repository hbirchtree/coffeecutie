#pragma once

#include "common.h"

namespace gl::groups {

// VertexAttribPropertyARB
enum class vertex_attrib_property_arb : ::libc_types::u32 {
#ifdef GL_CURRENT_VERTEX_ATTRIB
    current_vertex_attrib = GL_CURRENT_VERTEX_ATTRIB,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING
    vertex_attrib_array_buffer_binding = GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_DIVISOR
    vertex_attrib_array_divisor = GL_VERTEX_ATTRIB_ARRAY_DIVISOR,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_ENABLED
    vertex_attrib_array_enabled = GL_VERTEX_ATTRIB_ARRAY_ENABLED,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_INTEGER
    vertex_attrib_array_integer = GL_VERTEX_ATTRIB_ARRAY_INTEGER,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_INTEGER_EXT
    vertex_attrib_array_integer_ext = GL_VERTEX_ATTRIB_ARRAY_INTEGER_EXT,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_LONG
    vertex_attrib_array_long = GL_VERTEX_ATTRIB_ARRAY_LONG,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_NORMALIZED
    vertex_attrib_array_normalized = GL_VERTEX_ATTRIB_ARRAY_NORMALIZED,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_SIZE
    vertex_attrib_array_size = GL_VERTEX_ATTRIB_ARRAY_SIZE,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_STRIDE
    vertex_attrib_array_stride = GL_VERTEX_ATTRIB_ARRAY_STRIDE,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_TYPE
    vertex_attrib_array_type = GL_VERTEX_ATTRIB_ARRAY_TYPE,
#endif
#ifdef GL_VERTEX_ATTRIB_BINDING
    vertex_attrib_binding = GL_VERTEX_ATTRIB_BINDING,
#endif
#ifdef GL_VERTEX_ATTRIB_RELATIVE_OFFSET
    vertex_attrib_relative_offset = GL_VERTEX_ATTRIB_RELATIVE_OFFSET,
#endif
}; // enum class vertex_attrib_property_arb

} // namespace gl::groups
