#pragma once

#include "common.h"

namespace gl::group {

// EnableCap
enum class enable_cap : u32
{
    alpha_test                    = 0x0BC0, // GL_ALPHA_TEST
    auto_normal                   = 0x0D80, // GL_AUTO_NORMAL
    blend                         = 0x0BE2, // GL_BLEND
    clip_distance0                = 0x3000, // GL_CLIP_DISTANCE0
    clip_distance1                = 0x3001, // GL_CLIP_DISTANCE1
    clip_distance2                = 0x3002, // GL_CLIP_DISTANCE2
    clip_distance3                = 0x3003, // GL_CLIP_DISTANCE3
    clip_distance4                = 0x3004, // GL_CLIP_DISTANCE4
    clip_distance5                = 0x3005, // GL_CLIP_DISTANCE5
    clip_distance6                = 0x3006, // GL_CLIP_DISTANCE6
    clip_distance7                = 0x3007, // GL_CLIP_DISTANCE7
    clip_plane0                   = 0x3000, // GL_CLIP_PLANE0
    clip_plane1                   = 0x3001, // GL_CLIP_PLANE1
    clip_plane2                   = 0x3002, // GL_CLIP_PLANE2
    clip_plane3                   = 0x3003, // GL_CLIP_PLANE3
    clip_plane4                   = 0x3004, // GL_CLIP_PLANE4
    clip_plane5                   = 0x3005, // GL_CLIP_PLANE5
    color_array                   = 0x8076, // GL_COLOR_ARRAY
    color_logic_op                = 0x0BF2, // GL_COLOR_LOGIC_OP
    color_material                = 0x0B57, // GL_COLOR_MATERIAL
    color_table                   = 0x80D0, // GL_COLOR_TABLE
    cull_face                     = 0x0B44, // GL_CULL_FACE
    debug_output                  = 0x92E0, // GL_DEBUG_OUTPUT
    debug_output_synchronous      = 0x8242, // GL_DEBUG_OUTPUT_SYNCHRONOUS
    depth_clamp                   = 0x864F, // GL_DEPTH_CLAMP
    depth_test                    = 0x0B71, // GL_DEPTH_TEST
    dither                        = 0x0BD0, // GL_DITHER
    edge_flag_array               = 0x8079, // GL_EDGE_FLAG_ARRAY
    fog                           = 0x0B60, // GL_FOG
    framebuffer_srgb              = 0x8DB9, // GL_FRAMEBUFFER_SRGB
    index_array                   = 0x8077, // GL_INDEX_ARRAY
    index_logic_op                = 0x0BF1, // GL_INDEX_LOGIC_OP
    light0                        = 0x4000, // GL_LIGHT0
    light1                        = 0x4001, // GL_LIGHT1
    light2                        = 0x4002, // GL_LIGHT2
    light3                        = 0x4003, // GL_LIGHT3
    light4                        = 0x4004, // GL_LIGHT4
    light5                        = 0x4005, // GL_LIGHT5
    light6                        = 0x4006, // GL_LIGHT6
    light7                        = 0x4007, // GL_LIGHT7
    lighting                      = 0x0B50, // GL_LIGHTING
    line_smooth                   = 0x0B20, // GL_LINE_SMOOTH
    line_stipple                  = 0x0B24, // GL_LINE_STIPPLE
    map1_color_4                  = 0x0D90, // GL_MAP1_COLOR_4
    map1_index                    = 0x0D91, // GL_MAP1_INDEX
    map1_normal                   = 0x0D92, // GL_MAP1_NORMAL
    map1_texture_coord_1          = 0x0D93, // GL_MAP1_TEXTURE_COORD_1
    map1_texture_coord_2          = 0x0D94, // GL_MAP1_TEXTURE_COORD_2
    map1_texture_coord_3          = 0x0D95, // GL_MAP1_TEXTURE_COORD_3
    map1_texture_coord_4          = 0x0D96, // GL_MAP1_TEXTURE_COORD_4
    map1_vertex_3                 = 0x0D97, // GL_MAP1_VERTEX_3
    map1_vertex_4                 = 0x0D98, // GL_MAP1_VERTEX_4
    map2_color_4                  = 0x0DB0, // GL_MAP2_COLOR_4
    map2_index                    = 0x0DB1, // GL_MAP2_INDEX
    map2_normal                   = 0x0DB2, // GL_MAP2_NORMAL
    map2_texture_coord_1          = 0x0DB3, // GL_MAP2_TEXTURE_COORD_1
    map2_texture_coord_2          = 0x0DB4, // GL_MAP2_TEXTURE_COORD_2
    map2_texture_coord_3          = 0x0DB5, // GL_MAP2_TEXTURE_COORD_3
    map2_texture_coord_4          = 0x0DB6, // GL_MAP2_TEXTURE_COORD_4
    map2_vertex_3                 = 0x0DB7, // GL_MAP2_VERTEX_3
    map2_vertex_4                 = 0x0DB8, // GL_MAP2_VERTEX_4
    multisample                   = 0x809D, // GL_MULTISAMPLE
    normalize                     = 0x0BA1, // GL_NORMALIZE
    normal_array                  = 0x8075, // GL_NORMAL_ARRAY
    point_smooth                  = 0x0B10, // GL_POINT_SMOOTH
    polygon_offset_fill           = 0x8037, // GL_POLYGON_OFFSET_FILL
    polygon_offset_line           = 0x2A02, // GL_POLYGON_OFFSET_LINE
    polygon_offset_point          = 0x2A01, // GL_POLYGON_OFFSET_POINT
    polygon_smooth                = 0x0B41, // GL_POLYGON_SMOOTH
    polygon_stipple               = 0x0B42, // GL_POLYGON_STIPPLE
    post_color_matrix_color_table = 0x80D2, // GL_POST_COLOR_MATRIX_COLOR_TABLE
    post_convolution_color_table  = 0x80D1, // GL_POST_CONVOLUTION_COLOR_TABLE
    primitive_restart             = 0x8F9D, // GL_PRIMITIVE_RESTART
    primitive_restart_fixed_index = 0x8D69, // GL_PRIMITIVE_RESTART_FIXED_INDEX
    program_point_size            = 0x8642, // GL_PROGRAM_POINT_SIZE
    rasterizer_discard            = 0x8C89, // GL_RASTERIZER_DISCARD
    sample_alpha_to_coverage      = 0x809E, // GL_SAMPLE_ALPHA_TO_COVERAGE
    sample_alpha_to_one           = 0x809F, // GL_SAMPLE_ALPHA_TO_ONE
    sample_coverage               = 0x80A0, // GL_SAMPLE_COVERAGE
    sample_mask                   = 0x8E51, // GL_SAMPLE_MASK
    sample_shading                = 0x8C36, // GL_SAMPLE_SHADING
    scissor_test                  = 0x0C11, // GL_SCISSOR_TEST
    stencil_test                  = 0x0B90, // GL_STENCIL_TEST
    texture_1d                    = 0x0DE0, // GL_TEXTURE_1D
    texture_2d                    = 0x0DE1, // GL_TEXTURE_2D
    texture_coord_array           = 0x8078, // GL_TEXTURE_COORD_ARRAY
    texture_cube_map              = 0x8513, // GL_TEXTURE_CUBE_MAP
    texture_cube_map_seamless     = 0x884F, // GL_TEXTURE_CUBE_MAP_SEAMLESS
    texture_gen_q                 = 0x0C63, // GL_TEXTURE_GEN_Q
    texture_gen_r                 = 0x0C62, // GL_TEXTURE_GEN_R
    texture_gen_s                 = 0x0C60, // GL_TEXTURE_GEN_S
    texture_gen_t                 = 0x0C61, // GL_TEXTURE_GEN_T
    texture_rectangle             = 0x84F5, // GL_TEXTURE_RECTANGLE
    vertex_array                  = 0x8074, // GL_VERTEX_ARRAY
    async_draw_pixels_sgix        = 0x835D, // GL_ASYNC_DRAW_PIXELS_SGIX
    async_histogram_sgix          = 0x832C, // GL_ASYNC_HISTOGRAM_SGIX
    async_read_pixels_sgix        = 0x835E, // GL_ASYNC_READ_PIXELS_SGIX
    async_tex_image_sgix          = 0x835C, // GL_ASYNC_TEX_IMAGE_SGIX
    calligraphic_fragment_sgix    = 0x8183, // GL_CALLIGRAPHIC_FRAGMENT_SGIX
    color_table_sgi               = 0x80D0, // GL_COLOR_TABLE_SGI
    convolution_1d_ext            = 0x8010, // GL_CONVOLUTION_1D_EXT
    convolution_2d_ext            = 0x8011, // GL_CONVOLUTION_2D_EXT
    debug_output_synchronous_arb  = 0x8242, // GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB
    debug_output_synchronous_khr  = 0x8242, // GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR
    fetch_per_sample_arm          = 0x8F65, // GL_FETCH_PER_SAMPLE_ARM
    fog_offset_sgix               = 0x8198, // GL_FOG_OFFSET_SGIX
    fragment_color_material_sgix  = 0x8401, // GL_FRAGMENT_COLOR_MATERIAL_SGIX
    fragment_light0_sgix          = 0x840C, // GL_FRAGMENT_LIGHT0_SGIX
    fragment_light1_sgix          = 0x840D, // GL_FRAGMENT_LIGHT1_SGIX
    fragment_light2_sgix          = 0x840E, // GL_FRAGMENT_LIGHT2_SGIX
    fragment_light3_sgix          = 0x840F, // GL_FRAGMENT_LIGHT3_SGIX
    fragment_light4_sgix          = 0x8410, // GL_FRAGMENT_LIGHT4_SGIX
    fragment_light5_sgix          = 0x8411, // GL_FRAGMENT_LIGHT5_SGIX
    fragment_light6_sgix          = 0x8412, // GL_FRAGMENT_LIGHT6_SGIX
    fragment_light7_sgix          = 0x8413, // GL_FRAGMENT_LIGHT7_SGIX
    fragment_lighting_sgix        = 0x8400, // GL_FRAGMENT_LIGHTING_SGIX
    framebuffer_fetch_noncoherent_qcom =
        0x96A2,                   // GL_FRAMEBUFFER_FETCH_NONCOHERENT_QCOM
    framezoom_sgix      = 0x818B, // GL_FRAMEZOOM_SGIX
    histogram_ext       = 0x8024, // GL_HISTOGRAM_EXT
    interlace_sgix      = 0x8094, // GL_INTERLACE_SGIX
    ir_instrument1_sgix = 0x817F, // GL_IR_INSTRUMENT1_SGIX
    minmax_ext          = 0x802E, // GL_MINMAX_EXT
    multisample_sgis    = 0x809D, // GL_MULTISAMPLE_SGIS
    pixel_texture_sgis  = 0x8353, // GL_PIXEL_TEXTURE_SGIS
    pixel_tex_gen_sgix  = 0x8139, // GL_PIXEL_TEX_GEN_SGIX
    post_color_matrix_color_table_sgi =
        0x80D2, // GL_POST_COLOR_MATRIX_COLOR_TABLE_SGI
    post_convolution_color_table_sgi =
        0x80D1,                         // GL_POST_CONVOLUTION_COLOR_TABLE_SGI
    reference_plane_sgix      = 0x817D, // GL_REFERENCE_PLANE_SGIX
    rescale_normal_ext        = 0x803A, // GL_RESCALE_NORMAL_EXT
    sample_alpha_to_mask_sgis = 0x809E, // GL_SAMPLE_ALPHA_TO_MASK_SGIS
    sample_alpha_to_one_sgis  = 0x809F, // GL_SAMPLE_ALPHA_TO_ONE_SGIS
    sample_mask_sgis          = 0x80A0, // GL_SAMPLE_MASK_SGIS
    separable_2d_ext          = 0x8012, // GL_SEPARABLE_2D_EXT
    shading_rate_image_per_primitive_nv =
        0x95B1, // GL_SHADING_RATE_IMAGE_PER_PRIMITIVE_NV
    shading_rate_preserve_aspect_ratio_qcom =
        0x96A5, // GL_SHADING_RATE_PRESERVE_ASPECT_RATIO_QCOM
    shared_texture_palette_ext = 0x81FB, // GL_SHARED_TEXTURE_PALETTE_EXT
    sprite_sgix                = 0x8148, // GL_SPRITE_SGIX
    texture_3d_ext             = 0x806F, // GL_TEXTURE_3D_EXT
    texture_4d_sgis            = 0x8134, // GL_TEXTURE_4D_SGIS
    texture_color_table_sgi    = 0x80BC, // GL_TEXTURE_COLOR_TABLE_SGI
    texture_cube_map_arb       = 0x8513, // GL_TEXTURE_CUBE_MAP_ARB
    texture_cube_map_ext       = 0x8513, // GL_TEXTURE_CUBE_MAP_EXT
    texture_cube_map_oes       = 0x8513, // GL_TEXTURE_CUBE_MAP_OES
    texture_gen_str_oes        = 0x8D60, // GL_TEXTURE_GEN_STR_OES
    texture_rectangle_arb      = 0x84F5, // GL_TEXTURE_RECTANGLE_ARB
    texture_rectangle_nv       = 0x84F5, // GL_TEXTURE_RECTANGLE_NV
}; // enum class enable_cap

} // namespace gl::group
