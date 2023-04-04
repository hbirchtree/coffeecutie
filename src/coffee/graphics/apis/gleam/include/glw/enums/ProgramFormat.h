#pragma once

#include "common.h"

namespace gl::group {

// ProgramFormat
enum class program_format : u32
{
#ifdef GL_PROGRAM_FORMAT_ASCII_ARB
    ascii_arb = GL_PROGRAM_FORMAT_ASCII_ARB,
#endif
}; // enum class program_format

} // namespace gl::group
