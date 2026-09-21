#pragma once

#include "common.h"

namespace gl::group {

// GetTextureParameter
enum class get_texture_parameter : u32
{
    normal_map               = 0x8511, // GL_NORMAL_MAP
    reflection_map           = 0x8512, // GL_REFLECTION_MAP
    texture_alpha_size       = 0x805F, // GL_TEXTURE_ALPHA_SIZE
    texture_blue_size        = 0x805E, // GL_TEXTURE_BLUE_SIZE
    texture_border           = 0x1005, // GL_TEXTURE_BORDER
    texture_border_color     = 0x1004, // GL_TEXTURE_BORDER_COLOR
    texture_components       = 0x1003, // GL_TEXTURE_COMPONENTS
    texture_depth            = 0x8071, // GL_TEXTURE_DEPTH
    texture_green_size       = 0x805D, // GL_TEXTURE_GREEN_SIZE
    texture_height           = 0x1001, // GL_TEXTURE_HEIGHT
    texture_immutable_format = 0x912F, // GL_TEXTURE_IMMUTABLE_FORMAT
    texture_intensity_size   = 0x8061, // GL_TEXTURE_INTENSITY_SIZE
    texture_internal_format  = 0x1003, // GL_TEXTURE_INTERNAL_FORMAT
    texture_luminance_size   = 0x8060, // GL_TEXTURE_LUMINANCE_SIZE
    texture_mag_filter       = 0x2800, // GL_TEXTURE_MAG_FILTER
    texture_min_filter       = 0x2801, // GL_TEXTURE_MIN_FILTER
    texture_priority         = 0x8066, // GL_TEXTURE_PRIORITY
    texture_red_size         = 0x805C, // GL_TEXTURE_RED_SIZE
    texture_resident         = 0x8067, // GL_TEXTURE_RESIDENT
    texture_width            = 0x1000, // GL_TEXTURE_WIDTH
    texture_wrap_s           = 0x2802, // GL_TEXTURE_WRAP_S
    texture_wrap_t           = 0x2803, // GL_TEXTURE_WRAP_T
    detail_texture_func_points_sgis =
        0x809C, // GL_DETAIL_TEXTURE_FUNC_POINTS_SGIS
    detail_texture_level_sgis     = 0x809A, // GL_DETAIL_TEXTURE_LEVEL_SGIS
    detail_texture_mode_sgis      = 0x809B, // GL_DETAIL_TEXTURE_MODE_SGIS
    dual_texture_select_sgis      = 0x8124, // GL_DUAL_TEXTURE_SELECT_SGIS
    generate_mipmap_sgis          = 0x8191, // GL_GENERATE_MIPMAP_SGIS
    normal_map_arb                = 0x8511, // GL_NORMAL_MAP_ARB
    normal_map_ext                = 0x8511, // GL_NORMAL_MAP_EXT
    normal_map_nv                 = 0x8511, // GL_NORMAL_MAP_NV
    normal_map_oes                = 0x8511, // GL_NORMAL_MAP_OES
    post_texture_filter_bias_sgix = 0x8179, // GL_POST_TEXTURE_FILTER_BIAS_SGIX
    post_texture_filter_scale_sgix =
        0x817A,                        // GL_POST_TEXTURE_FILTER_SCALE_SGIX
    quad_texture_select_sgis = 0x8125, // GL_QUAD_TEXTURE_SELECT_SGIS
    reflection_map_arb       = 0x8512, // GL_REFLECTION_MAP_ARB
    reflection_map_ext       = 0x8512, // GL_REFLECTION_MAP_EXT
    reflection_map_nv        = 0x8512, // GL_REFLECTION_MAP_NV
    reflection_map_oes       = 0x8512, // GL_REFLECTION_MAP_OES
    shadow_ambient_sgix      = 0x80BF, // GL_SHADOW_AMBIENT_SGIX
    sharpen_texture_func_points_sgis =
        0x80B0,                           // GL_SHARPEN_TEXTURE_FUNC_POINTS_SGIS
    surface_compression_ext     = 0x96C0, // GL_SURFACE_COMPRESSION_EXT
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
    texture_gequal_r_sgix         = 0x819D, // GL_TEXTURE_GEQUAL_R_SGIX
    texture_lequal_r_sgix         = 0x819C, // GL_TEXTURE_LEQUAL_R_SGIX
    texture_lod_bias_r_sgix       = 0x8190, // GL_TEXTURE_LOD_BIAS_R_SGIX
    texture_lod_bias_s_sgix       = 0x818E, // GL_TEXTURE_LOD_BIAS_S_SGIX
    texture_lod_bias_t_sgix       = 0x818F, // GL_TEXTURE_LOD_BIAS_T_SGIX
    texture_max_clamp_r_sgix      = 0x836B, // GL_TEXTURE_MAX_CLAMP_R_SGIX
    texture_max_clamp_s_sgix      = 0x8369, // GL_TEXTURE_MAX_CLAMP_S_SGIX
    texture_max_clamp_t_sgix      = 0x836A, // GL_TEXTURE_MAX_CLAMP_T_SGIX
    texture_max_level_sgis        = 0x813D, // GL_TEXTURE_MAX_LEVEL_SGIS
    texture_max_lod_sgis          = 0x813B, // GL_TEXTURE_MAX_LOD_SGIS
    texture_min_lod_sgis          = 0x813A, // GL_TEXTURE_MIN_LOD_SGIS
    texture_unnormalized_coordinates_arm =
        0x8F6A,                      // GL_TEXTURE_UNNORMALIZED_COORDINATES_ARM
    texture_wrap_q_sgis    = 0x8137, // GL_TEXTURE_WRAP_Q_SGIS
    texture_wrap_r_ext     = 0x8072, // GL_TEXTURE_WRAP_R_EXT
    texture_y_degamma_qcom = 0x9710, // GL_TEXTURE_Y_DEGAMMA_QCOM
}; // enum class get_texture_parameter

} // namespace gl::group
