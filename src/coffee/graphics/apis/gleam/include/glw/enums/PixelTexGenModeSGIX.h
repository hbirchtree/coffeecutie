#pragma once

#include "common.h"

namespace gl::group {

// PixelTexGenModeSGIX
enum class pixel_tex_gen_mode_sgix : u32
{
    alpha                        = 0x1906, // GL_ALPHA
    none                         = 0,      // GL_NONE
    rgb                          = 0x1907, // GL_RGB
    rgba                         = 0x1908, // GL_RGBA
    pixel_tex_gen_alpha_ls_sgix  = 0x8189, // GL_PIXEL_TEX_GEN_ALPHA_LS_SGIX
    pixel_tex_gen_alpha_ms_sgix  = 0x818A, // GL_PIXEL_TEX_GEN_ALPHA_MS_SGIX
    pixel_tex_gen_q_ceiling_sgix = 0x8184, // GL_PIXEL_TEX_GEN_Q_CEILING_SGIX
    pixel_tex_gen_q_floor_sgix   = 0x8186, // GL_PIXEL_TEX_GEN_Q_FLOOR_SGIX
    pixel_tex_gen_q_round_sgix   = 0x8185, // GL_PIXEL_TEX_GEN_Q_ROUND_SGIX
}; // enum class pixel_tex_gen_mode_sgix

} // namespace gl::group
