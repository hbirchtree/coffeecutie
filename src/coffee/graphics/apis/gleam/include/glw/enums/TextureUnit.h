#pragma once

#include "common.h"

namespace gl::group {

// TextureUnit
enum class texture_unit : u32
{
    texture0  = 0x84C0, // GL_TEXTURE0
    texture1  = 0x84C1, // GL_TEXTURE1
    texture10 = 0x84CA, // GL_TEXTURE10
    texture11 = 0x84CB, // GL_TEXTURE11
    texture12 = 0x84CC, // GL_TEXTURE12
    texture13 = 0x84CD, // GL_TEXTURE13
    texture14 = 0x84CE, // GL_TEXTURE14
    texture15 = 0x84CF, // GL_TEXTURE15
    texture16 = 0x84D0, // GL_TEXTURE16
    texture17 = 0x84D1, // GL_TEXTURE17
    texture18 = 0x84D2, // GL_TEXTURE18
    texture19 = 0x84D3, // GL_TEXTURE19
    texture2  = 0x84C2, // GL_TEXTURE2
    texture20 = 0x84D4, // GL_TEXTURE20
    texture21 = 0x84D5, // GL_TEXTURE21
    texture22 = 0x84D6, // GL_TEXTURE22
    texture23 = 0x84D7, // GL_TEXTURE23
    texture24 = 0x84D8, // GL_TEXTURE24
    texture25 = 0x84D9, // GL_TEXTURE25
    texture26 = 0x84DA, // GL_TEXTURE26
    texture27 = 0x84DB, // GL_TEXTURE27
    texture28 = 0x84DC, // GL_TEXTURE28
    texture29 = 0x84DD, // GL_TEXTURE29
    texture3  = 0x84C3, // GL_TEXTURE3
    texture30 = 0x84DE, // GL_TEXTURE30
    texture31 = 0x84DF, // GL_TEXTURE31
    texture4  = 0x84C4, // GL_TEXTURE4
    texture5  = 0x84C5, // GL_TEXTURE5
    texture6  = 0x84C6, // GL_TEXTURE6
    texture7  = 0x84C7, // GL_TEXTURE7
    texture8  = 0x84C8, // GL_TEXTURE8
    texture9  = 0x84C9, // GL_TEXTURE9
}; // enum class texture_unit

} // namespace gl::group
