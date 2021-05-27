#pragma once

#include "common.h"

namespace gl::groups {

// ProgramFormat
enum class program_format : ::libc_types::u32 {
#ifdef GL_PROGRAM_FORMAT_ASCII_ARB
    ascii_arb = GL_PROGRAM_FORMAT_ASCII_ARB,
#endif
}; // enum class program_format

} // namespace gl::groups
