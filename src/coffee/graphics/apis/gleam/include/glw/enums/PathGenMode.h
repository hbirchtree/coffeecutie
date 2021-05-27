#pragma once

#include "common.h"

namespace gl::groups {

// PathGenMode
enum class path_gen_mode : ::libc_types::u32 {
#ifdef GL_CONSTANT
    constant = GL_CONSTANT,
#endif
#ifdef GL_EYE_LINEAR
    eye_linear = GL_EYE_LINEAR,
#endif
#ifdef GL_NONE
    none = GL_NONE,
#endif
#ifdef GL_OBJECT_LINEAR
    object_linear = GL_OBJECT_LINEAR,
#endif
#ifdef GL_PATH_OBJECT_BOUNDING_BOX_NV
    path_object_bounding_box_nv = GL_PATH_OBJECT_BOUNDING_BOX_NV,
#endif
}; // enum class path_gen_mode

} // namespace gl::groups
