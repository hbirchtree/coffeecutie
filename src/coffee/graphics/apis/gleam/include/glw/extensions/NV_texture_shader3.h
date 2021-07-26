#pragma once

#ifdef GL_NV_texture_shader3
namespace gl::nv::texture_shader3 {
namespace values {
constexpr libc_types::u32 offset_projective_texture_2d              = 0x8850;
constexpr libc_types::u32 offset_projective_texture_2d_scale        = 0x8851;
constexpr libc_types::u32 offset_projective_texture_rectangle       = 0x8852;
constexpr libc_types::u32 offset_projective_texture_rectangle_scale = 0x8853;
constexpr libc_types::u32 offset_hilo_texture_2d                    = 0x8854;
constexpr libc_types::u32 offset_hilo_texture_rectangle             = 0x8855;
constexpr libc_types::u32 offset_hilo_projective_texture_2d         = 0x8856;
constexpr libc_types::u32 offset_hilo_projective_texture_rectangle  = 0x8857;
constexpr libc_types::u32 dependent_hilo_texture_2d                 = 0x8858;
constexpr libc_types::u32 dependent_rgb_texture_3d                  = 0x8859;
constexpr libc_types::u32 dependent_rgb_texture_cube_map            = 0x885A;
constexpr libc_types::u32 dot_product_pass_through                  = 0x885B;
constexpr libc_types::u32 dot_product_texture_1d                    = 0x885C;
constexpr libc_types::u32 dot_product_affine_depth_replace          = 0x885D;
constexpr libc_types::u32 hilo8                                     = 0x885E;
constexpr libc_types::u32 signed_hilo8                              = 0x885F;
constexpr libc_types::u32 force_blue_to_one                         = 0x8860;
} // namespace values
} // namespace gl::nv::texture_shader3
#endif // GL_NV_texture_shader3
namespace gl::nv::texture_shader3 {
constexpr auto name = "GL_NV_texture_shader3";
}
