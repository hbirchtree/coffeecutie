#pragma once

#include "common.h"

namespace gl::group {

// MaterialParameter
enum class material_parameter : u32
{
#ifdef GL_AMBIENT
    ambient = GL_AMBIENT,
#endif
#ifdef GL_AMBIENT_AND_DIFFUSE
    ambient_and_diffuse = GL_AMBIENT_AND_DIFFUSE,
#endif
#ifdef GL_COLOR_INDEXES
    color_indexes = GL_COLOR_INDEXES,
#endif
#ifdef GL_DIFFUSE
    diffuse = GL_DIFFUSE,
#endif
#ifdef GL_EMISSION
    emission = GL_EMISSION,
#endif
#ifdef GL_SHININESS
    shininess = GL_SHININESS,
#endif
#ifdef GL_SPECULAR
    specular = GL_SPECULAR,
#endif
}; // enum class material_parameter

} // namespace gl::group
