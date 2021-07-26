#pragma once

#include "common.h"

namespace gl::group {

// CullParameterEXT
enum class cull_parameter_ext : ::libc_types::u32
{
#ifdef GL_CULL_VERTEX_EYE_POSITION_EXT
    cull_vertex_eye_position_ext = GL_CULL_VERTEX_EYE_POSITION_EXT,
#endif
#ifdef GL_CULL_VERTEX_OBJECT_POSITION_EXT
    cull_vertex_object_position_ext = GL_CULL_VERTEX_OBJECT_POSITION_EXT,
#endif
}; // enum class cull_parameter_ext

} // namespace gl::group
