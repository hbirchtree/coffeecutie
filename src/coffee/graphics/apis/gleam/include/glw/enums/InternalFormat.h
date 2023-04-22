#pragma once

#include "common.h"

namespace gl::group {

// InternalFormat
enum class internal_format : u32
{
#ifdef GL_ALPHA12
    alpha12 = GL_ALPHA12,
#endif
#ifdef GL_ALPHA12_EXT
    alpha12_ext = GL_ALPHA12_EXT,
#endif
#ifdef GL_ALPHA16
    alpha16 = GL_ALPHA16,
#endif
#ifdef GL_ALPHA16I_EXT
    alpha16i_ext = GL_ALPHA16I_EXT,
#endif
#ifdef GL_ALPHA16UI_EXT
    alpha16ui_ext = GL_ALPHA16UI_EXT,
#endif
#ifdef GL_ALPHA16_EXT
    alpha16_ext = GL_ALPHA16_EXT,
#endif
#ifdef GL_ALPHA32I_EXT
    alpha32i_ext = GL_ALPHA32I_EXT,
#endif
#ifdef GL_ALPHA32UI_EXT
    alpha32ui_ext = GL_ALPHA32UI_EXT,
#endif
#ifdef GL_ALPHA4
    alpha4 = GL_ALPHA4,
#endif
#ifdef GL_ALPHA4_EXT
    alpha4_ext = GL_ALPHA4_EXT,
#endif
#ifdef GL_ALPHA8
    alpha8 = GL_ALPHA8,
#endif
#ifdef GL_ALPHA8I_EXT
    alpha8i_ext = GL_ALPHA8I_EXT,
#endif
#ifdef GL_ALPHA8UI_EXT
    alpha8ui_ext = GL_ALPHA8UI_EXT,
#endif
#ifdef GL_ALPHA8_EXT
    alpha8_ext = GL_ALPHA8_EXT,
#endif
#ifdef GL_ALPHA8_OES
    alpha8_oes = GL_ALPHA8_OES,
#endif
#ifdef GL_COMPRESSED_R11_EAC
    compressed_r11_eac = GL_COMPRESSED_R11_EAC,
#endif
#ifdef GL_COMPRESSED_R11_EAC_OES
    compressed_r11_eac_oes = GL_COMPRESSED_R11_EAC_OES,
#endif
#ifdef GL_COMPRESSED_RED
    compressed_red = GL_COMPRESSED_RED,
#endif
#ifdef GL_COMPRESSED_RED_GREEN_RGTC2_EXT
    compressed_red_green_rgtc2_ext = GL_COMPRESSED_RED_GREEN_RGTC2_EXT,
#endif
#ifdef GL_COMPRESSED_RED_RGTC1
    compressed_red_rgtc1 = GL_COMPRESSED_RED_RGTC1,
#endif
#ifdef GL_COMPRESSED_RED_RGTC1_EXT
    compressed_red_rgtc1_ext = GL_COMPRESSED_RED_RGTC1_EXT,
#endif
#ifdef GL_COMPRESSED_RG
    compressed_rg = GL_COMPRESSED_RG,
#endif
#ifdef GL_COMPRESSED_RG11_EAC
    compressed_rg11_eac = GL_COMPRESSED_RG11_EAC,
#endif
#ifdef GL_COMPRESSED_RG11_EAC_OES
    compressed_rg11_eac_oes = GL_COMPRESSED_RG11_EAC_OES,
#endif
#ifdef GL_COMPRESSED_RGB
    compressed_rgb = GL_COMPRESSED_RGB,
#endif
#ifdef GL_COMPRESSED_RGB8_ETC2
    compressed_rgb8_etc2 = GL_COMPRESSED_RGB8_ETC2,
#endif
#ifdef GL_COMPRESSED_RGB8_ETC2_OES
    compressed_rgb8_etc2_oes = GL_COMPRESSED_RGB8_ETC2_OES,
#endif
#ifdef GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2
    compressed_rgb8_punchthrough_alpha1_etc2 =
        GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,
#endif
#ifdef GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2_OES
    compressed_rgb8_punchthrough_alpha1_etc2_oes =
        GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2_OES,
#endif
#ifdef GL_COMPRESSED_RGBA
    compressed_rgba = GL_COMPRESSED_RGBA,
#endif
#ifdef GL_COMPRESSED_RGBA8_ETC2_EAC
    compressed_rgba8_etc2_eac = GL_COMPRESSED_RGBA8_ETC2_EAC,
#endif
#ifdef GL_COMPRESSED_RGBA8_ETC2_EAC_OES
    compressed_rgba8_etc2_eac_oes = GL_COMPRESSED_RGBA8_ETC2_EAC_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_10x10
    compressed_rgba_astc_10x10 = GL_COMPRESSED_RGBA_ASTC_10x10,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_10x10_KHR
    compressed_rgba_astc_10x10_khr = GL_COMPRESSED_RGBA_ASTC_10x10_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_10x5
    compressed_rgba_astc_10x5 = GL_COMPRESSED_RGBA_ASTC_10x5,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_10x5_KHR
    compressed_rgba_astc_10x5_khr = GL_COMPRESSED_RGBA_ASTC_10x5_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_10x6
    compressed_rgba_astc_10x6 = GL_COMPRESSED_RGBA_ASTC_10x6,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_10x6_KHR
    compressed_rgba_astc_10x6_khr = GL_COMPRESSED_RGBA_ASTC_10x6_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_10x8
    compressed_rgba_astc_10x8 = GL_COMPRESSED_RGBA_ASTC_10x8,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_10x8_KHR
    compressed_rgba_astc_10x8_khr = GL_COMPRESSED_RGBA_ASTC_10x8_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_12x10
    compressed_rgba_astc_12x10 = GL_COMPRESSED_RGBA_ASTC_12x10,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_12x10_KHR
    compressed_rgba_astc_12x10_khr = GL_COMPRESSED_RGBA_ASTC_12x10_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_12x12
    compressed_rgba_astc_12x12 = GL_COMPRESSED_RGBA_ASTC_12x12,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_12x12_KHR
    compressed_rgba_astc_12x12_khr = GL_COMPRESSED_RGBA_ASTC_12x12_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_3x3x3_OES
    compressed_rgba_astc_3x3x3_oes = GL_COMPRESSED_RGBA_ASTC_3x3x3_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_4x3x3_OES
    compressed_rgba_astc_4x3x3_oes = GL_COMPRESSED_RGBA_ASTC_4x3x3_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_4x4
    compressed_rgba_astc_4x4 = GL_COMPRESSED_RGBA_ASTC_4x4,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_4x4_KHR
    compressed_rgba_astc_4x4_khr = GL_COMPRESSED_RGBA_ASTC_4x4_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_4x4x3_OES
    compressed_rgba_astc_4x4x3_oes = GL_COMPRESSED_RGBA_ASTC_4x4x3_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_4x4x4_OES
    compressed_rgba_astc_4x4x4_oes = GL_COMPRESSED_RGBA_ASTC_4x4x4_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_5x4
    compressed_rgba_astc_5x4 = GL_COMPRESSED_RGBA_ASTC_5x4,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_5x4_KHR
    compressed_rgba_astc_5x4_khr = GL_COMPRESSED_RGBA_ASTC_5x4_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_5x4x4_OES
    compressed_rgba_astc_5x4x4_oes = GL_COMPRESSED_RGBA_ASTC_5x4x4_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_5x5
    compressed_rgba_astc_5x5 = GL_COMPRESSED_RGBA_ASTC_5x5,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_5x5_KHR
    compressed_rgba_astc_5x5_khr = GL_COMPRESSED_RGBA_ASTC_5x5_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_5x5x4_OES
    compressed_rgba_astc_5x5x4_oes = GL_COMPRESSED_RGBA_ASTC_5x5x4_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_5x5x5_OES
    compressed_rgba_astc_5x5x5_oes = GL_COMPRESSED_RGBA_ASTC_5x5x5_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_6x5
    compressed_rgba_astc_6x5 = GL_COMPRESSED_RGBA_ASTC_6x5,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_6x5_KHR
    compressed_rgba_astc_6x5_khr = GL_COMPRESSED_RGBA_ASTC_6x5_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_6x5x5_OES
    compressed_rgba_astc_6x5x5_oes = GL_COMPRESSED_RGBA_ASTC_6x5x5_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_6x6
    compressed_rgba_astc_6x6 = GL_COMPRESSED_RGBA_ASTC_6x6,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_6x6_KHR
    compressed_rgba_astc_6x6_khr = GL_COMPRESSED_RGBA_ASTC_6x6_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_6x6x5_OES
    compressed_rgba_astc_6x6x5_oes = GL_COMPRESSED_RGBA_ASTC_6x6x5_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_6x6x6_OES
    compressed_rgba_astc_6x6x6_oes = GL_COMPRESSED_RGBA_ASTC_6x6x6_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_8x5
    compressed_rgba_astc_8x5 = GL_COMPRESSED_RGBA_ASTC_8x5,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_8x5_KHR
    compressed_rgba_astc_8x5_khr = GL_COMPRESSED_RGBA_ASTC_8x5_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_8x6
    compressed_rgba_astc_8x6 = GL_COMPRESSED_RGBA_ASTC_8x6,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_8x6_KHR
    compressed_rgba_astc_8x6_khr = GL_COMPRESSED_RGBA_ASTC_8x6_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_8x8
    compressed_rgba_astc_8x8 = GL_COMPRESSED_RGBA_ASTC_8x8,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_8x8_KHR
    compressed_rgba_astc_8x8_khr = GL_COMPRESSED_RGBA_ASTC_8x8_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_BPTC_UNORM
    compressed_rgba_bptc_unorm = GL_COMPRESSED_RGBA_BPTC_UNORM,
#endif
#ifdef GL_COMPRESSED_RGBA_BPTC_UNORM_ARB
    compressed_rgba_bptc_unorm_arb = GL_COMPRESSED_RGBA_BPTC_UNORM_ARB,
#endif
#ifdef GL_COMPRESSED_RGBA_BPTC_UNORM_EXT
    compressed_rgba_bptc_unorm_ext = GL_COMPRESSED_RGBA_BPTC_UNORM_EXT,
#endif
#ifdef GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG
    compressed_rgba_pvrtc_2bppv1_img = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG,
#endif
#ifdef GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG
    compressed_rgba_pvrtc_2bppv2_img = GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG,
#endif
#ifdef GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG
    compressed_rgba_pvrtc_4bppv1_img = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG,
#endif
#ifdef GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG
    compressed_rgba_pvrtc_4bppv2_img = GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG,
#endif
#ifdef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
    compressed_rgba_s3tc_dxt1_ext = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
#endif
#ifdef GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE
    compressed_rgba_s3tc_dxt3_angle = GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE,
#endif
#ifdef GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
    compressed_rgba_s3tc_dxt3_ext = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
#endif
#ifdef GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE
    compressed_rgba_s3tc_dxt5_angle = GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE,
#endif
#ifdef GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
    compressed_rgba_s3tc_dxt5_ext = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
#endif
#ifdef GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT
    compressed_rgb_bptc_signed_float = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT,
#endif
#ifdef GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB
    compressed_rgb_bptc_signed_float_arb =
        GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB,
#endif
#ifdef GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_EXT
    compressed_rgb_bptc_signed_float_ext =
        GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_EXT,
#endif
#ifdef GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT
    compressed_rgb_bptc_unsigned_float = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,
#endif
#ifdef GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB
    compressed_rgb_bptc_unsigned_float_arb =
        GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB,
#endif
#ifdef GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_EXT
    compressed_rgb_bptc_unsigned_float_ext =
        GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_EXT,
#endif
#ifdef GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG
    compressed_rgb_pvrtc_2bppv1_img = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG,
#endif
#ifdef GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG
    compressed_rgb_pvrtc_4bppv1_img = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG,
#endif
#ifdef GL_COMPRESSED_RGB_S3TC_DXT1_EXT
    compressed_rgb_s3tc_dxt1_ext = GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
#endif
#ifdef GL_COMPRESSED_RG_RGTC2
    compressed_rg_rgtc2 = GL_COMPRESSED_RG_RGTC2,
#endif
#ifdef GL_COMPRESSED_SIGNED_R11_EAC
    compressed_signed_r11_eac = GL_COMPRESSED_SIGNED_R11_EAC,
#endif
#ifdef GL_COMPRESSED_SIGNED_R11_EAC_OES
    compressed_signed_r11_eac_oes = GL_COMPRESSED_SIGNED_R11_EAC_OES,
#endif
#ifdef GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT
    compressed_signed_red_green_rgtc2_ext =
        GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT,
#endif
#ifdef GL_COMPRESSED_SIGNED_RED_RGTC1
    compressed_signed_red_rgtc1 = GL_COMPRESSED_SIGNED_RED_RGTC1,
#endif
#ifdef GL_COMPRESSED_SIGNED_RED_RGTC1_EXT
    compressed_signed_red_rgtc1_ext = GL_COMPRESSED_SIGNED_RED_RGTC1_EXT,
#endif
#ifdef GL_COMPRESSED_SIGNED_RG11_EAC
    compressed_signed_rg11_eac = GL_COMPRESSED_SIGNED_RG11_EAC,
#endif
#ifdef GL_COMPRESSED_SIGNED_RG11_EAC_OES
    compressed_signed_rg11_eac_oes = GL_COMPRESSED_SIGNED_RG11_EAC_OES,
#endif
#ifdef GL_COMPRESSED_SIGNED_RG_RGTC2
    compressed_signed_rg_rgtc2 = GL_COMPRESSED_SIGNED_RG_RGTC2,
#endif
#ifdef GL_COMPRESSED_SRGB
    compressed_srgb = GL_COMPRESSED_SRGB,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10
    compressed_srgb8_alpha8_astc_10x10 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR
    compressed_srgb8_alpha8_astc_10x10_khr =
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5
    compressed_srgb8_alpha8_astc_10x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR
    compressed_srgb8_alpha8_astc_10x5_khr =
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6
    compressed_srgb8_alpha8_astc_10x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR
    compressed_srgb8_alpha8_astc_10x6_khr =
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8
    compressed_srgb8_alpha8_astc_10x8 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR
    compressed_srgb8_alpha8_astc_10x8_khr =
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10
    compressed_srgb8_alpha8_astc_12x10 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR
    compressed_srgb8_alpha8_astc_12x10_khr =
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12
    compressed_srgb8_alpha8_astc_12x12 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR
    compressed_srgb8_alpha8_astc_12x12_khr =
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_3x3x3_OES
    compressed_srgb8_alpha8_astc_3x3x3_oes =
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_3x3x3_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x3x3_OES
    compressed_srgb8_alpha8_astc_4x3x3_oes =
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x3x3_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4
    compressed_srgb8_alpha8_astc_4x4 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR
    compressed_srgb8_alpha8_astc_4x4_khr =
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x3_OES
    compressed_srgb8_alpha8_astc_4x4x3_oes =
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x3_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x4_OES
    compressed_srgb8_alpha8_astc_4x4x4_oes =
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x4_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4
    compressed_srgb8_alpha8_astc_5x4 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR
    compressed_srgb8_alpha8_astc_5x4_khr =
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4x4_OES
    compressed_srgb8_alpha8_astc_5x4x4_oes =
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4x4_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5
    compressed_srgb8_alpha8_astc_5x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR
    compressed_srgb8_alpha8_astc_5x5_khr =
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x4_OES
    compressed_srgb8_alpha8_astc_5x5x4_oes =
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x4_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x5_OES
    compressed_srgb8_alpha8_astc_5x5x5_oes =
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x5_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5
    compressed_srgb8_alpha8_astc_6x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR
    compressed_srgb8_alpha8_astc_6x5_khr =
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5x5_OES
    compressed_srgb8_alpha8_astc_6x5x5_oes =
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5x5_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6
    compressed_srgb8_alpha8_astc_6x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR
    compressed_srgb8_alpha8_astc_6x6_khr =
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x5_OES
    compressed_srgb8_alpha8_astc_6x6x5_oes =
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x5_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x6_OES
    compressed_srgb8_alpha8_astc_6x6x6_oes =
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x6_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5
    compressed_srgb8_alpha8_astc_8x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR
    compressed_srgb8_alpha8_astc_8x5_khr =
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6
    compressed_srgb8_alpha8_astc_8x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR
    compressed_srgb8_alpha8_astc_8x6_khr =
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8
    compressed_srgb8_alpha8_astc_8x8 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR
    compressed_srgb8_alpha8_astc_8x8_khr =
        GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC
    compressed_srgb8_alpha8_etc2_eac = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC_OES
    compressed_srgb8_alpha8_etc2_eac_oes =
        GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ETC2
    compressed_srgb8_etc2 = GL_COMPRESSED_SRGB8_ETC2,
#endif
#ifdef GL_COMPRESSED_SRGB8_ETC2_OES
    compressed_srgb8_etc2_oes = GL_COMPRESSED_SRGB8_ETC2_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2
    compressed_srgb8_punchthrough_alpha1_etc2 =
        GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,
#endif
#ifdef GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2_OES
    compressed_srgb8_punchthrough_alpha1_etc2_oes =
        GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2_OES,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA
    compressed_srgb_alpha = GL_COMPRESSED_SRGB_ALPHA,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM
    compressed_srgb_alpha_bptc_unorm = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB
    compressed_srgb_alpha_bptc_unorm_arb =
        GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_EXT
    compressed_srgb_alpha_bptc_unorm_ext =
        GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_EXT,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT
    compressed_srgb_alpha_pvrtc_2bppv1_ext =
        GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG
    compressed_srgb_alpha_pvrtc_2bppv2_img =
        GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT
    compressed_srgb_alpha_pvrtc_4bppv1_ext =
        GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG
    compressed_srgb_alpha_pvrtc_4bppv2_img =
        GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT
    compressed_srgb_alpha_s3tc_dxt1_ext =
        GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_NV
    compressed_srgb_alpha_s3tc_dxt1_nv = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_NV,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT
    compressed_srgb_alpha_s3tc_dxt3_ext =
        GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_NV
    compressed_srgb_alpha_s3tc_dxt3_nv = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_NV,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT
    compressed_srgb_alpha_s3tc_dxt5_ext =
        GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_NV
    compressed_srgb_alpha_s3tc_dxt5_nv = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_NV,
#endif
#ifdef GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT
    compressed_srgb_pvrtc_2bppv1_ext = GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT,
#endif
#ifdef GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT
    compressed_srgb_pvrtc_4bppv1_ext = GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT,
#endif
#ifdef GL_COMPRESSED_SRGB_S3TC_DXT1_EXT
    compressed_srgb_s3tc_dxt1_ext = GL_COMPRESSED_SRGB_S3TC_DXT1_EXT,
#endif
#ifdef GL_COMPRESSED_SRGB_S3TC_DXT1_NV
    compressed_srgb_s3tc_dxt1_nv = GL_COMPRESSED_SRGB_S3TC_DXT1_NV,
#endif
#ifdef GL_DEPTH24_STENCIL8
    depth24_stencil8 = GL_DEPTH24_STENCIL8,
#endif
#ifdef GL_DEPTH24_STENCIL8_EXT
    depth24_stencil8_ext = GL_DEPTH24_STENCIL8_EXT,
#endif
#ifdef GL_DEPTH24_STENCIL8_OES
    depth24_stencil8_oes = GL_DEPTH24_STENCIL8_OES,
#endif
#ifdef GL_DEPTH32F_STENCIL8
    depth32f_stencil8 = GL_DEPTH32F_STENCIL8,
#endif
#ifdef GL_DEPTH32F_STENCIL8_NV
    depth32f_stencil8_nv = GL_DEPTH32F_STENCIL8_NV,
#endif
#ifdef GL_DEPTH_COMPONENT
    depth_component = GL_DEPTH_COMPONENT,
#endif
#ifdef GL_DEPTH_COMPONENT16
    depth_component16 = GL_DEPTH_COMPONENT16,
#endif
#ifdef GL_DEPTH_COMPONENT16_ARB
    depth_component16_arb = GL_DEPTH_COMPONENT16_ARB,
#endif
#ifdef GL_DEPTH_COMPONENT16_OES
    depth_component16_oes = GL_DEPTH_COMPONENT16_OES,
#endif
#ifdef GL_DEPTH_COMPONENT16_SGIX
    depth_component16_sgix = GL_DEPTH_COMPONENT16_SGIX,
#endif
#ifdef GL_DEPTH_COMPONENT24
    depth_component24 = GL_DEPTH_COMPONENT24,
#endif
#ifdef GL_DEPTH_COMPONENT24_ARB
    depth_component24_arb = GL_DEPTH_COMPONENT24_ARB,
#endif
#ifdef GL_DEPTH_COMPONENT24_OES
    depth_component24_oes = GL_DEPTH_COMPONENT24_OES,
#endif
#ifdef GL_DEPTH_COMPONENT24_SGIX
    depth_component24_sgix = GL_DEPTH_COMPONENT24_SGIX,
#endif
#ifdef GL_DEPTH_COMPONENT32
    depth_component32 = GL_DEPTH_COMPONENT32,
#endif
#ifdef GL_DEPTH_COMPONENT32F
    depth_component32f = GL_DEPTH_COMPONENT32F,
#endif
#ifdef GL_DEPTH_COMPONENT32F_NV
    depth_component32f_nv = GL_DEPTH_COMPONENT32F_NV,
#endif
#ifdef GL_DEPTH_COMPONENT32_ARB
    depth_component32_arb = GL_DEPTH_COMPONENT32_ARB,
#endif
#ifdef GL_DEPTH_COMPONENT32_OES
    depth_component32_oes = GL_DEPTH_COMPONENT32_OES,
#endif
#ifdef GL_DEPTH_COMPONENT32_SGIX
    depth_component32_sgix = GL_DEPTH_COMPONENT32_SGIX,
#endif
#ifdef GL_DEPTH_STENCIL
    depth_stencil = GL_DEPTH_STENCIL,
#endif
#ifdef GL_DEPTH_STENCIL_EXT
    depth_stencil_ext = GL_DEPTH_STENCIL_EXT,
#endif
#ifdef GL_DEPTH_STENCIL_MESA
    depth_stencil_mesa = GL_DEPTH_STENCIL_MESA,
#endif
#ifdef GL_DEPTH_STENCIL_NV
    depth_stencil_nv = GL_DEPTH_STENCIL_NV,
#endif
#ifdef GL_DEPTH_STENCIL_OES
    depth_stencil_oes = GL_DEPTH_STENCIL_OES,
#endif
#ifdef GL_DUAL_ALPHA12_SGIS
    dual_alpha12_sgis = GL_DUAL_ALPHA12_SGIS,
#endif
#ifdef GL_DUAL_ALPHA16_SGIS
    dual_alpha16_sgis = GL_DUAL_ALPHA16_SGIS,
#endif
#ifdef GL_DUAL_ALPHA4_SGIS
    dual_alpha4_sgis = GL_DUAL_ALPHA4_SGIS,
#endif
#ifdef GL_DUAL_ALPHA8_SGIS
    dual_alpha8_sgis = GL_DUAL_ALPHA8_SGIS,
#endif
#ifdef GL_DUAL_INTENSITY12_SGIS
    dual_intensity12_sgis = GL_DUAL_INTENSITY12_SGIS,
#endif
#ifdef GL_DUAL_INTENSITY16_SGIS
    dual_intensity16_sgis = GL_DUAL_INTENSITY16_SGIS,
#endif
#ifdef GL_DUAL_INTENSITY4_SGIS
    dual_intensity4_sgis = GL_DUAL_INTENSITY4_SGIS,
#endif
#ifdef GL_DUAL_INTENSITY8_SGIS
    dual_intensity8_sgis = GL_DUAL_INTENSITY8_SGIS,
#endif
#ifdef GL_DUAL_LUMINANCE12_SGIS
    dual_luminance12_sgis = GL_DUAL_LUMINANCE12_SGIS,
#endif
#ifdef GL_DUAL_LUMINANCE16_SGIS
    dual_luminance16_sgis = GL_DUAL_LUMINANCE16_SGIS,
#endif
#ifdef GL_DUAL_LUMINANCE4_SGIS
    dual_luminance4_sgis = GL_DUAL_LUMINANCE4_SGIS,
#endif
#ifdef GL_DUAL_LUMINANCE8_SGIS
    dual_luminance8_sgis = GL_DUAL_LUMINANCE8_SGIS,
#endif
#ifdef GL_DUAL_LUMINANCE_ALPHA4_SGIS
    dual_luminance_alpha4_sgis = GL_DUAL_LUMINANCE_ALPHA4_SGIS,
#endif
#ifdef GL_DUAL_LUMINANCE_ALPHA8_SGIS
    dual_luminance_alpha8_sgis = GL_DUAL_LUMINANCE_ALPHA8_SGIS,
#endif
#ifdef GL_ETC1_RGB8_OES
    etc1_rgb8_oes = GL_ETC1_RGB8_OES,
#endif
#ifdef GL_INTENSITY
    intensity = GL_INTENSITY,
#endif
#ifdef GL_INTENSITY12
    intensity12 = GL_INTENSITY12,
#endif
#ifdef GL_INTENSITY12_EXT
    intensity12_ext = GL_INTENSITY12_EXT,
#endif
#ifdef GL_INTENSITY16
    intensity16 = GL_INTENSITY16,
#endif
#ifdef GL_INTENSITY16I_EXT
    intensity16i_ext = GL_INTENSITY16I_EXT,
#endif
#ifdef GL_INTENSITY16UI_EXT
    intensity16ui_ext = GL_INTENSITY16UI_EXT,
#endif
#ifdef GL_INTENSITY16_EXT
    intensity16_ext = GL_INTENSITY16_EXT,
#endif
#ifdef GL_INTENSITY32I_EXT
    intensity32i_ext = GL_INTENSITY32I_EXT,
#endif
#ifdef GL_INTENSITY32UI_EXT
    intensity32ui_ext = GL_INTENSITY32UI_EXT,
#endif
#ifdef GL_INTENSITY4
    intensity4 = GL_INTENSITY4,
#endif
#ifdef GL_INTENSITY4_EXT
    intensity4_ext = GL_INTENSITY4_EXT,
#endif
#ifdef GL_INTENSITY8
    intensity8 = GL_INTENSITY8,
#endif
#ifdef GL_INTENSITY8I_EXT
    intensity8i_ext = GL_INTENSITY8I_EXT,
#endif
#ifdef GL_INTENSITY8UI_EXT
    intensity8ui_ext = GL_INTENSITY8UI_EXT,
#endif
#ifdef GL_INTENSITY8_EXT
    intensity8_ext = GL_INTENSITY8_EXT,
#endif
#ifdef GL_LUMINANCE12
    luminance12 = GL_LUMINANCE12,
#endif
#ifdef GL_LUMINANCE12_ALPHA12
    luminance12_alpha12 = GL_LUMINANCE12_ALPHA12,
#endif
#ifdef GL_LUMINANCE12_ALPHA12_EXT
    luminance12_alpha12_ext = GL_LUMINANCE12_ALPHA12_EXT,
#endif
#ifdef GL_LUMINANCE12_ALPHA4
    luminance12_alpha4 = GL_LUMINANCE12_ALPHA4,
#endif
#ifdef GL_LUMINANCE12_ALPHA4_EXT
    luminance12_alpha4_ext = GL_LUMINANCE12_ALPHA4_EXT,
#endif
#ifdef GL_LUMINANCE12_EXT
    luminance12_ext = GL_LUMINANCE12_EXT,
#endif
#ifdef GL_LUMINANCE16
    luminance16 = GL_LUMINANCE16,
#endif
#ifdef GL_LUMINANCE16I_EXT
    luminance16i_ext = GL_LUMINANCE16I_EXT,
#endif
#ifdef GL_LUMINANCE16UI_EXT
    luminance16ui_ext = GL_LUMINANCE16UI_EXT,
#endif
#ifdef GL_LUMINANCE16_ALPHA16
    luminance16_alpha16 = GL_LUMINANCE16_ALPHA16,
#endif
#ifdef GL_LUMINANCE16_ALPHA16_EXT
    luminance16_alpha16_ext = GL_LUMINANCE16_ALPHA16_EXT,
#endif
#ifdef GL_LUMINANCE16_EXT
    luminance16_ext = GL_LUMINANCE16_EXT,
#endif
#ifdef GL_LUMINANCE32I_EXT
    luminance32i_ext = GL_LUMINANCE32I_EXT,
#endif
#ifdef GL_LUMINANCE32UI_EXT
    luminance32ui_ext = GL_LUMINANCE32UI_EXT,
#endif
#ifdef GL_LUMINANCE4
    luminance4 = GL_LUMINANCE4,
#endif
#ifdef GL_LUMINANCE4_ALPHA4
    luminance4_alpha4 = GL_LUMINANCE4_ALPHA4,
#endif
#ifdef GL_LUMINANCE4_ALPHA4_EXT
    luminance4_alpha4_ext = GL_LUMINANCE4_ALPHA4_EXT,
#endif
#ifdef GL_LUMINANCE4_ALPHA4_OES
    luminance4_alpha4_oes = GL_LUMINANCE4_ALPHA4_OES,
#endif
#ifdef GL_LUMINANCE4_EXT
    luminance4_ext = GL_LUMINANCE4_EXT,
#endif
#ifdef GL_LUMINANCE6_ALPHA2
    luminance6_alpha2 = GL_LUMINANCE6_ALPHA2,
#endif
#ifdef GL_LUMINANCE6_ALPHA2_EXT
    luminance6_alpha2_ext = GL_LUMINANCE6_ALPHA2_EXT,
#endif
#ifdef GL_LUMINANCE8
    luminance8 = GL_LUMINANCE8,
#endif
#ifdef GL_LUMINANCE8I_EXT
    luminance8i_ext = GL_LUMINANCE8I_EXT,
#endif
#ifdef GL_LUMINANCE8UI_EXT
    luminance8ui_ext = GL_LUMINANCE8UI_EXT,
#endif
#ifdef GL_LUMINANCE8_ALPHA8
    luminance8_alpha8 = GL_LUMINANCE8_ALPHA8,
#endif
#ifdef GL_LUMINANCE8_ALPHA8_EXT
    luminance8_alpha8_ext = GL_LUMINANCE8_ALPHA8_EXT,
#endif
#ifdef GL_LUMINANCE8_ALPHA8_OES
    luminance8_alpha8_oes = GL_LUMINANCE8_ALPHA8_OES,
#endif
#ifdef GL_LUMINANCE8_EXT
    luminance8_ext = GL_LUMINANCE8_EXT,
#endif
#ifdef GL_LUMINANCE8_OES
    luminance8_oes = GL_LUMINANCE8_OES,
#endif
#ifdef GL_LUMINANCE_ALPHA16I_EXT
    luminance_alpha16i_ext = GL_LUMINANCE_ALPHA16I_EXT,
#endif
#ifdef GL_LUMINANCE_ALPHA16UI_EXT
    luminance_alpha16ui_ext = GL_LUMINANCE_ALPHA16UI_EXT,
#endif
#ifdef GL_LUMINANCE_ALPHA32I_EXT
    luminance_alpha32i_ext = GL_LUMINANCE_ALPHA32I_EXT,
#endif
#ifdef GL_LUMINANCE_ALPHA32UI_EXT
    luminance_alpha32ui_ext = GL_LUMINANCE_ALPHA32UI_EXT,
#endif
#ifdef GL_LUMINANCE_ALPHA8I_EXT
    luminance_alpha8i_ext = GL_LUMINANCE_ALPHA8I_EXT,
#endif
#ifdef GL_LUMINANCE_ALPHA8UI_EXT
    luminance_alpha8ui_ext = GL_LUMINANCE_ALPHA8UI_EXT,
#endif
#ifdef GL_QUAD_ALPHA4_SGIS
    quad_alpha4_sgis = GL_QUAD_ALPHA4_SGIS,
#endif
#ifdef GL_QUAD_ALPHA8_SGIS
    quad_alpha8_sgis = GL_QUAD_ALPHA8_SGIS,
#endif
#ifdef GL_QUAD_INTENSITY4_SGIS
    quad_intensity4_sgis = GL_QUAD_INTENSITY4_SGIS,
#endif
#ifdef GL_QUAD_INTENSITY8_SGIS
    quad_intensity8_sgis = GL_QUAD_INTENSITY8_SGIS,
#endif
#ifdef GL_QUAD_LUMINANCE4_SGIS
    quad_luminance4_sgis = GL_QUAD_LUMINANCE4_SGIS,
#endif
#ifdef GL_QUAD_LUMINANCE8_SGIS
    quad_luminance8_sgis = GL_QUAD_LUMINANCE8_SGIS,
#endif
#ifdef GL_R11F_G11F_B10F
    r11f_g11f_b10f = GL_R11F_G11F_B10F,
#endif
#ifdef GL_R11F_G11F_B10F_APPLE
    r11f_g11f_b10f_apple = GL_R11F_G11F_B10F_APPLE,
#endif
#ifdef GL_R11F_G11F_B10F_EXT
    r11f_g11f_b10f_ext = GL_R11F_G11F_B10F_EXT,
#endif
#ifdef GL_R16
    r16 = GL_R16,
#endif
#ifdef GL_R16F
    r16f = GL_R16F,
#endif
#ifdef GL_R16F_EXT
    r16f_ext = GL_R16F_EXT,
#endif
#ifdef GL_R16I
    r16i = GL_R16I,
#endif
#ifdef GL_R16UI
    r16ui = GL_R16UI,
#endif
#ifdef GL_R16_EXT
    r16_ext = GL_R16_EXT,
#endif
#ifdef GL_R16_SNORM
    r16_snorm = GL_R16_SNORM,
#endif
#ifdef GL_R16_SNORM_EXT
    r16_snorm_ext = GL_R16_SNORM_EXT,
#endif
#ifdef GL_R32F
    r32f = GL_R32F,
#endif
#ifdef GL_R32F_EXT
    r32f_ext = GL_R32F_EXT,
#endif
#ifdef GL_R32I
    r32i = GL_R32I,
#endif
#ifdef GL_R32UI
    r32ui = GL_R32UI,
#endif
#ifdef GL_R3_G3_B2
    r3_g3_b2 = GL_R3_G3_B2,
#endif
#ifdef GL_R8
    r8 = GL_R8,
#endif
#ifdef GL_R8I
    r8i = GL_R8I,
#endif
#ifdef GL_R8UI
    r8ui = GL_R8UI,
#endif
#ifdef GL_R8_EXT
    r8_ext = GL_R8_EXT,
#endif
#ifdef GL_R8_SNORM
    r8_snorm = GL_R8_SNORM,
#endif
#ifdef GL_RED
    red = GL_RED,
#endif
#ifdef GL_RED_EXT
    red_ext = GL_RED_EXT,
#endif
#ifdef GL_RG
    rg = GL_RG,
#endif
#ifdef GL_RG16
    rg16 = GL_RG16,
#endif
#ifdef GL_RG16F
    rg16f = GL_RG16F,
#endif
#ifdef GL_RG16F_EXT
    rg16f_ext = GL_RG16F_EXT,
#endif
#ifdef GL_RG16I
    rg16i = GL_RG16I,
#endif
#ifdef GL_RG16UI
    rg16ui = GL_RG16UI,
#endif
#ifdef GL_RG16_EXT
    rg16_ext = GL_RG16_EXT,
#endif
#ifdef GL_RG16_SNORM
    rg16_snorm = GL_RG16_SNORM,
#endif
#ifdef GL_RG16_SNORM_EXT
    rg16_snorm_ext = GL_RG16_SNORM_EXT,
#endif
#ifdef GL_RG32F
    rg32f = GL_RG32F,
#endif
#ifdef GL_RG32F_EXT
    rg32f_ext = GL_RG32F_EXT,
#endif
#ifdef GL_RG32I
    rg32i = GL_RG32I,
#endif
#ifdef GL_RG32UI
    rg32ui = GL_RG32UI,
#endif
#ifdef GL_RG8
    rg8 = GL_RG8,
#endif
#ifdef GL_RG8I
    rg8i = GL_RG8I,
#endif
#ifdef GL_RG8UI
    rg8ui = GL_RG8UI,
#endif
#ifdef GL_RG8_EXT
    rg8_ext = GL_RG8_EXT,
#endif
#ifdef GL_RG8_SNORM
    rg8_snorm = GL_RG8_SNORM,
#endif
#ifdef GL_RGB
    rgb = GL_RGB,
#endif
#ifdef GL_RGB10
    rgb10 = GL_RGB10,
#endif
#ifdef GL_RGB10_A2
    rgb10_a2 = GL_RGB10_A2,
#endif
#ifdef GL_RGB10_A2UI
    rgb10_a2ui = GL_RGB10_A2UI,
#endif
#ifdef GL_RGB10_A2_EXT
    rgb10_a2_ext = GL_RGB10_A2_EXT,
#endif
#ifdef GL_RGB10_EXT
    rgb10_ext = GL_RGB10_EXT,
#endif
#ifdef GL_RGB12
    rgb12 = GL_RGB12,
#endif
#ifdef GL_RGB12_EXT
    rgb12_ext = GL_RGB12_EXT,
#endif
#ifdef GL_RGB16
    rgb16 = GL_RGB16,
#endif
#ifdef GL_RGB16F
    rgb16f = GL_RGB16F,
#endif
#ifdef GL_RGB16F_ARB
    rgb16f_arb = GL_RGB16F_ARB,
#endif
#ifdef GL_RGB16F_EXT
    rgb16f_ext = GL_RGB16F_EXT,
#endif
#ifdef GL_RGB16I
    rgb16i = GL_RGB16I,
#endif
#ifdef GL_RGB16I_EXT
    rgb16i_ext = GL_RGB16I_EXT,
#endif
#ifdef GL_RGB16UI
    rgb16ui = GL_RGB16UI,
#endif
#ifdef GL_RGB16UI_EXT
    rgb16ui_ext = GL_RGB16UI_EXT,
#endif
#ifdef GL_RGB16_EXT
    rgb16_ext = GL_RGB16_EXT,
#endif
#ifdef GL_RGB16_SNORM
    rgb16_snorm = GL_RGB16_SNORM,
#endif
#ifdef GL_RGB16_SNORM_EXT
    rgb16_snorm_ext = GL_RGB16_SNORM_EXT,
#endif
#ifdef GL_RGB2_EXT
    rgb2_ext = GL_RGB2_EXT,
#endif
#ifdef GL_RGB32F
    rgb32f = GL_RGB32F,
#endif
#ifdef GL_RGB32F_ARB
    rgb32f_arb = GL_RGB32F_ARB,
#endif
#ifdef GL_RGB32F_EXT
    rgb32f_ext = GL_RGB32F_EXT,
#endif
#ifdef GL_RGB32I
    rgb32i = GL_RGB32I,
#endif
#ifdef GL_RGB32I_EXT
    rgb32i_ext = GL_RGB32I_EXT,
#endif
#ifdef GL_RGB32UI
    rgb32ui = GL_RGB32UI,
#endif
#ifdef GL_RGB32UI_EXT
    rgb32ui_ext = GL_RGB32UI_EXT,
#endif
#ifdef GL_RGB4
    rgb4 = GL_RGB4,
#endif
#ifdef GL_RGB4_EXT
    rgb4_ext = GL_RGB4_EXT,
#endif
#ifdef GL_RGB5
    rgb5 = GL_RGB5,
#endif
#ifdef GL_RGB565
    rgb565 = GL_RGB565,
#endif
#ifdef GL_RGB565_OES
    rgb565_oes = GL_RGB565_OES,
#endif
#ifdef GL_RGB5_A1
    rgb5_a1 = GL_RGB5_A1,
#endif
#ifdef GL_RGB5_A1_EXT
    rgb5_a1_ext = GL_RGB5_A1_EXT,
#endif
#ifdef GL_RGB5_A1_OES
    rgb5_a1_oes = GL_RGB5_A1_OES,
#endif
#ifdef GL_RGB5_EXT
    rgb5_ext = GL_RGB5_EXT,
#endif
#ifdef GL_RGB8
    rgb8 = GL_RGB8,
#endif
#ifdef GL_RGB8I
    rgb8i = GL_RGB8I,
#endif
#ifdef GL_RGB8I_EXT
    rgb8i_ext = GL_RGB8I_EXT,
#endif
#ifdef GL_RGB8UI
    rgb8ui = GL_RGB8UI,
#endif
#ifdef GL_RGB8UI_EXT
    rgb8ui_ext = GL_RGB8UI_EXT,
#endif
#ifdef GL_RGB8_EXT
    rgb8_ext = GL_RGB8_EXT,
#endif
#ifdef GL_RGB8_OES
    rgb8_oes = GL_RGB8_OES,
#endif
#ifdef GL_RGB8_SNORM
    rgb8_snorm = GL_RGB8_SNORM,
#endif
#ifdef GL_RGB9_E5
    rgb9_e5 = GL_RGB9_E5,
#endif
#ifdef GL_RGB9_E5_APPLE
    rgb9_e5_apple = GL_RGB9_E5_APPLE,
#endif
#ifdef GL_RGB9_E5_EXT
    rgb9_e5_ext = GL_RGB9_E5_EXT,
#endif
#ifdef GL_RGBA
    rgba = GL_RGBA,
#endif
#ifdef GL_RGBA12
    rgba12 = GL_RGBA12,
#endif
#ifdef GL_RGBA12_EXT
    rgba12_ext = GL_RGBA12_EXT,
#endif
#ifdef GL_RGBA16
    rgba16 = GL_RGBA16,
#endif
#ifdef GL_RGBA16F
    rgba16f = GL_RGBA16F,
#endif
#ifdef GL_RGBA16F_ARB
    rgba16f_arb = GL_RGBA16F_ARB,
#endif
#ifdef GL_RGBA16F_EXT
    rgba16f_ext = GL_RGBA16F_EXT,
#endif
#ifdef GL_RGBA16I
    rgba16i = GL_RGBA16I,
#endif
#ifdef GL_RGBA16I_EXT
    rgba16i_ext = GL_RGBA16I_EXT,
#endif
#ifdef GL_RGBA16UI
    rgba16ui = GL_RGBA16UI,
#endif
#ifdef GL_RGBA16UI_EXT
    rgba16ui_ext = GL_RGBA16UI_EXT,
#endif
#ifdef GL_RGBA16_EXT
    rgba16_ext = GL_RGBA16_EXT,
#endif
#ifdef GL_RGBA16_SNORM
    rgba16_snorm = GL_RGBA16_SNORM,
#endif
#ifdef GL_RGBA16_SNORM_EXT
    rgba16_snorm_ext = GL_RGBA16_SNORM_EXT,
#endif
#ifdef GL_RGBA2
    rgba2 = GL_RGBA2,
#endif
#ifdef GL_RGBA2_EXT
    rgba2_ext = GL_RGBA2_EXT,
#endif
#ifdef GL_RGBA32F
    rgba32f = GL_RGBA32F,
#endif
#ifdef GL_RGBA32F_ARB
    rgba32f_arb = GL_RGBA32F_ARB,
#endif
#ifdef GL_RGBA32F_EXT
    rgba32f_ext = GL_RGBA32F_EXT,
#endif
#ifdef GL_RGBA32I
    rgba32i = GL_RGBA32I,
#endif
#ifdef GL_RGBA32I_EXT
    rgba32i_ext = GL_RGBA32I_EXT,
#endif
#ifdef GL_RGBA32UI
    rgba32ui = GL_RGBA32UI,
#endif
#ifdef GL_RGBA32UI_EXT
    rgba32ui_ext = GL_RGBA32UI_EXT,
#endif
#ifdef GL_RGBA4
    rgba4 = GL_RGBA4,
#endif
#ifdef GL_RGBA4_EXT
    rgba4_ext = GL_RGBA4_EXT,
#endif
#ifdef GL_RGBA4_OES
    rgba4_oes = GL_RGBA4_OES,
#endif
#ifdef GL_RGBA8
    rgba8 = GL_RGBA8,
#endif
#ifdef GL_RGBA8I
    rgba8i = GL_RGBA8I,
#endif
#ifdef GL_RGBA8I_EXT
    rgba8i_ext = GL_RGBA8I_EXT,
#endif
#ifdef GL_RGBA8UI
    rgba8ui = GL_RGBA8UI,
#endif
#ifdef GL_RGBA8UI_EXT
    rgba8ui_ext = GL_RGBA8UI_EXT,
#endif
#ifdef GL_RGBA8_EXT
    rgba8_ext = GL_RGBA8_EXT,
#endif
#ifdef GL_RGBA8_OES
    rgba8_oes = GL_RGBA8_OES,
#endif
#ifdef GL_RGBA8_SNORM
    rgba8_snorm = GL_RGBA8_SNORM,
#endif
#ifdef GL_SR8_EXT
    sr8_ext = GL_SR8_EXT,
#endif
#ifdef GL_SRG8_EXT
    srg8_ext = GL_SRG8_EXT,
#endif
#ifdef GL_SRGB
    srgb = GL_SRGB,
#endif
#ifdef GL_SRGB8
    srgb8 = GL_SRGB8,
#endif
#ifdef GL_SRGB8_ALPHA8
    srgb8_alpha8 = GL_SRGB8_ALPHA8,
#endif
#ifdef GL_SRGB8_ALPHA8_EXT
    srgb8_alpha8_ext = GL_SRGB8_ALPHA8_EXT,
#endif
#ifdef GL_SRGB8_EXT
    srgb8_ext = GL_SRGB8_EXT,
#endif
#ifdef GL_SRGB8_NV
    srgb8_nv = GL_SRGB8_NV,
#endif
#ifdef GL_SRGB_ALPHA
    srgb_alpha = GL_SRGB_ALPHA,
#endif
#ifdef GL_SRGB_ALPHA_EXT
    srgb_alpha_ext = GL_SRGB_ALPHA_EXT,
#endif
#ifdef GL_SRGB_EXT
    srgb_ext = GL_SRGB_EXT,
#endif
#ifdef GL_STENCIL_INDEX
    stencil_index = GL_STENCIL_INDEX,
#endif
#ifdef GL_STENCIL_INDEX1
    stencil_index1 = GL_STENCIL_INDEX1,
#endif
#ifdef GL_STENCIL_INDEX16
    stencil_index16 = GL_STENCIL_INDEX16,
#endif
#ifdef GL_STENCIL_INDEX16_EXT
    stencil_index16_ext = GL_STENCIL_INDEX16_EXT,
#endif
#ifdef GL_STENCIL_INDEX1_EXT
    stencil_index1_ext = GL_STENCIL_INDEX1_EXT,
#endif
#ifdef GL_STENCIL_INDEX1_OES
    stencil_index1_oes = GL_STENCIL_INDEX1_OES,
#endif
#ifdef GL_STENCIL_INDEX4
    stencil_index4 = GL_STENCIL_INDEX4,
#endif
#ifdef GL_STENCIL_INDEX4_EXT
    stencil_index4_ext = GL_STENCIL_INDEX4_EXT,
#endif
#ifdef GL_STENCIL_INDEX4_OES
    stencil_index4_oes = GL_STENCIL_INDEX4_OES,
#endif
#ifdef GL_STENCIL_INDEX8
    stencil_index8 = GL_STENCIL_INDEX8,
#endif
#ifdef GL_STENCIL_INDEX8_EXT
    stencil_index8_ext = GL_STENCIL_INDEX8_EXT,
#endif
#ifdef GL_STENCIL_INDEX8_OES
    stencil_index8_oes = GL_STENCIL_INDEX8_OES,
#endif
#ifdef GL_STENCIL_INDEX_OES
    stencil_index_oes = GL_STENCIL_INDEX_OES,
#endif
}; // enum class internal_format

} // namespace gl::group
