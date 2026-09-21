#pragma once

#include "common.h"

namespace gl::group {

// TextureParameterName
enum class texture_parameter_name : u32
{
    depth_stencil_texture_mode = 0x90EA, // GL_DEPTH_STENCIL_TEXTURE_MODE
    generate_mipmap            = 0x8191, // GL_GENERATE_MIPMAP
    texture_alpha_size         = 0x805F, // GL_TEXTURE_ALPHA_SIZE
    texture_base_level         = 0x813C, // GL_TEXTURE_BASE_LEVEL
    texture_blue_size          = 0x805E, // GL_TEXTURE_BLUE_SIZE
    texture_border             = 0x1005, // GL_TEXTURE_BORDER
    texture_border_color       = 0x1004, // GL_TEXTURE_BORDER_COLOR
    texture_compare_func       = 0x884D, // GL_TEXTURE_COMPARE_FUNC
    texture_compare_mode       = 0x884C, // GL_TEXTURE_COMPARE_MODE
    texture_components         = 0x1003, // GL_TEXTURE_COMPONENTS
    texture_green_size         = 0x805D, // GL_TEXTURE_GREEN_SIZE
    texture_height             = 0x1001, // GL_TEXTURE_HEIGHT
    texture_intensity_size     = 0x8061, // GL_TEXTURE_INTENSITY_SIZE
    texture_internal_format    = 0x1003, // GL_TEXTURE_INTERNAL_FORMAT
    texture_lod_bias           = 0x8501, // GL_TEXTURE_LOD_BIAS
    texture_luminance_size     = 0x8060, // GL_TEXTURE_LUMINANCE_SIZE
    texture_mag_filter         = 0x2800, // GL_TEXTURE_MAG_FILTER
    texture_max_anisotropy     = 0x84FE, // GL_TEXTURE_MAX_ANISOTROPY
    texture_max_level          = 0x813D, // GL_TEXTURE_MAX_LEVEL
    texture_max_lod            = 0x813B, // GL_TEXTURE_MAX_LOD
    texture_min_filter         = 0x2801, // GL_TEXTURE_MIN_FILTER
    texture_min_lod            = 0x813A, // GL_TEXTURE_MIN_LOD
    texture_priority           = 0x8066, // GL_TEXTURE_PRIORITY
    texture_red_size           = 0x805C, // GL_TEXTURE_RED_SIZE
    texture_resident           = 0x8067, // GL_TEXTURE_RESIDENT
    texture_swizzle_a          = 0x8E45, // GL_TEXTURE_SWIZZLE_A
    texture_swizzle_b          = 0x8E44, // GL_TEXTURE_SWIZZLE_B
    texture_swizzle_g          = 0x8E43, // GL_TEXTURE_SWIZZLE_G
    texture_swizzle_r          = 0x8E42, // GL_TEXTURE_SWIZZLE_R
    texture_swizzle_rgba       = 0x8E46, // GL_TEXTURE_SWIZZLE_RGBA
    texture_width              = 0x1000, // GL_TEXTURE_WIDTH
    texture_wrap_r             = 0x8072, // GL_TEXTURE_WRAP_R
    texture_wrap_s             = 0x2802, // GL_TEXTURE_WRAP_S
    texture_wrap_t             = 0x2803, // GL_TEXTURE_WRAP_T
    detail_texture_func_points_sgis =
        0x809C, // GL_DETAIL_TEXTURE_FUNC_POINTS_SGIS
    detail_texture_level_sgis     = 0x809A, // GL_DETAIL_TEXTURE_LEVEL_SGIS
    detail_texture_mode_sgis      = 0x809B, // GL_DETAIL_TEXTURE_MODE_SGIS
    dual_texture_select_sgis      = 0x8124, // GL_DUAL_TEXTURE_SELECT_SGIS
    generate_mipmap_sgis          = 0x8191, // GL_GENERATE_MIPMAP_SGIS
    post_texture_filter_bias_sgix = 0x8179, // GL_POST_TEXTURE_FILTER_BIAS_SGIX
    post_texture_filter_scale_sgix =
        0x817A,                        // GL_POST_TEXTURE_FILTER_SCALE_SGIX
    quad_texture_select_sgis = 0x8125, // GL_QUAD_TEXTURE_SELECT_SGIS
    shadow_ambient_sgix      = 0x80BF, // GL_SHADOW_AMBIENT_SGIX
    sharpen_texture_func_points_sgis =
        0x80B0,                           // GL_SHARPEN_TEXTURE_FUNC_POINTS_SGIS
    texture_4dsize_sgis         = 0x8136, // GL_TEXTURE_4DSIZE_SGIS
    texture_base_level_sgis     = 0x813C, // GL_TEXTURE_BASE_LEVEL_SGIS
    texture_border_color_nv     = 0x1004, // GL_TEXTURE_BORDER_COLOR_NV
    texture_cbcr_degamma_qcom   = 0x9711, // GL_TEXTURE_CBCR_DEGAMMA_QCOM
    texture_clipmap_center_sgix = 0x8171, // GL_TEXTURE_CLIPMAP_CENTER_SGIX
    texture_clipmap_depth_sgix  = 0x8176, // GL_TEXTURE_CLIPMAP_DEPTH_SGIX
    texture_clipmap_frame_sgix  = 0x8172, // GL_TEXTURE_CLIPMAP_FRAME_SGIX
    texture_clipmap_lod_offset_sgix =
        0x8175,                           // GL_TEXTURE_CLIPMAP_LOD_OFFSET_SGIX
    texture_clipmap_offset_sgix = 0x8173, // GL_TEXTURE_CLIPMAP_OFFSET_SGIX
    texture_clipmap_virtual_depth_sgix =
        0x8174, // GL_TEXTURE_CLIPMAP_VIRTUAL_DEPTH_SGIX
    texture_compare_operator_sgix = 0x819B, // GL_TEXTURE_COMPARE_OPERATOR_SGIX
    texture_compare_sgix          = 0x819A, // GL_TEXTURE_COMPARE_SGIX
    texture_depth_ext             = 0x8071, // GL_TEXTURE_DEPTH_EXT
    texture_filter4_size_sgis     = 0x8147, // GL_TEXTURE_FILTER4_SIZE_SGIS
    texture_foveated_cutoff_density_qcom =
        0x96A0, // GL_TEXTURE_FOVEATED_CUTOFF_DENSITY_QCOM
    texture_gequal_r_sgix       = 0x819D, // GL_TEXTURE_GEQUAL_R_SGIX
    texture_lequal_r_sgix       = 0x819C, // GL_TEXTURE_LEQUAL_R_SGIX
    texture_lod_bias_ext        = 0x8501, // GL_TEXTURE_LOD_BIAS_EXT
    texture_lod_bias_r_sgix     = 0x8190, // GL_TEXTURE_LOD_BIAS_R_SGIX
    texture_lod_bias_s_sgix     = 0x818E, // GL_TEXTURE_LOD_BIAS_S_SGIX
    texture_lod_bias_t_sgix     = 0x818F, // GL_TEXTURE_LOD_BIAS_T_SGIX
    texture_max_anisotropy_ext  = 0x84FE, // GL_TEXTURE_MAX_ANISOTROPY_EXT
    texture_max_clamp_r_sgix    = 0x836B, // GL_TEXTURE_MAX_CLAMP_R_SGIX
    texture_max_clamp_s_sgix    = 0x8369, // GL_TEXTURE_MAX_CLAMP_S_SGIX
    texture_max_clamp_t_sgix    = 0x836A, // GL_TEXTURE_MAX_CLAMP_T_SGIX
    texture_max_level_sgis      = 0x813D, // GL_TEXTURE_MAX_LEVEL_SGIS
    texture_max_lod_sgis        = 0x813B, // GL_TEXTURE_MAX_LOD_SGIS
    texture_memory_layout_intel = 0x83FF, // GL_TEXTURE_MEMORY_LAYOUT_INTEL
    texture_min_lod_sgis        = 0x813A, // GL_TEXTURE_MIN_LOD_SGIS
    texture_priority_ext        = 0x8066, // GL_TEXTURE_PRIORITY_EXT
    texture_tiling_ext          = 0x9580, // GL_TEXTURE_TILING_EXT
    texture_unnormalized_coordinates_arm =
        0x8F6A,                      // GL_TEXTURE_UNNORMALIZED_COORDINATES_ARM
    texture_wrap_q_sgis    = 0x8137, // GL_TEXTURE_WRAP_Q_SGIS
    texture_wrap_r_ext     = 0x8072, // GL_TEXTURE_WRAP_R_EXT
    texture_wrap_r_oes     = 0x8072, // GL_TEXTURE_WRAP_R_OES
    texture_y_degamma_qcom = 0x9710, // GL_TEXTURE_Y_DEGAMMA_QCOM
}; // enum class texture_parameter_name

} // namespace gl::group
