#pragma once

#include "common.h"

namespace gl::group {

// VertexShaderWriteMaskEXT
enum class vertex_shader_write_mask_ext : u32
{
    false_ = 0, // GL_FALSE
    true_  = 1, // GL_TRUE
}; // enum class vertex_shader_write_mask_ext

} // namespace gl::group
