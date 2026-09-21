#pragma once

#include "common.h"

namespace gl::group {

// TextureCoordName
enum class texture_coord_name : u32
{
    q                   = 0x2003, // GL_Q
    r                   = 0x2002, // GL_R
    s                   = 0x2000, // GL_S
    t                   = 0x2001, // GL_T
    texture_gen_str_oes = 0x8D60, // GL_TEXTURE_GEN_STR_OES
}; // enum class texture_coord_name

} // namespace gl::group
