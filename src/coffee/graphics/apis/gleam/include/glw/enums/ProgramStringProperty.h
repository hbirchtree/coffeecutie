#pragma once

#include "common.h"

namespace gl::groups {

// ProgramStringProperty
enum class program_string_property : ::libc_types::u32 {
#ifdef GL_PROGRAM_STRING_ARB
    program_string_arb = GL_PROGRAM_STRING_ARB,
#endif
}; // enum class program_string_property

} // namespace gl::groups
