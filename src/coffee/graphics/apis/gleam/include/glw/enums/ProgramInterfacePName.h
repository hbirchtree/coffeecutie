#pragma once

#include "common.h"

namespace gl::group {

// ProgramInterfacePName
enum class program_interface_prop : u32
{
    active_resources         = 0x92F5, // GL_ACTIVE_RESOURCES
    max_name_length          = 0x92F6, // GL_MAX_NAME_LENGTH
    max_num_active_variables = 0x92F7, // GL_MAX_NUM_ACTIVE_VARIABLES
    max_num_compatible_subroutines =
        0x92F8, // GL_MAX_NUM_COMPATIBLE_SUBROUTINES
}; // enum class program_interface_prop

} // namespace gl::group
