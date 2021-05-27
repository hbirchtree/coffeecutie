#pragma once

#include "common.h"

namespace gl::groups {

// EnableCap
enum class enable_cap : ::libc_types::u32 {
#ifdef GL_ALPHA_TEST
    alpha_test = GL_ALPHA_TEST,
#endif
#ifdef GL_ASYNC_DRAW_PIXELS_SGIX
    async_draw_pixels_sgix = GL_ASYNC_DRAW_PIXELS_SGIX,
#endif
#ifdef GL_ASYNC_HISTOGRAM_SGIX
    async_histogram_sgix = GL_ASYNC_HISTOGRAM_SGIX,
#endif
#ifdef GL_ASYNC_READ_PIXELS_SGIX
    async_read_pixels_sgix = GL_ASYNC_READ_PIXELS_SGIX,
#endif
#ifdef GL_ASYNC_TEX_IMAGE_SGIX
    async_tex_image_sgix = GL_ASYNC_TEX_IMAGE_SGIX,
#endif
#ifdef GL_AUTO_NORMAL
    auto_normal = GL_AUTO_NORMAL,
#endif
#ifdef GL_BLEND
    blend = GL_BLEND,
#endif
#ifdef GL_CALLIGRAPHIC_FRAGMENT_SGIX
    calligraphic_fragment_sgix = GL_CALLIGRAPHIC_FRAGMENT_SGIX,
#endif
#ifdef GL_CLIP_DISTANCE0
    clip_distance0 = GL_CLIP_DISTANCE0,
#endif
#ifdef GL_CLIP_DISTANCE1
    clip_distance1 = GL_CLIP_DISTANCE1,
#endif
#ifdef GL_CLIP_DISTANCE2
    clip_distance2 = GL_CLIP_DISTANCE2,
#endif
#ifdef GL_CLIP_DISTANCE3
    clip_distance3 = GL_CLIP_DISTANCE3,
#endif
#ifdef GL_CLIP_DISTANCE4
    clip_distance4 = GL_CLIP_DISTANCE4,
#endif
#ifdef GL_CLIP_DISTANCE5
    clip_distance5 = GL_CLIP_DISTANCE5,
#endif
#ifdef GL_CLIP_DISTANCE6
    clip_distance6 = GL_CLIP_DISTANCE6,
#endif
#ifdef GL_CLIP_DISTANCE7
    clip_distance7 = GL_CLIP_DISTANCE7,
#endif
#ifdef GL_CLIP_PLANE0
    clip_plane0 = GL_CLIP_PLANE0,
#endif
#ifdef GL_CLIP_PLANE1
    clip_plane1 = GL_CLIP_PLANE1,
#endif
#ifdef GL_CLIP_PLANE2
    clip_plane2 = GL_CLIP_PLANE2,
#endif
#ifdef GL_CLIP_PLANE3
    clip_plane3 = GL_CLIP_PLANE3,
#endif
#ifdef GL_CLIP_PLANE4
    clip_plane4 = GL_CLIP_PLANE4,
#endif
#ifdef GL_CLIP_PLANE5
    clip_plane5 = GL_CLIP_PLANE5,
#endif
#ifdef GL_COLOR_ARRAY
    color_array = GL_COLOR_ARRAY,
#endif
#ifdef GL_COLOR_LOGIC_OP
    color_logic_op = GL_COLOR_LOGIC_OP,
#endif
#ifdef GL_COLOR_MATERIAL
    color_material = GL_COLOR_MATERIAL,
#endif
#ifdef GL_COLOR_TABLE
    color_table = GL_COLOR_TABLE,
#endif
#ifdef GL_COLOR_TABLE_SGI
    color_table_sgi = GL_COLOR_TABLE_SGI,
#endif
#ifdef GL_CONVOLUTION_1D_EXT
    convolution_1d_ext = GL_CONVOLUTION_1D_EXT,
#endif
#ifdef GL_CONVOLUTION_2D_EXT
    convolution_2d_ext = GL_CONVOLUTION_2D_EXT,
#endif
#ifdef GL_CULL_FACE
    cull_face = GL_CULL_FACE,
#endif
#ifdef GL_DEBUG_OUTPUT
    debug_output = GL_DEBUG_OUTPUT,
#endif
#ifdef GL_DEBUG_OUTPUT_SYNCHRONOUS
    debug_output_synchronous = GL_DEBUG_OUTPUT_SYNCHRONOUS,
#endif
#ifdef GL_DEPTH_CLAMP
    depth_clamp = GL_DEPTH_CLAMP,
#endif
#ifdef GL_DEPTH_TEST
    depth_test = GL_DEPTH_TEST,
#endif
#ifdef GL_DITHER
    dither = GL_DITHER,
#endif
#ifdef GL_EDGE_FLAG_ARRAY
    edge_flag_array = GL_EDGE_FLAG_ARRAY,
#endif
#ifdef GL_FOG
    fog = GL_FOG,
#endif
#ifdef GL_FOG_OFFSET_SGIX
    fog_offset_sgix = GL_FOG_OFFSET_SGIX,
#endif
#ifdef GL_FRAGMENT_COLOR_MATERIAL_SGIX
    fragment_color_material_sgix = GL_FRAGMENT_COLOR_MATERIAL_SGIX,
#endif
#ifdef GL_FRAGMENT_LIGHT0_SGIX
    fragment_light0_sgix = GL_FRAGMENT_LIGHT0_SGIX,
#endif
#ifdef GL_FRAGMENT_LIGHT1_SGIX
    fragment_light1_sgix = GL_FRAGMENT_LIGHT1_SGIX,
#endif
#ifdef GL_FRAGMENT_LIGHT2_SGIX
    fragment_light2_sgix = GL_FRAGMENT_LIGHT2_SGIX,
#endif
#ifdef GL_FRAGMENT_LIGHT3_SGIX
    fragment_light3_sgix = GL_FRAGMENT_LIGHT3_SGIX,
#endif
#ifdef GL_FRAGMENT_LIGHT4_SGIX
    fragment_light4_sgix = GL_FRAGMENT_LIGHT4_SGIX,
#endif
#ifdef GL_FRAGMENT_LIGHT5_SGIX
    fragment_light5_sgix = GL_FRAGMENT_LIGHT5_SGIX,
#endif
#ifdef GL_FRAGMENT_LIGHT6_SGIX
    fragment_light6_sgix = GL_FRAGMENT_LIGHT6_SGIX,
#endif
#ifdef GL_FRAGMENT_LIGHT7_SGIX
    fragment_light7_sgix = GL_FRAGMENT_LIGHT7_SGIX,
#endif
#ifdef GL_FRAGMENT_LIGHTING_SGIX
    fragment_lighting_sgix = GL_FRAGMENT_LIGHTING_SGIX,
#endif
#ifdef GL_FRAMEBUFFER_SRGB
    framebuffer_srgb = GL_FRAMEBUFFER_SRGB,
#endif
#ifdef GL_FRAMEZOOM_SGIX
    framezoom_sgix = GL_FRAMEZOOM_SGIX,
#endif
#ifdef GL_HISTOGRAM_EXT
    histogram_ext = GL_HISTOGRAM_EXT,
#endif
#ifdef GL_INDEX_ARRAY
    index_array = GL_INDEX_ARRAY,
#endif
#ifdef GL_INDEX_LOGIC_OP
    index_logic_op = GL_INDEX_LOGIC_OP,
#endif
#ifdef GL_INTERLACE_SGIX
    interlace_sgix = GL_INTERLACE_SGIX,
#endif
#ifdef GL_IR_INSTRUMENT1_SGIX
    ir_instrument1_sgix = GL_IR_INSTRUMENT1_SGIX,
#endif
#ifdef GL_LIGHT0
    light0 = GL_LIGHT0,
#endif
#ifdef GL_LIGHT1
    light1 = GL_LIGHT1,
#endif
#ifdef GL_LIGHT2
    light2 = GL_LIGHT2,
#endif
#ifdef GL_LIGHT3
    light3 = GL_LIGHT3,
#endif
#ifdef GL_LIGHT4
    light4 = GL_LIGHT4,
#endif
#ifdef GL_LIGHT5
    light5 = GL_LIGHT5,
#endif
#ifdef GL_LIGHT6
    light6 = GL_LIGHT6,
#endif
#ifdef GL_LIGHT7
    light7 = GL_LIGHT7,
#endif
#ifdef GL_LIGHTING
    lighting = GL_LIGHTING,
#endif
#ifdef GL_LINE_SMOOTH
    line_smooth = GL_LINE_SMOOTH,
#endif
#ifdef GL_LINE_STIPPLE
    line_stipple = GL_LINE_STIPPLE,
#endif
#ifdef GL_MAP1_COLOR_4
    map1_color_4 = GL_MAP1_COLOR_4,
#endif
#ifdef GL_MAP1_INDEX
    map1_index = GL_MAP1_INDEX,
#endif
#ifdef GL_MAP1_NORMAL
    map1_normal = GL_MAP1_NORMAL,
#endif
#ifdef GL_MAP1_TEXTURE_COORD_1
    map1_texture_coord_1 = GL_MAP1_TEXTURE_COORD_1,
#endif
#ifdef GL_MAP1_TEXTURE_COORD_2
    map1_texture_coord_2 = GL_MAP1_TEXTURE_COORD_2,
#endif
#ifdef GL_MAP1_TEXTURE_COORD_3
    map1_texture_coord_3 = GL_MAP1_TEXTURE_COORD_3,
#endif
#ifdef GL_MAP1_TEXTURE_COORD_4
    map1_texture_coord_4 = GL_MAP1_TEXTURE_COORD_4,
#endif
#ifdef GL_MAP1_VERTEX_3
    map1_vertex_3 = GL_MAP1_VERTEX_3,
#endif
#ifdef GL_MAP1_VERTEX_4
    map1_vertex_4 = GL_MAP1_VERTEX_4,
#endif
#ifdef GL_MAP2_COLOR_4
    map2_color_4 = GL_MAP2_COLOR_4,
#endif
#ifdef GL_MAP2_INDEX
    map2_index = GL_MAP2_INDEX,
#endif
#ifdef GL_MAP2_NORMAL
    map2_normal = GL_MAP2_NORMAL,
#endif
#ifdef GL_MAP2_TEXTURE_COORD_1
    map2_texture_coord_1 = GL_MAP2_TEXTURE_COORD_1,
#endif
#ifdef GL_MAP2_TEXTURE_COORD_2
    map2_texture_coord_2 = GL_MAP2_TEXTURE_COORD_2,
#endif
#ifdef GL_MAP2_TEXTURE_COORD_3
    map2_texture_coord_3 = GL_MAP2_TEXTURE_COORD_3,
#endif
#ifdef GL_MAP2_TEXTURE_COORD_4
    map2_texture_coord_4 = GL_MAP2_TEXTURE_COORD_4,
#endif
#ifdef GL_MAP2_VERTEX_3
    map2_vertex_3 = GL_MAP2_VERTEX_3,
#endif
#ifdef GL_MAP2_VERTEX_4
    map2_vertex_4 = GL_MAP2_VERTEX_4,
#endif
#ifdef GL_MINMAX_EXT
    minmax_ext = GL_MINMAX_EXT,
#endif
#ifdef GL_MULTISAMPLE
    multisample = GL_MULTISAMPLE,
#endif
#ifdef GL_MULTISAMPLE_SGIS
    multisample_sgis = GL_MULTISAMPLE_SGIS,
#endif
#ifdef GL_NORMALIZE
    normalize = GL_NORMALIZE,
#endif
#ifdef GL_NORMAL_ARRAY
    normal_array = GL_NORMAL_ARRAY,
#endif
#ifdef GL_PIXEL_TEXTURE_SGIS
    pixel_texture_sgis = GL_PIXEL_TEXTURE_SGIS,
#endif
#ifdef GL_PIXEL_TEX_GEN_SGIX
    pixel_tex_gen_sgix = GL_PIXEL_TEX_GEN_SGIX,
#endif
#ifdef GL_POINT_SMOOTH
    point_smooth = GL_POINT_SMOOTH,
#endif
#ifdef GL_POLYGON_OFFSET_FILL
    polygon_offset_fill = GL_POLYGON_OFFSET_FILL,
#endif
#ifdef GL_POLYGON_OFFSET_LINE
    polygon_offset_line = GL_POLYGON_OFFSET_LINE,
#endif
#ifdef GL_POLYGON_OFFSET_POINT
    polygon_offset_point = GL_POLYGON_OFFSET_POINT,
#endif
#ifdef GL_POLYGON_SMOOTH
    polygon_smooth = GL_POLYGON_SMOOTH,
#endif
#ifdef GL_POLYGON_STIPPLE
    polygon_stipple = GL_POLYGON_STIPPLE,
#endif
#ifdef GL_POST_COLOR_MATRIX_COLOR_TABLE
    post_color_matrix_color_table = GL_POST_COLOR_MATRIX_COLOR_TABLE,
#endif
#ifdef GL_POST_COLOR_MATRIX_COLOR_TABLE_SGI
    post_color_matrix_color_table_sgi = GL_POST_COLOR_MATRIX_COLOR_TABLE_SGI,
#endif
#ifdef GL_POST_CONVOLUTION_COLOR_TABLE
    post_convolution_color_table = GL_POST_CONVOLUTION_COLOR_TABLE,
#endif
#ifdef GL_POST_CONVOLUTION_COLOR_TABLE_SGI
    post_convolution_color_table_sgi = GL_POST_CONVOLUTION_COLOR_TABLE_SGI,
#endif
#ifdef GL_PRIMITIVE_RESTART
    primitive_restart = GL_PRIMITIVE_RESTART,
#endif
#ifdef GL_PRIMITIVE_RESTART_FIXED_INDEX
    primitive_restart_fixed_index = GL_PRIMITIVE_RESTART_FIXED_INDEX,
#endif
#ifdef GL_PROGRAM_POINT_SIZE
    program_point_size = GL_PROGRAM_POINT_SIZE,
#endif
#ifdef GL_RASTERIZER_DISCARD
    rasterizer_discard = GL_RASTERIZER_DISCARD,
#endif
#ifdef GL_REFERENCE_PLANE_SGIX
    reference_plane_sgix = GL_REFERENCE_PLANE_SGIX,
#endif
#ifdef GL_RESCALE_NORMAL_EXT
    rescale_normal_ext = GL_RESCALE_NORMAL_EXT,
#endif
#ifdef GL_SAMPLE_ALPHA_TO_COVERAGE
    sample_alpha_to_coverage = GL_SAMPLE_ALPHA_TO_COVERAGE,
#endif
#ifdef GL_SAMPLE_ALPHA_TO_MASK_SGIS
    sample_alpha_to_mask_sgis = GL_SAMPLE_ALPHA_TO_MASK_SGIS,
#endif
#ifdef GL_SAMPLE_ALPHA_TO_ONE
    sample_alpha_to_one = GL_SAMPLE_ALPHA_TO_ONE,
#endif
#ifdef GL_SAMPLE_ALPHA_TO_ONE_SGIS
    sample_alpha_to_one_sgis = GL_SAMPLE_ALPHA_TO_ONE_SGIS,
#endif
#ifdef GL_SAMPLE_COVERAGE
    sample_coverage = GL_SAMPLE_COVERAGE,
#endif
#ifdef GL_SAMPLE_MASK
    sample_mask = GL_SAMPLE_MASK,
#endif
#ifdef GL_SAMPLE_MASK_SGIS
    sample_mask_sgis = GL_SAMPLE_MASK_SGIS,
#endif
#ifdef GL_SAMPLE_SHADING
    sample_shading = GL_SAMPLE_SHADING,
#endif
#ifdef GL_SCISSOR_TEST
    scissor_test = GL_SCISSOR_TEST,
#endif
#ifdef GL_SEPARABLE_2D_EXT
    separable_2d_ext = GL_SEPARABLE_2D_EXT,
#endif
#ifdef GL_SHADING_RATE_IMAGE_PER_PRIMITIVE_NV
    shading_rate_image_per_primitive_nv = GL_SHADING_RATE_IMAGE_PER_PRIMITIVE_NV,
#endif
#ifdef GL_SHADING_RATE_PRESERVE_ASPECT_RATIO_QCOM
    shading_rate_preserve_aspect_ratio_qcom = GL_SHADING_RATE_PRESERVE_ASPECT_RATIO_QCOM,
#endif
#ifdef GL_SHARED_TEXTURE_PALETTE_EXT
    shared_texture_palette_ext = GL_SHARED_TEXTURE_PALETTE_EXT,
#endif
#ifdef GL_SPRITE_SGIX
    sprite_sgix = GL_SPRITE_SGIX,
#endif
#ifdef GL_STENCIL_TEST
    stencil_test = GL_STENCIL_TEST,
#endif
#ifdef GL_TEXTURE_1D
    texture_1d = GL_TEXTURE_1D,
#endif
#ifdef GL_TEXTURE_2D
    texture_2d = GL_TEXTURE_2D,
#endif
#ifdef GL_TEXTURE_3D_EXT
    texture_3d_ext = GL_TEXTURE_3D_EXT,
#endif
#ifdef GL_TEXTURE_4D_SGIS
    texture_4d_sgis = GL_TEXTURE_4D_SGIS,
#endif
#ifdef GL_TEXTURE_COLOR_TABLE_SGI
    texture_color_table_sgi = GL_TEXTURE_COLOR_TABLE_SGI,
#endif
#ifdef GL_TEXTURE_COORD_ARRAY
    texture_coord_array = GL_TEXTURE_COORD_ARRAY,
#endif
#ifdef GL_TEXTURE_CUBE_MAP_SEAMLESS
    texture_cube_map_seamless = GL_TEXTURE_CUBE_MAP_SEAMLESS,
#endif
#ifdef GL_TEXTURE_GEN_Q
    texture_gen_q = GL_TEXTURE_GEN_Q,
#endif
#ifdef GL_TEXTURE_GEN_R
    texture_gen_r = GL_TEXTURE_GEN_R,
#endif
#ifdef GL_TEXTURE_GEN_S
    texture_gen_s = GL_TEXTURE_GEN_S,
#endif
#ifdef GL_TEXTURE_GEN_T
    texture_gen_t = GL_TEXTURE_GEN_T,
#endif
#ifdef GL_VERTEX_ARRAY
    vertex_array = GL_VERTEX_ARRAY,
#endif
}; // enum class enable_cap

} // namespace gl::groups
