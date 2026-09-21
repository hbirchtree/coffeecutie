#pragma once

#include "common.h"

namespace gl::group {

// MaterialParameter
enum class material_parameter : u32
{
    ambient             = 0x1200, // GL_AMBIENT
    ambient_and_diffuse = 0x1602, // GL_AMBIENT_AND_DIFFUSE
    color_indexes       = 0x1603, // GL_COLOR_INDEXES
    diffuse             = 0x1201, // GL_DIFFUSE
    emission            = 0x1600, // GL_EMISSION
    shininess           = 0x1601, // GL_SHININESS
    specular            = 0x1202, // GL_SPECULAR
}; // enum class material_parameter

} // namespace gl::group
