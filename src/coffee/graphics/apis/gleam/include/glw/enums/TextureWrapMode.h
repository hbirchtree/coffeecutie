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
#ifdef GL_CLAMP_TO_BORDER_EXT
    clamp_to_border_ext = GL_CLAMP_TO_BORDER_EXT,
#endif
#ifdef GL_CLAMP_TO_BORDER_NV
    clamp_to_border_nv = GL_CLAMP_TO_BORDER_NV,
#endif
#ifdef GL_CLAMP_TO_BORDER_OES
    clamp_to_border_oes = GL_CLAMP_TO_BORDER_OES,
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
#ifdef GL_MIRRORED_REPEAT
    mirrored_repeat = GL_MIRRORED_REPEAT,
#endif
#ifdef GL_MIRRORED_REPEAT_ARB
    mirrored_repeat_arb = GL_MIRRORED_REPEAT_ARB,
#endif
#ifdef GL_MIRRORED_REPEAT_IBM
    mirrored_repeat_ibm = GL_MIRRORED_REPEAT_IBM,
#endif
#ifdef GL_MIRRORED_REPEAT_OES
    mirrored_repeat_oes = GL_MIRRORED_REPEAT_OES,
#endif
#ifdef GL_MIRROR_CLAMP_ATI
    mirror_clamp_ati = GL_MIRROR_CLAMP_ATI,
#endif
#ifdef GL_MIRROR_CLAMP_EXT
    mirror_clamp_ext = GL_MIRROR_CLAMP_EXT,
#endif
#ifdef GL_MIRROR_CLAMP_TO_BORDER_EXT
    mirror_clamp_to_border_ext = GL_MIRROR_CLAMP_TO_BORDER_EXT,
#endif
#ifdef GL_MIRROR_CLAMP_TO_EDGE
    mirror_clamp_to_edge = GL_MIRROR_CLAMP_TO_EDGE,
#endif
#ifdef GL_MIRROR_CLAMP_TO_EDGE_ATI
    mirror_clamp_to_edge_ati = GL_MIRROR_CLAMP_TO_EDGE_ATI,
#endif
#ifdef GL_MIRROR_CLAMP_TO_EDGE_EXT
    mirror_clamp_to_edge_ext = GL_MIRROR_CLAMP_TO_EDGE_EXT,
#endif
#ifdef GL_REPEAT
    repeat = GL_REPEAT,
#endif
}; // enum class texture_wrap_mode

} // namespace gl::group
