#pragma once

#ifdef GL_EXT_vertex_array_bgra
#include "../enums/PixelFormat.h"
namespace gl::ext::vertex_array_bgra {
using gl::group::pixel_format;
namespace values {
} // namespace values
} // namespace gl::ext::vertex_array_bgra
#endif // GL_EXT_vertex_array_bgra
namespace gl::ext::vertex_array_bgra {
constexpr auto name = "GL_EXT_vertex_array_bgra";
}
