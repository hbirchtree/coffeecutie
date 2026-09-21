#pragma once

#include "common.h"

namespace gl::group {

// VertexAttribPointerPropertyARB
enum class vertex_attrib_pointer_property_arb : u32
{
    vertex_attrib_array_pointer = 0x8645, // GL_VERTEX_ATTRIB_ARRAY_POINTER
    vertex_attrib_array_pointer_arb =
        0x8645, // GL_VERTEX_ATTRIB_ARRAY_POINTER_ARB
}; // enum class vertex_attrib_pointer_property_arb

} // namespace gl::group
