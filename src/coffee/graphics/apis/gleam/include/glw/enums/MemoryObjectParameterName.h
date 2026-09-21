#pragma once

#include "common.h"

namespace gl::group {

// MemoryObjectParameterName
enum class memory_object_parameter_name : u32
{
    dedicated_memory_object_ext = 0x9581, // GL_DEDICATED_MEMORY_OBJECT_EXT
    protected_memory_object_ext = 0x959B, // GL_PROTECTED_MEMORY_OBJECT_EXT
}; // enum class memory_object_parameter_name

} // namespace gl::group
