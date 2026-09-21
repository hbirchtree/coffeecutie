#pragma once

#include "common.h"

namespace gl::group {

// VertexProvokingMode
enum class vertex_provoking_mode : u32
{
    first_vertex_convention = 0x8E4D, // GL_FIRST_VERTEX_CONVENTION
    last_vertex_convention  = 0x8E4E, // GL_LAST_VERTEX_CONVENTION
}; // enum class vertex_provoking_mode

} // namespace gl::group
