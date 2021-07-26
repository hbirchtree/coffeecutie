#pragma once

#include "common.h"

namespace gl::group {

// LightName
enum class light_name : ::libc_types::u32
{
#ifdef GL_FRAGMENT_LIGHT0_SGIX
    fragment_light0_sgix = GL_FRAGMENT_LIGHT0_SGIX,
#endif
#ifdef GL_FRAGMENT_LIGHT1_SGIX
    fragment_light1_sgix = GL_FRAGMENT_LIGHT1_SGIX,
#endif
#ifdef GL_FRAGMENT_LIGHT2_SGIX
    fragment_light2_sgix = GL_FRAGMENT_LIGHT2_SGIX,
#endif
#ifdef GL_FRAGMENT_LIGHT3_SGIX
    fragment_light3_sgix = GL_FRAGMENT_LIGHT3_SGIX,
#endif
#ifdef GL_FRAGMENT_LIGHT4_SGIX
    fragment_light4_sgix = GL_FRAGMENT_LIGHT4_SGIX,
#endif
#ifdef GL_FRAGMENT_LIGHT5_SGIX
    fragment_light5_sgix = GL_FRAGMENT_LIGHT5_SGIX,
#endif
#ifdef GL_FRAGMENT_LIGHT6_SGIX
    fragment_light6_sgix = GL_FRAGMENT_LIGHT6_SGIX,
#endif
#ifdef GL_FRAGMENT_LIGHT7_SGIX
    fragment_light7_sgix = GL_FRAGMENT_LIGHT7_SGIX,
#endif
#ifdef GL_LIGHT0
    light0 = GL_LIGHT0,
#endif
#ifdef GL_LIGHT1
    light1 = GL_LIGHT1,
#endif
#ifdef GL_LIGHT2
    light2 = GL_LIGHT2,
#endif
#ifdef GL_LIGHT3
    light3 = GL_LIGHT3,
#endif
#ifdef GL_LIGHT4
    light4 = GL_LIGHT4,
#endif
#ifdef GL_LIGHT5
    light5 = GL_LIGHT5,
#endif
#ifdef GL_LIGHT6
    light6 = GL_LIGHT6,
#endif
#ifdef GL_LIGHT7
    light7 = GL_LIGHT7,
#endif
}; // enum class light_name

} // namespace gl::group
