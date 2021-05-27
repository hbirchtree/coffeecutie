#pragma once

#include "common.h"

namespace gl::groups {

// VertexAttribEnum
enum class vertex_attrib_enum : ::libc_types::u32 {
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
}; // enum class vertex_attrib_enum

} // namespace gl::groups
