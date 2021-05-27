#pragma once

#include "common.h"

namespace gl::groups {

// VertexProvokingMode
enum class vertex_provoking_mode : ::libc_types::u32 {
#ifdef GL_FIRST_VERTEX_CONVENTION
    first_vertex_convention = GL_FIRST_VERTEX_CONVENTION,
#endif
#ifdef GL_LAST_VERTEX_CONVENTION
    last_vertex_convention = GL_LAST_VERTEX_CONVENTION,
#endif
}; // enum class vertex_provoking_mode

} // namespace gl::groups
