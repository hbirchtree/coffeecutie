#pragma once

#include "common.h"

namespace gl::groups {

// VertexAttribEnumNV
enum class vertex_attrib_enum_nv : ::libc_types::u32 {
#ifdef GL_PROGRAM_PARAMETER_NV
    program_parameter_nv = GL_PROGRAM_PARAMETER_NV,
#endif
}; // enum class vertex_attrib_enum_nv

} // namespace gl::groups
