#pragma once

#include "common.h"

namespace gl::group {

// PixelMap
enum class pixel_map : u32
{
    a_to_a = 0x0C79, // GL_PIXEL_MAP_A_TO_A
    b_to_b = 0x0C78, // GL_PIXEL_MAP_B_TO_B
    g_to_g = 0x0C77, // GL_PIXEL_MAP_G_TO_G
    i_to_a = 0x0C75, // GL_PIXEL_MAP_I_TO_A
    i_to_b = 0x0C74, // GL_PIXEL_MAP_I_TO_B
    i_to_g = 0x0C73, // GL_PIXEL_MAP_I_TO_G
    i_to_i = 0x0C70, // GL_PIXEL_MAP_I_TO_I
    i_to_r = 0x0C72, // GL_PIXEL_MAP_I_TO_R
    r_to_r = 0x0C76, // GL_PIXEL_MAP_R_TO_R
    s_to_s = 0x0C71, // GL_PIXEL_MAP_S_TO_S
}; // enum class pixel_map

} // namespace gl::group
