#pragma once

#include "common.h"

namespace gl::group {

// SubroutineParameterName
enum class subroutine_parameter_name : u32
{
#ifdef GL_COMPATIBLE_SUBROUTINES
    compatible_subroutines = GL_COMPATIBLE_SUBROUTINES,
#endif
#ifdef GL_NUM_COMPATIBLE_SUBROUTINES
    num_compatible_subroutines = GL_NUM_COMPATIBLE_SUBROUTINES,
#endif
#ifdef GL_UNIFORM_NAME_LENGTH
    uniform_name_length = GL_UNIFORM_NAME_LENGTH,
#endif
#ifdef GL_UNIFORM_SIZE
    uniform_size = GL_UNIFORM_SIZE,
#endif
}; // enum class subroutine_parameter_name

} // namespace gl::group
