#pragma once

#include "common.h"

namespace gl::group {

// VertexShaderTextureUnitParameter
enum class vertex_shader_texture_unit_parameter : ::libc_types::u32
{
#ifdef GL_CURRENT_TEXTURE_COORDS
    current_texture_coords = GL_CURRENT_TEXTURE_COORDS,
#endif
#ifdef GL_TEXTURE_MATRIX
    texture_matrix = GL_TEXTURE_MATRIX,
#endif
}; // enum class vertex_shader_texture_unit_parameter

} // namespace gl::group
