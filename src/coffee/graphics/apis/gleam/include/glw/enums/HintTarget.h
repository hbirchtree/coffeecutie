#pragma once

#include "common.h"

namespace gl::group {

// HintTarget
enum class hint_target : u32
{
    fog_hint = 0x0C54, // GL_FOG_HINT
    fragment_shader_derivative_hint =
        0x8B8B,                           // GL_FRAGMENT_SHADER_DERIVATIVE_HINT
    generate_mipmap_hint        = 0x8192, // GL_GENERATE_MIPMAP_HINT
    line_smooth_hint            = 0x0C52, // GL_LINE_SMOOTH_HINT
    perspective_correction_hint = 0x0C50, // GL_PERSPECTIVE_CORRECTION_HINT
    point_smooth_hint           = 0x0C51, // GL_POINT_SMOOTH_HINT
    polygon_smooth_hint         = 0x0C53, // GL_POLYGON_SMOOTH_HINT
    program_binary_retrievable_hint =
        0x8257, // GL_PROGRAM_BINARY_RETRIEVABLE_HINT
    texture_compression_hint      = 0x84EF,  // GL_TEXTURE_COMPRESSION_HINT
    allow_draw_frg_hint_pgi       = 0x1A210, // GL_ALLOW_DRAW_FRG_HINT_PGI
    allow_draw_mem_hint_pgi       = 0x1A211, // GL_ALLOW_DRAW_MEM_HINT_PGI
    allow_draw_obj_hint_pgi       = 0x1A20E, // GL_ALLOW_DRAW_OBJ_HINT_PGI
    allow_draw_win_hint_pgi       = 0x1A20F, // GL_ALLOW_DRAW_WIN_HINT_PGI
    always_fast_hint_pgi          = 0x1A20C, // GL_ALWAYS_FAST_HINT_PGI
    always_soft_hint_pgi          = 0x1A20D, // GL_ALWAYS_SOFT_HINT_PGI
    back_normals_hint_pgi         = 0x1A223, // GL_BACK_NORMALS_HINT_PGI
    binning_control_hint_qcom     = 0x8FB0,  // GL_BINNING_CONTROL_HINT_QCOM
    clip_far_hint_pgi             = 0x1A221, // GL_CLIP_FAR_HINT_PGI
    clip_near_hint_pgi            = 0x1A220, // GL_CLIP_NEAR_HINT_PGI
    clip_volume_clipping_hint_ext = 0x80F0,  // GL_CLIP_VOLUME_CLIPPING_HINT_EXT
    conserve_memory_hint_pgi      = 0x1A1FD, // GL_CONSERVE_MEMORY_HINT_PGI
    convolution_hint_sgix         = 0x8316,  // GL_CONVOLUTION_HINT_SGIX
    fragment_shader_derivative_hint_arb =
        0x8B8B, // GL_FRAGMENT_SHADER_DERIVATIVE_HINT_ARB
    fragment_shader_derivative_hint_oes =
        0x8B8B, // GL_FRAGMENT_SHADER_DERIVATIVE_HINT_OES
    full_stipple_hint_pgi      = 0x1A219, // GL_FULL_STIPPLE_HINT_PGI
    generate_mipmap_hint_sgis  = 0x8192,  // GL_GENERATE_MIPMAP_HINT_SGIS
    line_quality_hint_sgix     = 0x835B,  // GL_LINE_QUALITY_HINT_SGIX
    material_side_hint_pgi     = 0x1A22C, // GL_MATERIAL_SIDE_HINT_PGI
    max_vertex_hint_pgi        = 0x1A22D, // GL_MAX_VERTEX_HINT_PGI
    multisample_filter_hint_nv = 0x8534,  // GL_MULTISAMPLE_FILTER_HINT_NV
    native_graphics_begin_hint_pgi =
        0x1A203,                            // GL_NATIVE_GRAPHICS_BEGIN_HINT_PGI
    native_graphics_end_hint_pgi = 0x1A204, // GL_NATIVE_GRAPHICS_END_HINT_PGI
    pack_cmyk_hint_ext           = 0x800E,  // GL_PACK_CMYK_HINT_EXT
    phong_hint_win               = 0x80EB,  // GL_PHONG_HINT_WIN
    prefer_doublebuffer_hint_pgi = 0x1A1F8, // GL_PREFER_DOUBLEBUFFER_HINT_PGI
    reclaim_memory_hint_pgi      = 0x1A1FE, // GL_RECLAIM_MEMORY_HINT_PGI
    scalebias_hint_sgix          = 0x8322,  // GL_SCALEBIAS_HINT_SGIX
    strict_depthfunc_hint_pgi    = 0x1A216, // GL_STRICT_DEPTHFUNC_HINT_PGI
    strict_lighting_hint_pgi     = 0x1A217, // GL_STRICT_LIGHTING_HINT_PGI
    strict_scissor_hint_pgi      = 0x1A218, // GL_STRICT_SCISSOR_HINT_PGI
    texture_compression_hint_arb = 0x84EF,  // GL_TEXTURE_COMPRESSION_HINT_ARB
    texture_multi_buffer_hint_sgix =
        0x812E,                          // GL_TEXTURE_MULTI_BUFFER_HINT_SGIX
    texture_storage_hint_apple = 0x85BC, // GL_TEXTURE_STORAGE_HINT_APPLE
    transform_hint_apple       = 0x85B1, // GL_TRANSFORM_HINT_APPLE
    unpack_cmyk_hint_ext       = 0x800F, // GL_UNPACK_CMYK_HINT_EXT
    vertex_array_storage_hint_apple =
        0x851F,                           // GL_VERTEX_ARRAY_STORAGE_HINT_APPLE
    vertex_consistent_hint_pgi = 0x1A22B, // GL_VERTEX_CONSISTENT_HINT_PGI
    vertex_data_hint_pgi       = 0x1A22A, // GL_VERTEX_DATA_HINT_PGI
    vertex_preclip_hint_sgix   = 0x83EF,  // GL_VERTEX_PRECLIP_HINT_SGIX
    vertex_preclip_sgix        = 0x83EE,  // GL_VERTEX_PRECLIP_SGIX
    wide_line_hint_pgi         = 0x1A222, // GL_WIDE_LINE_HINT_PGI
}; // enum class hint_target

} // namespace gl::group
