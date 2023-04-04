#pragma once

#include "common.h"

namespace gl::group {

// ReadBufferMode
enum class read_buffer_mode : u32
{
#ifdef GL_AUX0
    aux0 = GL_AUX0,
#endif
#ifdef GL_AUX1
    aux1 = GL_AUX1,
#endif
#ifdef GL_AUX2
    aux2 = GL_AUX2,
#endif
#ifdef GL_AUX3
    aux3 = GL_AUX3,
#endif
#ifdef GL_BACK
    back = GL_BACK,
#endif
#ifdef GL_BACK_LEFT
    back_left = GL_BACK_LEFT,
#endif
#ifdef GL_BACK_RIGHT
    back_right = GL_BACK_RIGHT,
#endif
#ifdef GL_COLOR_ATTACHMENT0
    color_attachment0 = GL_COLOR_ATTACHMENT0,
#endif
#ifdef GL_COLOR_ATTACHMENT1
    color_attachment1 = GL_COLOR_ATTACHMENT1,
#endif
#ifdef GL_COLOR_ATTACHMENT10
    color_attachment10 = GL_COLOR_ATTACHMENT10,
#endif
#ifdef GL_COLOR_ATTACHMENT11
    color_attachment11 = GL_COLOR_ATTACHMENT11,
#endif
#ifdef GL_COLOR_ATTACHMENT12
    color_attachment12 = GL_COLOR_ATTACHMENT12,
#endif
#ifdef GL_COLOR_ATTACHMENT13
    color_attachment13 = GL_COLOR_ATTACHMENT13,
#endif
#ifdef GL_COLOR_ATTACHMENT14
    color_attachment14 = GL_COLOR_ATTACHMENT14,
#endif
#ifdef GL_COLOR_ATTACHMENT15
    color_attachment15 = GL_COLOR_ATTACHMENT15,
#endif
#ifdef GL_COLOR_ATTACHMENT2
    color_attachment2 = GL_COLOR_ATTACHMENT2,
#endif
#ifdef GL_COLOR_ATTACHMENT3
    color_attachment3 = GL_COLOR_ATTACHMENT3,
#endif
#ifdef GL_COLOR_ATTACHMENT4
    color_attachment4 = GL_COLOR_ATTACHMENT4,
#endif
#ifdef GL_COLOR_ATTACHMENT5
    color_attachment5 = GL_COLOR_ATTACHMENT5,
#endif
#ifdef GL_COLOR_ATTACHMENT6
    color_attachment6 = GL_COLOR_ATTACHMENT6,
#endif
#ifdef GL_COLOR_ATTACHMENT7
    color_attachment7 = GL_COLOR_ATTACHMENT7,
#endif
#ifdef GL_COLOR_ATTACHMENT8
    color_attachment8 = GL_COLOR_ATTACHMENT8,
#endif
#ifdef GL_COLOR_ATTACHMENT9
    color_attachment9 = GL_COLOR_ATTACHMENT9,
#endif
#ifdef GL_FRONT
    front = GL_FRONT,
#endif
#ifdef GL_FRONT_LEFT
    front_left = GL_FRONT_LEFT,
#endif
#ifdef GL_FRONT_RIGHT
    front_right = GL_FRONT_RIGHT,
#endif
#ifdef GL_LEFT
    left = GL_LEFT,
#endif
#ifdef GL_NONE
    none = GL_NONE,
#endif
#ifdef GL_NONE_OES
    none_oes = GL_NONE_OES,
#endif
#ifdef GL_RIGHT
    right = GL_RIGHT,
#endif
}; // enum class read_buffer_mode

} // namespace gl::group
