#pragma once

#include "common.h"

namespace gl::group {

// LightName
enum class light_name : u32
{
    light0               = 0x4000, // GL_LIGHT0
    light1               = 0x4001, // GL_LIGHT1
    light2               = 0x4002, // GL_LIGHT2
    light3               = 0x4003, // GL_LIGHT3
    light4               = 0x4004, // GL_LIGHT4
    light5               = 0x4005, // GL_LIGHT5
    light6               = 0x4006, // GL_LIGHT6
    light7               = 0x4007, // GL_LIGHT7
    fragment_light0_sgix = 0x840C, // GL_FRAGMENT_LIGHT0_SGIX
    fragment_light1_sgix = 0x840D, // GL_FRAGMENT_LIGHT1_SGIX
    fragment_light2_sgix = 0x840E, // GL_FRAGMENT_LIGHT2_SGIX
    fragment_light3_sgix = 0x840F, // GL_FRAGMENT_LIGHT3_SGIX
    fragment_light4_sgix = 0x8410, // GL_FRAGMENT_LIGHT4_SGIX
    fragment_light5_sgix = 0x8411, // GL_FRAGMENT_LIGHT5_SGIX
    fragment_light6_sgix = 0x8412, // GL_FRAGMENT_LIGHT6_SGIX
    fragment_light7_sgix = 0x8413, // GL_FRAGMENT_LIGHT7_SGIX
}; // enum class light_name

} // namespace gl::group
