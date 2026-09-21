#pragma once

#include "common.h"

namespace gl::group {

// SubroutineParameterName
enum class subroutine_parameter_name : u32
{
    compatible_subroutines     = 0x8E4B, // GL_COMPATIBLE_SUBROUTINES
    num_compatible_subroutines = 0x8E4A, // GL_NUM_COMPATIBLE_SUBROUTINES
    uniform_name_length        = 0x8A39, // GL_UNIFORM_NAME_LENGTH
    uniform_size               = 0x8A38, // GL_UNIFORM_SIZE
}; // enum class subroutine_parameter_name

} // namespace gl::group
