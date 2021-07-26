#pragma once

#ifdef GL_EXT_pvrtc_sRGB
namespace gl::ext::pvrtc_srgb {
namespace values {
constexpr libc_types::u32 compressed_srgb_pvrtc_2bppv1           = 0x8A54;
constexpr libc_types::u32 compressed_srgb_pvrtc_4bppv1           = 0x8A55;
constexpr libc_types::u32 compressed_srgb_alpha_pvrtc_2bppv1     = 0x8A56;
constexpr libc_types::u32 compressed_srgb_alpha_pvrtc_4bppv1     = 0x8A57;
constexpr libc_types::u32 compressed_srgb_alpha_pvrtc_2bppv2_img = 0x93F0;
constexpr libc_types::u32 compressed_srgb_alpha_pvrtc_4bppv2_img = 0x93F1;
} // namespace values
} // namespace gl::ext::pvrtc_srgb
#endif // GL_EXT_pvrtc_sRGB
namespace gl::ext::pvrtc_srgb {
constexpr auto name = "GL_EXT_pvrtc_sRGB";
}
