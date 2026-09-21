#pragma once

#include "common.h"

namespace gl::group {

// VertexAttribEnum
enum class vertex_attrib_enum : u32
{
    current_vertex_attrib = 0x8626, // GL_CURRENT_VERTEX_ATTRIB
    vertex_attrib_array_buffer_binding =
        0x889F, // GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING
    vertex_attrib_array_divisor = 0x88FE, // GL_VERTEX_ATTRIB_ARRAY_DIVISOR
    vertex_attrib_array_enabled = 0x8622, // GL_VERTEX_ATTRIB_ARRAY_ENABLED
    vertex_attrib_array_integer = 0x88FD, // GL_VERTEX_ATTRIB_ARRAY_INTEGER
    vertex_attrib_array_normalized =
        0x886A,                          // GL_VERTEX_ATTRIB_ARRAY_NORMALIZED
    vertex_attrib_array_size   = 0x8623, // GL_VERTEX_ATTRIB_ARRAY_SIZE
    vertex_attrib_array_stride = 0x8624, // GL_VERTEX_ATTRIB_ARRAY_STRIDE
    vertex_attrib_array_type   = 0x8625, // GL_VERTEX_ATTRIB_ARRAY_TYPE
}; // enum class vertex_attrib_enum

} // namespace gl::group
