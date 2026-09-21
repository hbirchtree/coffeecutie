#pragma once

#include "common.h"

namespace gl::group {

// CullParameterEXT
enum class cull_parameter_ext : u32
{
    cull_vertex_eye_position_ext = 0x81AB, // GL_CULL_VERTEX_EYE_POSITION_EXT
    cull_vertex_object_position_ext =
        0x81AC, // GL_CULL_VERTEX_OBJECT_POSITION_EXT
}; // enum class cull_parameter_ext

} // namespace gl::group
