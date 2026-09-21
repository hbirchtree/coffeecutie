#pragma once

#include "common.h"

namespace gl::group {

// VertexShaderStorageTypeEXT
enum class vertex_shader_storage_type_ext : u32
{
    invariant_ext      = 0x87C2, // GL_INVARIANT_EXT
    local_constant_ext = 0x87C3, // GL_LOCAL_CONSTANT_EXT
    local_ext          = 0x87C4, // GL_LOCAL_EXT
    variant_ext        = 0x87C1, // GL_VARIANT_EXT
}; // enum class vertex_shader_storage_type_ext

} // namespace gl::group
