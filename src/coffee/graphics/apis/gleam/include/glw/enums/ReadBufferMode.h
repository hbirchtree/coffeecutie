#pragma once

#include "common.h"

namespace gl::group {

// ReadBufferMode
enum class read_buffer_mode : u32
{
    aux0               = 0x0409, // GL_AUX0
    aux1               = 0x040A, // GL_AUX1
    aux2               = 0x040B, // GL_AUX2
    aux3               = 0x040C, // GL_AUX3
    back               = 0x0405, // GL_BACK
    back_left          = 0x0402, // GL_BACK_LEFT
    back_right         = 0x0403, // GL_BACK_RIGHT
    color_attachment0  = 0x8CE0, // GL_COLOR_ATTACHMENT0
    color_attachment1  = 0x8CE1, // GL_COLOR_ATTACHMENT1
    color_attachment10 = 0x8CEA, // GL_COLOR_ATTACHMENT10
    color_attachment11 = 0x8CEB, // GL_COLOR_ATTACHMENT11
    color_attachment12 = 0x8CEC, // GL_COLOR_ATTACHMENT12
    color_attachment13 = 0x8CED, // GL_COLOR_ATTACHMENT13
    color_attachment14 = 0x8CEE, // GL_COLOR_ATTACHMENT14
    color_attachment15 = 0x8CEF, // GL_COLOR_ATTACHMENT15
    color_attachment2  = 0x8CE2, // GL_COLOR_ATTACHMENT2
    color_attachment3  = 0x8CE3, // GL_COLOR_ATTACHMENT3
    color_attachment4  = 0x8CE4, // GL_COLOR_ATTACHMENT4
    color_attachment5  = 0x8CE5, // GL_COLOR_ATTACHMENT5
    color_attachment6  = 0x8CE6, // GL_COLOR_ATTACHMENT6
    color_attachment7  = 0x8CE7, // GL_COLOR_ATTACHMENT7
    color_attachment8  = 0x8CE8, // GL_COLOR_ATTACHMENT8
    color_attachment9  = 0x8CE9, // GL_COLOR_ATTACHMENT9
    front              = 0x0404, // GL_FRONT
    front_left         = 0x0400, // GL_FRONT_LEFT
    front_right        = 0x0401, // GL_FRONT_RIGHT
    left               = 0x0406, // GL_LEFT
    none               = 0,      // GL_NONE
    right              = 0x0407, // GL_RIGHT
    none_oes           = 0,      // GL_NONE_OES
}; // enum class read_buffer_mode

} // namespace gl::group
