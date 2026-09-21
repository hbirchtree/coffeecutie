#pragma once

#include "common.h"

namespace gl::group {

// VertexArrayPName
enum class vertex_array_prop : u32
{
    vertex_attrib_array_divisor = 0x88FE, // GL_VERTEX_ATTRIB_ARRAY_DIVISOR
    vertex_attrib_array_enabled = 0x8622, // GL_VERTEX_ATTRIB_ARRAY_ENABLED
    vertex_attrib_array_integer = 0x88FD, // GL_VERTEX_ATTRIB_ARRAY_INTEGER
    vertex_attrib_array_long    = 0x874E, // GL_VERTEX_ATTRIB_ARRAY_LONG
    vertex_attrib_array_normalized =
        0x886A,                             // GL_VERTEX_ATTRIB_ARRAY_NORMALIZED
    vertex_attrib_array_size      = 0x8623, // GL_VERTEX_ATTRIB_ARRAY_SIZE
    vertex_attrib_array_stride    = 0x8624, // GL_VERTEX_ATTRIB_ARRAY_STRIDE
    vertex_attrib_array_type      = 0x8625, // GL_VERTEX_ATTRIB_ARRAY_TYPE
    vertex_attrib_relative_offset = 0x82D5, // GL_VERTEX_ATTRIB_RELATIVE_OFFSET
}; // enum class vertex_array_prop

} // namespace gl::group
