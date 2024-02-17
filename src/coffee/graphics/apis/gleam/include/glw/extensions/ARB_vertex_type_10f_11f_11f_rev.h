#pragma once

#ifdef GL_ARB_vertex_type_10f_11f_11f_rev
#include "../enums/PixelType.h"
#include "../enums/VertexAttribPointerType.h"
#include "../enums/VertexAttribType.h"

namespace gl::arb::vertex_type_10f_11f_11f_rev {
using gl::group::pixel_type;
using gl::group::vertex_attrib_pointer_type;
using gl::group::vertex_attrib_type;

namespace values {
} // namespace values
} // namespace gl::arb::vertex_type_10f_11f_11f_rev
#endif // GL_ARB_vertex_type_10f_11f_11f_rev
namespace gl::arb::vertex_type_10f_11f_11f_rev {
constexpr auto name = "GL_ARB_vertex_type_10f_11f_11f_rev";
}
