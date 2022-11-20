#pragma once

#include "common.h"

namespace gl::group {

// ContainerType
enum class container_type : ::libc_types::u32
{
#ifdef GL_PROGRAM_OBJECT_ARB
    program_object_arb = GL_PROGRAM_OBJECT_ARB,
#endif
#ifdef GL_PROGRAM_OBJECT_EXT
    program_object_ext = GL_PROGRAM_OBJECT_EXT,
#endif
}; // enum class container_type

} // namespace gl::group
