#pragma once

#include "common.h"

namespace gl::groups {

// HintTarget
enum class hint_target : ::libc_types::u32 {
#ifdef GL_ALLOW_DRAW_FRG_HINT_PGI
    allow_draw_frg_hint_pgi = GL_ALLOW_DRAW_FRG_HINT_PGI,
#endif
#ifdef GL_ALLOW_DRAW_MEM_HINT_PGI
    allow_draw_mem_hint_pgi = GL_ALLOW_DRAW_MEM_HINT_PGI,
#endif
#ifdef GL_ALLOW_DRAW_OBJ_HINT_PGI
    allow_draw_obj_hint_pgi = GL_ALLOW_DRAW_OBJ_HINT_PGI,
#endif
#ifdef GL_ALLOW_DRAW_WIN_HINT_PGI
    allow_draw_win_hint_pgi = GL_ALLOW_DRAW_WIN_HINT_PGI,
#endif
#ifdef GL_ALWAYS_FAST_HINT_PGI
    always_fast_hint_pgi = GL_ALWAYS_FAST_HINT_PGI,
#endif
#ifdef GL_ALWAYS_SOFT_HINT_PGI
    always_soft_hint_pgi = GL_ALWAYS_SOFT_HINT_PGI,
#endif
#ifdef GL_BACK_NORMALS_HINT_PGI
    back_normals_hint_pgi = GL_BACK_NORMALS_HINT_PGI,
#endif
#ifdef GL_BINNING_CONTROL_HINT_QCOM
    binning_control_hint_qcom = GL_BINNING_CONTROL_HINT_QCOM,
#endif
#ifdef GL_CLIP_FAR_HINT_PGI
    clip_far_hint_pgi = GL_CLIP_FAR_HINT_PGI,
#endif
#ifdef GL_CLIP_NEAR_HINT_PGI
    clip_near_hint_pgi = GL_CLIP_NEAR_HINT_PGI,
#endif
#ifdef GL_CLIP_VOLUME_CLIPPING_HINT_EXT
    clip_volume_clipping_hint_ext = GL_CLIP_VOLUME_CLIPPING_HINT_EXT,
#endif
#ifdef GL_CONSERVE_MEMORY_HINT_PGI
    conserve_memory_hint_pgi = GL_CONSERVE_MEMORY_HINT_PGI,
#endif
#ifdef GL_CONVOLUTION_HINT_SGIX
    convolution_hint_sgix = GL_CONVOLUTION_HINT_SGIX,
#endif
#ifdef GL_FOG_HINT
    fog_hint = GL_FOG_HINT,
#endif
#ifdef GL_FRAGMENT_SHADER_DERIVATIVE_HINT
    fragment_shader_derivative_hint = GL_FRAGMENT_SHADER_DERIVATIVE_HINT,
#endif
#ifdef GL_FRAGMENT_SHADER_DERIVATIVE_HINT_ARB
    fragment_shader_derivative_hint_arb = GL_FRAGMENT_SHADER_DERIVATIVE_HINT_ARB,
#endif
#ifdef GL_FRAGMENT_SHADER_DERIVATIVE_HINT_OES
    fragment_shader_derivative_hint_oes = GL_FRAGMENT_SHADER_DERIVATIVE_HINT_OES,
#endif
#ifdef GL_FULL_STIPPLE_HINT_PGI
    full_stipple_hint_pgi = GL_FULL_STIPPLE_HINT_PGI,
#endif
#ifdef GL_GENERATE_MIPMAP_HINT
    generate_mipmap_hint = GL_GENERATE_MIPMAP_HINT,
#endif
#ifdef GL_GENERATE_MIPMAP_HINT_SGIS
    generate_mipmap_hint_sgis = GL_GENERATE_MIPMAP_HINT_SGIS,
#endif
#ifdef GL_LINE_QUALITY_HINT_SGIX
    line_quality_hint_sgix = GL_LINE_QUALITY_HINT_SGIX,
#endif
#ifdef GL_LINE_SMOOTH_HINT
    line_smooth_hint = GL_LINE_SMOOTH_HINT,
#endif
#ifdef GL_MATERIAL_SIDE_HINT_PGI
    material_side_hint_pgi = GL_MATERIAL_SIDE_HINT_PGI,
#endif
#ifdef GL_MAX_VERTEX_HINT_PGI
    max_vertex_hint_pgi = GL_MAX_VERTEX_HINT_PGI,
#endif
#ifdef GL_MULTISAMPLE_FILTER_HINT_NV
    multisample_filter_hint_nv = GL_MULTISAMPLE_FILTER_HINT_NV,
#endif
#ifdef GL_NATIVE_GRAPHICS_BEGIN_HINT_PGI
    native_graphics_begin_hint_pgi = GL_NATIVE_GRAPHICS_BEGIN_HINT_PGI,
#endif
#ifdef GL_NATIVE_GRAPHICS_END_HINT_PGI
    native_graphics_end_hint_pgi = GL_NATIVE_GRAPHICS_END_HINT_PGI,
#endif
#ifdef GL_PACK_CMYK_HINT_EXT
    pack_cmyk_hint_ext = GL_PACK_CMYK_HINT_EXT,
#endif
#ifdef GL_PERSPECTIVE_CORRECTION_HINT
    perspective_correction_hint = GL_PERSPECTIVE_CORRECTION_HINT,
#endif
#ifdef GL_PHONG_HINT_WIN
    phong_hint_win = GL_PHONG_HINT_WIN,
#endif
#ifdef GL_POINT_SMOOTH_HINT
    point_smooth_hint = GL_POINT_SMOOTH_HINT,
#endif
#ifdef GL_POLYGON_SMOOTH_HINT
    polygon_smooth_hint = GL_POLYGON_SMOOTH_HINT,
#endif
#ifdef GL_PREFER_DOUBLEBUFFER_HINT_PGI
    prefer_doublebuffer_hint_pgi = GL_PREFER_DOUBLEBUFFER_HINT_PGI,
#endif
#ifdef GL_PROGRAM_BINARY_RETRIEVABLE_HINT
    program_binary_retrievable_hint = GL_PROGRAM_BINARY_RETRIEVABLE_HINT,
#endif
#ifdef GL_RECLAIM_MEMORY_HINT_PGI
    reclaim_memory_hint_pgi = GL_RECLAIM_MEMORY_HINT_PGI,
#endif
#ifdef GL_SCALEBIAS_HINT_SGIX
    scalebias_hint_sgix = GL_SCALEBIAS_HINT_SGIX,
#endif
#ifdef GL_STRICT_DEPTHFUNC_HINT_PGI
    strict_depthfunc_hint_pgi = GL_STRICT_DEPTHFUNC_HINT_PGI,
#endif
#ifdef GL_STRICT_LIGHTING_HINT_PGI
    strict_lighting_hint_pgi = GL_STRICT_LIGHTING_HINT_PGI,
#endif
#ifdef GL_STRICT_SCISSOR_HINT_PGI
    strict_scissor_hint_pgi = GL_STRICT_SCISSOR_HINT_PGI,
#endif
#ifdef GL_TEXTURE_COMPRESSION_HINT
    texture_compression_hint = GL_TEXTURE_COMPRESSION_HINT,
#endif
#ifdef GL_TEXTURE_COMPRESSION_HINT_ARB
    texture_compression_hint_arb = GL_TEXTURE_COMPRESSION_HINT_ARB,
#endif
#ifdef GL_TEXTURE_MULTI_BUFFER_HINT_SGIX
    texture_multi_buffer_hint_sgix = GL_TEXTURE_MULTI_BUFFER_HINT_SGIX,
#endif
#ifdef GL_TEXTURE_STORAGE_HINT_APPLE
    texture_storage_hint_apple = GL_TEXTURE_STORAGE_HINT_APPLE,
#endif
#ifdef GL_TRANSFORM_HINT_APPLE
    transform_hint_apple = GL_TRANSFORM_HINT_APPLE,
#endif
#ifdef GL_UNPACK_CMYK_HINT_EXT
    unpack_cmyk_hint_ext = GL_UNPACK_CMYK_HINT_EXT,
#endif
#ifdef GL_VERTEX_ARRAY_STORAGE_HINT_APPLE
    vertex_array_storage_hint_apple = GL_VERTEX_ARRAY_STORAGE_HINT_APPLE,
#endif
#ifdef GL_VERTEX_CONSISTENT_HINT_PGI
    vertex_consistent_hint_pgi = GL_VERTEX_CONSISTENT_HINT_PGI,
#endif
#ifdef GL_VERTEX_DATA_HINT_PGI
    vertex_data_hint_pgi = GL_VERTEX_DATA_HINT_PGI,
#endif
#ifdef GL_VERTEX_PRECLIP_HINT_SGIX
    vertex_preclip_hint_sgix = GL_VERTEX_PRECLIP_HINT_SGIX,
#endif
#ifdef GL_VERTEX_PRECLIP_SGIX
    vertex_preclip_sgix = GL_VERTEX_PRECLIP_SGIX,
#endif
#ifdef GL_WIDE_LINE_HINT_PGI
    wide_line_hint_pgi = GL_WIDE_LINE_HINT_PGI,
#endif
}; // enum class hint_target

} // namespace gl::groups
