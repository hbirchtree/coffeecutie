#pragma once

#include "GetPName.h"

namespace gl::limits {

inline std::vector<std::pair<group::get_prop, std::string_view>> properties()
{
    using p = group::get_prop;
    return
    {
#if defined(GL_MAX_3D_TEXTURE_SIZE)
        {p::max_3d_texture_size, "max_3d_texture_size"},
#endif
#if defined(GL_MAX_3D_TEXTURE_SIZE_EXT)
            {p::max_3d_texture_size_ext, "max_3d_texture_size_ext"},
#endif
#if defined(GL_MAX_4D_TEXTURE_SIZE_SGIS)
            {p::max_4d_texture_size_sgis, "max_4d_texture_size_sgis"},
#endif
#if defined(GL_MAX_ACTIVE_LIGHTS_SGIX)
            {p::max_active_lights_sgix, "max_active_lights_sgix"},
#endif
#if defined(GL_MAX_ARRAY_TEXTURE_LAYERS)
            {p::max_array_texture_layers, "max_array_texture_layers"},
#endif
#if defined(GL_MAX_ASYNC_DRAW_PIXELS_SGIX)
            {p::max_async_draw_pixels_sgix, "max_async_draw_pixels_sgix"},
#endif
#if defined(GL_MAX_ASYNC_HISTOGRAM_SGIX)
            {p::max_async_histogram_sgix, "max_async_histogram_sgix"},
#endif
#if defined(GL_MAX_ASYNC_READ_PIXELS_SGIX)
            {p::max_async_read_pixels_sgix, "max_async_read_pixels_sgix"},
#endif
#if defined(GL_MAX_ASYNC_TEX_IMAGE_SGIX)
            {p::max_async_tex_image_sgix, "max_async_tex_image_sgix"},
#endif
#if defined(GL_MAX_ATTRIB_STACK_DEPTH)
            {p::max_attrib_stack_depth, "max_attrib_stack_depth"},
#endif
#if defined(GL_MAX_CLIENT_ATTRIB_STACK_DEPTH)
            {p::max_client_attrib_stack_depth, "max_client_attrib_stack_depth"},
#endif
#if defined(GL_MAX_CLIPMAP_DEPTH_SGIX)
            {p::max_clipmap_depth_sgix, "max_clipmap_depth_sgix"},
#endif
#if defined(GL_MAX_CLIPMAP_VIRTUAL_DEPTH_SGIX)
            {p::max_clipmap_virtual_depth_sgix,
             "max_clipmap_virtual_depth_sgix"},
#endif
#if defined(GL_MAX_CLIP_DISTANCES)
            {p::max_clip_distances, "max_clip_distances"},
#endif
#if defined(GL_MAX_CLIP_PLANES)
            {p::max_clip_planes, "max_clip_planes"},
#endif
#if defined(GL_MAX_COLOR_ATTACHMENTS)
            {p::max_color_attachments, "max_color_attachments"},
#endif
#if defined(GL_MAX_COLOR_ATTACHMENTS_EXT)
            {p::max_color_attachments_ext, "max_color_attachments_ext"},
#endif
#if defined(GL_MAX_COLOR_ATTACHMENTS_NV)
            {p::max_color_attachments_nv, "max_color_attachments_nv"},
#endif
#if defined(GL_MAX_COLOR_MATRIX_STACK_DEPTH_SGI)
            {p::max_color_matrix_stack_depth_sgi,
             "max_color_matrix_stack_depth_sgi"},
#endif
#if defined(GL_MAX_COLOR_TEXTURE_SAMPLES)
            {p::max_color_texture_samples, "max_color_texture_samples"},
#endif
#if defined(GL_MAX_COMBINED_ATOMIC_COUNTERS)
            {p::max_combined_atomic_counters, "max_combined_atomic_counters"},
#endif
#if defined(GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS)
            {p::max_combined_compute_uniform_components,
             "max_combined_compute_uniform_components"},
#endif
#if defined(GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS)
            {p::max_combined_fragment_uniform_components,
             "max_combined_fragment_uniform_components"},
#endif
#if defined(GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS)
            {p::max_combined_geometry_uniform_components,
             "max_combined_geometry_uniform_components"},
#endif
#if defined(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS)
            {p::max_combined_shader_storage_blocks,
             "max_combined_shader_storage_blocks"},
#endif
#if defined(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS)
            {p::max_combined_texture_image_units,
             "max_combined_texture_image_units"},
#endif
#if defined(GL_MAX_COMBINED_UNIFORM_BLOCKS)
            {p::max_combined_uniform_blocks, "max_combined_uniform_blocks"},
#endif
#if defined(GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS)
            {p::max_combined_vertex_uniform_components,
             "max_combined_vertex_uniform_components"},
#endif
#if defined(GL_MAX_COMPUTE_ATOMIC_COUNTERS)
            {p::max_compute_atomic_counters, "max_compute_atomic_counters"},
#endif
#if defined(GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS)
            {p::max_compute_atomic_counter_buffers,
             "max_compute_atomic_counter_buffers"},
#endif
#if defined(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS)
            {p::max_compute_shader_storage_blocks,
             "max_compute_shader_storage_blocks"},
#endif
#if defined(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS)
            {p::max_compute_texture_image_units,
             "max_compute_texture_image_units"},
#endif
#if defined(GL_MAX_COMPUTE_UNIFORM_BLOCKS)
            {p::max_compute_uniform_blocks, "max_compute_uniform_blocks"},
#endif
#if defined(GL_MAX_COMPUTE_UNIFORM_COMPONENTS)
            {p::max_compute_uniform_components,
             "max_compute_uniform_components"},
#endif
#if defined(GL_MAX_COMPUTE_WORK_GROUP_COUNT)
            {p::max_compute_work_group_count, "max_compute_work_group_count"},
#endif
#if defined(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS)
            {p::max_compute_work_group_invocations,
             "max_compute_work_group_invocations"},
#endif
#if defined(GL_MAX_COMPUTE_WORK_GROUP_SIZE)
            {p::max_compute_work_group_size, "max_compute_work_group_size"},
#endif
#if defined(GL_MAX_CUBE_MAP_TEXTURE_SIZE)
            {p::max_cube_map_texture_size, "max_cube_map_texture_size"},
#endif
#if defined(GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB)
            {p::max_cube_map_texture_size_arb, "max_cube_map_texture_size_arb"},
#endif
#if defined(GL_MAX_CUBE_MAP_TEXTURE_SIZE_EXT)
            {p::max_cube_map_texture_size_ext, "max_cube_map_texture_size_ext"},
#endif
#if defined(GL_MAX_CUBE_MAP_TEXTURE_SIZE_OES)
            {p::max_cube_map_texture_size_oes, "max_cube_map_texture_size_oes"},
#endif
#if defined(GL_MAX_DEBUG_GROUP_STACK_DEPTH)
            {p::max_debug_group_stack_depth, "max_debug_group_stack_depth"},
#endif
#if defined(GL_MAX_DEPTH_TEXTURE_SAMPLES)
            {p::max_depth_texture_samples, "max_depth_texture_samples"},
#endif
#if defined(GL_MAX_DRAW_BUFFERS)
            {p::max_draw_buffers, "max_draw_buffers"},
#endif
#if defined(GL_MAX_DUAL_SOURCE_DRAW_BUFFERS)
            {p::max_dual_source_draw_buffers, "max_dual_source_draw_buffers"},
#endif
#if defined(GL_MAX_ELEMENTS_INDICES)
            {p::max_elements_indices, "max_elements_indices"},
#endif
#if defined(GL_MAX_ELEMENTS_VERTICES)
            {p::max_elements_vertices, "max_elements_vertices"},
#endif
#if defined(GL_MAX_ELEMENT_INDEX)
            {p::max_element_index, "max_element_index"},
#endif
#if defined(GL_MAX_EVAL_ORDER)
            {p::max_eval_order, "max_eval_order"},
#endif
#if defined(GL_MAX_FOG_FUNC_POINTS_SGIS)
            {p::max_fog_func_points_sgis, "max_fog_func_points_sgis"},
#endif
#if defined(GL_MAX_FRAGMENT_ATOMIC_COUNTERS)
            {p::max_fragment_atomic_counters, "max_fragment_atomic_counters"},
#endif
#if defined(GL_MAX_FRAGMENT_INPUT_COMPONENTS)
            {p::max_fragment_input_components, "max_fragment_input_components"},
#endif
#if defined(GL_MAX_FRAGMENT_LIGHTS_SGIX)
            {p::max_fragment_lights_sgix, "max_fragment_lights_sgix"},
#endif
#if defined(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS)
            {p::max_fragment_shader_storage_blocks,
             "max_fragment_shader_storage_blocks"},
#endif
#if defined(GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_LAYERS_EXT)
            {p::max_fragment_shading_rate_attachment_layers_ext,
             "max_fragment_shading_rate_attachment_layers_ext"},
#endif
#if defined(GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_ASPECT_RATIO_EXT)
            {p::max_fragment_shading_rate_attachment_texel_aspect_ratio_ext,
             "max_fragment_shading_rate_attachment_texel_aspect_ratio_ext"},
#endif
#if defined(GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_HEIGHT_EXT)
            {p::max_fragment_shading_rate_attachment_texel_height_ext,
             "max_fragment_shading_rate_attachment_texel_height_ext"},
#endif
#if defined(GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_WIDTH_EXT)
            {p::max_fragment_shading_rate_attachment_texel_width_ext,
             "max_fragment_shading_rate_attachment_texel_width_ext"},
#endif
#if defined(GL_MAX_FRAGMENT_UNIFORM_BLOCKS)
            {p::max_fragment_uniform_blocks, "max_fragment_uniform_blocks"},
#endif
#if defined(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS)
            {p::max_fragment_uniform_components,
             "max_fragment_uniform_components"},
#endif
#if defined(GL_MAX_FRAGMENT_UNIFORM_VECTORS)
            {p::max_fragment_uniform_vectors, "max_fragment_uniform_vectors"},
#endif
#if defined(GL_MAX_FRAMEBUFFER_HEIGHT)
            {p::max_framebuffer_height, "max_framebuffer_height"},
#endif
#if defined(GL_MAX_FRAMEBUFFER_LAYERS)
            {p::max_framebuffer_layers, "max_framebuffer_layers"},
#endif
#if defined(GL_MAX_FRAMEBUFFER_SAMPLES)
            {p::max_framebuffer_samples, "max_framebuffer_samples"},
#endif
#if defined(GL_MAX_FRAMEBUFFER_WIDTH)
            {p::max_framebuffer_width, "max_framebuffer_width"},
#endif
#if defined(GL_MAX_FRAMEZOOM_FACTOR_SGIX)
            {p::max_framezoom_factor_sgix, "max_framezoom_factor_sgix"},
#endif
#if defined(GL_MAX_GEOMETRY_ATOMIC_COUNTERS)
            {p::max_geometry_atomic_counters, "max_geometry_atomic_counters"},
#endif
#if defined(GL_MAX_GEOMETRY_INPUT_COMPONENTS)
            {p::max_geometry_input_components, "max_geometry_input_components"},
#endif
#if defined(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS)
            {p::max_geometry_output_components,
             "max_geometry_output_components"},
#endif
#if defined(GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS)
            {p::max_geometry_shader_storage_blocks,
             "max_geometry_shader_storage_blocks"},
#endif
#if defined(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS)
            {p::max_geometry_texture_image_units,
             "max_geometry_texture_image_units"},
#endif
#if defined(GL_MAX_GEOMETRY_UNIFORM_BLOCKS)
            {p::max_geometry_uniform_blocks, "max_geometry_uniform_blocks"},
#endif
#if defined(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS)
            {p::max_geometry_uniform_components,
             "max_geometry_uniform_components"},
#endif
#if defined(GL_MAX_INTEGER_SAMPLES)
            {p::max_integer_samples, "max_integer_samples"},
#endif
#if defined(GL_MAX_LABEL_LENGTH)
            {p::max_label_length, "max_label_length"},
#endif
#if defined(GL_MAX_LIGHTS)
            {p::max_lights, "max_lights"},
#endif
#if defined(GL_MAX_LIST_NESTING)
            {p::max_list_nesting, "max_list_nesting"},
#endif
#if defined(GL_MAX_MODELVIEW_STACK_DEPTH)
            {p::max_modelview_stack_depth, "max_modelview_stack_depth"},
#endif
#if defined(GL_MAX_NAME_STACK_DEPTH)
            {p::max_name_stack_depth, "max_name_stack_depth"},
#endif
#if defined(GL_MAX_PIXEL_MAP_TABLE)
            {p::max_pixel_map_table, "max_pixel_map_table"},
#endif
#if defined(GL_MAX_PROGRAM_TEXEL_OFFSET)
            {p::max_program_texel_offset, "max_program_texel_offset"},
#endif
#if defined(GL_MAX_PROJECTION_STACK_DEPTH)
            {p::max_projection_stack_depth, "max_projection_stack_depth"},
#endif
#if defined(GL_MAX_RECTANGLE_TEXTURE_SIZE)
            {p::max_rectangle_texture_size, "max_rectangle_texture_size"},
#endif
#if defined(GL_MAX_RENDERBUFFER_SIZE)
            {p::max_renderbuffer_size, "max_renderbuffer_size"},
#endif
#if defined(GL_MAX_SAMPLE_MASK_WORDS)
            {p::max_sample_mask_words, "max_sample_mask_words"},
#endif
#if defined(GL_MAX_SERVER_WAIT_TIMEOUT)
            {p::max_server_wait_timeout, "max_server_wait_timeout"},
#endif
#if defined(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS)
            {p::max_shader_storage_buffer_bindings,
             "max_shader_storage_buffer_bindings"},
#endif
#if defined(GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS)
            {p::max_tess_control_atomic_counters,
             "max_tess_control_atomic_counters"},
#endif
#if defined(GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS)
            {p::max_tess_control_shader_storage_blocks,
             "max_tess_control_shader_storage_blocks"},
#endif
#if defined(GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS)
            {p::max_tess_control_uniform_blocks,
             "max_tess_control_uniform_blocks"},
#endif
#if defined(GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS)
            {p::max_tess_evaluation_atomic_counters,
             "max_tess_evaluation_atomic_counters"},
#endif
#if defined(GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS)
            {p::max_tess_evaluation_shader_storage_blocks,
             "max_tess_evaluation_shader_storage_blocks"},
#endif
#if defined(GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS)
            {p::max_tess_evaluation_uniform_blocks,
             "max_tess_evaluation_uniform_blocks"},
#endif
#if defined(GL_MAX_TEXTURE_BUFFER_SIZE)
            {p::max_texture_buffer_size, "max_texture_buffer_size"},
#endif
#if defined(GL_MAX_TEXTURE_IMAGE_UNITS)
            {p::max_texture_image_units, "max_texture_image_units"},
#endif
#if defined(GL_MAX_TEXTURE_LOD_BIAS)
            {p::max_texture_lod_bias, "max_texture_lod_bias"},
#endif
#if defined(GL_MAX_TEXTURE_LOD_BIAS_EXT)
            {p::max_texture_lod_bias_ext, "max_texture_lod_bias_ext"},
#endif
#if defined(GL_MAX_TEXTURE_MAX_ANISOTROPY)
            {p::max_texture_max_anisotropy, "max_texture_max_anisotropy"},
#endif
#if defined(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT)
            {p::max_texture_max_anisotropy_ext,
             "max_texture_max_anisotropy_ext"},
#endif
#if defined(GL_MAX_TEXTURE_SIZE)
            {p::max_texture_size, "max_texture_size"},
#endif
#if defined(GL_MAX_TEXTURE_STACK_DEPTH)
            {p::max_texture_stack_depth, "max_texture_stack_depth"},
#endif
#if defined(GL_MAX_TIMELINE_SEMAPHORE_VALUE_DIFFERENCE_NV)
            {p::max_timeline_semaphore_value_difference_nv,
             "max_timeline_semaphore_value_difference_nv"},
#endif
#if defined(GL_MAX_UNIFORM_BLOCK_SIZE)
            {p::max_uniform_block_size, "max_uniform_block_size"},
#endif
#if defined(GL_MAX_UNIFORM_BUFFER_BINDINGS)
            {p::max_uniform_buffer_bindings, "max_uniform_buffer_bindings"},
#endif
#if defined(GL_MAX_UNIFORM_LOCATIONS)
            {p::max_uniform_locations, "max_uniform_locations"},
#endif
#if defined(GL_MAX_VARYING_COMPONENTS)
            {p::max_varying_components, "max_varying_components"},
#endif
#if defined(GL_MAX_VARYING_FLOATS)
            {p::max_varying_floats, "max_varying_floats"},
#endif
#if defined(GL_MAX_VARYING_VECTORS)
            {p::max_varying_vectors, "max_varying_vectors"},
#endif
#if defined(GL_MAX_VERTEX_ATOMIC_COUNTERS)
            {p::max_vertex_atomic_counters, "max_vertex_atomic_counters"},
#endif
#if defined(GL_MAX_VERTEX_ATTRIBS)
            {p::max_vertex_attribs, "max_vertex_attribs"},
#endif
#if defined(GL_MAX_VERTEX_ATTRIB_BINDINGS)
            {p::max_vertex_attrib_bindings, "max_vertex_attrib_bindings"},
#endif
#if defined(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET)
            {p::max_vertex_attrib_relative_offset,
             "max_vertex_attrib_relative_offset"},
#endif
#if defined(GL_MAX_VERTEX_OUTPUT_COMPONENTS)
            {p::max_vertex_output_components, "max_vertex_output_components"},
#endif
#if defined(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS)
            {p::max_vertex_shader_storage_blocks,
             "max_vertex_shader_storage_blocks"},
#endif
#if defined(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS)
            {p::max_vertex_texture_image_units,
             "max_vertex_texture_image_units"},
#endif
#if defined(GL_MAX_VERTEX_UNIFORM_BLOCKS)
            {p::max_vertex_uniform_blocks, "max_vertex_uniform_blocks"},
#endif
#if defined(GL_MAX_VERTEX_UNIFORM_COMPONENTS)
            {p::max_vertex_uniform_components, "max_vertex_uniform_components"},
#endif
#if defined(GL_MAX_VERTEX_UNIFORM_VECTORS)
            {p::max_vertex_uniform_vectors, "max_vertex_uniform_vectors"},
#endif
#if defined(GL_MAX_VIEWPORTS)
            {p::max_viewports, "max_viewports"},
#endif
#if defined(GL_MAX_VIEWPORT_DIMS)
            {p::max_viewport_dims, "max_viewport_dims"},
#endif
#if defined(GL_MIN_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_HEIGHT_EXT)
            {p::min_fragment_shading_rate_attachment_texel_height_ext,
             "min_fragment_shading_rate_attachment_texel_height_ext"},
#endif
#if defined(GL_MIN_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_WIDTH_EXT)
            {p::min_fragment_shading_rate_attachment_texel_width_ext,
             "min_fragment_shading_rate_attachment_texel_width_ext"},
#endif
#if defined(GL_MIN_MAP_BUFFER_ALIGNMENT)
            {p::min_map_buffer_alignment, "min_map_buffer_alignment"},
#endif
#if defined(GL_MIN_PROGRAM_TEXEL_OFFSET)
            {p::min_program_texel_offset, "min_program_texel_offset"},
#endif
    };
}

} // namespace gl::limits
