#pragma once

#include "common.h"

namespace gl::group {

// VertexShaderTextureUnitParameter
enum class vertex_shader_texture_unit_parameter : u32
{
    current_texture_coords = 0x0B03, // GL_CURRENT_TEXTURE_COORDS
    texture_matrix         = 0x0BA8, // GL_TEXTURE_MATRIX
}; // enum class vertex_shader_texture_unit_parameter

} // namespace gl::group
