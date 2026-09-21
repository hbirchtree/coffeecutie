#pragma once

#include "common.h"

namespace gl::group {

// ContainerType
enum class container_type : u32
{
    program_object_arb = 0x8B40, // GL_PROGRAM_OBJECT_ARB
    program_object_ext = 0x8B40, // GL_PROGRAM_OBJECT_EXT
}; // enum class container_type

} // namespace gl::group
