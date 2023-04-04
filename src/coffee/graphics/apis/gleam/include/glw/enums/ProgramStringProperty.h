#pragma once

#include "common.h"

namespace gl::group {

// ProgramStringProperty
enum class program_string_property : u32
{
#ifdef GL_PROGRAM_STRING_ARB
    program_string_arb = GL_PROGRAM_STRING_ARB,
#endif
}; // enum class program_string_property

} // namespace gl::group
