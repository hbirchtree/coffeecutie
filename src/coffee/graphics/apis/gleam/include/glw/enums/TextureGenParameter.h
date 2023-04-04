#pragma once

#include "common.h"

namespace gl::group {

// TextureGenParameter
enum class texture_gen_parameter : u32
{
#ifdef GL_EYE_LINE_SGIS
    eye_line_sgis = GL_EYE_LINE_SGIS,
#endif
#ifdef GL_EYE_PLANE
    eye_plane = GL_EYE_PLANE,
#endif
#ifdef GL_EYE_POINT_SGIS
    eye_point_sgis = GL_EYE_POINT_SGIS,
#endif
#ifdef GL_OBJECT_LINE_SGIS
    object_line_sgis = GL_OBJECT_LINE_SGIS,
#endif
#ifdef GL_OBJECT_PLANE
    object_plane = GL_OBJECT_PLANE,
#endif
#ifdef GL_OBJECT_POINT_SGIS
    object_point_sgis = GL_OBJECT_POINT_SGIS,
#endif
#ifdef GL_TEXTURE_GEN_MODE
    texture_gen_mode = GL_TEXTURE_GEN_MODE,
#endif
#ifdef GL_TEXTURE_GEN_MODE_OES
    texture_gen_mode_oes = GL_TEXTURE_GEN_MODE_OES,
#endif
}; // enum class texture_gen_parameter

} // namespace gl::group
