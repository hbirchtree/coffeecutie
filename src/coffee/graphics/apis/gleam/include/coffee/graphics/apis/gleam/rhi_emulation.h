#pragma once

#include "rhi.h"

namespace gleam::emulation {
namespace img {
inline auto powervr_sgx530()
{
    return api::load_options_t{
        .api_version = 0x200,
        .api_type    = api_type_t::es,
        // clang-format off
        .api_extensions = std::set<std::string>{
            "GL_EXT_blend_minmax",
            "GL_EXT_discard_framebuffer",
            "GL_EXT_multi_draw_arrays",
            "GL_EXT_multisampled_render_to_texture",
            "GL_EXT_shader_texture_lod",
            "GL_EXT_texture_format_BGRA8888",
            "GL_EXT_texture_rg",
            "GL_IMG_multisampled_render_to_texture",
            "GL_IMG_program_binary",
            "GL_IMG_read_format",
            "GL_IMG_shader_binary",
            "GL_IMG_texture_compression_pvrtc",
            "GL_IMG_texture_format_BGRA8888",
            "GL_IMG_texture_npot",
            "GL_IMG_uniform_buffer_object",
            "GL_KHR_debug",
            "GL_OES_EGL_image",
            "GL_OES_EGL_image_external",
            "GL_OES_compressed_ETC1_RGB8_texture",
            "GL_OES_depth24",
            "GL_OES_depth_texture",
            "GL_OES_egl_sync",
            "GL_OES_element_index_uint",
            "GL_OES_fragment_precision_high",
            "GL_OES_get_program_binary",
            "GL_OES_mapbuffer",
            "GL_OES_packed_depth_stencil",
            "GL_OES_required_internalformat",
            "GL_OES_rgb8_rgba8",
            "GL_OES_standard_derivatives",
            "GL_OES_surfaceless_context",
            "GL_OES_texture_float",
            "GL_OES_texture_half_float",
            "GL_OES_vertex_array_object",
            "GL_OES_vertex_half_float"
        },
        // clang-format on
    };
}
}

namespace arm {
inline auto mali_400mp()
{
    return api::load_options_t{
        .api_version = 0x200,
        .api_type = api_type_t::es,
        // clang-format off
        .api_extensions = std::set<std::string>{
            "GL_ARM_rgba8",
            "GL_ARM_mali_shader_binary",
            "GL_EXT_blend_minmax",
            "GL_EXT_debug_marker",
            "GL_EXT_discard_framebuffer",
            "GL_EXT_multisampled_render_to_texture",
            "GL_EXT_texture_format_BGRA8888",
            "GL_OES_compressed_ETC1_RGB8_texture",
            "GL_OES_depth_texture",
            "GL_OES_depth24",
            "GL_OES_EGL_image",
            "GL_OES_EGL_image_external",
            "GL_OES_EGL_sync",
            "GL_OES_packed_depth_stencil",
            "GL_OES_rgb8_rgba8",
            "GL_OES_standard_derivatives",
            "GL_OES_texture_npot",
        },
        // clang-format on
    };
}
inline auto mali_g710()
{
    return api::load_options_t{
        .api_version = 0x320,
        .api_type = api_type_t::es,
        // clang-format off
        .api_extensions = std::set<std::string>{
            "GL_ANDROID_extension_pack_es31a",
            "GL_ARM_mali_program_binary",
            "GL_ARM_mali_shader_binary",
            "GL_ARM_rgba8",
            "GL_ARM_shader_framebuffer_fetch",
            "GL_ARM_shader_framebuffer_fetch_depth_stencil",
            "GL_ARM_texture_unnormalized_coordinates",
            "GL_EXT_EGL_image_array",
            "GL_EXT_YUV_target",
            "GL_EXT_blend_minmax",
            "GL_EXT_buffer_storage",
            "GL_EXT_clip_control",
            "GL_EXT_color_buffer_float",
            "GL_EXT_color_buffer_half_float",
            "GL_EXT_copy_image",
            "GL_EXT_debug_marker",
            "GL_EXT_discard_framebuffer",
            "GL_EXT_disjoint_timer_query",
            "GL_EXT_draw_buffers_indexed",
            "GL_EXT_draw_elements_base_vertex",
            "GL_EXT_external_buffer",
            "GL_EXT_float_blend",
            "GL_EXT_geometry_shader",
            "GL_EXT_gpu_shader5",
            "GL_EXT_multisampled_render_to_texture",
            "GL_EXT_multisampled_render_to_texture2",
            "GL_EXT_occlusion_query_boolean",
            "GL_EXT_primitive_bounding_box",
            "GL_EXT_protected_textures",
            "GL_EXT_read_format_bgra",
            "GL_EXT_robustness",
            "GL_EXT_sRGB",
            "GL_EXT_sRGB_write_control",
            "GL_EXT_shader_framebuffer_fetch",
            "GL_EXT_shader_io_blocks",
            "GL_EXT_shader_non_constant_global_initializers",
            "GL_EXT_shader_pixel_local_storage",
            "GL_EXT_shadow_samplers",
            "GL_EXT_tessellation_shader",
            "GL_EXT_texture_border_clamp",
            "GL_EXT_texture_buffer",
            "GL_EXT_texture_compression_astc_decode_mode",
            "GL_EXT_texture_compression_astc_decode_mode_rgb9e5",
            "GL_EXT_texture_cube_map_array",
            "GL_EXT_texture_filter_anisotropic",
            "GL_EXT_texture_format_BGRA8888",
            "GL_EXT_texture_rg",
            "GL_EXT_texture_sRGB_R8",
            "GL_EXT_texture_sRGB_RG8",
            "GL_EXT_texture_sRGB_decode",
            "GL_EXT_texture_storage",
            "GL_EXT_texture_type_2_10_10_10_REV",
            "GL_EXT_unpack_subimage",
            "GL_KHR_blend_equation_advanced",
            "GL_KHR_blend_equation_advanced_coherent",
            "GL_KHR_debug",
            "GL_KHR_robust_buffer_access_behavior",
            "GL_KHR_robustness",
            "GL_KHR_texture_compression_astc_hdr",
            "GL_KHR_texture_compression_astc_ldr",
            "GL_KHR_texture_compression_astc_sliced_3d",
            "GL_OES_EGL_image",
            "GL_OES_EGL_image_external",
            "GL_OES_EGL_image_external_essl3",
            "GL_OES_EGL_sync",
            "GL_OES_compressed_ETC1_RGB8_texture",
            "GL_OES_compressed_paletted_texture",
            "GL_OES_copy_image",
            "GL_OES_depth24",
            "GL_OES_depth_texture",
            "GL_OES_depth_texture_cube_map",
            "GL_OES_draw_buffers_indexed",
            "GL_OES_draw_elements_base_vertex",
            "GL_OES_element_index_uint",
            "GL_OES_fbo_render_mipmap",
            "GL_OES_geometry_shader",
            "GL_OES_get_program_binary",
            "GL_OES_gpu_shader5",
            "GL_OES_mapbuffer",
            "GL_OES_packed_depth_stencil",
            "GL_OES_primitive_bounding_box",
            "GL_OES_required_internalformat",
            "GL_OES_rgb8_rgba8",
            "GL_OES_sample_shading",
            "GL_OES_sample_variables",
            "GL_OES_shader_image_atomic",
            "GL_OES_shader_io_blocks",
            "GL_OES_shader_multisample_interpolation",
            "GL_OES_standard_derivatives",
            "GL_OES_surfaceless_context",
            "GL_OES_tessellation_shader",
            "GL_OES_texture_3D",
            "GL_OES_texture_border_clamp",
            "GL_OES_texture_buffer",
            "GL_OES_texture_compression_astc",
            "GL_OES_texture_cube_map_array",
            "GL_OES_texture_float_linear",
            "GL_OES_texture_npot",
            "GL_OES_texture_stencil8",
            "GL_OES_texture_storage_multisample_2d_array",
            "GL_OES_vertex_array_object",
            "GL_OES_vertex_half_float",
            "GL_OVR_multiview",
            "GL_OVR_multiview2",
            "GL_OVR_multiview_multisampled_render_to_texture",
        },
        // clang-format on
    };
}
}

namespace qcom {
inline auto adreno_320()
{
    return api::load_options_t{
        .api_version = 0x300,
        .api_type = api_type_t::es,
        // clang-format off
        .api_extensions = std::set<std::string>{
            "GL_AMD_compressed_ATC_texture",
            "GL_AMD_performance_monitor",
            "GL_AMD_program_binary_Z400",
            "GL_EXT_color_buffer_float",
            "GL_EXT_color_buffer_half_float",
            "GL_EXT_debug_label",
            "GL_EXT_debug_marker",
            "GL_EXT_discard_framebuffer",
            "GL_EXT_disjoint_timer_query",
            "GL_EXT_multisampled_render_to_texture",
            "GL_EXT_robustness",
            "GL_EXT_sRGB",
            "GL_EXT_texture_filter_anisotropic",
            "GL_EXT_texture_format_BGRA8888",
            "GL_EXT_texture_type_2_10_10_10_REV",
            "GL_NV_fence",
            "GL_OES_compressed_ETC1_RGB8_texture",
            "GL_OES_depth24",
            "GL_OES_depth_texture",
            "GL_OES_depth_texture_cube_map",
            "GL_OES_EGL_image",
            "GL_OES_EGL_image_external",
            "GL_OES_element_index_uint",
            "GL_OES_fbo_render_mipmap",
            "GL_OES_fragment_precision_high",
            "GL_OES_get_program_binary",
            "GL_OES_packed_depth_stencil",
            "GL_OES_rgb8_rgba8",
            "GL_OES_standard_derivatives",
            "GL_OES_texture_3D",
            "GL_OES_texture_float",
            "GL_OES_texture_half_float",
            "GL_OES_texture_half_float_linear",
            "GL_OES_texture_npot",
            "GL_OES_vertex_half_float",
            "GL_OES_vertex_type_10_10_10_2",
            "GL_OES_vertex_array_object",
            "GL_QCOM_alpha_test",
            "GL_QCOM_binning_control",
            "GL_QCOM_driver_control",
            "GL_QCOM_perfmon_global_mode",
            "GL_QCOM_extended_get",
            "GL_QCOM_extended_get2",
            "GL_QCOM_tiled_rendering",
            "GL_QCOM_writeonly_rendering",
        },
        // clang-format on
    };
}
inline auto adreno_540()
{
    return api::load_options_t{
        .api_version = 0x320,
        .api_type = api_type_t::es,
        // clang-format off
        .api_extensions = std::set<std::string>{
            "GL_AMD_compressed_ATC_texture",
            "GL_ANDROID_extension_pack_es31a",
            "GL_ARM_shader_framebuffer_fetch_depth_stencil",
            "GL_EXT_EGL_image_array",
            "GL_EXT_YUV_target",
            "GL_EXT_blit_framebuffer_params",
            "GL_EXT_buffer_storage",
            "GL_EXT_clip_cull_distance",
            "GL_EXT_color_buffer_float",
            "GL_EXT_color_buffer_half_float",
            "GL_EXT_copy_image",
            "GL_EXT_debug_label",
            "GL_EXT_debug_marker",
            "GL_EXT_discard_framebuffer",
            "GL_EXT_disjoint_timer_query",
            "GL_EXT_draw_buffers_indexed",
            "GL_EXT_external_buffer",
            "GL_EXT_geometry_shader",
            "GL_EXT_gpu_shader5",
            "GL_EXT_multisampled_render_to_texture",
            "GL_EXT_multisampled_render_to_texture2",
            "GL_EXT_primitive_bounding_box",
            "GL_EXT_protected_textures",
            "GL_EXT_robustness",
            "GL_EXT_sRGB",
            "GL_EXT_sRGB_write_control",
            "GL_EXT_shader_framebuffer_fetch",
            "GL_EXT_shader_io_blocks",
            "GL_EXT_shader_non_constant_global_initializers",
            "GL_EXT_tessellation_shader",
            "GL_EXT_texture_border_clamp",
            "GL_EXT_texture_buffer",
            "GL_EXT_texture_cube_map_array",
            "GL_EXT_texture_filter_anisotropic",
            "GL_EXT_texture_format_BGRA8888",
            "GL_EXT_texture_norm16",
            "GL_EXT_texture_sRGB_R8",
            "GL_EXT_texture_sRGB_decode",
            "GL_EXT_texture_type_2_10_10_10_REV",
            "GL_KHR_blend_equation_advanced",
            "GL_KHR_blend_equation_advanced_coherent",
            "GL_KHR_debug",
            "GL_KHR_no_error",
            "GL_KHR_texture_compression_astc_hdr",
            "GL_KHR_texture_compression_astc_ldr",
            "GL_NV_shader_noperspective_interpolation",
            "GL_OES_EGL_image",
            "GL_OES_EGL_image_external",
            "GL_OES_EGL_image_external_essl3",
            "GL_OES_EGL_sync",
            "GL_OES_compressed_ETC1_RGB8_texture",
            "GL_OES_depth24",
            "GL_OES_depth_texture",
            "GL_OES_depth_texture_cube_map",
            "GL_OES_element_index_uint",
            "GL_OES_framebuffer_object",
            "GL_OES_get_program_binary",
            "GL_OES_packed_depth_stencil",
            "GL_OES_rgb8_rgba8",
            "GL_OES_sample_shading",
            "GL_OES_sample_variables",
            "GL_OES_shader_image_atomic",
            "GL_OES_shader_multisample_interpolation",
            "GL_OES_standard_derivatives",
            "GL_OES_surfaceless_context",
            "GL_OES_texture_3D",
            "GL_OES_texture_compression_astc",
            "GL_OES_texture_float",
            "GL_OES_texture_float_linear",
            "GL_OES_texture_half_float",
            "GL_OES_texture_half_float_linear",
            "GL_OES_texture_npot",
            "GL_OES_texture_stencil8",
            "GL_OES_texture_storage_multisample_2d_array",
            "GL_OES_vertex_array_object",
            "GL_OES_vertex_half_float",
            "GL_OVR_multiview",
            "GL_OVR_multiview2",
            "GL_OVR_multiview_multisampled_render_to_texture",
            "GL_QCOM_alpha_test",
            "GL_QCOM_framebuffer_foveated",
            "GL_QCOM_shader_framebuffer_fetch_noncoherent",
            "GL_QCOM_tiled_rendering",
        },
        // clang-format on
    };
}
inline auto adreno_620()
{
    return api::load_options_t{
        .api_version = 0x320,
        .api_type = api_type_t::es,
        // clang-format off
        .api_extensions = std::set<std::string>{
            "GL_AMD_compressed_ATC_texture",
            "GL_ANDROID_extension_pack_es31a",
            "GL_ARM_shader_framebuffer_fetch_depth_stencil",
            "GL_EXT_EGL_image_array",
            "GL_EXT_EGL_image_external_wrap_modes",
            "GL_EXT_EGL_image_storage",
            "GL_EXT_YUV_target",
            "GL_EXT_blend_func_extended",
            "GL_EXT_blit_framebuffer_params",
            "GL_EXT_buffer_storage",
            "GL_EXT_clip_control",
            "GL_EXT_clip_cull_distance",
            "GL_EXT_color_buffer_float",
            "GL_EXT_color_buffer_half_float",
            "GL_EXT_copy_image",
            "GL_EXT_debug_label",
            "GL_EXT_debug_marker",
            "GL_EXT_discard_framebuffer",
            "GL_EXT_disjoint_timer_query",
            "GL_EXT_draw_buffers_indexed",
            "GL_EXT_external_buffer",
            "GL_EXT_fragment_invocation_density",
            "GL_EXT_geometry_shader",
            "GL_EXT_gpu_shader5",
            "GL_EXT_memory_object",
            "GL_EXT_memory_object_fd",
            "GL_EXT_multisampled_render_to_texture",
            "GL_EXT_multisampled_render_to_texture2",
            "GL_EXT_primitive_bounding_box",
            "GL_EXT_protected_textures",
            "GL_EXT_read_format_bgra",
            "GL_EXT_robustness",
            "GL_EXT_sRGB",
            "GL_EXT_sRGB_write_control",
            "GL_EXT_shader_framebuffer_fetch",
            "GL_EXT_shader_io_blocks",
            "GL_EXT_shader_non_constant_global_initializers",
            "GL_EXT_tessellation_shader",
            "GL_EXT_texture_border_clamp",
            "GL_EXT_texture_buffer",
            "GL_EXT_texture_cube_map_array",
            "GL_EXT_texture_filter_anisotropic",
            "GL_EXT_texture_format_BGRA8888",
            "GL_EXT_texture_format_sRGB_override",
            "GL_EXT_texture_norm16",
            "GL_EXT_texture_sRGB_R8",
            "GL_EXT_texture_sRGB_decode",
            "GL_EXT_texture_type_2_10_10_10_REV",
            "GL_KHR_blend_equation_advanced",
            "GL_KHR_blend_equation_advanced_coherent",
            "GL_KHR_debug",
            "GL_KHR_no_error",
            "GL_KHR_robust_buffer_access_behavior",
            "GL_KHR_texture_compression_astc_hdr",
            "GL_KHR_texture_compression_astc_ldr",
            "GL_NV_shader_noperspective_interpolation",
            "GL_OES_EGL_image",
            "GL_OES_EGL_image_external",
            "GL_OES_EGL_image_external_essl3",
            "GL_OES_EGL_sync",
            "GL_OES_compressed_ETC1_RGB8_texture",
            "GL_OES_depth24",
            "GL_OES_depth_texture",
            "GL_OES_depth_texture_cube_map",
            "GL_OES_element_index_uint",
            "GL_OES_framebuffer_object",
            "GL_OES_get_program_binary",
            "GL_OES_packed_depth_stencil",
            "GL_OES_rgb8_rgba8",
            "GL_OES_sample_shading",
            "GL_OES_sample_variables",
            "GL_OES_shader_image_atomic",
            "GL_OES_shader_multisample_interpolation",
            "GL_OES_standard_derivatives",
            "GL_OES_surfaceless_context",
            "GL_OES_texture_3D",
            "GL_OES_texture_compression_astc",
            "GL_OES_texture_float",
            "GL_OES_texture_float_linear",
            "GL_OES_texture_half_float",
            "GL_OES_texture_half_float_linear",
            "GL_OES_texture_npot",
            "GL_OES_texture_stencil8",
            "GL_OES_texture_storage_multisample_2d_array",
            "GL_OES_texture_view",
            "GL_OES_vertex_array_object",
            "GL_OES_vertex_half_float",
            "GL_OVR_multiview",
            "GL_OVR_multiview2",
            "GL_OVR_multiview_multisampled_render_to_texture",
            "GL_QCOM_YUV_texture_gather",
            "GL_QCOM_alpha_test",
            "GL_QCOM_motion_estimation",
            "GL_QCOM_shader_framebuffer_fetch_noncoherent",
            "GL_QCOM_shader_framebuffer_fetch_rate",
            "GL_QCOM_texture_foveated",
            "GL_QCOM_texture_foveated_subsampled_layout",
            "GL_QCOM_tiled_rendering",
            "GL_QCOM_validate_shader_binary",
        },
        // clang-format on
    };
}
}

} // namespace gleam::emulation
