#pragma once

#ifdef GL_NV_texture_shader
namespace gl::nv::texture_shader {
namespace values {
constexpr u32 offset_texture_rectangle               = 0x864C;
constexpr u32 offset_texture_rectangle_scale         = 0x864D;
constexpr u32 dot_product_texture_rectangle          = 0x864E;
constexpr u32 rgba_unsigned_dot_product_mapping      = 0x86D9;
constexpr u32 unsigned_int_s8_s8_8_8                 = 0x86DA;
constexpr u32 unsigned_int_8_8_s8_s8_rev             = 0x86DB;
constexpr u32 dsdt_mag_intensity                     = 0x86DC;
constexpr u32 shader_consistent                      = 0x86DD;
constexpr u32 texture_shader                         = 0x86DE;
constexpr u32 shader_operation                       = 0x86DF;
constexpr u32 cull_modes                             = 0x86E0;
constexpr u32 offset_texture_matrix                  = 0x86E1;
constexpr u32 offset_texture_2d_matrix               = 0x86E1;
constexpr u32 offset_texture_scale                   = 0x86E2;
constexpr u32 offset_texture_2d_scale                = 0x86E2;
constexpr u32 offset_texture_bias                    = 0x86E3;
constexpr u32 offset_texture_2d_bias                 = 0x86E3;
constexpr u32 previous_texture_input                 = 0x86E4;
constexpr u32 const_eye                              = 0x86E5;
constexpr u32 pass_through                           = 0x86E6;
constexpr u32 cull_fragment                          = 0x86E7;
constexpr u32 offset_texture_2d                      = 0x86E8;
constexpr u32 dependent_ar_texture_2d                = 0x86E9;
constexpr u32 dependent_gb_texture_2d                = 0x86EA;
constexpr u32 dot_product                            = 0x86EC;
constexpr u32 dot_product_depth_replace              = 0x86ED;
constexpr u32 dot_product_texture_2d                 = 0x86EE;
constexpr u32 dot_product_texture_cube_map           = 0x86F0;
constexpr u32 dot_product_diffuse_cube_map           = 0x86F1;
constexpr u32 dot_product_reflect_cube_map           = 0x86F2;
constexpr u32 dot_product_const_eye_reflect_cube_map = 0x86F3;
constexpr u32 hilo                                   = 0x86F4;
constexpr u32 dsdt                                   = 0x86F5;
constexpr u32 dsdt_mag                               = 0x86F6;
constexpr u32 dsdt_mag_vib                           = 0x86F7;
constexpr u32 hilo16                                 = 0x86F8;
constexpr u32 signed_hilo                            = 0x86F9;
constexpr u32 signed_hilo16                          = 0x86FA;
constexpr u32 signed_rgba                            = 0x86FB;
constexpr u32 signed_rgba8                           = 0x86FC;
constexpr u32 signed_rgb                             = 0x86FE;
constexpr u32 signed_rgb8                            = 0x86FF;
constexpr u32 signed_luminance                       = 0x8701;
constexpr u32 signed_luminance8                      = 0x8702;
constexpr u32 signed_luminance_alpha                 = 0x8703;
constexpr u32 signed_luminance8_alpha8               = 0x8704;
constexpr u32 signed_alpha                           = 0x8705;
constexpr u32 signed_alpha8                          = 0x8706;
constexpr u32 signed_intensity                       = 0x8707;
constexpr u32 signed_intensity8                      = 0x8708;
constexpr u32 dsdt8                                  = 0x8709;
constexpr u32 dsdt8_mag8                             = 0x870A;
constexpr u32 dsdt8_mag8_intensity8                  = 0x870B;
constexpr u32 signed_rgb_unsigned_alpha              = 0x870C;
constexpr u32 signed_rgb8_unsigned_alpha8            = 0x870D;
constexpr u32 hi_scale                               = 0x870E;
constexpr u32 lo_scale                               = 0x870F;
constexpr u32 ds_scale                               = 0x8710;
constexpr u32 dt_scale                               = 0x8711;
constexpr u32 magnitude_scale                        = 0x8712;
constexpr u32 vibrance_scale                         = 0x8713;
constexpr u32 hi_bias                                = 0x8714;
constexpr u32 lo_bias                                = 0x8715;
constexpr u32 ds_bias                                = 0x8716;
constexpr u32 dt_bias                                = 0x8717;
constexpr u32 magnitude_bias                         = 0x8718;
constexpr u32 vibrance_bias                          = 0x8719;
constexpr u32 texture_border_values                  = 0x871A;
constexpr u32 texture_hi_size                        = 0x871B;
constexpr u32 texture_lo_size                        = 0x871C;
constexpr u32 texture_ds_size                        = 0x871D;
constexpr u32 texture_dt_size                        = 0x871E;
constexpr u32 texture_mag_size                       = 0x871F;
} // namespace values
} // namespace gl::nv::texture_shader
#endif // GL_NV_texture_shader
namespace gl::nv::texture_shader {
constexpr auto name = "GL_NV_texture_shader";
}
