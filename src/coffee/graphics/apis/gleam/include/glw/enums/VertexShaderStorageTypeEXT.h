#pragma once

#include "common.h"

namespace gl::group {

// VertexShaderStorageTypeEXT
enum class vertex_shader_storage_type_ext : u32
{
#ifdef GL_INVARIANT_EXT
    invariant_ext = GL_INVARIANT_EXT,
#endif
#ifdef GL_LOCAL_CONSTANT_EXT
    local_constant_ext = GL_LOCAL_CONSTANT_EXT,
#endif
#ifdef GL_LOCAL_EXT
    local_ext = GL_LOCAL_EXT,
#endif
#ifdef GL_VARIANT_EXT
    variant_ext = GL_VARIANT_EXT,
#endif
}; // enum class vertex_shader_storage_type_ext

} // namespace gl::group
