#pragma once

#ifdef GL_ARB_vertex_array_bgra
#include "../enums/PixelFormat.h"

namespace gl::arb::vertex_array_bgra {
using gl::group::pixel_format;

namespace values {
} // namespace values
} // namespace gl::arb::vertex_array_bgra
#endif // GL_ARB_vertex_array_bgra
namespace gl::arb::vertex_array_bgra {
constexpr auto name = "GL_ARB_vertex_array_bgra";
}
