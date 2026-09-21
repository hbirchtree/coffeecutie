#pragma once

#include "common.h"

namespace gl::group {

// PatchParameterName
enum class patch_parameter_name : u32
{
    patch_default_inner_level = 0x8E73, // GL_PATCH_DEFAULT_INNER_LEVEL
    patch_default_outer_level = 0x8E74, // GL_PATCH_DEFAULT_OUTER_LEVEL
    patch_vertices            = 0x8E72, // GL_PATCH_VERTICES
}; // enum class patch_parameter_name

} // namespace gl::group
