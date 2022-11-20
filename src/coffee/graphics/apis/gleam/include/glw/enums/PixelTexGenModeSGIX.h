#pragma once

#include "common.h"

namespace gl::group {

// PixelTexGenModeSGIX
enum class pixel_tex_gen_mode_sgix : ::libc_types::u32
{
#ifdef GL_ALPHA
    alpha = GL_ALPHA,
#endif
#ifdef GL_NONE
    none = GL_NONE,
#endif
#ifdef GL_PIXEL_TEX_GEN_ALPHA_LS_SGIX
    pixel_tex_gen_alpha_ls_sgix = GL_PIXEL_TEX_GEN_ALPHA_LS_SGIX,
#endif
#ifdef GL_PIXEL_TEX_GEN_ALPHA_MS_SGIX
    pixel_tex_gen_alpha_ms_sgix = GL_PIXEL_TEX_GEN_ALPHA_MS_SGIX,
#endif
#ifdef GL_PIXEL_TEX_GEN_Q_CEILING_SGIX
    pixel_tex_gen_q_ceiling_sgix = GL_PIXEL_TEX_GEN_Q_CEILING_SGIX,
#endif
#ifdef GL_PIXEL_TEX_GEN_Q_FLOOR_SGIX
    pixel_tex_gen_q_floor_sgix = GL_PIXEL_TEX_GEN_Q_FLOOR_SGIX,
#endif
#ifdef GL_PIXEL_TEX_GEN_Q_ROUND_SGIX
    pixel_tex_gen_q_round_sgix = GL_PIXEL_TEX_GEN_Q_ROUND_SGIX,
#endif
#ifdef GL_RGB
    rgb = GL_RGB,
#endif
#ifdef GL_RGBA
    rgba = GL_RGBA,
#endif
}; // enum class pixel_tex_gen_mode_sgix

} // namespace gl::group
