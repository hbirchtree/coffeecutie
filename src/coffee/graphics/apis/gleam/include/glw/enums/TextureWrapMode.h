#pragma once

#include "common.h"

namespace gl::group {

// TextureWrapMode
enum class texture_wrap_mode : u32
{
    clamp                      = 0x2900, // GL_CLAMP
    clamp_to_border            = 0x812D, // GL_CLAMP_TO_BORDER
    clamp_to_edge              = 0x812F, // GL_CLAMP_TO_EDGE
    mirrored_repeat            = 0x8370, // GL_MIRRORED_REPEAT
    mirror_clamp_to_edge       = 0x8743, // GL_MIRROR_CLAMP_TO_EDGE
    repeat                     = 0x2901, // GL_REPEAT
    clamp_to_border_arb        = 0x812D, // GL_CLAMP_TO_BORDER_ARB
    clamp_to_border_ext        = 0x812D, // GL_CLAMP_TO_BORDER_EXT
    clamp_to_border_nv         = 0x812D, // GL_CLAMP_TO_BORDER_NV
    clamp_to_border_oes        = 0x812D, // GL_CLAMP_TO_BORDER_OES
    clamp_to_border_sgis       = 0x812D, // GL_CLAMP_TO_BORDER_SGIS
    clamp_to_edge_sgis         = 0x812F, // GL_CLAMP_TO_EDGE_SGIS
    mirrored_repeat_arb        = 0x8370, // GL_MIRRORED_REPEAT_ARB
    mirrored_repeat_ibm        = 0x8370, // GL_MIRRORED_REPEAT_IBM
    mirrored_repeat_oes        = 0x8370, // GL_MIRRORED_REPEAT_OES
    mirror_clamp_ati           = 0x8742, // GL_MIRROR_CLAMP_ATI
    mirror_clamp_ext           = 0x8742, // GL_MIRROR_CLAMP_EXT
    mirror_clamp_to_border_ext = 0x8912, // GL_MIRROR_CLAMP_TO_BORDER_EXT
    mirror_clamp_to_edge_ati   = 0x8743, // GL_MIRROR_CLAMP_TO_EDGE_ATI
    mirror_clamp_to_edge_ext   = 0x8743, // GL_MIRROR_CLAMP_TO_EDGE_EXT
}; // enum class texture_wrap_mode

} // namespace gl::group
