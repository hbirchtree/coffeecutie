#pragma once

#include "common.h"

namespace gl::group {

// TextureUnit
enum class texture_unit : ::libc_types::u32
{
#ifdef GL_TEXTURE0
    texture0 = GL_TEXTURE0,
#endif
#ifdef GL_TEXTURE1
    texture1 = GL_TEXTURE1,
#endif
#ifdef GL_TEXTURE10
    texture10 = GL_TEXTURE10,
#endif
#ifdef GL_TEXTURE11
    texture11 = GL_TEXTURE11,
#endif
#ifdef GL_TEXTURE12
    texture12 = GL_TEXTURE12,
#endif
#ifdef GL_TEXTURE13
    texture13 = GL_TEXTURE13,
#endif
#ifdef GL_TEXTURE14
    texture14 = GL_TEXTURE14,
#endif
#ifdef GL_TEXTURE15
    texture15 = GL_TEXTURE15,
#endif
#ifdef GL_TEXTURE16
    texture16 = GL_TEXTURE16,
#endif
#ifdef GL_TEXTURE17
    texture17 = GL_TEXTURE17,
#endif
#ifdef GL_TEXTURE18
    texture18 = GL_TEXTURE18,
#endif
#ifdef GL_TEXTURE19
    texture19 = GL_TEXTURE19,
#endif
#ifdef GL_TEXTURE2
    texture2 = GL_TEXTURE2,
#endif
#ifdef GL_TEXTURE20
    texture20 = GL_TEXTURE20,
#endif
#ifdef GL_TEXTURE21
    texture21 = GL_TEXTURE21,
#endif
#ifdef GL_TEXTURE22
    texture22 = GL_TEXTURE22,
#endif
#ifdef GL_TEXTURE23
    texture23 = GL_TEXTURE23,
#endif
#ifdef GL_TEXTURE24
    texture24 = GL_TEXTURE24,
#endif
#ifdef GL_TEXTURE25
    texture25 = GL_TEXTURE25,
#endif
#ifdef GL_TEXTURE26
    texture26 = GL_TEXTURE26,
#endif
#ifdef GL_TEXTURE27
    texture27 = GL_TEXTURE27,
#endif
#ifdef GL_TEXTURE28
    texture28 = GL_TEXTURE28,
#endif
#ifdef GL_TEXTURE29
    texture29 = GL_TEXTURE29,
#endif
#ifdef GL_TEXTURE3
    texture3 = GL_TEXTURE3,
#endif
#ifdef GL_TEXTURE30
    texture30 = GL_TEXTURE30,
#endif
#ifdef GL_TEXTURE31
    texture31 = GL_TEXTURE31,
#endif
#ifdef GL_TEXTURE4
    texture4 = GL_TEXTURE4,
#endif
#ifdef GL_TEXTURE5
    texture5 = GL_TEXTURE5,
#endif
#ifdef GL_TEXTURE6
    texture6 = GL_TEXTURE6,
#endif
#ifdef GL_TEXTURE7
    texture7 = GL_TEXTURE7,
#endif
#ifdef GL_TEXTURE8
    texture8 = GL_TEXTURE8,
#endif
#ifdef GL_TEXTURE9
    texture9 = GL_TEXTURE9,
#endif
}; // enum class texture_unit

} // namespace gl::group
