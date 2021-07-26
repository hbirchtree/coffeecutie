#pragma once

#ifdef GL_EXT_texture
#include "../enums/ErrorCode.h"
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"
#include "../enums/TextureEnvMode.h"
#include "../enums/TextureTarget.h"
namespace gl::ext::texture {
using gl::group::error_code;
using gl::group::internal_format;
using gl::group::sized_internal_format;
using gl::group::texture_env_mode;
using gl::group::texture_target;
namespace values {
constexpr libc_types::u32 intensity              = 0x8049;
constexpr libc_types::u32 texture_red_size       = 0x805C;
constexpr libc_types::u32 texture_green_size     = 0x805D;
constexpr libc_types::u32 texture_blue_size      = 0x805E;
constexpr libc_types::u32 texture_alpha_size     = 0x805F;
constexpr libc_types::u32 texture_luminance_size = 0x8060;
constexpr libc_types::u32 texture_intensity_size = 0x8061;
} // namespace values
} // namespace gl::ext::texture
#endif // GL_EXT_texture
namespace gl::ext::texture {
constexpr auto name = "GL_EXT_texture";
}
