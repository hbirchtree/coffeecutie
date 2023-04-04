#pragma once

#include "common.h"

namespace gl::group {

// VertexAttribPointerPropertyARB
enum class vertex_attrib_pointer_property_arb : u32
{
#ifdef GL_VERTEX_ATTRIB_ARRAY_POINTER
    vertex_attrib_array_pointer = GL_VERTEX_ATTRIB_ARRAY_POINTER,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_POINTER_ARB
    vertex_attrib_array_pointer_arb = GL_VERTEX_ATTRIB_ARRAY_POINTER_ARB,
#endif
}; // enum class vertex_attrib_pointer_property_arb

} // namespace gl::group
