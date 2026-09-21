#pragma once

#include "common.h"

namespace gl::group {

// SizedInternalFormat
enum class sized_internal_format : u32
{
    alpha12              = 0x803D, // GL_ALPHA12
    alpha16              = 0x803E, // GL_ALPHA16
    alpha4               = 0x803B, // GL_ALPHA4
    alpha8               = 0x803C, // GL_ALPHA8
    compressed_r11_eac   = 0x9270, // GL_COMPRESSED_R11_EAC
    compressed_red_rgtc1 = 0x8DBB, // GL_COMPRESSED_RED_RGTC1
    compressed_rg11_eac  = 0x9272, // GL_COMPRESSED_RG11_EAC
    compressed_rgb8_etc2 = 0x9274, // GL_COMPRESSED_RGB8_ETC2
    compressed_rgb8_punchthrough_alpha1_etc2 =
        0x9276, // GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2
    compressed_rgba8_etc2_eac  = 0x9278, // GL_COMPRESSED_RGBA8_ETC2_EAC
    compressed_rgba_astc_10x10 = 0x93BB, // GL_COMPRESSED_RGBA_ASTC_10x10
    compressed_rgba_astc_10x5  = 0x93B8, // GL_COMPRESSED_RGBA_ASTC_10x5
    compressed_rgba_astc_10x6  = 0x93B9, // GL_COMPRESSED_RGBA_ASTC_10x6
    compressed_rgba_astc_10x8  = 0x93BA, // GL_COMPRESSED_RGBA_ASTC_10x8
    compressed_rgba_astc_12x10 = 0x93BC, // GL_COMPRESSED_RGBA_ASTC_12x10
    compressed_rgba_astc_12x12 = 0x93BD, // GL_COMPRESSED_RGBA_ASTC_12x12
    compressed_rgba_astc_4x4   = 0x93B0, // GL_COMPRESSED_RGBA_ASTC_4x4
    compressed_rgba_astc_5x4   = 0x93B1, // GL_COMPRESSED_RGBA_ASTC_5x4
    compressed_rgba_astc_5x5   = 0x93B2, // GL_COMPRESSED_RGBA_ASTC_5x5
    compressed_rgba_astc_6x5   = 0x93B3, // GL_COMPRESSED_RGBA_ASTC_6x5
    compressed_rgba_astc_6x6   = 0x93B4, // GL_COMPRESSED_RGBA_ASTC_6x6
    compressed_rgba_astc_8x5   = 0x93B5, // GL_COMPRESSED_RGBA_ASTC_8x5
    compressed_rgba_astc_8x6   = 0x93B6, // GL_COMPRESSED_RGBA_ASTC_8x6
    compressed_rgba_astc_8x8   = 0x93B7, // GL_COMPRESSED_RGBA_ASTC_8x8
    compressed_rgba_bptc_unorm = 0x8E8C, // GL_COMPRESSED_RGBA_BPTC_UNORM
    compressed_rgb_bptc_signed_float =
        0x8E8E, // GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT
    compressed_rgb_bptc_unsigned_float =
        0x8E8F, // GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT
    compressed_rg_rgtc2         = 0x8DBD, // GL_COMPRESSED_RG_RGTC2
    compressed_signed_r11_eac   = 0x9271, // GL_COMPRESSED_SIGNED_R11_EAC
    compressed_signed_red_rgtc1 = 0x8DBC, // GL_COMPRESSED_SIGNED_RED_RGTC1
    compressed_signed_rg11_eac  = 0x9273, // GL_COMPRESSED_SIGNED_RG11_EAC
    compressed_signed_rg_rgtc2  = 0x8DBE, // GL_COMPRESSED_SIGNED_RG_RGTC2
    compressed_srgb8_alpha8_astc_10x10 =
        0x93DB, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10
    compressed_srgb8_alpha8_astc_10x5 =
        0x93D8, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5
    compressed_srgb8_alpha8_astc_10x6 =
        0x93D9, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6
    compressed_srgb8_alpha8_astc_10x8 =
        0x93DA, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8
    compressed_srgb8_alpha8_astc_12x10 =
        0x93DC, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10
    compressed_srgb8_alpha8_astc_12x12 =
        0x93DD, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12
    compressed_srgb8_alpha8_astc_4x4 =
        0x93D0, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4
    compressed_srgb8_alpha8_astc_5x4 =
        0x93D1, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4
    compressed_srgb8_alpha8_astc_5x5 =
        0x93D2, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5
    compressed_srgb8_alpha8_astc_6x5 =
        0x93D3, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5
    compressed_srgb8_alpha8_astc_6x6 =
        0x93D4, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6
    compressed_srgb8_alpha8_astc_8x5 =
        0x93D5, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5
    compressed_srgb8_alpha8_astc_8x6 =
        0x93D6, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6
    compressed_srgb8_alpha8_astc_8x8 =
        0x93D7, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8
    compressed_srgb8_alpha8_etc2_eac =
        0x9279,                     // GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC
    compressed_srgb8_etc2 = 0x9275, // GL_COMPRESSED_SRGB8_ETC2
    compressed_srgb8_punchthrough_alpha1_etc2 =
        0x9277, // GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2
    compressed_srgb_alpha_bptc_unorm =
        0x8E8D,                           // GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM
    depth24_stencil8            = 0x88F0, // GL_DEPTH24_STENCIL8
    depth32f_stencil8           = 0x8CAD, // GL_DEPTH32F_STENCIL8
    depth_component16           = 0x81A5, // GL_DEPTH_COMPONENT16
    depth_component24           = 0x81A6, // GL_DEPTH_COMPONENT24
    depth_component32           = 0x81A7, // GL_DEPTH_COMPONENT32
    depth_component32f          = 0x8CAC, // GL_DEPTH_COMPONENT32F
    intensity12                 = 0x804C, // GL_INTENSITY12
    intensity16                 = 0x804D, // GL_INTENSITY16
    intensity4                  = 0x804A, // GL_INTENSITY4
    intensity8                  = 0x804B, // GL_INTENSITY8
    luminance12                 = 0x8041, // GL_LUMINANCE12
    luminance12_alpha12         = 0x8047, // GL_LUMINANCE12_ALPHA12
    luminance12_alpha4          = 0x8046, // GL_LUMINANCE12_ALPHA4
    luminance16                 = 0x8042, // GL_LUMINANCE16
    luminance16_alpha16         = 0x8048, // GL_LUMINANCE16_ALPHA16
    luminance4                  = 0x803F, // GL_LUMINANCE4
    luminance4_alpha4           = 0x8043, // GL_LUMINANCE4_ALPHA4
    luminance6_alpha2           = 0x8044, // GL_LUMINANCE6_ALPHA2
    luminance8                  = 0x8040, // GL_LUMINANCE8
    luminance8_alpha8           = 0x8045, // GL_LUMINANCE8_ALPHA8
    r11f_g11f_b10f              = 0x8C3A, // GL_R11F_G11F_B10F
    r16                         = 0x822A, // GL_R16
    r16f                        = 0x822D, // GL_R16F
    r16i                        = 0x8233, // GL_R16I
    r16ui                       = 0x8234, // GL_R16UI
    r16_snorm                   = 0x8F98, // GL_R16_SNORM
    r32f                        = 0x822E, // GL_R32F
    r32i                        = 0x8235, // GL_R32I
    r32ui                       = 0x8236, // GL_R32UI
    r3_g3_b2                    = 0x2A10, // GL_R3_G3_B2
    r8                          = 0x8229, // GL_R8
    r8i                         = 0x8231, // GL_R8I
    r8ui                        = 0x8232, // GL_R8UI
    r8_snorm                    = 0x8F94, // GL_R8_SNORM
    rg16                        = 0x822C, // GL_RG16
    rg16f                       = 0x822F, // GL_RG16F
    rg16i                       = 0x8239, // GL_RG16I
    rg16ui                      = 0x823A, // GL_RG16UI
    rg16_snorm                  = 0x8F99, // GL_RG16_SNORM
    rg32f                       = 0x8230, // GL_RG32F
    rg32i                       = 0x823B, // GL_RG32I
    rg32ui                      = 0x823C, // GL_RG32UI
    rg8                         = 0x822B, // GL_RG8
    rg8i                        = 0x8237, // GL_RG8I
    rg8ui                       = 0x8238, // GL_RG8UI
    rg8_snorm                   = 0x8F95, // GL_RG8_SNORM
    rgb10                       = 0x8052, // GL_RGB10
    rgb10_a2                    = 0x8059, // GL_RGB10_A2
    rgb10_a2ui                  = 0x906F, // GL_RGB10_A2UI
    rgb12                       = 0x8053, // GL_RGB12
    rgb16                       = 0x8054, // GL_RGB16
    rgb16f                      = 0x881B, // GL_RGB16F
    rgb16i                      = 0x8D89, // GL_RGB16I
    rgb16ui                     = 0x8D77, // GL_RGB16UI
    rgb16_snorm                 = 0x8F9A, // GL_RGB16_SNORM
    rgb32f                      = 0x8815, // GL_RGB32F
    rgb32i                      = 0x8D83, // GL_RGB32I
    rgb32ui                     = 0x8D71, // GL_RGB32UI
    rgb4                        = 0x804F, // GL_RGB4
    rgb5                        = 0x8050, // GL_RGB5
    rgb565                      = 0x8D62, // GL_RGB565
    rgb5_a1                     = 0x8057, // GL_RGB5_A1
    rgb8                        = 0x8051, // GL_RGB8
    rgb8i                       = 0x8D8F, // GL_RGB8I
    rgb8ui                      = 0x8D7D, // GL_RGB8UI
    rgb8_snorm                  = 0x8F96, // GL_RGB8_SNORM
    rgb9_e5                     = 0x8C3D, // GL_RGB9_E5
    rgba12                      = 0x805A, // GL_RGBA12
    rgba16                      = 0x805B, // GL_RGBA16
    rgba16f                     = 0x881A, // GL_RGBA16F
    rgba16i                     = 0x8D88, // GL_RGBA16I
    rgba16ui                    = 0x8D76, // GL_RGBA16UI
    rgba16_snorm                = 0x8F9B, // GL_RGBA16_SNORM
    rgba2                       = 0x8055, // GL_RGBA2
    rgba32f                     = 0x8814, // GL_RGBA32F
    rgba32i                     = 0x8D82, // GL_RGBA32I
    rgba32ui                    = 0x8D70, // GL_RGBA32UI
    rgba4                       = 0x8056, // GL_RGBA4
    rgba8                       = 0x8058, // GL_RGBA8
    rgba8i                      = 0x8D8E, // GL_RGBA8I
    rgba8ui                     = 0x8D7C, // GL_RGBA8UI
    rgba8_snorm                 = 0x8F97, // GL_RGBA8_SNORM
    srgb8                       = 0x8C41, // GL_SRGB8
    srgb8_alpha8                = 0x8C43, // GL_SRGB8_ALPHA8
    stencil_index1              = 0x8D46, // GL_STENCIL_INDEX1
    stencil_index16             = 0x8D49, // GL_STENCIL_INDEX16
    stencil_index4              = 0x8D47, // GL_STENCIL_INDEX4
    stencil_index8              = 0x8D48, // GL_STENCIL_INDEX8
    n3dc_xy_amd                 = 0x87FA, // GL_3DC_XY_AMD
    n3dc_x_amd                  = 0x87F9, // GL_3DC_X_AMD
    alpha12_ext                 = 0x803D, // GL_ALPHA12_EXT
    alpha16i_ext                = 0x8D8A, // GL_ALPHA16I_EXT
    alpha16ui_ext               = 0x8D78, // GL_ALPHA16UI_EXT
    alpha16_ext                 = 0x803E, // GL_ALPHA16_EXT
    alpha32i_ext                = 0x8D84, // GL_ALPHA32I_EXT
    alpha32ui_ext               = 0x8D72, // GL_ALPHA32UI_EXT
    alpha4_ext                  = 0x803B, // GL_ALPHA4_EXT
    alpha8i_ext                 = 0x8D90, // GL_ALPHA8I_EXT
    alpha8ui_ext                = 0x8D7E, // GL_ALPHA8UI_EXT
    alpha8_ext                  = 0x803C, // GL_ALPHA8_EXT
    alpha8_oes                  = 0x803C, // GL_ALPHA8_OES
    atc_rgba_explicit_alpha_amd = 0x8C93, // GL_ATC_RGBA_EXPLICIT_ALPHA_AMD
    atc_rgba_interpolated_alpha_amd =
        0x87EE,           // GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD
    atc_rgb_amd = 0x8C92, // GL_ATC_RGB_AMD
    compressed_luminance_alpha_latc2_ext =
        0x8C72, // GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT
    compressed_luminance_latc1_ext =
        0x8C70,                      // GL_COMPRESSED_LUMINANCE_LATC1_EXT
    compressed_r11_eac_oes = 0x9270, // GL_COMPRESSED_R11_EAC_OES
    compressed_red_green_rgtc2_ext =
        0x8DBD,                        // GL_COMPRESSED_RED_GREEN_RGTC2_EXT
    compressed_red_rgtc1_ext = 0x8DBB, // GL_COMPRESSED_RED_RGTC1_EXT
    compressed_rg11_eac_oes  = 0x9272, // GL_COMPRESSED_RG11_EAC_OES
    compressed_rgb8_etc2_oes = 0x9274, // GL_COMPRESSED_RGB8_ETC2_OES
    compressed_rgb8_punchthrough_alpha1_etc2_oes =
        0x9276, // GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2_OES
    compressed_rgba8_etc2_eac_oes = 0x9278, // GL_COMPRESSED_RGBA8_ETC2_EAC_OES
    compressed_rgba_astc_10x10_khr =
        0x93BB,                             // GL_COMPRESSED_RGBA_ASTC_10x10_KHR
    compressed_rgba_astc_10x5_khr = 0x93B8, // GL_COMPRESSED_RGBA_ASTC_10x5_KHR
    compressed_rgba_astc_10x6_khr = 0x93B9, // GL_COMPRESSED_RGBA_ASTC_10x6_KHR
    compressed_rgba_astc_10x8_khr = 0x93BA, // GL_COMPRESSED_RGBA_ASTC_10x8_KHR
    compressed_rgba_astc_12x10_khr =
        0x93BC, // GL_COMPRESSED_RGBA_ASTC_12x10_KHR
    compressed_rgba_astc_12x12_khr =
        0x93BD, // GL_COMPRESSED_RGBA_ASTC_12x12_KHR
    compressed_rgba_astc_3x3x3_oes =
        0x93C0, // GL_COMPRESSED_RGBA_ASTC_3x3x3_OES
    compressed_rgba_astc_4x3x3_oes =
        0x93C1,                            // GL_COMPRESSED_RGBA_ASTC_4x3x3_OES
    compressed_rgba_astc_4x4_khr = 0x93B0, // GL_COMPRESSED_RGBA_ASTC_4x4_KHR
    compressed_rgba_astc_4x4x3_oes =
        0x93C2, // GL_COMPRESSED_RGBA_ASTC_4x4x3_OES
    compressed_rgba_astc_4x4x4_oes =
        0x93C3,                            // GL_COMPRESSED_RGBA_ASTC_4x4x4_OES
    compressed_rgba_astc_5x4_khr = 0x93B1, // GL_COMPRESSED_RGBA_ASTC_5x4_KHR
    compressed_rgba_astc_5x4x4_oes =
        0x93C4,                            // GL_COMPRESSED_RGBA_ASTC_5x4x4_OES
    compressed_rgba_astc_5x5_khr = 0x93B2, // GL_COMPRESSED_RGBA_ASTC_5x5_KHR
    compressed_rgba_astc_5x5x4_oes =
        0x93C5, // GL_COMPRESSED_RGBA_ASTC_5x5x4_OES
    compressed_rgba_astc_5x5x5_oes =
        0x93C6,                            // GL_COMPRESSED_RGBA_ASTC_5x5x5_OES
    compressed_rgba_astc_6x5_khr = 0x93B3, // GL_COMPRESSED_RGBA_ASTC_6x5_KHR
    compressed_rgba_astc_6x5x5_oes =
        0x93C7,                            // GL_COMPRESSED_RGBA_ASTC_6x5x5_OES
    compressed_rgba_astc_6x6_khr = 0x93B4, // GL_COMPRESSED_RGBA_ASTC_6x6_KHR
    compressed_rgba_astc_6x6x5_oes =
        0x93C8, // GL_COMPRESSED_RGBA_ASTC_6x6x5_OES
    compressed_rgba_astc_6x6x6_oes =
        0x93C9,                            // GL_COMPRESSED_RGBA_ASTC_6x6x6_OES
    compressed_rgba_astc_8x5_khr = 0x93B5, // GL_COMPRESSED_RGBA_ASTC_8x5_KHR
    compressed_rgba_astc_8x6_khr = 0x93B6, // GL_COMPRESSED_RGBA_ASTC_8x6_KHR
    compressed_rgba_astc_8x8_khr = 0x93B7, // GL_COMPRESSED_RGBA_ASTC_8x8_KHR
    compressed_rgba_bptc_unorm_arb =
        0x8E8C, // GL_COMPRESSED_RGBA_BPTC_UNORM_ARB
    compressed_rgba_bptc_unorm_ext =
        0x8E8C,                         // GL_COMPRESSED_RGBA_BPTC_UNORM_EXT
    compressed_rgba_fxt1_3dfx = 0x86B1, // GL_COMPRESSED_RGBA_FXT1_3DFX
    compressed_rgba_pvrtc_2bppv1_img =
        0x8C03, // GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG
    compressed_rgba_pvrtc_2bppv2_img =
        0x9137, // GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG
    compressed_rgba_pvrtc_4bppv1_img =
        0x8C02, // GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG
    compressed_rgba_pvrtc_4bppv2_img =
        0x9138, // GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG
    compressed_rgba_s3tc_dxt1_ext = 0x83F1, // GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
    compressed_rgba_s3tc_dxt3_angle =
        0x83F2, // GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE
    compressed_rgba_s3tc_dxt3_ext = 0x83F2, // GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
    compressed_rgba_s3tc_dxt5_angle =
        0x83F3, // GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE
    compressed_rgba_s3tc_dxt5_ext = 0x83F3, // GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
    compressed_rgb_bptc_signed_float_arb =
        0x8E8E, // GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB
    compressed_rgb_bptc_signed_float_ext =
        0x8E8E, // GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_EXT
    compressed_rgb_bptc_unsigned_float_arb =
        0x8E8F, // GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB
    compressed_rgb_bptc_unsigned_float_ext =
        0x8E8F, // GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_EXT
    compressed_rgb_fxt1_3dfx = 0x86B0, // GL_COMPRESSED_RGB_FXT1_3DFX
    compressed_rgb_pvrtc_2bppv1_img =
        0x8C01, // GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG
    compressed_rgb_pvrtc_4bppv1_img =
        0x8C00,                            // GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG
    compressed_rgb_s3tc_dxt1_ext = 0x83F0, // GL_COMPRESSED_RGB_S3TC_DXT1_EXT
    compressed_signed_luminance_alpha_latc2_ext =
        0x8C73, // GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT
    compressed_signed_luminance_latc1_ext =
        0x8C71, // GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT
    compressed_signed_r11_eac_oes = 0x9271, // GL_COMPRESSED_SIGNED_R11_EAC_OES
    compressed_signed_red_green_rgtc2_ext =
        0x8DBE, // GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT
    compressed_signed_red_rgtc1_ext =
        0x8DBC, // GL_COMPRESSED_SIGNED_RED_RGTC1_EXT
    compressed_signed_rg11_eac_oes =
        0x9273, // GL_COMPRESSED_SIGNED_RG11_EAC_OES
    compressed_srgb8_alpha8_astc_10x10_khr =
        0x93DB, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR
    compressed_srgb8_alpha8_astc_10x5_khr =
        0x93D8, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR
    compressed_srgb8_alpha8_astc_10x6_khr =
        0x93D9, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR
    compressed_srgb8_alpha8_astc_10x8_khr =
        0x93DA, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR
    compressed_srgb8_alpha8_astc_12x10_khr =
        0x93DC, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR
    compressed_srgb8_alpha8_astc_12x12_khr =
        0x93DD, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR
    compressed_srgb8_alpha8_astc_3x3x3_oes =
        0x93E0, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_3x3x3_OES
    compressed_srgb8_alpha8_astc_4x3x3_oes =
        0x93E1, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x3x3_OES
    compressed_srgb8_alpha8_astc_4x4_khr =
        0x93D0, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR
    compressed_srgb8_alpha8_astc_4x4x3_oes =
        0x93E2, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x3_OES
    compressed_srgb8_alpha8_astc_4x4x4_oes =
        0x93E3, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x4_OES
    compressed_srgb8_alpha8_astc_5x4_khr =
        0x93D1, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR
    compressed_srgb8_alpha8_astc_5x4x4_oes =
        0x93E4, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4x4_OES
    compressed_srgb8_alpha8_astc_5x5_khr =
        0x93D2, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR
    compressed_srgb8_alpha8_astc_5x5x4_oes =
        0x93E5, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x4_OES
    compressed_srgb8_alpha8_astc_5x5x5_oes =
        0x93E6, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x5_OES
    compressed_srgb8_alpha8_astc_6x5_khr =
        0x93D3, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR
    compressed_srgb8_alpha8_astc_6x5x5_oes =
        0x93E7, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5x5_OES
    compressed_srgb8_alpha8_astc_6x6_khr =
        0x93D4, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR
    compressed_srgb8_alpha8_astc_6x6x5_oes =
        0x93E8, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x5_OES
    compressed_srgb8_alpha8_astc_6x6x6_oes =
        0x93E9, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x6_OES
    compressed_srgb8_alpha8_astc_8x5_khr =
        0x93D5, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR
    compressed_srgb8_alpha8_astc_8x6_khr =
        0x93D6, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR
    compressed_srgb8_alpha8_astc_8x8_khr =
        0x93D7, // GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR
    compressed_srgb8_alpha8_etc2_eac_oes =
        0x9279, // GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC_OES
    compressed_srgb8_etc2_oes = 0x9275, // GL_COMPRESSED_SRGB8_ETC2_OES
    compressed_srgb8_punchthrough_alpha1_etc2_oes =
        0x9277, // GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2_OES
    compressed_srgb_alpha_bptc_unorm_arb =
        0x8E8D, // GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB
    compressed_srgb_alpha_bptc_unorm_ext =
        0x8E8D, // GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_EXT
    compressed_srgb_alpha_pvrtc_2bppv1_ext =
        0x8A56, // GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT
    compressed_srgb_alpha_pvrtc_2bppv2_img =
        0x93F0, // GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG
    compressed_srgb_alpha_pvrtc_4bppv1_ext =
        0x8A57, // GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT
    compressed_srgb_alpha_pvrtc_4bppv2_img =
        0x93F1, // GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG
    compressed_srgb_alpha_s3tc_dxt1_ext =
        0x8C4D, // GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT
    compressed_srgb_alpha_s3tc_dxt1_nv =
        0x8C4D, // GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_NV
    compressed_srgb_alpha_s3tc_dxt3_ext =
        0x8C4E, // GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT
    compressed_srgb_alpha_s3tc_dxt3_nv =
        0x8C4E, // GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_NV
    compressed_srgb_alpha_s3tc_dxt5_ext =
        0x8C4F, // GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT
    compressed_srgb_alpha_s3tc_dxt5_nv =
        0x8C4F, // GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_NV
    compressed_srgb_pvrtc_2bppv1_ext =
        0x8A54, // GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT
    compressed_srgb_pvrtc_4bppv1_ext =
        0x8A55, // GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT
    compressed_srgb_s3tc_dxt1_ext = 0x8C4C, // GL_COMPRESSED_SRGB_S3TC_DXT1_EXT
    compressed_srgb_s3tc_dxt1_nv  = 0x8C4C, // GL_COMPRESSED_SRGB_S3TC_DXT1_NV
    depth24_stencil8_ext          = 0x88F0, // GL_DEPTH24_STENCIL8_EXT
    depth24_stencil8_oes          = 0x88F0, // GL_DEPTH24_STENCIL8_OES
    depth32f_stencil8_nv          = 0x8DAC, // GL_DEPTH32F_STENCIL8_NV
    depth_component16_arb         = 0x81A5, // GL_DEPTH_COMPONENT16_ARB
    depth_component16_oes         = 0x81A5, // GL_DEPTH_COMPONENT16_OES
    depth_component16_sgix        = 0x81A5, // GL_DEPTH_COMPONENT16_SGIX
    depth_component24_arb         = 0x81A6, // GL_DEPTH_COMPONENT24_ARB
    depth_component24_oes         = 0x81A6, // GL_DEPTH_COMPONENT24_OES
    depth_component24_sgix        = 0x81A6, // GL_DEPTH_COMPONENT24_SGIX
    depth_component32f_nv         = 0x8DAB, // GL_DEPTH_COMPONENT32F_NV
    depth_component32_arb         = 0x81A7, // GL_DEPTH_COMPONENT32_ARB
    depth_component32_oes         = 0x81A7, // GL_DEPTH_COMPONENT32_OES
    depth_component32_sgix        = 0x81A7, // GL_DEPTH_COMPONENT32_SGIX
    etc1_rgb8_oes                 = 0x8D64, // GL_ETC1_RGB8_OES
    etc1_srgb8_nv                 = 0x88EE, // GL_ETC1_SRGB8_NV
    intensity12_ext               = 0x804C, // GL_INTENSITY12_EXT
    intensity16i_ext              = 0x8D8B, // GL_INTENSITY16I_EXT
    intensity16ui_ext             = 0x8D79, // GL_INTENSITY16UI_EXT
    intensity16_ext               = 0x804D, // GL_INTENSITY16_EXT
    intensity32i_ext              = 0x8D85, // GL_INTENSITY32I_EXT
    intensity32ui_ext             = 0x8D73, // GL_INTENSITY32UI_EXT
    intensity4_ext                = 0x804A, // GL_INTENSITY4_EXT
    intensity8i_ext               = 0x8D91, // GL_INTENSITY8I_EXT
    intensity8ui_ext              = 0x8D7F, // GL_INTENSITY8UI_EXT
    intensity8_ext                = 0x804B, // GL_INTENSITY8_EXT
    luminance12_alpha12_ext       = 0x8047, // GL_LUMINANCE12_ALPHA12_EXT
    luminance12_alpha4_ext        = 0x8046, // GL_LUMINANCE12_ALPHA4_EXT
    luminance12_ext               = 0x8041, // GL_LUMINANCE12_EXT
    luminance16i_ext              = 0x8D8C, // GL_LUMINANCE16I_EXT
    luminance16ui_ext             = 0x8D7A, // GL_LUMINANCE16UI_EXT
    luminance16_alpha16_ext       = 0x8048, // GL_LUMINANCE16_ALPHA16_EXT
    luminance16_ext               = 0x8042, // GL_LUMINANCE16_EXT
    luminance32i_ext              = 0x8D86, // GL_LUMINANCE32I_EXT
    luminance32ui_ext             = 0x8D74, // GL_LUMINANCE32UI_EXT
    luminance4_alpha4_ext         = 0x8043, // GL_LUMINANCE4_ALPHA4_EXT
    luminance4_alpha4_oes         = 0x8043, // GL_LUMINANCE4_ALPHA4_OES
    luminance4_ext                = 0x803F, // GL_LUMINANCE4_EXT
    luminance6_alpha2_ext         = 0x8044, // GL_LUMINANCE6_ALPHA2_EXT
    luminance8i_ext               = 0x8D92, // GL_LUMINANCE8I_EXT
    luminance8ui_ext              = 0x8D80, // GL_LUMINANCE8UI_EXT
    luminance8_alpha8_ext         = 0x8045, // GL_LUMINANCE8_ALPHA8_EXT
    luminance8_alpha8_oes         = 0x8045, // GL_LUMINANCE8_ALPHA8_OES
    luminance8_ext                = 0x8040, // GL_LUMINANCE8_EXT
    luminance8_oes                = 0x8040, // GL_LUMINANCE8_OES
    luminance_alpha16i_ext        = 0x8D8D, // GL_LUMINANCE_ALPHA16I_EXT
    luminance_alpha16ui_ext       = 0x8D7B, // GL_LUMINANCE_ALPHA16UI_EXT
    luminance_alpha32i_ext        = 0x8D87, // GL_LUMINANCE_ALPHA32I_EXT
    luminance_alpha32ui_ext       = 0x8D75, // GL_LUMINANCE_ALPHA32UI_EXT
    luminance_alpha8i_ext         = 0x8D93, // GL_LUMINANCE_ALPHA8I_EXT
    luminance_alpha8ui_ext        = 0x8D81, // GL_LUMINANCE_ALPHA8UI_EXT
    r11f_g11f_b10f_apple          = 0x8C3A, // GL_R11F_G11F_B10F_APPLE
    r11f_g11f_b10f_ext            = 0x8C3A, // GL_R11F_G11F_B10F_EXT
    r16f_ext                      = 0x822D, // GL_R16F_EXT
    r16_ext                       = 0x822A, // GL_R16_EXT
    r16_snorm_ext                 = 0x8F98, // GL_R16_SNORM_EXT
    r32f_ext                      = 0x822E, // GL_R32F_EXT
    r8_ext                        = 0x8229, // GL_R8_EXT
    rg16f_ext                     = 0x822F, // GL_RG16F_EXT
    rg16_ext                      = 0x822C, // GL_RG16_EXT
    rg16_snorm_ext                = 0x8F99, // GL_RG16_SNORM_EXT
    rg32f_ext                     = 0x8230, // GL_RG32F_EXT
    rg8_ext                       = 0x822B, // GL_RG8_EXT
    rgb10_a2_ext                  = 0x8059, // GL_RGB10_A2_EXT
    rgb10_ext                     = 0x8052, // GL_RGB10_EXT
    rgb12_ext                     = 0x8053, // GL_RGB12_EXT
    rgb16f_arb                    = 0x881B, // GL_RGB16F_ARB
    rgb16f_ext                    = 0x881B, // GL_RGB16F_EXT
    rgb16i_ext                    = 0x8D89, // GL_RGB16I_EXT
    rgb16ui_ext                   = 0x8D77, // GL_RGB16UI_EXT
    rgb16_ext                     = 0x8054, // GL_RGB16_EXT
    rgb16_snorm_ext               = 0x8F9A, // GL_RGB16_SNORM_EXT
    rgb2_ext                      = 0x804E, // GL_RGB2_EXT
    rgb32f_arb                    = 0x8815, // GL_RGB32F_ARB
    rgb32f_ext                    = 0x8815, // GL_RGB32F_EXT
    rgb32i_ext                    = 0x8D83, // GL_RGB32I_EXT
    rgb32ui_ext                   = 0x8D71, // GL_RGB32UI_EXT
    rgb4_ext                      = 0x804F, // GL_RGB4_EXT
    rgb565_oes                    = 0x8D62, // GL_RGB565_OES
    rgb5_a1_ext                   = 0x8057, // GL_RGB5_A1_EXT
    rgb5_a1_oes                   = 0x8057, // GL_RGB5_A1_OES
    rgb5_ext                      = 0x8050, // GL_RGB5_EXT
    rgb8i_ext                     = 0x8D8F, // GL_RGB8I_EXT
    rgb8ui_ext                    = 0x8D7D, // GL_RGB8UI_EXT
    rgb8_ext                      = 0x8051, // GL_RGB8_EXT
    rgb8_oes                      = 0x8051, // GL_RGB8_OES
    rgb9_e5_apple                 = 0x8C3D, // GL_RGB9_E5_APPLE
    rgb9_e5_ext                   = 0x8C3D, // GL_RGB9_E5_EXT
    rgba12_ext                    = 0x805A, // GL_RGBA12_EXT
    rgba16f_arb                   = 0x881A, // GL_RGBA16F_ARB
    rgba16f_ext                   = 0x881A, // GL_RGBA16F_EXT
    rgba16i_ext                   = 0x8D88, // GL_RGBA16I_EXT
    rgba16ui_ext                  = 0x8D76, // GL_RGBA16UI_EXT
    rgba16_ext                    = 0x805B, // GL_RGBA16_EXT
    rgba16_snorm_ext              = 0x8F9B, // GL_RGBA16_SNORM_EXT
    rgba2_ext                     = 0x8055, // GL_RGBA2_EXT
    rgba32f_arb                   = 0x8814, // GL_RGBA32F_ARB
    rgba32f_ext                   = 0x8814, // GL_RGBA32F_EXT
    rgba32i_ext                   = 0x8D82, // GL_RGBA32I_EXT
    rgba32ui_ext                  = 0x8D70, // GL_RGBA32UI_EXT
    rgba4_ext                     = 0x8056, // GL_RGBA4_EXT
    rgba4_oes                     = 0x8056, // GL_RGBA4_OES
    rgba8i_ext                    = 0x8D8E, // GL_RGBA8I_EXT
    rgba8ui_ext                   = 0x8D7C, // GL_RGBA8UI_EXT
    rgba8_ext                     = 0x8058, // GL_RGBA8_EXT
    rgba8_oes                     = 0x8058, // GL_RGBA8_OES
    srgb8_alpha8_ext              = 0x8C43, // GL_SRGB8_ALPHA8_EXT
    srgb8_ext                     = 0x8C41, // GL_SRGB8_EXT
    srgb8_nv                      = 0x8C41, // GL_SRGB8_NV
    stencil_index16_ext           = 0x8D49, // GL_STENCIL_INDEX16_EXT
    stencil_index1_ext            = 0x8D46, // GL_STENCIL_INDEX1_EXT
    stencil_index1_oes            = 0x8D46, // GL_STENCIL_INDEX1_OES
    stencil_index4_ext            = 0x8D47, // GL_STENCIL_INDEX4_EXT
    stencil_index4_oes            = 0x8D47, // GL_STENCIL_INDEX4_OES
    stencil_index8_ext            = 0x8D48, // GL_STENCIL_INDEX8_EXT
    stencil_index8_oes            = 0x8D48, // GL_STENCIL_INDEX8_OES
}; // enum class sized_internal_format

} // namespace gl::group
