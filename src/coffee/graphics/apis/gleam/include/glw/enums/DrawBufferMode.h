#pragma once

#include "common.h"

namespace gl::group {

// DrawBufferMode
enum class draw_buffer_mode : u32
{
    aux0                  = 0x0409, // GL_AUX0
    aux1                  = 0x040A, // GL_AUX1
    aux2                  = 0x040B, // GL_AUX2
    aux3                  = 0x040C, // GL_AUX3
    back                  = 0x0405, // GL_BACK
    back_left             = 0x0402, // GL_BACK_LEFT
    back_right            = 0x0403, // GL_BACK_RIGHT
    color_attachment0     = 0x8CE0, // GL_COLOR_ATTACHMENT0
    color_attachment1     = 0x8CE1, // GL_COLOR_ATTACHMENT1
    color_attachment10    = 0x8CEA, // GL_COLOR_ATTACHMENT10
    color_attachment11    = 0x8CEB, // GL_COLOR_ATTACHMENT11
    color_attachment12    = 0x8CEC, // GL_COLOR_ATTACHMENT12
    color_attachment13    = 0x8CED, // GL_COLOR_ATTACHMENT13
    color_attachment14    = 0x8CEE, // GL_COLOR_ATTACHMENT14
    color_attachment15    = 0x8CEF, // GL_COLOR_ATTACHMENT15
    color_attachment16    = 0x8CF0, // GL_COLOR_ATTACHMENT16
    color_attachment17    = 0x8CF1, // GL_COLOR_ATTACHMENT17
    color_attachment18    = 0x8CF2, // GL_COLOR_ATTACHMENT18
    color_attachment19    = 0x8CF3, // GL_COLOR_ATTACHMENT19
    color_attachment2     = 0x8CE2, // GL_COLOR_ATTACHMENT2
    color_attachment20    = 0x8CF4, // GL_COLOR_ATTACHMENT20
    color_attachment21    = 0x8CF5, // GL_COLOR_ATTACHMENT21
    color_attachment22    = 0x8CF6, // GL_COLOR_ATTACHMENT22
    color_attachment23    = 0x8CF7, // GL_COLOR_ATTACHMENT23
    color_attachment24    = 0x8CF8, // GL_COLOR_ATTACHMENT24
    color_attachment25    = 0x8CF9, // GL_COLOR_ATTACHMENT25
    color_attachment26    = 0x8CFA, // GL_COLOR_ATTACHMENT26
    color_attachment27    = 0x8CFB, // GL_COLOR_ATTACHMENT27
    color_attachment28    = 0x8CFC, // GL_COLOR_ATTACHMENT28
    color_attachment29    = 0x8CFD, // GL_COLOR_ATTACHMENT29
    color_attachment3     = 0x8CE3, // GL_COLOR_ATTACHMENT3
    color_attachment30    = 0x8CFE, // GL_COLOR_ATTACHMENT30
    color_attachment31    = 0x8CFF, // GL_COLOR_ATTACHMENT31
    color_attachment4     = 0x8CE4, // GL_COLOR_ATTACHMENT4
    color_attachment5     = 0x8CE5, // GL_COLOR_ATTACHMENT5
    color_attachment6     = 0x8CE6, // GL_COLOR_ATTACHMENT6
    color_attachment7     = 0x8CE7, // GL_COLOR_ATTACHMENT7
    color_attachment8     = 0x8CE8, // GL_COLOR_ATTACHMENT8
    color_attachment9     = 0x8CE9, // GL_COLOR_ATTACHMENT9
    front                 = 0x0404, // GL_FRONT
    front_and_back        = 0x0408, // GL_FRONT_AND_BACK
    front_left            = 0x0400, // GL_FRONT_LEFT
    front_right           = 0x0401, // GL_FRONT_RIGHT
    left                  = 0x0406, // GL_LEFT
    none                  = 0,      // GL_NONE
    right                 = 0x0407, // GL_RIGHT
    color_attachment0_nv  = 0x8CE0, // GL_COLOR_ATTACHMENT0_NV
    color_attachment10_nv = 0x8CEA, // GL_COLOR_ATTACHMENT10_NV
    color_attachment11_nv = 0x8CEB, // GL_COLOR_ATTACHMENT11_NV
    color_attachment12_nv = 0x8CEC, // GL_COLOR_ATTACHMENT12_NV
    color_attachment13_nv = 0x8CED, // GL_COLOR_ATTACHMENT13_NV
    color_attachment14_nv = 0x8CEE, // GL_COLOR_ATTACHMENT14_NV
    color_attachment15_nv = 0x8CEF, // GL_COLOR_ATTACHMENT15_NV
    color_attachment1_nv  = 0x8CE1, // GL_COLOR_ATTACHMENT1_NV
    color_attachment2_nv  = 0x8CE2, // GL_COLOR_ATTACHMENT2_NV
    color_attachment3_nv  = 0x8CE3, // GL_COLOR_ATTACHMENT3_NV
    color_attachment4_nv  = 0x8CE4, // GL_COLOR_ATTACHMENT4_NV
    color_attachment5_nv  = 0x8CE5, // GL_COLOR_ATTACHMENT5_NV
    color_attachment6_nv  = 0x8CE6, // GL_COLOR_ATTACHMENT6_NV
    color_attachment7_nv  = 0x8CE7, // GL_COLOR_ATTACHMENT7_NV
    color_attachment8_nv  = 0x8CE8, // GL_COLOR_ATTACHMENT8_NV
    color_attachment9_nv  = 0x8CE9, // GL_COLOR_ATTACHMENT9_NV
    none_oes              = 0,      // GL_NONE_OES
}; // enum class draw_buffer_mode

} // namespace gl::group
