#pragma once

#include "rhi.h"

namespace gleam::emulation {
namespace img {
inline auto powervr_sgx530_bbb()
{
    // Generated from BeagleBone Black
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
            // "GL_KHR_debug",
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
        .limits = api_limits{
            .buffers = {},
            .draws = {
                .element_count        = 1024 * 64,
                .element_index        = 1024 * 64,
                .element_vertex_count = 1024 * 64,

                .instance_count = 1,
                .instance_offset = 0,
            },
            .shaders = {
                .vertex_uniform_vectors   = 64,
                .fragment_uniform_vectors = 64,
            },
            .textures = {
                .texture_units = 8,
                .d2_size       = 2048,
                .cube_size     = 2048,
            },
        },
        // clang-format on
        .resolution = size_2d<u32>{848, 480},
        .device_info = std::make_tuple("Imagination Technologies", "PowerVR SGX 530"),
        .version_string = "OpenGL ES 2.0 build 1.14@3699939 (MAIN)",
        .glsl_version_string = "OpenGL ES GLSL ES 1.00 build 1.14@3699939 (MAIN)",
    };
}

inline auto powervr_sgx530_n900()
{
    // Dump from Nokia N900:
    // clang-format off
    // EGL vendor: Imagination Technologies
    // EGL version: 1.4 build 1.3.13.1607
    // EGL APIs: OpenGL_ES OpenVG
    // EGL extensions: EGL_KHR_image EGL_KHR_gl_texture_2D_image EGL_KHR_gl_texture_cubemap_image EGL_KHR_gl_renderbuffer_image EGL_KHR_vg_parent_image EGL_NOKIA_texture_from_pixmap
    // GL vendor: Imagination Technologies
    // GL renderer: PowerVR SGX 530
    // GL version: OpenGL ES 2.0
    // GL shader language: OpenGL ES GLSL ES 1.00
    // GL extensions: GL_OES_rgb8_rgba8 GL_OES_depth24 GL_OES_vertex_half_float GL_OES_texture_float GL_OES_texture_half_float GL_OES_element_index_uint GL_OES_mapbuffer GL_OES_fragment_precision_high GL_OES_compressed_ETC1_RGB8_texture GL_OES_EGL_image GL_EXT_multi_draw_arrays GL_IMG_shader_binary GL_IMG_texture_compression_pvrtc GL_IMG_texture_stream2 GL_IMG_texture_npot
    // clang-format on
    return api::load_options_t{
        .api_version = 0x200,
        .api_type    = api_type_t::es,
        // clang-format off
        .api_extensions = std::set<std::string>{
            "GL_EXT_multi_draw_arrays",
            "GL_IMG_shader_binary",
            "GL_IMG_texture_compression_pvrtc",
            "GL_IMG_texture_npot",
            "GL_IMG_texture_stream2",
            "GL_OES_EGL_image",
            "GL_OES_compressed_ETC1_RGB8_texture",
            "GL_OES_depth24",
            "GL_OES_element_index_uint",
            "GL_OES_fragment_precision_high",
            "GL_OES_mapbuffer",
            "GL_OES_rgb8_rgba8",
            "GL_OES_texture_float",
            "GL_OES_texture_half_float",
            "GL_OES_vertex_half_float",
        },
        .limits = api_limits{
            .buffers = {},
            .draws = {
                .element_count        = 1024 * 64,
                .element_index        = 1024 * 64,
                .element_vertex_count = 1024 * 64,

                .instance_count = 1,
                .instance_offset = 0,
            },
            .shaders = {
                .vertex_uniform_vectors   = 128,
                .fragment_uniform_vectors = 64,
            },
            .textures = {
                .texture_units = 8,
                .d2_size       = 2048,
                .cube_size     = 2048,
            },
        },
        // clang-format on
        .resolution = size_2d<u32>{848, 480},
        .device_info = std::make_tuple("Imagination Technologies", "PowerVR SGX 530"),
        .version_string = "OpenGL ES 2.0",
        .glsl_version_string = "OpenGL ES GLSL ES 1.00",
    };
}
} // namespace img

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
        .resolution = size_2d<u32>{1280, 720},
    };
}

inline auto mesa_mali_g610()
{
    return api::load_options_t{
        .api_version = 0x310,
        .api_type = api_type_t::es,
        // clang-format off
        .api_extensions = std::set<std::string>{
            "GL_ANGLE_pack_reverse_row_order",
            "GL_ANGLE_texture_compression_dxt3",
            "GL_ANGLE_texture_compression_dxt5",
            "GL_APPLE_texture_max_level",
            "GL_EXT_EGL_image_storage",
            "GL_EXT_blend_func_extended",
            "GL_EXT_blend_minmax",
            "GL_EXT_buffer_storage",
            "GL_EXT_clip_control",
            "GL_EXT_color_buffer_float",
            "GL_EXT_color_buffer_half_float",
            "GL_EXT_compressed_ETC1_RGB8_sub_texture",
            "GL_EXT_debug_label",
            "GL_EXT_depth_clamp",
            "GL_EXT_discard_framebuffer",
            "GL_EXT_disjoint_timer_query",
            "GL_EXT_draw_buffers",
            "GL_EXT_draw_buffers_indexed",
            "GL_EXT_draw_elements_base_vertex",
            "GL_EXT_draw_instanced",
            "GL_EXT_float_blend",
            "GL_EXT_frag_depth",
            "GL_EXT_gpu_shader5",
            "GL_EXT_map_buffer_range",
            "GL_EXT_multi_draw_arrays",
            "GL_EXT_multisampled_render_to_texture",
            "GL_EXT_multisampled_render_to_texture2",
            "GL_EXT_occlusion_query_boolean",
            "GL_EXT_primitive_bounding_box",
            "GL_EXT_read_format_bgra",
            "GL_EXT_sRGB_write_control",
            "GL_EXT_separate_shader_objects",
            "GL_EXT_shader_framebuffer_fetch",
            "GL_EXT_shader_framebuffer_fetch_non_coherent",
            "GL_EXT_shader_implicit_conversions",
            "GL_EXT_shader_integer_mix",
            "GL_EXT_shader_io_blocks",
            "GL_EXT_texture_border_clamp",
            "GL_EXT_texture_buffer",
            "GL_EXT_texture_compression_bptc",
            "GL_EXT_texture_compression_dxt1",
            "GL_EXT_texture_compression_rgtc",
            "GL_EXT_texture_compression_s3tc",
            "GL_EXT_texture_compression_s3tc_srgb",
            "GL_EXT_texture_filter_anisotropic",
            "GL_EXT_texture_format_BGRA8888",
            "GL_EXT_texture_mirror_clamp_to_edge",
            "GL_EXT_texture_norm16",
            "GL_EXT_texture_rg",
            "GL_EXT_texture_sRGB_R8",
            "GL_EXT_texture_sRGB_RG8",
            "GL_EXT_texture_sRGB_decode",
            "GL_EXT_texture_type_2_10_10_10_REV",
            "GL_EXT_texture_view",
            "GL_EXT_unpack_subimage",
            "GL_INTEL_blackhole_render",
            "GL_KHR_blend_equation_advanced",
            "GL_KHR_blend_equation_advanced_coherent",
            "GL_KHR_context_flush_control",
            "GL_KHR_debug",
            "GL_KHR_no_error",
            "GL_KHR_parallel_shader_compile",
            "GL_KHR_texture_compression_astc_ldr",
            "GL_KHR_texture_compression_astc_sliced_3d",
            "GL_MESA_bgra",
            "GL_MESA_framebuffer_flip_y",
            "GL_MESA_shader_integer_functions",
            "GL_NV_conditional_render",
            "GL_NV_draw_buffers",
            "GL_NV_fbo_color_attachments",
            "GL_NV_image_formats",
            "GL_NV_pack_subimage",
            "GL_NV_pixel_buffer_object",
            "GL_NV_read_buffer",
            "GL_NV_read_depth",
            "GL_NV_read_depth_stencil",
            "GL_NV_read_stencil",
            "GL_NV_shader_noperspective_interpolation",
            "GL_OES_EGL_image",
            "GL_OES_EGL_image_external",
            "GL_OES_EGL_image_external_essl3",
            "GL_OES_EGL_sync",
            "GL_OES_compressed_ETC1_RGB8_texture",
            "GL_OES_depth24",
            "GL_OES_depth_texture",
            "GL_OES_depth_texture_cube_map",
            "GL_OES_draw_buffers_indexed",
            "GL_OES_draw_elements_base_vertex",
            "GL_OES_element_index_uint",
            "GL_OES_fbo_render_mipmap",
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
            "GL_OES_stencil8",
            "GL_OES_surfaceless_context",
            "GL_OES_texture_3D",
            "GL_OES_texture_border_clamp",
            "GL_OES_texture_buffer",
            "GL_OES_texture_float",
            "GL_OES_texture_float_linear",
            "GL_OES_texture_half_float",
            "GL_OES_texture_half_float_linear",
            "GL_OES_texture_npot",
            "GL_OES_texture_stencil8",
            "GL_OES_texture_storage_multisample_2d_array",
            "GL_OES_texture_view",
            "GL_OES_vertex_array_object",
            "GL_OES_vertex_half_float"
        },
        .limits = api_limits{
            .buffers = {
                .ssbo_size = 2147483647,
                .ubo_size = 65536,

                .ssbo_vertex = 0,
                .ssbo_fragment = 8,
                .ssbo_compute = 8,

                .ubo_vertex = 15,
                .ubo_fragment = 15,
                .ubo_compute = 15,
                .ubo_alignment = 16,
                .ssbo_alignment = 4,
            },
            .draws = {
                .element_index = 2147483647,
                .instance_count = 2147483647,
                .instance_offset = 2147483647,
            },
            .shaders = {
                .vertex_uniform_vectors = 4088,
                .fragment_uniform_vectors = 4096,
            },
            .textures = {
                .texture_units = 96,
                .d2_size = 8192,
                .d3_size = 8192,
                .d2_max_layers = 2048,
                .cube_size = 8192,
            },
        },
        // clang-format on
        .resolution = size_2d<u32>{1920, 1080},
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
        .limits = api_limits{
            .buffers = {
                .ssbo_size = 2147483647,
                .ubo_size = 65536,

                .ssbo_vertex = 0,
                .ssbo_fragment = 35,
                .ssbo_compute = 35,

                .ubo_vertex = 36,
                .ubo_fragment = 36,
                .ubo_compute = 36,
                .ubo_alignment = 16,
                .ssbo_alignment = 16,
            },
            .draws = {
                .element_index = 2147483647,
                .instance_count = 2147483647,
                .instance_offset = 2147483647,
            },
            .shaders = {
                .vertex_uniform_vectors = 4096,
                .fragment_uniform_vectors = 4096,
            },
            .textures = {
                .texture_units = 384,
                .d2_size = 16384,
                .d3_size = 16384,
                .d2_max_layers = 4096,
                .cube_size = 16384,
            },
        },
        .resolution = size_2d<u32>{2892, 1440},
    };
}
} // namespace arm

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
        .resolution = size_2d<u32>{1920, 1200},
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
        .resolution = size_2d<u32>{2824, 1440},
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
        .resolution = size_2d<u32>{2072, 1080},
    };
}
} // namespace qcom

namespace webgl {
inline auto mobile()
{
    return api::load_options_t{
        .api_version = 0x300,
        .api_type = api_type_t::es,
        // clang-format off
        .api_extensions = std::set<std::string>{
            "EXT_color_buffer_float",
            "EXT_color_buffer_half_float",
            "EXT_texture_filter_anisotropic",
            "WEBGL_compressed_texture_astc",
            "WEBGL_compressed_texture_etc",
            "WEBGL_compressed_texture_etc1",
            "WEBGL_multi_draw",
        },
        // clang-format on
    };
}
inline auto desktop()
{
    return api::load_options_t{
        .api_version = 0x300,
        .api_type = api_type_t::es,
        // clang-format off
        .api_extensions = std::set<std::string>{
            "EXT_color_buffer_float",
            "EXT_color_buffer_half_float",
            "EXT_texture_compression_bptc",
            "EXT_texture_compression_rgtc",
            "EXT_texture_filter_anisotropic",
            "WEBGL_compressed_texture_astc",
            "WEBGL_compressed_texture_etc",
            "WEBGL_compressed_texture_etc1",
            "WEBGL_compressed_texture_s3tc",
            "WEBGL_debug_renderer_info",
            "WEBGL_draw_instanced_base_vertex_base_instance",
            "WEBGL_lose_context",
            "WEBGL_multi_draw",
            "WEBGL_multi_draw_instanced_base_vertex_base_instance",
        },
        // clang-format on
    };
}
inline auto desktop_macos()
{
    return api::load_options_t{
        .api_version = 0x300,
        .api_type = api_type_t::es,
        // clang-format off
        .api_extensions = std::set<std::string>{
            "EXT_color_buffer_float",
            "EXT_color_buffer_half_float",
            "EXT_texture_compression_rgtc",
            "EXT_texture_filter_anisotropic",
            "WEBGL_compressed_texture_etc1",
            "WEBGL_compressed_texture_s3tc",
            "WEBGL_debug_renderer_info",
            "WEBGL_draw_instanced_base_vertex_base_instance",
            "WEBGL_lose_context",
            "WEBGL_multi_draw",
            "WEBGL_multi_draw_instanced_base_vertex_base_instance",
        },
        // clang-format on
    };
}
} // namespace webgl

namespace amd {
inline auto rx560_pro()
{
    return api::load_options_t{
        .api_version = 0x410,
        .api_type = api_type_t::core,
        // clang-format off
        .api_extensions = std::set<std::string>{
            "GL_APPLE_client_storage",
            "GL_APPLE_container_object_shareable",
            "GL_APPLE_flush_render",
            "GL_APPLE_object_purgeable",
            "GL_APPLE_rgb_422",
            "GL_APPLE_row_bytes",
            "GL_APPLE_texture_range",
            "GL_ARB_ES2_compatibility",
            "GL_ARB_blend_func_extended",
            "GL_ARB_draw_buffers_blend",
            "GL_ARB_draw_indirect",
            "GL_ARB_explicit_attrib_location",
            "GL_ARB_gpu_shader5",
            "GL_ARB_gpu_shader_fp64",
            "GL_ARB_instanced_arrays",
            "GL_ARB_internalformat_query",
            "GL_ARB_occlusion_query2",
            "GL_ARB_sample_shading",
            "GL_ARB_sampler_objects",
            "GL_ARB_separate_shader_objects",
            "GL_ARB_shader_bit_encoding",
            "GL_ARB_shader_subroutine",
            "GL_ARB_shading_language_include",
            "GL_ARB_tessellation_shader",
            "GL_ARB_texture_buffer_object_rgb32",
            "GL_ARB_texture_cube_map_array",
            "GL_ARB_texture_gather",
            "GL_ARB_texture_query_lod",
            "GL_ARB_texture_rgb10_a2ui",
            "GL_ARB_texture_storage",
            "GL_ARB_texture_swizzle",
            "GL_ARB_timer_query",
            "GL_ARB_transform_feedback2",
            "GL_ARB_transform_feedback3",
            "GL_ARB_vertex_attrib_64bit",
            "GL_ARB_vertex_type_2_10_10_10_rev",
            "GL_ARB_viewport_array",
            "GL_ATI_texture_mirror_once",
            "GL_EXT_debug_label",
            "GL_EXT_debug_marker",
            "GL_EXT_depth_bounds_test",
            "GL_EXT_texture_compression_s3tc",
            "GL_EXT_texture_filter_anisotropic",
            "GL_EXT_texture_mirror_clamp",
            "GL_EXT_texture_sRGB_decode",
            "GL_NV_texture_barrier",
        },
        // clang-format on
        .resolution = size_2d<u32>{2880, 1800},
    };
}
} // namespace amd

} // namespace gleam::emulation
