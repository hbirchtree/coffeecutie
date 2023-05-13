#pragma once

#ifdef GL_NV_texture_rectangle
#include "../enums/EnableCap.h"
#include "../enums/GetPName.h"
#include "../enums/TextureTarget.h"
namespace gl::nv::texture_rectangle {
using gl::group::enable_cap;
using gl::group::get_prop;
using gl::group::texture_target;
namespace values {
constexpr u32 max_rectangle_texture_size = 0x84F8;
} // namespace values
} // namespace gl::nv::texture_rectangle
#endif // GL_NV_texture_rectangle
namespace gl::nv::texture_rectangle {
constexpr auto name = "GL_NV_texture_rectangle";
}
