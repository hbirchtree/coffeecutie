#pragma once

#ifdef GL_NV_texture_shader3
namespace gl::nv::texture_shader3 {
namespace values {
constexpr u32 offset_projective_texture_2d              = 0x8850;
constexpr u32 offset_projective_texture_2d_scale        = 0x8851;
constexpr u32 offset_projective_texture_rectangle       = 0x8852;
constexpr u32 offset_projective_texture_rectangle_scale = 0x8853;
constexpr u32 offset_hilo_texture_2d                    = 0x8854;
constexpr u32 offset_hilo_texture_rectangle             = 0x8855;
constexpr u32 offset_hilo_projective_texture_2d         = 0x8856;
constexpr u32 offset_hilo_projective_texture_rectangle  = 0x8857;
constexpr u32 dependent_hilo_texture_2d                 = 0x8858;
constexpr u32 dependent_rgb_texture_3d                  = 0x8859;
constexpr u32 dependent_rgb_texture_cube_map            = 0x885A;
constexpr u32 dot_product_pass_through                  = 0x885B;
constexpr u32 dot_product_texture_1d                    = 0x885C;
constexpr u32 dot_product_affine_depth_replace          = 0x885D;
constexpr u32 hilo8                                     = 0x885E;
constexpr u32 signed_hilo8                              = 0x885F;
constexpr u32 force_blue_to_one                         = 0x8860;
} // namespace values
} // namespace gl::nv::texture_shader3
#endif // GL_NV_texture_shader3
namespace gl::nv::texture_shader3 {
constexpr auto name = "GL_NV_texture_shader3";
}
