#pragma once

#include "common.h"

namespace gl::group {

// TextureGenParameter
enum class texture_gen_parameter : u32
{
    eye_plane            = 0x2502, // GL_EYE_PLANE
    object_plane         = 0x2501, // GL_OBJECT_PLANE
    texture_gen_mode     = 0x2500, // GL_TEXTURE_GEN_MODE
    eye_line_sgis        = 0x81F6, // GL_EYE_LINE_SGIS
    eye_point_sgis       = 0x81F4, // GL_EYE_POINT_SGIS
    object_line_sgis     = 0x81F7, // GL_OBJECT_LINE_SGIS
    object_point_sgis    = 0x81F5, // GL_OBJECT_POINT_SGIS
    texture_gen_mode_oes = 0x2500, // GL_TEXTURE_GEN_MODE_OES
}; // enum class texture_gen_parameter

} // namespace gl::group
