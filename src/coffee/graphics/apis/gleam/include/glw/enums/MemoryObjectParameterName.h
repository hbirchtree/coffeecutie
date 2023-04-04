#pragma once

#include "common.h"

namespace gl::group {

// MemoryObjectParameterName
enum class memory_object_parameter_name : u32
{
#ifdef GL_DEDICATED_MEMORY_OBJECT_EXT
    dedicated_memory_object_ext = GL_DEDICATED_MEMORY_OBJECT_EXT,
#endif
#ifdef GL_PROTECTED_MEMORY_OBJECT_EXT
    protected_memory_object_ext = GL_PROTECTED_MEMORY_OBJECT_EXT,
#endif
}; // enum class memory_object_parameter_name

} // namespace gl::group
