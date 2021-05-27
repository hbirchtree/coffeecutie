#pragma once

#include "common.h"

namespace gl::groups {

// ProgramParameterPName
enum class program_parameter_prop : ::libc_types::u32 {
#ifdef GL_PROGRAM_BINARY_RETRIEVABLE_HINT
    program_binary_retrievable_hint = GL_PROGRAM_BINARY_RETRIEVABLE_HINT,
#endif
#ifdef GL_PROGRAM_SEPARABLE
    program_separable = GL_PROGRAM_SEPARABLE,
#endif
}; // enum class program_parameter_prop

} // namespace gl::groups
