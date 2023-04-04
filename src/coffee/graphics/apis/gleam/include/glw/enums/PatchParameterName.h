#pragma once

#include "common.h"

namespace gl::group {

// PatchParameterName
enum class patch_parameter_name : u32
{
#ifdef GL_PATCH_DEFAULT_INNER_LEVEL
    patch_default_inner_level = GL_PATCH_DEFAULT_INNER_LEVEL,
#endif
#ifdef GL_PATCH_DEFAULT_OUTER_LEVEL
    patch_default_outer_level = GL_PATCH_DEFAULT_OUTER_LEVEL,
#endif
#ifdef GL_PATCH_VERTICES
    patch_vertices = GL_PATCH_VERTICES,
#endif
}; // enum class patch_parameter_name

} // namespace gl::group
