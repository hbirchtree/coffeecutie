#pragma once

#include "common.h"

namespace gl::group {

// VertexShaderWriteMaskEXT
enum class vertex_shader_write_mask_ext : u32
{
#ifdef GL_FALSE
    false_ = GL_FALSE,
#endif
#ifdef GL_TRUE
    true_ = GL_TRUE,
#endif
}; // enum class vertex_shader_write_mask_ext

} // namespace gl::group
