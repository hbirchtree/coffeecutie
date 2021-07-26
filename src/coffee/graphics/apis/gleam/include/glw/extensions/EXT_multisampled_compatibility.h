#pragma once

#ifdef GL_EXT_multisampled_compatibility
namespace gl::ext::multisampled_compatibility {
namespace values {
constexpr libc_types::u32 multisample         = 0x809D;
constexpr libc_types::u32 sample_alpha_to_one = 0x809F;
} // namespace values
} // namespace gl::ext::multisampled_compatibility
#endif // GL_EXT_multisampled_compatibility
namespace gl::ext::multisampled_compatibility {
constexpr auto name = "GL_EXT_multisampled_compatibility";
}
