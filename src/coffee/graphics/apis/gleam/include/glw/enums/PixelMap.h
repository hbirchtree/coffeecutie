#pragma once

#include "common.h"

namespace gl::groups {

// PixelMap
enum class pixel_map : ::libc_types::u32 {
#ifdef GL_PIXEL_MAP_A_TO_A
    a_to_a = GL_PIXEL_MAP_A_TO_A,
#endif
#ifdef GL_PIXEL_MAP_B_TO_B
    b_to_b = GL_PIXEL_MAP_B_TO_B,
#endif
#ifdef GL_PIXEL_MAP_G_TO_G
    g_to_g = GL_PIXEL_MAP_G_TO_G,
#endif
#ifdef GL_PIXEL_MAP_I_TO_A
    i_to_a = GL_PIXEL_MAP_I_TO_A,
#endif
#ifdef GL_PIXEL_MAP_I_TO_B
    i_to_b = GL_PIXEL_MAP_I_TO_B,
#endif
#ifdef GL_PIXEL_MAP_I_TO_G
    i_to_g = GL_PIXEL_MAP_I_TO_G,
#endif
#ifdef GL_PIXEL_MAP_I_TO_I
    i_to_i = GL_PIXEL_MAP_I_TO_I,
#endif
#ifdef GL_PIXEL_MAP_I_TO_R
    i_to_r = GL_PIXEL_MAP_I_TO_R,
#endif
#ifdef GL_PIXEL_MAP_R_TO_R
    r_to_r = GL_PIXEL_MAP_R_TO_R,
#endif
#ifdef GL_PIXEL_MAP_S_TO_S
    s_to_s = GL_PIXEL_MAP_S_TO_S,
#endif
}; // enum class pixel_map

} // namespace gl::groups
