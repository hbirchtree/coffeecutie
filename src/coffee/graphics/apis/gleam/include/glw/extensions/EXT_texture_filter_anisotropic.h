#pragma once

#ifdef GL_EXT_texture_filter_anisotropic
#include "../enums/GetPName.h"
namespace gl::ext::texture_filter_anisotropic {
using gl::group::get_prop;
namespace values {
constexpr libc_types::u32 texture_max_anisotropy = 0x84FE;
} // namespace values
} // namespace gl::ext::texture_filter_anisotropic
#endif // GL_EXT_texture_filter_anisotropic
namespace gl::ext::texture_filter_anisotropic {
constexpr auto name = "GL_EXT_texture_filter_anisotropic";
}
