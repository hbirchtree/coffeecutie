#pragma once

#ifdef GL_EXT_protected_textures
#include "../enums/ContextFlagMask.h"
namespace gl::ext::protected_textures {
using gl::group::context_flag_mask;
namespace values {
constexpr u32 texture_protected = 0x8BFA;
} // namespace values
} // namespace gl::ext::protected_textures
#endif // GL_EXT_protected_textures
namespace gl::ext::protected_textures {
constexpr auto name = "GL_EXT_protected_textures";
}
