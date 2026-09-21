#pragma once

#include "common.h"

namespace gl::group {

// HintMode
enum class hint_mode : u32
{
    dont_care = 0x1100, // GL_DONT_CARE
    fastest   = 0x1101, // GL_FASTEST
    nicest    = 0x1102, // GL_NICEST
}; // enum class hint_mode

} // namespace gl::group
