#pragma once

#include "common.h"

namespace gl::group {

// VertexWeightPointerTypeEXT
enum class vertex_weight_pointer_type_ext : u32
{
#ifdef GL_FLOAT
    float_ = GL_FLOAT,
#endif
}; // enum class vertex_weight_pointer_type_ext

} // namespace gl::group
