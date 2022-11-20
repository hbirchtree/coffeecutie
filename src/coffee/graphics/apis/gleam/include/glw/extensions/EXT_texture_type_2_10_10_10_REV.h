#pragma once

#ifdef GL_EXT_texture_type_2_10_10_10_REV
#include "../enums/PixelType.h"
#include "../enums/VertexAttribPointerType.h"
#include "../enums/VertexAttribType.h"
namespace gl::ext::texture_type_2_10_10_10_rev {
using gl::group::pixel_type;
using gl::group::vertex_attrib_pointer_type;
using gl::group::vertex_attrib_type;
namespace values {
} // namespace values
} // namespace gl::ext::texture_type_2_10_10_10_rev
#endif // GL_EXT_texture_type_2_10_10_10_REV
namespace gl::ext::texture_type_2_10_10_10_rev {
constexpr auto name = "GL_EXT_texture_type_2_10_10_10_REV";
}
