#pragma once

#ifdef GL_NV_texture_rectangle
#include "../enums/TextureTarget.h"
namespace gl::nv::texture_rectangle {
using gl::group::texture_target;
namespace values {
constexpr libc_types::u32 texture_rectangle          = 0x84F5;
constexpr libc_types::u32 texture_binding_rectangle  = 0x84F6;
constexpr libc_types::u32 max_rectangle_texture_size = 0x84F8;
} // namespace values
} // namespace gl::nv::texture_rectangle
#endif // GL_NV_texture_rectangle
namespace gl::nv::texture_rectangle {
constexpr auto name = "GL_NV_texture_rectangle";
}
