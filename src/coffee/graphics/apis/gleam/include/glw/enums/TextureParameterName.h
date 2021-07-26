#pragma once

#include "common.h"

namespace gl::group {

// TextureParameterName
enum class texture_parameter_name : ::libc_types::u32
{
#ifdef GL_DEPTH_STENCIL_TEXTURE_MODE
    depth_stencil_texture_mode = GL_DEPTH_STENCIL_TEXTURE_MODE,
#endif
#ifdef GL_DETAIL_TEXTURE_FUNC_POINTS_SGIS
    detail_texture_func_points_sgis = GL_DETAIL_TEXTURE_FUNC_POINTS_SGIS,
#endif
#ifdef GL_DETAIL_TEXTURE_LEVEL_SGIS
    detail_texture_level_sgis = GL_DETAIL_TEXTURE_LEVEL_SGIS,
#endif
#ifdef GL_DETAIL_TEXTURE_MODE_SGIS
    detail_texture_mode_sgis = GL_DETAIL_TEXTURE_MODE_SGIS,
#endif
#ifdef GL_DUAL_TEXTURE_SELECT_SGIS
    dual_texture_select_sgis = GL_DUAL_TEXTURE_SELECT_SGIS,
#endif
#ifdef GL_GENERATE_MIPMAP
    generate_mipmap = GL_GENERATE_MIPMAP,
#endif
#ifdef GL_GENERATE_MIPMAP_SGIS
    generate_mipmap_sgis = GL_GENERATE_MIPMAP_SGIS,
#endif
#ifdef GL_POST_TEXTURE_FILTER_BIAS_SGIX
    post_texture_filter_bias_sgix = GL_POST_TEXTURE_FILTER_BIAS_SGIX,
#endif
#ifdef GL_POST_TEXTURE_FILTER_SCALE_SGIX
    post_texture_filter_scale_sgix = GL_POST_TEXTURE_FILTER_SCALE_SGIX,
#endif
#ifdef GL_QUAD_TEXTURE_SELECT_SGIS
    quad_texture_select_sgis = GL_QUAD_TEXTURE_SELECT_SGIS,
#endif
#ifdef GL_SHADOW_AMBIENT_SGIX
    shadow_ambient_sgix = GL_SHADOW_AMBIENT_SGIX,
#endif
#ifdef GL_SHARPEN_TEXTURE_FUNC_POINTS_SGIS
    sharpen_texture_func_points_sgis = GL_SHARPEN_TEXTURE_FUNC_POINTS_SGIS,
#endif
#ifdef GL_TEXTURE_4DSIZE_SGIS
    texture_4dsize_sgis = GL_TEXTURE_4DSIZE_SGIS,
#endif
#ifdef GL_TEXTURE_ALPHA_SIZE
    texture_alpha_size = GL_TEXTURE_ALPHA_SIZE,
#endif
#ifdef GL_TEXTURE_BASE_LEVEL
    texture_base_level = GL_TEXTURE_BASE_LEVEL,
#endif
#ifdef GL_TEXTURE_BASE_LEVEL_SGIS
    texture_base_level_sgis = GL_TEXTURE_BASE_LEVEL_SGIS,
#endif
#ifdef GL_TEXTURE_BLUE_SIZE
    texture_blue_size = GL_TEXTURE_BLUE_SIZE,
#endif
#ifdef GL_TEXTURE_BORDER
    texture_border = GL_TEXTURE_BORDER,
#endif
#ifdef GL_TEXTURE_BORDER_COLOR
    texture_border_color = GL_TEXTURE_BORDER_COLOR,
#endif
#ifdef GL_TEXTURE_BORDER_COLOR_NV
    texture_border_color_nv = GL_TEXTURE_BORDER_COLOR_NV,
#endif
#ifdef GL_TEXTURE_CLIPMAP_CENTER_SGIX
    texture_clipmap_center_sgix = GL_TEXTURE_CLIPMAP_CENTER_SGIX,
#endif
#ifdef GL_TEXTURE_CLIPMAP_DEPTH_SGIX
    texture_clipmap_depth_sgix = GL_TEXTURE_CLIPMAP_DEPTH_SGIX,
#endif
#ifdef GL_TEXTURE_CLIPMAP_FRAME_SGIX
    texture_clipmap_frame_sgix = GL_TEXTURE_CLIPMAP_FRAME_SGIX,
#endif
#ifdef GL_TEXTURE_CLIPMAP_LOD_OFFSET_SGIX
    texture_clipmap_lod_offset_sgix = GL_TEXTURE_CLIPMAP_LOD_OFFSET_SGIX,
#endif
#ifdef GL_TEXTURE_CLIPMAP_OFFSET_SGIX
    texture_clipmap_offset_sgix = GL_TEXTURE_CLIPMAP_OFFSET_SGIX,
#endif
#ifdef GL_TEXTURE_CLIPMAP_VIRTUAL_DEPTH_SGIX
    texture_clipmap_virtual_depth_sgix = GL_TEXTURE_CLIPMAP_VIRTUAL_DEPTH_SGIX,
#endif
#ifdef GL_TEXTURE_COMPARE_FUNC
    texture_compare_func = GL_TEXTURE_COMPARE_FUNC,
#endif
#ifdef GL_TEXTURE_COMPARE_MODE
    texture_compare_mode = GL_TEXTURE_COMPARE_MODE,
#endif
#ifdef GL_TEXTURE_COMPARE_OPERATOR_SGIX
    texture_compare_operator_sgix = GL_TEXTURE_COMPARE_OPERATOR_SGIX,
#endif
#ifdef GL_TEXTURE_COMPARE_SGIX
    texture_compare_sgix = GL_TEXTURE_COMPARE_SGIX,
#endif
#ifdef GL_TEXTURE_COMPONENTS
    texture_components = GL_TEXTURE_COMPONENTS,
#endif
#ifdef GL_TEXTURE_DEPTH_EXT
    texture_depth_ext = GL_TEXTURE_DEPTH_EXT,
#endif
#ifdef GL_TEXTURE_FILTER4_SIZE_SGIS
    texture_filter4_size_sgis = GL_TEXTURE_FILTER4_SIZE_SGIS,
#endif
#ifdef GL_TEXTURE_FOVEATED_CUTOFF_DENSITY_QCOM
    texture_foveated_cutoff_density_qcom =
        GL_TEXTURE_FOVEATED_CUTOFF_DENSITY_QCOM,
#endif
#ifdef GL_TEXTURE_GEQUAL_R_SGIX
    texture_gequal_r_sgix = GL_TEXTURE_GEQUAL_R_SGIX,
#endif
#ifdef GL_TEXTURE_GREEN_SIZE
    texture_green_size = GL_TEXTURE_GREEN_SIZE,
#endif
#ifdef GL_TEXTURE_HEIGHT
    texture_height = GL_TEXTURE_HEIGHT,
#endif
#ifdef GL_TEXTURE_INTENSITY_SIZE
    texture_intensity_size = GL_TEXTURE_INTENSITY_SIZE,
#endif
#ifdef GL_TEXTURE_INTERNAL_FORMAT
    texture_internal_format = GL_TEXTURE_INTERNAL_FORMAT,
#endif
#ifdef GL_TEXTURE_LEQUAL_R_SGIX
    texture_lequal_r_sgix = GL_TEXTURE_LEQUAL_R_SGIX,
#endif
#ifdef GL_TEXTURE_LOD_BIAS
    texture_lod_bias = GL_TEXTURE_LOD_BIAS,
#endif
#ifdef GL_TEXTURE_LOD_BIAS_R_SGIX
    texture_lod_bias_r_sgix = GL_TEXTURE_LOD_BIAS_R_SGIX,
#endif
#ifdef GL_TEXTURE_LOD_BIAS_S_SGIX
    texture_lod_bias_s_sgix = GL_TEXTURE_LOD_BIAS_S_SGIX,
#endif
#ifdef GL_TEXTURE_LOD_BIAS_T_SGIX
    texture_lod_bias_t_sgix = GL_TEXTURE_LOD_BIAS_T_SGIX,
#endif
#ifdef GL_TEXTURE_LUMINANCE_SIZE
    texture_luminance_size = GL_TEXTURE_LUMINANCE_SIZE,
#endif
#ifdef GL_TEXTURE_MAG_FILTER
    texture_mag_filter = GL_TEXTURE_MAG_FILTER,
#endif
#ifdef GL_TEXTURE_MAX_CLAMP_R_SGIX
    texture_max_clamp_r_sgix = GL_TEXTURE_MAX_CLAMP_R_SGIX,
#endif
#ifdef GL_TEXTURE_MAX_CLAMP_S_SGIX
    texture_max_clamp_s_sgix = GL_TEXTURE_MAX_CLAMP_S_SGIX,
#endif
#ifdef GL_TEXTURE_MAX_CLAMP_T_SGIX
    texture_max_clamp_t_sgix = GL_TEXTURE_MAX_CLAMP_T_SGIX,
#endif
#ifdef GL_TEXTURE_MAX_LEVEL
    texture_max_level = GL_TEXTURE_MAX_LEVEL,
#endif
#ifdef GL_TEXTURE_MAX_LEVEL_SGIS
    texture_max_level_sgis = GL_TEXTURE_MAX_LEVEL_SGIS,
#endif
#ifdef GL_TEXTURE_MAX_LOD
    texture_max_lod = GL_TEXTURE_MAX_LOD,
#endif
#ifdef GL_TEXTURE_MAX_LOD_SGIS
    texture_max_lod_sgis = GL_TEXTURE_MAX_LOD_SGIS,
#endif
#ifdef GL_TEXTURE_MIN_FILTER
    texture_min_filter = GL_TEXTURE_MIN_FILTER,
#endif
#ifdef GL_TEXTURE_MIN_LOD
    texture_min_lod = GL_TEXTURE_MIN_LOD,
#endif
#ifdef GL_TEXTURE_MIN_LOD_SGIS
    texture_min_lod_sgis = GL_TEXTURE_MIN_LOD_SGIS,
#endif
#ifdef GL_TEXTURE_PRIORITY
    texture_priority = GL_TEXTURE_PRIORITY,
#endif
#ifdef GL_TEXTURE_PRIORITY_EXT
    texture_priority_ext = GL_TEXTURE_PRIORITY_EXT,
#endif
#ifdef GL_TEXTURE_RED_SIZE
    texture_red_size = GL_TEXTURE_RED_SIZE,
#endif
#ifdef GL_TEXTURE_RESIDENT
    texture_resident = GL_TEXTURE_RESIDENT,
#endif
#ifdef GL_TEXTURE_SWIZZLE_A
    texture_swizzle_a = GL_TEXTURE_SWIZZLE_A,
#endif
#ifdef GL_TEXTURE_SWIZZLE_B
    texture_swizzle_b = GL_TEXTURE_SWIZZLE_B,
#endif
#ifdef GL_TEXTURE_SWIZZLE_G
    texture_swizzle_g = GL_TEXTURE_SWIZZLE_G,
#endif
#ifdef GL_TEXTURE_SWIZZLE_R
    texture_swizzle_r = GL_TEXTURE_SWIZZLE_R,
#endif
#ifdef GL_TEXTURE_SWIZZLE_RGBA
    texture_swizzle_rgba = GL_TEXTURE_SWIZZLE_RGBA,
#endif
#ifdef GL_TEXTURE_TILING_EXT
    texture_tiling_ext = GL_TEXTURE_TILING_EXT,
#endif
#ifdef GL_TEXTURE_UNNORMALIZED_COORDINATES_ARM
    texture_unnormalized_coordinates_arm =
        GL_TEXTURE_UNNORMALIZED_COORDINATES_ARM,
#endif
#ifdef GL_TEXTURE_WIDTH
    texture_width = GL_TEXTURE_WIDTH,
#endif
#ifdef GL_TEXTURE_WRAP_Q_SGIS
    texture_wrap_q_sgis = GL_TEXTURE_WRAP_Q_SGIS,
#endif
#ifdef GL_TEXTURE_WRAP_R
    texture_wrap_r = GL_TEXTURE_WRAP_R,
#endif
#ifdef GL_TEXTURE_WRAP_R_EXT
    texture_wrap_r_ext = GL_TEXTURE_WRAP_R_EXT,
#endif
#ifdef GL_TEXTURE_WRAP_R_OES
    texture_wrap_r_oes = GL_TEXTURE_WRAP_R_OES,
#endif
#ifdef GL_TEXTURE_WRAP_S
    texture_wrap_s = GL_TEXTURE_WRAP_S,
#endif
#ifdef GL_TEXTURE_WRAP_T
    texture_wrap_t = GL_TEXTURE_WRAP_T,
#endif
}; // enum class texture_parameter_name

} // namespace gl::group
