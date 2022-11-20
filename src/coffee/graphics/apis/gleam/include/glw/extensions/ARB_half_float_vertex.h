#pragma once

#ifdef GL_ARB_half_float_vertex
#include "../enums/PixelType.h"
#include "../enums/VertexAttribPointerType.h"
#include "../enums/VertexAttribType.h"
namespace gl::arb::half_float_vertex {
using gl::group::pixel_type;
using gl::group::vertex_attrib_pointer_type;
using gl::group::vertex_attrib_type;
namespace values {
} // namespace values
} // namespace gl::arb::half_float_vertex
#endif // GL_ARB_half_float_vertex
namespace gl::arb::half_float_vertex {
constexpr auto name = "GL_ARB_half_float_vertex";
}
