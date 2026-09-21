#pragma once

#include "common.h"

namespace gl::group {

// ProgramParameterPName
enum class program_parameter_prop : u32
{
    program_binary_retrievable_hint =
        0x8257,                     // GL_PROGRAM_BINARY_RETRIEVABLE_HINT
    program_separable     = 0x8258, // GL_PROGRAM_SEPARABLE
    program_separable_ext = 0x8258, // GL_PROGRAM_SEPARABLE_EXT
}; // enum class program_parameter_prop

} // namespace gl::group
