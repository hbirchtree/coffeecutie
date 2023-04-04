#pragma once

#include "common.h"

namespace gl::group {

// TextureWrapMode
enum class texture_wrap_mode : u32
{
#ifdef GL_CLAMP
    clamp = GL_CLAMP,
#endif
#ifdef GL_CLAMP_TO_BORDER
    clamp_to_border = GL_CLAMP_TO_BORDER,
#endif
#ifdef GL_CLAMP_TO_BORDER_ARB
    clamp_to_border_arb = GL_CLAMP_TO_BORDER_ARB,
#endif
#ifdef GL_CLAMP_TO_BORDER_NV
    clamp_to_border_nv = GL_CLAMP_TO_BORDER_NV,
#endif
#ifdef GL_CLAMP_TO_BORDER_SGIS
    clamp_to_border_sgis = GL_CLAMP_TO_BORDER_SGIS,
#endif
#ifdef GL_CLAMP_TO_EDGE
    clamp_to_edge = GL_CLAMP_TO_EDGE,
#endif
#ifdef GL_CLAMP_TO_EDGE_SGIS
    clamp_to_edge_sgis = GL_CLAMP_TO_EDGE_SGIS,
#endif
#ifdef GL_LINEAR_MIPMAP_LINEAR
    linear_mipmap_linear = GL_LINEAR_MIPMAP_LINEAR,
#endif
#ifdef GL_MIRRORED_REPEAT
    mirrored_repeat = GL_MIRRORED_REPEAT,
#endif
#ifdef GL_REPEAT
    repeat = GL_REPEAT,
#endif
}; // enum class texture_wrap_mode

} // namespace gl::group
