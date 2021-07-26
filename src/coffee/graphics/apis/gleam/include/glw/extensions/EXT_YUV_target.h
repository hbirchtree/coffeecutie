#pragma once

#ifdef GL_EXT_YUV_target
namespace gl::ext::yuv_target {
namespace values {
constexpr libc_types::u32 sampler_external_2d_y2y          = 0x8BE7;
constexpr libc_types::u32 texture_external_oes             = 0x8D65;
constexpr libc_types::u32 texture_binding_external_oes     = 0x8D67;
constexpr libc_types::u32 required_texture_image_units_oes = 0x8D68;
} // namespace values
} // namespace gl::ext::yuv_target
#endif // GL_EXT_YUV_target
namespace gl::ext::yuv_target {
constexpr auto name = "GL_EXT_YUV_target";
}
