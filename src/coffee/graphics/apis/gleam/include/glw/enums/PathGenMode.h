#pragma once

#include "common.h"

namespace gl::group {

// PathGenMode
enum class path_gen_mode : u32
{
    constant                    = 0x8576, // GL_CONSTANT
    eye_linear                  = 0x2400, // GL_EYE_LINEAR
    none                        = 0,      // GL_NONE
    object_linear               = 0x2401, // GL_OBJECT_LINEAR
    path_object_bounding_box_nv = 0x908A, // GL_PATH_OBJECT_BOUNDING_BOX_NV
}; // enum class path_gen_mode

} // namespace gl::group
