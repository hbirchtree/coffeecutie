#pragma once

#include "common.h"

namespace gl::group {

// ProgramInterfacePName
enum class program_interface_prop : u32
{
#ifdef GL_ACTIVE_RESOURCES
    active_resources = GL_ACTIVE_RESOURCES,
#endif
#ifdef GL_MAX_NAME_LENGTH
    max_name_length = GL_MAX_NAME_LENGTH,
#endif
#ifdef GL_MAX_NUM_ACTIVE_VARIABLES
    max_num_active_variables = GL_MAX_NUM_ACTIVE_VARIABLES,
#endif
#ifdef GL_MAX_NUM_COMPATIBLE_SUBROUTINES
    max_num_compatible_subroutines = GL_MAX_NUM_COMPATIBLE_SUBROUTINES,
#endif
}; // enum class program_interface_prop

} // namespace gl::group
