#pragma once

#include "common.h"

namespace gl::group {

// GetPName
enum class get_prop : u32
{
#ifdef GL_ACCUM_ALPHA_BITS
    accum_alpha_bits = GL_ACCUM_ALPHA_BITS,
#endif
#ifdef GL_ACCUM_BLUE_BITS
    accum_blue_bits = GL_ACCUM_BLUE_BITS,
#endif
#ifdef GL_ACCUM_CLEAR_VALUE
    accum_clear_value = GL_ACCUM_CLEAR_VALUE,
#endif
#ifdef GL_ACCUM_GREEN_BITS
    accum_green_bits = GL_ACCUM_GREEN_BITS,
#endif
#ifdef GL_ACCUM_RED_BITS
    accum_red_bits = GL_ACCUM_RED_BITS,
#endif
#ifdef GL_ACTIVE_TEXTURE
    active_texture = GL_ACTIVE_TEXTURE,
#endif
#ifdef GL_ALIASED_LINE_WIDTH_RANGE
    aliased_line_width_range = GL_ALIASED_LINE_WIDTH_RANGE,
#endif
#ifdef GL_ALIASED_POINT_SIZE_RANGE
    aliased_point_size_range = GL_ALIASED_POINT_SIZE_RANGE,
#endif
#ifdef GL_ALPHA_BIAS
    alpha_bias = GL_ALPHA_BIAS,
#endif
#ifdef GL_ALPHA_BITS
    alpha_bits = GL_ALPHA_BITS,
#endif
#ifdef GL_ALPHA_SCALE
    alpha_scale = GL_ALPHA_SCALE,
#endif
#ifdef GL_ALPHA_TEST
    alpha_test = GL_ALPHA_TEST,
#endif
#ifdef GL_ALPHA_TEST_FUNC
    alpha_test_func = GL_ALPHA_TEST_FUNC,
#endif
#ifdef GL_ALPHA_TEST_FUNC_QCOM
    alpha_test_func_qcom = GL_ALPHA_TEST_FUNC_QCOM,
#endif
#ifdef GL_ALPHA_TEST_QCOM
    alpha_test_qcom = GL_ALPHA_TEST_QCOM,
#endif
#ifdef GL_ALPHA_TEST_REF
    alpha_test_ref = GL_ALPHA_TEST_REF,
#endif
#ifdef GL_ALPHA_TEST_REF_QCOM
    alpha_test_ref_qcom = GL_ALPHA_TEST_REF_QCOM,
#endif
#ifdef GL_ARRAY_BUFFER_BINDING
    array_buffer_binding = GL_ARRAY_BUFFER_BINDING,
#endif
#ifdef GL_ASYNC_DRAW_PIXELS_SGIX
    async_draw_pixels_sgix = GL_ASYNC_DRAW_PIXELS_SGIX,
#endif
#ifdef GL_ASYNC_HISTOGRAM_SGIX
    async_histogram_sgix = GL_ASYNC_HISTOGRAM_SGIX,
#endif
#ifdef GL_ASYNC_MARKER_SGIX
    async_marker_sgix = GL_ASYNC_MARKER_SGIX,
#endif
#ifdef GL_ASYNC_READ_PIXELS_SGIX
    async_read_pixels_sgix = GL_ASYNC_READ_PIXELS_SGIX,
#endif
#ifdef GL_ASYNC_TEX_IMAGE_SGIX
    async_tex_image_sgix = GL_ASYNC_TEX_IMAGE_SGIX,
#endif
#ifdef GL_ATTRIB_STACK_DEPTH
    attrib_stack_depth = GL_ATTRIB_STACK_DEPTH,
#endif
#ifdef GL_AUTO_NORMAL
    auto_normal = GL_AUTO_NORMAL,
#endif
#ifdef GL_AUX_BUFFERS
    aux_buffers = GL_AUX_BUFFERS,
#endif
#ifdef GL_BLEND
    blend = GL_BLEND,
#endif
#ifdef GL_BLEND_COLOR
    blend_color = GL_BLEND_COLOR,
#endif
#ifdef GL_BLEND_COLOR_EXT
    blend_color_ext = GL_BLEND_COLOR_EXT,
#endif
#ifdef GL_BLEND_DST
    blend_dst = GL_BLEND_DST,
#endif
#ifdef GL_BLEND_DST_ALPHA
    blend_dst_alpha = GL_BLEND_DST_ALPHA,
#endif
#ifdef GL_BLEND_DST_RGB
    blend_dst_rgb = GL_BLEND_DST_RGB,
#endif
#ifdef GL_BLEND_EQUATION
    blend_equation = GL_BLEND_EQUATION,
#endif
#ifdef GL_BLEND_EQUATION_ALPHA
    blend_equation_alpha = GL_BLEND_EQUATION_ALPHA,
#endif
#ifdef GL_BLEND_EQUATION_EXT
    blend_equation_ext = GL_BLEND_EQUATION_EXT,
#endif
#ifdef GL_BLEND_EQUATION_OES
    blend_equation_oes = GL_BLEND_EQUATION_OES,
#endif
#ifdef GL_BLEND_EQUATION_RGB
    blend_equation_rgb = GL_BLEND_EQUATION_RGB,
#endif
#ifdef GL_BLEND_SRC
    blend_src = GL_BLEND_SRC,
#endif
#ifdef GL_BLEND_SRC_ALPHA
    blend_src_alpha = GL_BLEND_SRC_ALPHA,
#endif
#ifdef GL_BLEND_SRC_RGB
    blend_src_rgb = GL_BLEND_SRC_RGB,
#endif
#ifdef GL_BLUE_BIAS
    blue_bias = GL_BLUE_BIAS,
#endif
#ifdef GL_BLUE_BITS
    blue_bits = GL_BLUE_BITS,
#endif
#ifdef GL_BLUE_SCALE
    blue_scale = GL_BLUE_SCALE,
#endif
#ifdef GL_CALLIGRAPHIC_FRAGMENT_SGIX
    calligraphic_fragment_sgix = GL_CALLIGRAPHIC_FRAGMENT_SGIX,
#endif
#ifdef GL_CLIENT_ATTRIB_STACK_DEPTH
    client_attrib_stack_depth = GL_CLIENT_ATTRIB_STACK_DEPTH,
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
#ifdef GL_COLOR_ARRAY_COUNT_EXT
    color_array_count_ext = GL_COLOR_ARRAY_COUNT_EXT,
#endif
#ifdef GL_COLOR_ARRAY_SIZE
    color_array_size = GL_COLOR_ARRAY_SIZE,
#endif
#ifdef GL_COLOR_ARRAY_STRIDE
    color_array_stride = GL_COLOR_ARRAY_STRIDE,
#endif
#ifdef GL_COLOR_ARRAY_TYPE
    color_array_type = GL_COLOR_ARRAY_TYPE,
#endif
#ifdef GL_COLOR_CLEAR_VALUE
    color_clear_value = GL_COLOR_CLEAR_VALUE,
#endif
#ifdef GL_COLOR_LOGIC_OP
    color_logic_op = GL_COLOR_LOGIC_OP,
#endif
#ifdef GL_COLOR_MATERIAL
    color_material = GL_COLOR_MATERIAL,
#endif
#ifdef GL_COLOR_MATERIAL_FACE
    color_material_face = GL_COLOR_MATERIAL_FACE,
#endif
#ifdef GL_COLOR_MATERIAL_PARAMETER
    color_material_parameter = GL_COLOR_MATERIAL_PARAMETER,
#endif
#ifdef GL_COLOR_MATRIX_SGI
    color_matrix_sgi = GL_COLOR_MATRIX_SGI,
#endif
#ifdef GL_COLOR_MATRIX_STACK_DEPTH_SGI
    color_matrix_stack_depth_sgi = GL_COLOR_MATRIX_STACK_DEPTH_SGI,
#endif
#ifdef GL_COLOR_TABLE_SGI
    color_table_sgi = GL_COLOR_TABLE_SGI,
#endif
#ifdef GL_COLOR_WRITEMASK
    color_writemask = GL_COLOR_WRITEMASK,
#endif
#ifdef GL_COMPRESSED_TEXTURE_FORMATS
    compressed_texture_formats = GL_COMPRESSED_TEXTURE_FORMATS,
#endif
#ifdef GL_CONTEXT_FLAGS
    context_flags = GL_CONTEXT_FLAGS,
#endif
#ifdef GL_CONTEXT_PROFILE_MASK
    context_profile_mask = GL_CONTEXT_PROFILE_MASK,
#endif
#ifdef GL_CONVOLUTION_1D_EXT
    convolution_1d_ext = GL_CONVOLUTION_1D_EXT,
#endif
#ifdef GL_CONVOLUTION_2D_EXT
    convolution_2d_ext = GL_CONVOLUTION_2D_EXT,
#endif
#ifdef GL_CONVOLUTION_HINT_SGIX
    convolution_hint_sgix = GL_CONVOLUTION_HINT_SGIX,
#endif
#ifdef GL_CULL_FACE
    cull_face = GL_CULL_FACE,
#endif
#ifdef GL_CULL_FACE_MODE
    cull_face_mode = GL_CULL_FACE_MODE,
#endif
#ifdef GL_CURRENT_COLOR
    current_color = GL_CURRENT_COLOR,
#endif
#ifdef GL_CURRENT_INDEX
    current_index = GL_CURRENT_INDEX,
#endif
#ifdef GL_CURRENT_NORMAL
    current_normal = GL_CURRENT_NORMAL,
#endif
#ifdef GL_CURRENT_PROGRAM
    current_program = GL_CURRENT_PROGRAM,
#endif
#ifdef GL_CURRENT_RASTER_COLOR
    current_raster_color = GL_CURRENT_RASTER_COLOR,
#endif
#ifdef GL_CURRENT_RASTER_DISTANCE
    current_raster_distance = GL_CURRENT_RASTER_DISTANCE,
#endif
#ifdef GL_CURRENT_RASTER_INDEX
    current_raster_index = GL_CURRENT_RASTER_INDEX,
#endif
#ifdef GL_CURRENT_RASTER_POSITION
    current_raster_position = GL_CURRENT_RASTER_POSITION,
#endif
#ifdef GL_CURRENT_RASTER_POSITION_VALID
    current_raster_position_valid = GL_CURRENT_RASTER_POSITION_VALID,
#endif
#ifdef GL_CURRENT_RASTER_TEXTURE_COORDS
    current_raster_texture_coords = GL_CURRENT_RASTER_TEXTURE_COORDS,
#endif
#ifdef GL_CURRENT_TEXTURE_COORDS
    current_texture_coords = GL_CURRENT_TEXTURE_COORDS,
#endif
#ifdef GL_DEBUG_GROUP_STACK_DEPTH
    debug_group_stack_depth = GL_DEBUG_GROUP_STACK_DEPTH,
#endif
#ifdef GL_DEFORMATIONS_MASK_SGIX
    deformations_mask_sgix = GL_DEFORMATIONS_MASK_SGIX,
#endif
#ifdef GL_DEPTH_BIAS
    depth_bias = GL_DEPTH_BIAS,
#endif
#ifdef GL_DEPTH_BITS
    depth_bits = GL_DEPTH_BITS,
#endif
#ifdef GL_DEPTH_CLEAR_VALUE
    depth_clear_value = GL_DEPTH_CLEAR_VALUE,
#endif
#ifdef GL_DEPTH_FUNC
    depth_func = GL_DEPTH_FUNC,
#endif
#ifdef GL_DEPTH_RANGE
    depth_range = GL_DEPTH_RANGE,
#endif
#ifdef GL_DEPTH_SCALE
    depth_scale = GL_DEPTH_SCALE,
#endif
#ifdef GL_DEPTH_TEST
    depth_test = GL_DEPTH_TEST,
#endif
#ifdef GL_DEPTH_WRITEMASK
    depth_writemask = GL_DEPTH_WRITEMASK,
#endif
#ifdef GL_DETAIL_TEXTURE_2D_BINDING_SGIS
    detail_texture_2d_binding_sgis = GL_DETAIL_TEXTURE_2D_BINDING_SGIS,
#endif
#ifdef GL_DEVICE_LUID_EXT
    device_luid_ext = GL_DEVICE_LUID_EXT,
#endif
#ifdef GL_DEVICE_NODE_MASK_EXT
    device_node_mask_ext = GL_DEVICE_NODE_MASK_EXT,
#endif
#ifdef GL_DEVICE_UUID_EXT
    device_uuid_ext = GL_DEVICE_UUID_EXT,
#endif
#ifdef GL_DISPATCH_INDIRECT_BUFFER_BINDING
    dispatch_indirect_buffer_binding = GL_DISPATCH_INDIRECT_BUFFER_BINDING,
#endif
#ifdef GL_DISTANCE_ATTENUATION_EXT
    distance_attenuation_ext = GL_DISTANCE_ATTENUATION_EXT,
#endif
#ifdef GL_DISTANCE_ATTENUATION_SGIS
    distance_attenuation_sgis = GL_DISTANCE_ATTENUATION_SGIS,
#endif
#ifdef GL_DITHER
    dither = GL_DITHER,
#endif
#ifdef GL_DOUBLEBUFFER
    doublebuffer = GL_DOUBLEBUFFER,
#endif
#ifdef GL_DRAW_BUFFER
    draw_buffer = GL_DRAW_BUFFER,
#endif
#ifdef GL_DRAW_BUFFER_EXT
    draw_buffer_ext = GL_DRAW_BUFFER_EXT,
#endif
#ifdef GL_DRAW_FRAMEBUFFER_BINDING
    draw_framebuffer_binding = GL_DRAW_FRAMEBUFFER_BINDING,
#endif
#ifdef GL_DRIVER_UUID_EXT
    driver_uuid_ext = GL_DRIVER_UUID_EXT,
#endif
#ifdef GL_EDGE_FLAG
    edge_flag = GL_EDGE_FLAG,
#endif
#ifdef GL_EDGE_FLAG_ARRAY
    edge_flag_array = GL_EDGE_FLAG_ARRAY,
#endif
#ifdef GL_EDGE_FLAG_ARRAY_COUNT_EXT
    edge_flag_array_count_ext = GL_EDGE_FLAG_ARRAY_COUNT_EXT,
#endif
#ifdef GL_EDGE_FLAG_ARRAY_STRIDE
    edge_flag_array_stride = GL_EDGE_FLAG_ARRAY_STRIDE,
#endif
#ifdef GL_ELEMENT_ARRAY_BUFFER_BINDING
    element_array_buffer_binding = GL_ELEMENT_ARRAY_BUFFER_BINDING,
#endif
#ifdef GL_FEEDBACK_BUFFER_SIZE
    feedback_buffer_size = GL_FEEDBACK_BUFFER_SIZE,
#endif
#ifdef GL_FEEDBACK_BUFFER_TYPE
    feedback_buffer_type = GL_FEEDBACK_BUFFER_TYPE,
#endif
#ifdef GL_FETCH_PER_SAMPLE_ARM
    fetch_per_sample_arm = GL_FETCH_PER_SAMPLE_ARM,
#endif
#ifdef GL_FOG
    fog = GL_FOG,
#endif
#ifdef GL_FOG_COLOR
    fog_color = GL_FOG_COLOR,
#endif
#ifdef GL_FOG_DENSITY
    fog_density = GL_FOG_DENSITY,
#endif
#ifdef GL_FOG_END
    fog_end = GL_FOG_END,
#endif
#ifdef GL_FOG_FUNC_POINTS_SGIS
    fog_func_points_sgis = GL_FOG_FUNC_POINTS_SGIS,
#endif
#ifdef GL_FOG_HINT
    fog_hint = GL_FOG_HINT,
#endif
#ifdef GL_FOG_INDEX
    fog_index = GL_FOG_INDEX,
#endif
#ifdef GL_FOG_MODE
    fog_mode = GL_FOG_MODE,
#endif
#ifdef GL_FOG_OFFSET_SGIX
    fog_offset_sgix = GL_FOG_OFFSET_SGIX,
#endif
#ifdef GL_FOG_OFFSET_VALUE_SGIX
    fog_offset_value_sgix = GL_FOG_OFFSET_VALUE_SGIX,
#endif
#ifdef GL_FOG_START
    fog_start = GL_FOG_START,
#endif
#ifdef GL_FRAGMENT_COLOR_MATERIAL_FACE_SGIX
    fragment_color_material_face_sgix = GL_FRAGMENT_COLOR_MATERIAL_FACE_SGIX,
#endif
#ifdef GL_FRAGMENT_COLOR_MATERIAL_PARAMETER_SGIX
    fragment_color_material_parameter_sgix =
        GL_FRAGMENT_COLOR_MATERIAL_PARAMETER_SGIX,
#endif
#ifdef GL_FRAGMENT_COLOR_MATERIAL_SGIX
    fragment_color_material_sgix = GL_FRAGMENT_COLOR_MATERIAL_SGIX,
#endif
#ifdef GL_FRAGMENT_LIGHT0_SGIX
    fragment_light0_sgix = GL_FRAGMENT_LIGHT0_SGIX,
#endif
#ifdef GL_FRAGMENT_LIGHTING_SGIX
    fragment_lighting_sgix = GL_FRAGMENT_LIGHTING_SGIX,
#endif
#ifdef GL_FRAGMENT_LIGHT_MODEL_AMBIENT_SGIX
    fragment_light_model_ambient_sgix = GL_FRAGMENT_LIGHT_MODEL_AMBIENT_SGIX,
#endif
#ifdef GL_FRAGMENT_LIGHT_MODEL_LOCAL_VIEWER_SGIX
    fragment_light_model_local_viewer_sgix =
        GL_FRAGMENT_LIGHT_MODEL_LOCAL_VIEWER_SGIX,
#endif
#ifdef GL_FRAGMENT_LIGHT_MODEL_NORMAL_INTERPOLATION_SGIX
    fragment_light_model_normal_interpolation_sgix =
        GL_FRAGMENT_LIGHT_MODEL_NORMAL_INTERPOLATION_SGIX,
#endif
#ifdef GL_FRAGMENT_LIGHT_MODEL_TWO_SIDE_SGIX
    fragment_light_model_two_side_sgix = GL_FRAGMENT_LIGHT_MODEL_TWO_SIDE_SGIX,
#endif
#ifdef GL_FRAGMENT_SHADER_ATI
    fragment_shader_ati = GL_FRAGMENT_SHADER_ATI,
#endif
#ifdef GL_FRAGMENT_SHADER_DERIVATIVE_HINT
    fragment_shader_derivative_hint = GL_FRAGMENT_SHADER_DERIVATIVE_HINT,
#endif
#ifdef GL_FRAGMENT_SHADER_FRAMEBUFFER_FETCH_MRT_ARM
    fragment_shader_framebuffer_fetch_mrt_arm =
        GL_FRAGMENT_SHADER_FRAMEBUFFER_FETCH_MRT_ARM,
#endif
#ifdef GL_FRAGMENT_SHADING_RATE_ATTACHMENT_WITH_DEFAULT_FRAMEBUFFER_SUPPORTED_EXT
    fragment_shading_rate_attachment_with_default_framebuffer_supported_ext =
        GL_FRAGMENT_SHADING_RATE_ATTACHMENT_WITH_DEFAULT_FRAMEBUFFER_SUPPORTED_EXT,
#endif
#ifdef GL_FRAGMENT_SHADING_RATE_NON_TRIVIAL_COMBINERS_SUPPORTED_EXT
    fragment_shading_rate_non_trivial_combiners_supported_ext =
        GL_FRAGMENT_SHADING_RATE_NON_TRIVIAL_COMBINERS_SUPPORTED_EXT,
#endif
#ifdef GL_FRAGMENT_SHADING_RATE_WITH_SAMPLE_MASK_SUPPORTED_EXT
    fragment_shading_rate_with_sample_mask_supported_ext =
        GL_FRAGMENT_SHADING_RATE_WITH_SAMPLE_MASK_SUPPORTED_EXT,
#endif
#ifdef GL_FRAGMENT_SHADING_RATE_WITH_SHADER_DEPTH_STENCIL_WRITES_SUPPORTED_EXT
    fragment_shading_rate_with_shader_depth_stencil_writes_supported_ext =
        GL_FRAGMENT_SHADING_RATE_WITH_SHADER_DEPTH_STENCIL_WRITES_SUPPORTED_EXT,
#endif
#ifdef GL_FRAMEBUFFER_BINDING
    framebuffer_binding = GL_FRAMEBUFFER_BINDING,
#endif
#ifdef GL_FRAMEBUFFER_FETCH_NONCOHERENT_QCOM
    framebuffer_fetch_noncoherent_qcom = GL_FRAMEBUFFER_FETCH_NONCOHERENT_QCOM,
#endif
#ifdef GL_FRAMEZOOM_FACTOR_SGIX
    framezoom_factor_sgix = GL_FRAMEZOOM_FACTOR_SGIX,
#endif
#ifdef GL_FRAMEZOOM_SGIX
    framezoom_sgix = GL_FRAMEZOOM_SGIX,
#endif
#ifdef GL_FRONT_FACE
    front_face = GL_FRONT_FACE,
#endif
#ifdef GL_GENERATE_MIPMAP_HINT_SGIS
    generate_mipmap_hint_sgis = GL_GENERATE_MIPMAP_HINT_SGIS,
#endif
#ifdef GL_GREEN_BIAS
    green_bias = GL_GREEN_BIAS,
#endif
#ifdef GL_GREEN_BITS
    green_bits = GL_GREEN_BITS,
#endif
#ifdef GL_GREEN_SCALE
    green_scale = GL_GREEN_SCALE,
#endif
#ifdef GL_HISTOGRAM_EXT
    histogram_ext = GL_HISTOGRAM_EXT,
#endif
#ifdef GL_IMPLEMENTATION_COLOR_READ_FORMAT
    implementation_color_read_format = GL_IMPLEMENTATION_COLOR_READ_FORMAT,
#endif
#ifdef GL_IMPLEMENTATION_COLOR_READ_TYPE
    implementation_color_read_type = GL_IMPLEMENTATION_COLOR_READ_TYPE,
#endif
#ifdef GL_INDEX_ARRAY
    index_array = GL_INDEX_ARRAY,
#endif
#ifdef GL_INDEX_ARRAY_COUNT_EXT
    index_array_count_ext = GL_INDEX_ARRAY_COUNT_EXT,
#endif
#ifdef GL_INDEX_ARRAY_STRIDE
    index_array_stride = GL_INDEX_ARRAY_STRIDE,
#endif
#ifdef GL_INDEX_ARRAY_TYPE
    index_array_type = GL_INDEX_ARRAY_TYPE,
#endif
#ifdef GL_INDEX_BITS
    index_bits = GL_INDEX_BITS,
#endif
#ifdef GL_INDEX_CLEAR_VALUE
    index_clear_value = GL_INDEX_CLEAR_VALUE,
#endif
#ifdef GL_INDEX_LOGIC_OP
    index_logic_op = GL_INDEX_LOGIC_OP,
#endif
#ifdef GL_INDEX_MODE
    index_mode = GL_INDEX_MODE,
#endif
#ifdef GL_INDEX_OFFSET
    index_offset = GL_INDEX_OFFSET,
#endif
#ifdef GL_INDEX_SHIFT
    index_shift = GL_INDEX_SHIFT,
#endif
#ifdef GL_INDEX_WRITEMASK
    index_writemask = GL_INDEX_WRITEMASK,
#endif
#ifdef GL_INSTRUMENT_MEASUREMENTS_SGIX
    instrument_measurements_sgix = GL_INSTRUMENT_MEASUREMENTS_SGIX,
#endif
#ifdef GL_INTERLACE_SGIX
    interlace_sgix = GL_INTERLACE_SGIX,
#endif
#ifdef GL_IR_INSTRUMENT1_SGIX
    ir_instrument1_sgix = GL_IR_INSTRUMENT1_SGIX,
#endif
#ifdef GL_LAYER_PROVOKING_VERTEX
    layer_provoking_vertex = GL_LAYER_PROVOKING_VERTEX,
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
#ifdef GL_LIGHT_ENV_MODE_SGIX
    light_env_mode_sgix = GL_LIGHT_ENV_MODE_SGIX,
#endif
#ifdef GL_LIGHT_MODEL_AMBIENT
    light_model_ambient = GL_LIGHT_MODEL_AMBIENT,
#endif
#ifdef GL_LIGHT_MODEL_COLOR_CONTROL
    light_model_color_control = GL_LIGHT_MODEL_COLOR_CONTROL,
#endif
#ifdef GL_LIGHT_MODEL_LOCAL_VIEWER
    light_model_local_viewer = GL_LIGHT_MODEL_LOCAL_VIEWER,
#endif
#ifdef GL_LIGHT_MODEL_TWO_SIDE
    light_model_two_side = GL_LIGHT_MODEL_TWO_SIDE,
#endif
#ifdef GL_LINE_SMOOTH
    line_smooth = GL_LINE_SMOOTH,
#endif
#ifdef GL_LINE_SMOOTH_HINT
    line_smooth_hint = GL_LINE_SMOOTH_HINT,
#endif
#ifdef GL_LINE_STIPPLE
    line_stipple = GL_LINE_STIPPLE,
#endif
#ifdef GL_LINE_STIPPLE_PATTERN
    line_stipple_pattern = GL_LINE_STIPPLE_PATTERN,
#endif
#ifdef GL_LINE_STIPPLE_REPEAT
    line_stipple_repeat = GL_LINE_STIPPLE_REPEAT,
#endif
#ifdef GL_LINE_WIDTH
    line_width = GL_LINE_WIDTH,
#endif
#ifdef GL_LINE_WIDTH_GRANULARITY
    line_width_granularity = GL_LINE_WIDTH_GRANULARITY,
#endif
#ifdef GL_LINE_WIDTH_RANGE
    line_width_range = GL_LINE_WIDTH_RANGE,
#endif
#ifdef GL_LIST_BASE
    list_base = GL_LIST_BASE,
#endif
#ifdef GL_LIST_INDEX
    list_index = GL_LIST_INDEX,
#endif
#ifdef GL_LIST_MODE
    list_mode = GL_LIST_MODE,
#endif
#ifdef GL_LOGIC_OP
    logic_op = GL_LOGIC_OP,
#endif
#ifdef GL_LOGIC_OP_MODE
    logic_op_mode = GL_LOGIC_OP_MODE,
#endif
#ifdef GL_MAJOR_VERSION
    major_version = GL_MAJOR_VERSION,
#endif
#ifdef GL_MAP1_COLOR_4
    map1_color_4 = GL_MAP1_COLOR_4,
#endif
#ifdef GL_MAP1_GRID_DOMAIN
    map1_grid_domain = GL_MAP1_GRID_DOMAIN,
#endif
#ifdef GL_MAP1_GRID_SEGMENTS
    map1_grid_segments = GL_MAP1_GRID_SEGMENTS,
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
#ifdef GL_MAP2_GRID_DOMAIN
    map2_grid_domain = GL_MAP2_GRID_DOMAIN,
#endif
#ifdef GL_MAP2_GRID_SEGMENTS
    map2_grid_segments = GL_MAP2_GRID_SEGMENTS,
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
#ifdef GL_MAP_COLOR
    map_color = GL_MAP_COLOR,
#endif
#ifdef GL_MAP_STENCIL
    map_stencil = GL_MAP_STENCIL,
#endif
#ifdef GL_MATRIX_MODE
    matrix_mode = GL_MATRIX_MODE,
#endif
#ifdef GL_MAX_3D_TEXTURE_SIZE
    max_3d_texture_size = GL_MAX_3D_TEXTURE_SIZE,
#endif
#ifdef GL_MAX_3D_TEXTURE_SIZE_EXT
    max_3d_texture_size_ext = GL_MAX_3D_TEXTURE_SIZE_EXT,
#endif
#ifdef GL_MAX_4D_TEXTURE_SIZE_SGIS
    max_4d_texture_size_sgis = GL_MAX_4D_TEXTURE_SIZE_SGIS,
#endif
#ifdef GL_MAX_ACTIVE_LIGHTS_SGIX
    max_active_lights_sgix = GL_MAX_ACTIVE_LIGHTS_SGIX,
#endif
#ifdef GL_MAX_ARRAY_TEXTURE_LAYERS
    max_array_texture_layers = GL_MAX_ARRAY_TEXTURE_LAYERS,
#endif
#ifdef GL_MAX_ASYNC_DRAW_PIXELS_SGIX
    max_async_draw_pixels_sgix = GL_MAX_ASYNC_DRAW_PIXELS_SGIX,
#endif
#ifdef GL_MAX_ASYNC_HISTOGRAM_SGIX
    max_async_histogram_sgix = GL_MAX_ASYNC_HISTOGRAM_SGIX,
#endif
#ifdef GL_MAX_ASYNC_READ_PIXELS_SGIX
    max_async_read_pixels_sgix = GL_MAX_ASYNC_READ_PIXELS_SGIX,
#endif
#ifdef GL_MAX_ASYNC_TEX_IMAGE_SGIX
    max_async_tex_image_sgix = GL_MAX_ASYNC_TEX_IMAGE_SGIX,
#endif
#ifdef GL_MAX_ATTRIB_STACK_DEPTH
    max_attrib_stack_depth = GL_MAX_ATTRIB_STACK_DEPTH,
#endif
#ifdef GL_MAX_CLIENT_ATTRIB_STACK_DEPTH
    max_client_attrib_stack_depth = GL_MAX_CLIENT_ATTRIB_STACK_DEPTH,
#endif
#ifdef GL_MAX_CLIPMAP_DEPTH_SGIX
    max_clipmap_depth_sgix = GL_MAX_CLIPMAP_DEPTH_SGIX,
#endif
#ifdef GL_MAX_CLIPMAP_VIRTUAL_DEPTH_SGIX
    max_clipmap_virtual_depth_sgix = GL_MAX_CLIPMAP_VIRTUAL_DEPTH_SGIX,
#endif
#ifdef GL_MAX_CLIP_DISTANCES
    max_clip_distances = GL_MAX_CLIP_DISTANCES,
#endif
#ifdef GL_MAX_CLIP_PLANES
    max_clip_planes = GL_MAX_CLIP_PLANES,
#endif
#ifdef GL_MAX_COLOR_ATTACHMENTS
    max_color_attachments = GL_MAX_COLOR_ATTACHMENTS,
#endif
#ifdef GL_MAX_COLOR_ATTACHMENTS_EXT
    max_color_attachments_ext = GL_MAX_COLOR_ATTACHMENTS_EXT,
#endif
#ifdef GL_MAX_COLOR_ATTACHMENTS_NV
    max_color_attachments_nv = GL_MAX_COLOR_ATTACHMENTS_NV,
#endif
#ifdef GL_MAX_COLOR_MATRIX_STACK_DEPTH_SGI
    max_color_matrix_stack_depth_sgi = GL_MAX_COLOR_MATRIX_STACK_DEPTH_SGI,
#endif
#ifdef GL_MAX_COLOR_TEXTURE_SAMPLES
    max_color_texture_samples = GL_MAX_COLOR_TEXTURE_SAMPLES,
#endif
#ifdef GL_MAX_COMBINED_ATOMIC_COUNTERS
    max_combined_atomic_counters = GL_MAX_COMBINED_ATOMIC_COUNTERS,
#endif
#ifdef GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS
    max_combined_compute_uniform_components =
        GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS,
#endif
#ifdef GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS
    max_combined_fragment_uniform_components =
        GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS,
#endif
#ifdef GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS
    max_combined_geometry_uniform_components =
        GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS,
#endif
#ifdef GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS
    max_combined_shader_storage_blocks = GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS,
#endif
#ifdef GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS
    max_combined_texture_image_units = GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
#endif
#ifdef GL_MAX_COMBINED_UNIFORM_BLOCKS
    max_combined_uniform_blocks = GL_MAX_COMBINED_UNIFORM_BLOCKS,
#endif
#ifdef GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS
    max_combined_vertex_uniform_components =
        GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS,
#endif
#ifdef GL_MAX_COMPUTE_ATOMIC_COUNTERS
    max_compute_atomic_counters = GL_MAX_COMPUTE_ATOMIC_COUNTERS,
#endif
#ifdef GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS
    max_compute_atomic_counter_buffers = GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS,
#endif
#ifdef GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS
    max_compute_shader_storage_blocks = GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS,
#endif
#ifdef GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS
    max_compute_texture_image_units = GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS,
#endif
#ifdef GL_MAX_COMPUTE_UNIFORM_BLOCKS
    max_compute_uniform_blocks = GL_MAX_COMPUTE_UNIFORM_BLOCKS,
#endif
#ifdef GL_MAX_COMPUTE_UNIFORM_COMPONENTS
    max_compute_uniform_components = GL_MAX_COMPUTE_UNIFORM_COMPONENTS,
#endif
#ifdef GL_MAX_COMPUTE_WORK_GROUP_COUNT
    max_compute_work_group_count = GL_MAX_COMPUTE_WORK_GROUP_COUNT,
#endif
#ifdef GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS
    max_compute_work_group_invocations = GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS,
#endif
#ifdef GL_MAX_COMPUTE_WORK_GROUP_SIZE
    max_compute_work_group_size = GL_MAX_COMPUTE_WORK_GROUP_SIZE,
#endif
#ifdef GL_MAX_CUBE_MAP_TEXTURE_SIZE
    max_cube_map_texture_size = GL_MAX_CUBE_MAP_TEXTURE_SIZE,
#endif
#ifdef GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB
    max_cube_map_texture_size_arb = GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB,
#endif
#ifdef GL_MAX_CUBE_MAP_TEXTURE_SIZE_EXT
    max_cube_map_texture_size_ext = GL_MAX_CUBE_MAP_TEXTURE_SIZE_EXT,
#endif
#ifdef GL_MAX_CUBE_MAP_TEXTURE_SIZE_OES
    max_cube_map_texture_size_oes = GL_MAX_CUBE_MAP_TEXTURE_SIZE_OES,
#endif
#ifdef GL_MAX_DEBUG_GROUP_STACK_DEPTH
    max_debug_group_stack_depth = GL_MAX_DEBUG_GROUP_STACK_DEPTH,
#endif
#ifdef GL_MAX_DEPTH_TEXTURE_SAMPLES
    max_depth_texture_samples = GL_MAX_DEPTH_TEXTURE_SAMPLES,
#endif
#ifdef GL_MAX_DRAW_BUFFERS
    max_draw_buffers = GL_MAX_DRAW_BUFFERS,
#endif
#ifdef GL_MAX_DUAL_SOURCE_DRAW_BUFFERS
    max_dual_source_draw_buffers = GL_MAX_DUAL_SOURCE_DRAW_BUFFERS,
#endif
#ifdef GL_MAX_ELEMENTS_INDICES
    max_elements_indices = GL_MAX_ELEMENTS_INDICES,
#endif
#ifdef GL_MAX_ELEMENTS_VERTICES
    max_elements_vertices = GL_MAX_ELEMENTS_VERTICES,
#endif
#ifdef GL_MAX_ELEMENT_INDEX
    max_element_index = GL_MAX_ELEMENT_INDEX,
#endif
#ifdef GL_MAX_EVAL_ORDER
    max_eval_order = GL_MAX_EVAL_ORDER,
#endif
#ifdef GL_MAX_FOG_FUNC_POINTS_SGIS
    max_fog_func_points_sgis = GL_MAX_FOG_FUNC_POINTS_SGIS,
#endif
#ifdef GL_MAX_FRAGMENT_ATOMIC_COUNTERS
    max_fragment_atomic_counters = GL_MAX_FRAGMENT_ATOMIC_COUNTERS,
#endif
#ifdef GL_MAX_FRAGMENT_INPUT_COMPONENTS
    max_fragment_input_components = GL_MAX_FRAGMENT_INPUT_COMPONENTS,
#endif
#ifdef GL_MAX_FRAGMENT_LIGHTS_SGIX
    max_fragment_lights_sgix = GL_MAX_FRAGMENT_LIGHTS_SGIX,
#endif
#ifdef GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS
    max_fragment_shader_storage_blocks = GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS,
#endif
#ifdef GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_LAYERS_EXT
    max_fragment_shading_rate_attachment_layers_ext =
        GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_LAYERS_EXT,
#endif
#ifdef GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_ASPECT_RATIO_EXT
    max_fragment_shading_rate_attachment_texel_aspect_ratio_ext =
        GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_ASPECT_RATIO_EXT,
#endif
#ifdef GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_HEIGHT_EXT
    max_fragment_shading_rate_attachment_texel_height_ext =
        GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_HEIGHT_EXT,
#endif
#ifdef GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_WIDTH_EXT
    max_fragment_shading_rate_attachment_texel_width_ext =
        GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_WIDTH_EXT,
#endif
#ifdef GL_MAX_FRAGMENT_UNIFORM_BLOCKS
    max_fragment_uniform_blocks = GL_MAX_FRAGMENT_UNIFORM_BLOCKS,
#endif
#ifdef GL_MAX_FRAGMENT_UNIFORM_COMPONENTS
    max_fragment_uniform_components = GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
#endif
#ifdef GL_MAX_FRAGMENT_UNIFORM_VECTORS
    max_fragment_uniform_vectors = GL_MAX_FRAGMENT_UNIFORM_VECTORS,
#endif
#ifdef GL_MAX_FRAMEBUFFER_HEIGHT
    max_framebuffer_height = GL_MAX_FRAMEBUFFER_HEIGHT,
#endif
#ifdef GL_MAX_FRAMEBUFFER_LAYERS
    max_framebuffer_layers = GL_MAX_FRAMEBUFFER_LAYERS,
#endif
#ifdef GL_MAX_FRAMEBUFFER_SAMPLES
    max_framebuffer_samples = GL_MAX_FRAMEBUFFER_SAMPLES,
#endif
#ifdef GL_MAX_FRAMEBUFFER_WIDTH
    max_framebuffer_width = GL_MAX_FRAMEBUFFER_WIDTH,
#endif
#ifdef GL_MAX_FRAMEZOOM_FACTOR_SGIX
    max_framezoom_factor_sgix = GL_MAX_FRAMEZOOM_FACTOR_SGIX,
#endif
#ifdef GL_MAX_GEOMETRY_ATOMIC_COUNTERS
    max_geometry_atomic_counters = GL_MAX_GEOMETRY_ATOMIC_COUNTERS,
#endif
#ifdef GL_MAX_GEOMETRY_INPUT_COMPONENTS
    max_geometry_input_components = GL_MAX_GEOMETRY_INPUT_COMPONENTS,
#endif
#ifdef GL_MAX_GEOMETRY_OUTPUT_COMPONENTS
    max_geometry_output_components = GL_MAX_GEOMETRY_OUTPUT_COMPONENTS,
#endif
#ifdef GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS
    max_geometry_shader_storage_blocks = GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS,
#endif
#ifdef GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS
    max_geometry_texture_image_units = GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS,
#endif
#ifdef GL_MAX_GEOMETRY_UNIFORM_BLOCKS
    max_geometry_uniform_blocks = GL_MAX_GEOMETRY_UNIFORM_BLOCKS,
#endif
#ifdef GL_MAX_GEOMETRY_UNIFORM_COMPONENTS
    max_geometry_uniform_components = GL_MAX_GEOMETRY_UNIFORM_COMPONENTS,
#endif
#ifdef GL_MAX_INTEGER_SAMPLES
    max_integer_samples = GL_MAX_INTEGER_SAMPLES,
#endif
#ifdef GL_MAX_LABEL_LENGTH
    max_label_length = GL_MAX_LABEL_LENGTH,
#endif
#ifdef GL_MAX_LIGHTS
    max_lights = GL_MAX_LIGHTS,
#endif
#ifdef GL_MAX_LIST_NESTING
    max_list_nesting = GL_MAX_LIST_NESTING,
#endif
#ifdef GL_MAX_MODELVIEW_STACK_DEPTH
    max_modelview_stack_depth = GL_MAX_MODELVIEW_STACK_DEPTH,
#endif
#ifdef GL_MAX_NAME_STACK_DEPTH
    max_name_stack_depth = GL_MAX_NAME_STACK_DEPTH,
#endif
#ifdef GL_MAX_PIXEL_MAP_TABLE
    max_pixel_map_table = GL_MAX_PIXEL_MAP_TABLE,
#endif
#ifdef GL_MAX_PROGRAM_TEXEL_OFFSET
    max_program_texel_offset = GL_MAX_PROGRAM_TEXEL_OFFSET,
#endif
#ifdef GL_MAX_PROJECTION_STACK_DEPTH
    max_projection_stack_depth = GL_MAX_PROJECTION_STACK_DEPTH,
#endif
#ifdef GL_MAX_RECTANGLE_TEXTURE_SIZE
    max_rectangle_texture_size = GL_MAX_RECTANGLE_TEXTURE_SIZE,
#endif
#ifdef GL_MAX_RENDERBUFFER_SIZE
    max_renderbuffer_size = GL_MAX_RENDERBUFFER_SIZE,
#endif
#ifdef GL_MAX_SAMPLE_MASK_WORDS
    max_sample_mask_words = GL_MAX_SAMPLE_MASK_WORDS,
#endif
#ifdef GL_MAX_SERVER_WAIT_TIMEOUT
    max_server_wait_timeout = GL_MAX_SERVER_WAIT_TIMEOUT,
#endif
#ifdef GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS
    max_shader_storage_buffer_bindings = GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS,
#endif
#ifdef GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS
    max_tess_control_atomic_counters = GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS,
#endif
#ifdef GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS
    max_tess_control_shader_storage_blocks =
        GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS,
#endif
#ifdef GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS
    max_tess_control_uniform_blocks = GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS,
#endif
#ifdef GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS
    max_tess_evaluation_atomic_counters =
        GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS,
#endif
#ifdef GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS
    max_tess_evaluation_shader_storage_blocks =
        GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS,
#endif
#ifdef GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS
    max_tess_evaluation_uniform_blocks = GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS,
#endif
#ifdef GL_MAX_TEXTURE_BUFFER_SIZE
    max_texture_buffer_size = GL_MAX_TEXTURE_BUFFER_SIZE,
#endif
#ifdef GL_MAX_TEXTURE_IMAGE_UNITS
    max_texture_image_units = GL_MAX_TEXTURE_IMAGE_UNITS,
#endif
#ifdef GL_MAX_TEXTURE_LOD_BIAS
    max_texture_lod_bias = GL_MAX_TEXTURE_LOD_BIAS,
#endif
#ifdef GL_MAX_TEXTURE_LOD_BIAS_EXT
    max_texture_lod_bias_ext = GL_MAX_TEXTURE_LOD_BIAS_EXT,
#endif
#ifdef GL_MAX_TEXTURE_MAX_ANISOTROPY
    max_texture_max_anisotropy = GL_MAX_TEXTURE_MAX_ANISOTROPY,
#endif
#ifdef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
    max_texture_max_anisotropy_ext = GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,
#endif
#ifdef GL_MAX_TEXTURE_SIZE
    max_texture_size = GL_MAX_TEXTURE_SIZE,
#endif
#ifdef GL_MAX_TEXTURE_STACK_DEPTH
    max_texture_stack_depth = GL_MAX_TEXTURE_STACK_DEPTH,
#endif
#ifdef GL_MAX_TIMELINE_SEMAPHORE_VALUE_DIFFERENCE_NV
    max_timeline_semaphore_value_difference_nv =
        GL_MAX_TIMELINE_SEMAPHORE_VALUE_DIFFERENCE_NV,
#endif
#ifdef GL_MAX_UNIFORM_BLOCK_SIZE
    max_uniform_block_size = GL_MAX_UNIFORM_BLOCK_SIZE,
#endif
#ifdef GL_MAX_UNIFORM_BUFFER_BINDINGS
    max_uniform_buffer_bindings = GL_MAX_UNIFORM_BUFFER_BINDINGS,
#endif
#ifdef GL_MAX_UNIFORM_LOCATIONS
    max_uniform_locations = GL_MAX_UNIFORM_LOCATIONS,
#endif
#ifdef GL_MAX_VARYING_COMPONENTS
    max_varying_components = GL_MAX_VARYING_COMPONENTS,
#endif
#ifdef GL_MAX_VARYING_FLOATS
    max_varying_floats = GL_MAX_VARYING_FLOATS,
#endif
#ifdef GL_MAX_VARYING_VECTORS
    max_varying_vectors = GL_MAX_VARYING_VECTORS,
#endif
#ifdef GL_MAX_VERTEX_ATOMIC_COUNTERS
    max_vertex_atomic_counters = GL_MAX_VERTEX_ATOMIC_COUNTERS,
#endif
#ifdef GL_MAX_VERTEX_ATTRIBS
    max_vertex_attribs = GL_MAX_VERTEX_ATTRIBS,
#endif
#ifdef GL_MAX_VERTEX_ATTRIB_BINDINGS
    max_vertex_attrib_bindings = GL_MAX_VERTEX_ATTRIB_BINDINGS,
#endif
#ifdef GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET
    max_vertex_attrib_relative_offset = GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET,
#endif
#ifdef GL_MAX_VERTEX_OUTPUT_COMPONENTS
    max_vertex_output_components = GL_MAX_VERTEX_OUTPUT_COMPONENTS,
#endif
#ifdef GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS
    max_vertex_shader_storage_blocks = GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS,
#endif
#ifdef GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS
    max_vertex_texture_image_units = GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
#endif
#ifdef GL_MAX_VERTEX_UNIFORM_BLOCKS
    max_vertex_uniform_blocks = GL_MAX_VERTEX_UNIFORM_BLOCKS,
#endif
#ifdef GL_MAX_VERTEX_UNIFORM_COMPONENTS
    max_vertex_uniform_components = GL_MAX_VERTEX_UNIFORM_COMPONENTS,
#endif
#ifdef GL_MAX_VERTEX_UNIFORM_VECTORS
    max_vertex_uniform_vectors = GL_MAX_VERTEX_UNIFORM_VECTORS,
#endif
#ifdef GL_MAX_VIEWPORTS
    max_viewports = GL_MAX_VIEWPORTS,
#endif
#ifdef GL_MAX_VIEWPORT_DIMS
    max_viewport_dims = GL_MAX_VIEWPORT_DIMS,
#endif
#ifdef GL_MINMAX_EXT
    minmax_ext = GL_MINMAX_EXT,
#endif
#ifdef GL_MINOR_VERSION
    minor_version = GL_MINOR_VERSION,
#endif
#ifdef GL_MIN_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_HEIGHT_EXT
    min_fragment_shading_rate_attachment_texel_height_ext =
        GL_MIN_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_HEIGHT_EXT,
#endif
#ifdef GL_MIN_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_WIDTH_EXT
    min_fragment_shading_rate_attachment_texel_width_ext =
        GL_MIN_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_WIDTH_EXT,
#endif
#ifdef GL_MIN_MAP_BUFFER_ALIGNMENT
    min_map_buffer_alignment = GL_MIN_MAP_BUFFER_ALIGNMENT,
#endif
#ifdef GL_MIN_PROGRAM_TEXEL_OFFSET
    min_program_texel_offset = GL_MIN_PROGRAM_TEXEL_OFFSET,
#endif
#ifdef GL_MODELVIEW0_MATRIX_EXT
    modelview0_matrix_ext = GL_MODELVIEW0_MATRIX_EXT,
#endif
#ifdef GL_MODELVIEW0_STACK_DEPTH_EXT
    modelview0_stack_depth_ext = GL_MODELVIEW0_STACK_DEPTH_EXT,
#endif
#ifdef GL_MODELVIEW_MATRIX
    modelview_matrix = GL_MODELVIEW_MATRIX,
#endif
#ifdef GL_MODELVIEW_STACK_DEPTH
    modelview_stack_depth = GL_MODELVIEW_STACK_DEPTH,
#endif
#ifdef GL_MOTION_ESTIMATION_SEARCH_BLOCK_X_QCOM
    motion_estimation_search_block_x_qcom =
        GL_MOTION_ESTIMATION_SEARCH_BLOCK_X_QCOM,
#endif
#ifdef GL_MOTION_ESTIMATION_SEARCH_BLOCK_Y_QCOM
    motion_estimation_search_block_y_qcom =
        GL_MOTION_ESTIMATION_SEARCH_BLOCK_Y_QCOM,
#endif
#ifdef GL_MULTISAMPLE_SGIS
    multisample_sgis = GL_MULTISAMPLE_SGIS,
#endif
#ifdef GL_NAME_STACK_DEPTH
    name_stack_depth = GL_NAME_STACK_DEPTH,
#endif
#ifdef GL_NORMALIZE
    normalize = GL_NORMALIZE,
#endif
#ifdef GL_NORMAL_ARRAY
    normal_array = GL_NORMAL_ARRAY,
#endif
#ifdef GL_NORMAL_ARRAY_COUNT_EXT
    normal_array_count_ext = GL_NORMAL_ARRAY_COUNT_EXT,
#endif
#ifdef GL_NORMAL_ARRAY_STRIDE
    normal_array_stride = GL_NORMAL_ARRAY_STRIDE,
#endif
#ifdef GL_NORMAL_ARRAY_TYPE
    normal_array_type = GL_NORMAL_ARRAY_TYPE,
#endif
#ifdef GL_NUM_COMPRESSED_TEXTURE_FORMATS
    num_compressed_texture_formats = GL_NUM_COMPRESSED_TEXTURE_FORMATS,
#endif
#ifdef GL_NUM_DEVICE_UUIDS_EXT
    num_device_uuids_ext = GL_NUM_DEVICE_UUIDS_EXT,
#endif
#ifdef GL_NUM_EXTENSIONS
    num_extensions = GL_NUM_EXTENSIONS,
#endif
#ifdef GL_NUM_PROGRAM_BINARY_FORMATS
    num_program_binary_formats = GL_NUM_PROGRAM_BINARY_FORMATS,
#endif
#ifdef GL_NUM_SHADER_BINARY_FORMATS
    num_shader_binary_formats = GL_NUM_SHADER_BINARY_FORMATS,
#endif
#ifdef GL_PACK_ALIGNMENT
    pack_alignment = GL_PACK_ALIGNMENT,
#endif
#ifdef GL_PACK_CMYK_HINT_EXT
    pack_cmyk_hint_ext = GL_PACK_CMYK_HINT_EXT,
#endif
#ifdef GL_PACK_IMAGE_DEPTH_SGIS
    pack_image_depth_sgis = GL_PACK_IMAGE_DEPTH_SGIS,
#endif
#ifdef GL_PACK_IMAGE_HEIGHT
    pack_image_height = GL_PACK_IMAGE_HEIGHT,
#endif
#ifdef GL_PACK_IMAGE_HEIGHT_EXT
    pack_image_height_ext = GL_PACK_IMAGE_HEIGHT_EXT,
#endif
#ifdef GL_PACK_LSB_FIRST
    pack_lsb_first = GL_PACK_LSB_FIRST,
#endif
#ifdef GL_PACK_RESAMPLE_SGIX
    pack_resample_sgix = GL_PACK_RESAMPLE_SGIX,
#endif
#ifdef GL_PACK_ROW_LENGTH
    pack_row_length = GL_PACK_ROW_LENGTH,
#endif
#ifdef GL_PACK_SKIP_IMAGES
    pack_skip_images = GL_PACK_SKIP_IMAGES,
#endif
#ifdef GL_PACK_SKIP_IMAGES_EXT
    pack_skip_images_ext = GL_PACK_SKIP_IMAGES_EXT,
#endif
#ifdef GL_PACK_SKIP_PIXELS
    pack_skip_pixels = GL_PACK_SKIP_PIXELS,
#endif
#ifdef GL_PACK_SKIP_ROWS
    pack_skip_rows = GL_PACK_SKIP_ROWS,
#endif
#ifdef GL_PACK_SKIP_VOLUMES_SGIS
    pack_skip_volumes_sgis = GL_PACK_SKIP_VOLUMES_SGIS,
#endif
#ifdef GL_PACK_SUBSAMPLE_RATE_SGIX
    pack_subsample_rate_sgix = GL_PACK_SUBSAMPLE_RATE_SGIX,
#endif
#ifdef GL_PACK_SWAP_BYTES
    pack_swap_bytes = GL_PACK_SWAP_BYTES,
#endif
#ifdef GL_PERSPECTIVE_CORRECTION_HINT
    perspective_correction_hint = GL_PERSPECTIVE_CORRECTION_HINT,
#endif
#ifdef GL_PIXEL_MAP_A_TO_A_SIZE
    pixel_map_a_to_a_size = GL_PIXEL_MAP_A_TO_A_SIZE,
#endif
#ifdef GL_PIXEL_MAP_B_TO_B_SIZE
    pixel_map_b_to_b_size = GL_PIXEL_MAP_B_TO_B_SIZE,
#endif
#ifdef GL_PIXEL_MAP_G_TO_G_SIZE
    pixel_map_g_to_g_size = GL_PIXEL_MAP_G_TO_G_SIZE,
#endif
#ifdef GL_PIXEL_MAP_I_TO_A_SIZE
    pixel_map_i_to_a_size = GL_PIXEL_MAP_I_TO_A_SIZE,
#endif
#ifdef GL_PIXEL_MAP_I_TO_B_SIZE
    pixel_map_i_to_b_size = GL_PIXEL_MAP_I_TO_B_SIZE,
#endif
#ifdef GL_PIXEL_MAP_I_TO_G_SIZE
    pixel_map_i_to_g_size = GL_PIXEL_MAP_I_TO_G_SIZE,
#endif
#ifdef GL_PIXEL_MAP_I_TO_I_SIZE
    pixel_map_i_to_i_size = GL_PIXEL_MAP_I_TO_I_SIZE,
#endif
#ifdef GL_PIXEL_MAP_I_TO_R_SIZE
    pixel_map_i_to_r_size = GL_PIXEL_MAP_I_TO_R_SIZE,
#endif
#ifdef GL_PIXEL_MAP_R_TO_R_SIZE
    pixel_map_r_to_r_size = GL_PIXEL_MAP_R_TO_R_SIZE,
#endif
#ifdef GL_PIXEL_MAP_S_TO_S_SIZE
    pixel_map_s_to_s_size = GL_PIXEL_MAP_S_TO_S_SIZE,
#endif
#ifdef GL_PIXEL_PACK_BUFFER_BINDING
    pixel_pack_buffer_binding = GL_PIXEL_PACK_BUFFER_BINDING,
#endif
#ifdef GL_PIXEL_TEXTURE_SGIS
    pixel_texture_sgis = GL_PIXEL_TEXTURE_SGIS,
#endif
#ifdef GL_PIXEL_TEX_GEN_MODE_SGIX
    pixel_tex_gen_mode_sgix = GL_PIXEL_TEX_GEN_MODE_SGIX,
#endif
#ifdef GL_PIXEL_TEX_GEN_SGIX
    pixel_tex_gen_sgix = GL_PIXEL_TEX_GEN_SGIX,
#endif
#ifdef GL_PIXEL_TILE_BEST_ALIGNMENT_SGIX
    pixel_tile_best_alignment_sgix = GL_PIXEL_TILE_BEST_ALIGNMENT_SGIX,
#endif
#ifdef GL_PIXEL_TILE_CACHE_INCREMENT_SGIX
    pixel_tile_cache_increment_sgix = GL_PIXEL_TILE_CACHE_INCREMENT_SGIX,
#endif
#ifdef GL_PIXEL_TILE_CACHE_SIZE_SGIX
    pixel_tile_cache_size_sgix = GL_PIXEL_TILE_CACHE_SIZE_SGIX,
#endif
#ifdef GL_PIXEL_TILE_GRID_DEPTH_SGIX
    pixel_tile_grid_depth_sgix = GL_PIXEL_TILE_GRID_DEPTH_SGIX,
#endif
#ifdef GL_PIXEL_TILE_GRID_HEIGHT_SGIX
    pixel_tile_grid_height_sgix = GL_PIXEL_TILE_GRID_HEIGHT_SGIX,
#endif
#ifdef GL_PIXEL_TILE_GRID_WIDTH_SGIX
    pixel_tile_grid_width_sgix = GL_PIXEL_TILE_GRID_WIDTH_SGIX,
#endif
#ifdef GL_PIXEL_TILE_HEIGHT_SGIX
    pixel_tile_height_sgix = GL_PIXEL_TILE_HEIGHT_SGIX,
#endif
#ifdef GL_PIXEL_TILE_WIDTH_SGIX
    pixel_tile_width_sgix = GL_PIXEL_TILE_WIDTH_SGIX,
#endif
#ifdef GL_PIXEL_UNPACK_BUFFER_BINDING
    pixel_unpack_buffer_binding = GL_PIXEL_UNPACK_BUFFER_BINDING,
#endif
#ifdef GL_POINT_DISTANCE_ATTENUATION
    point_distance_attenuation = GL_POINT_DISTANCE_ATTENUATION,
#endif
#ifdef GL_POINT_DISTANCE_ATTENUATION_ARB
    point_distance_attenuation_arb = GL_POINT_DISTANCE_ATTENUATION_ARB,
#endif
#ifdef GL_POINT_FADE_THRESHOLD_SIZE
    point_fade_threshold_size = GL_POINT_FADE_THRESHOLD_SIZE,
#endif
#ifdef GL_POINT_FADE_THRESHOLD_SIZE_ARB
    point_fade_threshold_size_arb = GL_POINT_FADE_THRESHOLD_SIZE_ARB,
#endif
#ifdef GL_POINT_FADE_THRESHOLD_SIZE_EXT
    point_fade_threshold_size_ext = GL_POINT_FADE_THRESHOLD_SIZE_EXT,
#endif
#ifdef GL_POINT_FADE_THRESHOLD_SIZE_SGIS
    point_fade_threshold_size_sgis = GL_POINT_FADE_THRESHOLD_SIZE_SGIS,
#endif
#ifdef GL_POINT_SIZE
    point_size = GL_POINT_SIZE,
#endif
#ifdef GL_POINT_SIZE_GRANULARITY
    point_size_granularity = GL_POINT_SIZE_GRANULARITY,
#endif
#ifdef GL_POINT_SIZE_MAX
    point_size_max = GL_POINT_SIZE_MAX,
#endif
#ifdef GL_POINT_SIZE_MAX_ARB
    point_size_max_arb = GL_POINT_SIZE_MAX_ARB,
#endif
#ifdef GL_POINT_SIZE_MAX_EXT
    point_size_max_ext = GL_POINT_SIZE_MAX_EXT,
#endif
#ifdef GL_POINT_SIZE_MAX_SGIS
    point_size_max_sgis = GL_POINT_SIZE_MAX_SGIS,
#endif
#ifdef GL_POINT_SIZE_MIN
    point_size_min = GL_POINT_SIZE_MIN,
#endif
#ifdef GL_POINT_SIZE_MIN_ARB
    point_size_min_arb = GL_POINT_SIZE_MIN_ARB,
#endif
#ifdef GL_POINT_SIZE_MIN_EXT
    point_size_min_ext = GL_POINT_SIZE_MIN_EXT,
#endif
#ifdef GL_POINT_SIZE_MIN_SGIS
    point_size_min_sgis = GL_POINT_SIZE_MIN_SGIS,
#endif
#ifdef GL_POINT_SIZE_RANGE
    point_size_range = GL_POINT_SIZE_RANGE,
#endif
#ifdef GL_POINT_SMOOTH
    point_smooth = GL_POINT_SMOOTH,
#endif
#ifdef GL_POINT_SMOOTH_HINT
    point_smooth_hint = GL_POINT_SMOOTH_HINT,
#endif
#ifdef GL_POLYGON_MODE
    polygon_mode = GL_POLYGON_MODE,
#endif
#ifdef GL_POLYGON_OFFSET_BIAS_EXT
    polygon_offset_bias_ext = GL_POLYGON_OFFSET_BIAS_EXT,
#endif
#ifdef GL_POLYGON_OFFSET_FACTOR
    polygon_offset_factor = GL_POLYGON_OFFSET_FACTOR,
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
#ifdef GL_POLYGON_OFFSET_UNITS
    polygon_offset_units = GL_POLYGON_OFFSET_UNITS,
#endif
#ifdef GL_POLYGON_SMOOTH
    polygon_smooth = GL_POLYGON_SMOOTH,
#endif
#ifdef GL_POLYGON_SMOOTH_HINT
    polygon_smooth_hint = GL_POLYGON_SMOOTH_HINT,
#endif
#ifdef GL_POLYGON_STIPPLE
    polygon_stipple = GL_POLYGON_STIPPLE,
#endif
#ifdef GL_POST_COLOR_MATRIX_ALPHA_BIAS_SGI
    post_color_matrix_alpha_bias_sgi = GL_POST_COLOR_MATRIX_ALPHA_BIAS_SGI,
#endif
#ifdef GL_POST_COLOR_MATRIX_ALPHA_SCALE_SGI
    post_color_matrix_alpha_scale_sgi = GL_POST_COLOR_MATRIX_ALPHA_SCALE_SGI,
#endif
#ifdef GL_POST_COLOR_MATRIX_BLUE_BIAS_SGI
    post_color_matrix_blue_bias_sgi = GL_POST_COLOR_MATRIX_BLUE_BIAS_SGI,
#endif
#ifdef GL_POST_COLOR_MATRIX_BLUE_SCALE_SGI
    post_color_matrix_blue_scale_sgi = GL_POST_COLOR_MATRIX_BLUE_SCALE_SGI,
#endif
#ifdef GL_POST_COLOR_MATRIX_COLOR_TABLE_SGI
    post_color_matrix_color_table_sgi = GL_POST_COLOR_MATRIX_COLOR_TABLE_SGI,
#endif
#ifdef GL_POST_COLOR_MATRIX_GREEN_BIAS_SGI
    post_color_matrix_green_bias_sgi = GL_POST_COLOR_MATRIX_GREEN_BIAS_SGI,
#endif
#ifdef GL_POST_COLOR_MATRIX_GREEN_SCALE_SGI
    post_color_matrix_green_scale_sgi = GL_POST_COLOR_MATRIX_GREEN_SCALE_SGI,
#endif
#ifdef GL_POST_COLOR_MATRIX_RED_BIAS_SGI
    post_color_matrix_red_bias_sgi = GL_POST_COLOR_MATRIX_RED_BIAS_SGI,
#endif
#ifdef GL_POST_COLOR_MATRIX_RED_SCALE_SGI
    post_color_matrix_red_scale_sgi = GL_POST_COLOR_MATRIX_RED_SCALE_SGI,
#endif
#ifdef GL_POST_CONVOLUTION_ALPHA_BIAS_EXT
    post_convolution_alpha_bias_ext = GL_POST_CONVOLUTION_ALPHA_BIAS_EXT,
#endif
#ifdef GL_POST_CONVOLUTION_ALPHA_SCALE_EXT
    post_convolution_alpha_scale_ext = GL_POST_CONVOLUTION_ALPHA_SCALE_EXT,
#endif
#ifdef GL_POST_CONVOLUTION_BLUE_BIAS_EXT
    post_convolution_blue_bias_ext = GL_POST_CONVOLUTION_BLUE_BIAS_EXT,
#endif
#ifdef GL_POST_CONVOLUTION_BLUE_SCALE_EXT
    post_convolution_blue_scale_ext = GL_POST_CONVOLUTION_BLUE_SCALE_EXT,
#endif
#ifdef GL_POST_CONVOLUTION_COLOR_TABLE_SGI
    post_convolution_color_table_sgi = GL_POST_CONVOLUTION_COLOR_TABLE_SGI,
#endif
#ifdef GL_POST_CONVOLUTION_GREEN_BIAS_EXT
    post_convolution_green_bias_ext = GL_POST_CONVOLUTION_GREEN_BIAS_EXT,
#endif
#ifdef GL_POST_CONVOLUTION_GREEN_SCALE_EXT
    post_convolution_green_scale_ext = GL_POST_CONVOLUTION_GREEN_SCALE_EXT,
#endif
#ifdef GL_POST_CONVOLUTION_RED_BIAS_EXT
    post_convolution_red_bias_ext = GL_POST_CONVOLUTION_RED_BIAS_EXT,
#endif
#ifdef GL_POST_CONVOLUTION_RED_SCALE_EXT
    post_convolution_red_scale_ext = GL_POST_CONVOLUTION_RED_SCALE_EXT,
#endif
#ifdef GL_POST_TEXTURE_FILTER_BIAS_RANGE_SGIX
    post_texture_filter_bias_range_sgix =
        GL_POST_TEXTURE_FILTER_BIAS_RANGE_SGIX,
#endif
#ifdef GL_POST_TEXTURE_FILTER_SCALE_RANGE_SGIX
    post_texture_filter_scale_range_sgix =
        GL_POST_TEXTURE_FILTER_SCALE_RANGE_SGIX,
#endif
#ifdef GL_PRIMITIVE_RESTART_INDEX
    primitive_restart_index = GL_PRIMITIVE_RESTART_INDEX,
#endif
#ifdef GL_PROGRAM_BINARY_FORMATS
    program_binary_formats = GL_PROGRAM_BINARY_FORMATS,
#endif
#ifdef GL_PROGRAM_PIPELINE_BINDING
    program_pipeline_binding = GL_PROGRAM_PIPELINE_BINDING,
#endif
#ifdef GL_PROGRAM_POINT_SIZE
    program_point_size = GL_PROGRAM_POINT_SIZE,
#endif
#ifdef GL_PROJECTION_MATRIX
    projection_matrix = GL_PROJECTION_MATRIX,
#endif
#ifdef GL_PROJECTION_STACK_DEPTH
    projection_stack_depth = GL_PROJECTION_STACK_DEPTH,
#endif
#ifdef GL_PROVOKING_VERTEX
    provoking_vertex = GL_PROVOKING_VERTEX,
#endif
#ifdef GL_READ_BUFFER
    read_buffer = GL_READ_BUFFER,
#endif
#ifdef GL_READ_BUFFER_EXT
    read_buffer_ext = GL_READ_BUFFER_EXT,
#endif
#ifdef GL_READ_BUFFER_NV
    read_buffer_nv = GL_READ_BUFFER_NV,
#endif
#ifdef GL_READ_FRAMEBUFFER_BINDING
    read_framebuffer_binding = GL_READ_FRAMEBUFFER_BINDING,
#endif
#ifdef GL_RED_BIAS
    red_bias = GL_RED_BIAS,
#endif
#ifdef GL_RED_BITS
    red_bits = GL_RED_BITS,
#endif
#ifdef GL_RED_SCALE
    red_scale = GL_RED_SCALE,
#endif
#ifdef GL_REFERENCE_PLANE_EQUATION_SGIX
    reference_plane_equation_sgix = GL_REFERENCE_PLANE_EQUATION_SGIX,
#endif
#ifdef GL_REFERENCE_PLANE_SGIX
    reference_plane_sgix = GL_REFERENCE_PLANE_SGIX,
#endif
#ifdef GL_RENDERBUFFER_BINDING
    renderbuffer_binding = GL_RENDERBUFFER_BINDING,
#endif
#ifdef GL_RENDER_MODE
    render_mode = GL_RENDER_MODE,
#endif
#ifdef GL_RESCALE_NORMAL_EXT
    rescale_normal_ext = GL_RESCALE_NORMAL_EXT,
#endif
#ifdef GL_RGBA_MODE
    rgba_mode = GL_RGBA_MODE,
#endif
#ifdef GL_SAMPLER_BINDING
    sampler_binding = GL_SAMPLER_BINDING,
#endif
#ifdef GL_SAMPLES
    samples = GL_SAMPLES,
#endif
#ifdef GL_SAMPLES_SGIS
    samples_sgis = GL_SAMPLES_SGIS,
#endif
#ifdef GL_SAMPLE_ALPHA_TO_MASK_SGIS
    sample_alpha_to_mask_sgis = GL_SAMPLE_ALPHA_TO_MASK_SGIS,
#endif
#ifdef GL_SAMPLE_ALPHA_TO_ONE_SGIS
    sample_alpha_to_one_sgis = GL_SAMPLE_ALPHA_TO_ONE_SGIS,
#endif
#ifdef GL_SAMPLE_BUFFERS
    sample_buffers = GL_SAMPLE_BUFFERS,
#endif
#ifdef GL_SAMPLE_BUFFERS_SGIS
    sample_buffers_sgis = GL_SAMPLE_BUFFERS_SGIS,
#endif
#ifdef GL_SAMPLE_COVERAGE_INVERT
    sample_coverage_invert = GL_SAMPLE_COVERAGE_INVERT,
#endif
#ifdef GL_SAMPLE_COVERAGE_VALUE
    sample_coverage_value = GL_SAMPLE_COVERAGE_VALUE,
#endif
#ifdef GL_SAMPLE_MASK_INVERT_SGIS
    sample_mask_invert_sgis = GL_SAMPLE_MASK_INVERT_SGIS,
#endif
#ifdef GL_SAMPLE_MASK_SGIS
    sample_mask_sgis = GL_SAMPLE_MASK_SGIS,
#endif
#ifdef GL_SAMPLE_MASK_VALUE_SGIS
    sample_mask_value_sgis = GL_SAMPLE_MASK_VALUE_SGIS,
#endif
#ifdef GL_SAMPLE_PATTERN_SGIS
    sample_pattern_sgis = GL_SAMPLE_PATTERN_SGIS,
#endif
#ifdef GL_SCISSOR_BOX
    scissor_box = GL_SCISSOR_BOX,
#endif
#ifdef GL_SCISSOR_TEST
    scissor_test = GL_SCISSOR_TEST,
#endif
#ifdef GL_SELECTION_BUFFER_SIZE
    selection_buffer_size = GL_SELECTION_BUFFER_SIZE,
#endif
#ifdef GL_SEPARABLE_2D_EXT
    separable_2d_ext = GL_SEPARABLE_2D_EXT,
#endif
#ifdef GL_SHADER_BINARY_FORMATS
    shader_binary_formats = GL_SHADER_BINARY_FORMATS,
#endif
#ifdef GL_SHADER_COMPILER
    shader_compiler = GL_SHADER_COMPILER,
#endif
#ifdef GL_SHADER_STORAGE_BUFFER_BINDING
    shader_storage_buffer_binding = GL_SHADER_STORAGE_BUFFER_BINDING,
#endif
#ifdef GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT
    shader_storage_buffer_offset_alignment =
        GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT,
#endif
#ifdef GL_SHADER_STORAGE_BUFFER_SIZE
    shader_storage_buffer_size = GL_SHADER_STORAGE_BUFFER_SIZE,
#endif
#ifdef GL_SHADER_STORAGE_BUFFER_START
    shader_storage_buffer_start = GL_SHADER_STORAGE_BUFFER_START,
#endif
#ifdef GL_SHADE_MODEL
    shade_model = GL_SHADE_MODEL,
#endif
#ifdef GL_SHADING_RATE_EXT
    shading_rate_ext = GL_SHADING_RATE_EXT,
#endif
#ifdef GL_SHADING_RATE_IMAGE_PALETTE_COUNT_NV
    shading_rate_image_palette_count_nv =
        GL_SHADING_RATE_IMAGE_PALETTE_COUNT_NV,
#endif
#ifdef GL_SHADING_RATE_IMAGE_PER_PRIMITIVE_NV
    shading_rate_image_per_primitive_nv =
        GL_SHADING_RATE_IMAGE_PER_PRIMITIVE_NV,
#endif
#ifdef GL_SHADING_RATE_QCOM
    shading_rate_qcom = GL_SHADING_RATE_QCOM,
#endif
#ifdef GL_SHARED_TEXTURE_PALETTE_EXT
    shared_texture_palette_ext = GL_SHARED_TEXTURE_PALETTE_EXT,
#endif
#ifdef GL_SMOOTH_LINE_WIDTH_GRANULARITY
    smooth_line_width_granularity = GL_SMOOTH_LINE_WIDTH_GRANULARITY,
#endif
#ifdef GL_SMOOTH_LINE_WIDTH_RANGE
    smooth_line_width_range = GL_SMOOTH_LINE_WIDTH_RANGE,
#endif
#ifdef GL_SMOOTH_POINT_SIZE_GRANULARITY
    smooth_point_size_granularity = GL_SMOOTH_POINT_SIZE_GRANULARITY,
#endif
#ifdef GL_SMOOTH_POINT_SIZE_RANGE
    smooth_point_size_range = GL_SMOOTH_POINT_SIZE_RANGE,
#endif
#ifdef GL_SPRITE_AXIS_SGIX
    sprite_axis_sgix = GL_SPRITE_AXIS_SGIX,
#endif
#ifdef GL_SPRITE_MODE_SGIX
    sprite_mode_sgix = GL_SPRITE_MODE_SGIX,
#endif
#ifdef GL_SPRITE_SGIX
    sprite_sgix = GL_SPRITE_SGIX,
#endif
#ifdef GL_SPRITE_TRANSLATION_SGIX
    sprite_translation_sgix = GL_SPRITE_TRANSLATION_SGIX,
#endif
#ifdef GL_STENCIL_BACK_FAIL
    stencil_back_fail = GL_STENCIL_BACK_FAIL,
#endif
#ifdef GL_STENCIL_BACK_FUNC
    stencil_back_func = GL_STENCIL_BACK_FUNC,
#endif
#ifdef GL_STENCIL_BACK_PASS_DEPTH_FAIL
    stencil_back_pass_depth_fail = GL_STENCIL_BACK_PASS_DEPTH_FAIL,
#endif
#ifdef GL_STENCIL_BACK_PASS_DEPTH_PASS
    stencil_back_pass_depth_pass = GL_STENCIL_BACK_PASS_DEPTH_PASS,
#endif
#ifdef GL_STENCIL_BACK_REF
    stencil_back_ref = GL_STENCIL_BACK_REF,
#endif
#ifdef GL_STENCIL_BACK_VALUE_MASK
    stencil_back_value_mask = GL_STENCIL_BACK_VALUE_MASK,
#endif
#ifdef GL_STENCIL_BACK_WRITEMASK
    stencil_back_writemask = GL_STENCIL_BACK_WRITEMASK,
#endif
#ifdef GL_STENCIL_BITS
    stencil_bits = GL_STENCIL_BITS,
#endif
#ifdef GL_STENCIL_CLEAR_VALUE
    stencil_clear_value = GL_STENCIL_CLEAR_VALUE,
#endif
#ifdef GL_STENCIL_FAIL
    stencil_fail = GL_STENCIL_FAIL,
#endif
#ifdef GL_STENCIL_FUNC
    stencil_func = GL_STENCIL_FUNC,
#endif
#ifdef GL_STENCIL_PASS_DEPTH_FAIL
    stencil_pass_depth_fail = GL_STENCIL_PASS_DEPTH_FAIL,
#endif
#ifdef GL_STENCIL_PASS_DEPTH_PASS
    stencil_pass_depth_pass = GL_STENCIL_PASS_DEPTH_PASS,
#endif
#ifdef GL_STENCIL_REF
    stencil_ref = GL_STENCIL_REF,
#endif
#ifdef GL_STENCIL_TEST
    stencil_test = GL_STENCIL_TEST,
#endif
#ifdef GL_STENCIL_VALUE_MASK
    stencil_value_mask = GL_STENCIL_VALUE_MASK,
#endif
#ifdef GL_STENCIL_WRITEMASK
    stencil_writemask = GL_STENCIL_WRITEMASK,
#endif
#ifdef GL_STEREO
    stereo = GL_STEREO,
#endif
#ifdef GL_SUBPIXEL_BITS
    subpixel_bits = GL_SUBPIXEL_BITS,
#endif
#ifdef GL_TEXTURE_1D
    texture_1d = GL_TEXTURE_1D,
#endif
#ifdef GL_TEXTURE_2D
    texture_2d = GL_TEXTURE_2D,
#endif
#ifdef GL_TEXTURE_3D_BINDING_EXT
    texture_3d_binding_ext = GL_TEXTURE_3D_BINDING_EXT,
#endif
#ifdef GL_TEXTURE_3D_EXT
    texture_3d_ext = GL_TEXTURE_3D_EXT,
#endif
#ifdef GL_TEXTURE_4D_BINDING_SGIS
    texture_4d_binding_sgis = GL_TEXTURE_4D_BINDING_SGIS,
#endif
#ifdef GL_TEXTURE_4D_SGIS
    texture_4d_sgis = GL_TEXTURE_4D_SGIS,
#endif
#ifdef GL_TEXTURE_BINDING_1D
    texture_binding_1d = GL_TEXTURE_BINDING_1D,
#endif
#ifdef GL_TEXTURE_BINDING_1D_ARRAY
    texture_binding_1d_array = GL_TEXTURE_BINDING_1D_ARRAY,
#endif
#ifdef GL_TEXTURE_BINDING_2D
    texture_binding_2d = GL_TEXTURE_BINDING_2D,
#endif
#ifdef GL_TEXTURE_BINDING_2D_ARRAY
    texture_binding_2d_array = GL_TEXTURE_BINDING_2D_ARRAY,
#endif
#ifdef GL_TEXTURE_BINDING_2D_MULTISAMPLE
    texture_binding_2d_multisample = GL_TEXTURE_BINDING_2D_MULTISAMPLE,
#endif
#ifdef GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY
    texture_binding_2d_multisample_array =
        GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY,
#endif
#ifdef GL_TEXTURE_BINDING_3D
    texture_binding_3d = GL_TEXTURE_BINDING_3D,
#endif
#ifdef GL_TEXTURE_BINDING_BUFFER
    texture_binding_buffer = GL_TEXTURE_BINDING_BUFFER,
#endif
#ifdef GL_TEXTURE_BINDING_CUBE_MAP
    texture_binding_cube_map = GL_TEXTURE_BINDING_CUBE_MAP,
#endif
#ifdef GL_TEXTURE_BINDING_CUBE_MAP_ARB
    texture_binding_cube_map_arb = GL_TEXTURE_BINDING_CUBE_MAP_ARB,
#endif
#ifdef GL_TEXTURE_BINDING_CUBE_MAP_EXT
    texture_binding_cube_map_ext = GL_TEXTURE_BINDING_CUBE_MAP_EXT,
#endif
#ifdef GL_TEXTURE_BINDING_CUBE_MAP_OES
    texture_binding_cube_map_oes = GL_TEXTURE_BINDING_CUBE_MAP_OES,
#endif
#ifdef GL_TEXTURE_BINDING_RECTANGLE
    texture_binding_rectangle = GL_TEXTURE_BINDING_RECTANGLE,
#endif
#ifdef GL_TEXTURE_BINDING_RECTANGLE_ARB
    texture_binding_rectangle_arb = GL_TEXTURE_BINDING_RECTANGLE_ARB,
#endif
#ifdef GL_TEXTURE_BINDING_RECTANGLE_NV
    texture_binding_rectangle_nv = GL_TEXTURE_BINDING_RECTANGLE_NV,
#endif
#ifdef GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT
    texture_buffer_offset_alignment = GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT,
#endif
#ifdef GL_TEXTURE_COLOR_TABLE_SGI
    texture_color_table_sgi = GL_TEXTURE_COLOR_TABLE_SGI,
#endif
#ifdef GL_TEXTURE_COMPRESSION_HINT
    texture_compression_hint = GL_TEXTURE_COMPRESSION_HINT,
#endif
#ifdef GL_TEXTURE_COORD_ARRAY
    texture_coord_array = GL_TEXTURE_COORD_ARRAY,
#endif
#ifdef GL_TEXTURE_COORD_ARRAY_COUNT_EXT
    texture_coord_array_count_ext = GL_TEXTURE_COORD_ARRAY_COUNT_EXT,
#endif
#ifdef GL_TEXTURE_COORD_ARRAY_SIZE
    texture_coord_array_size = GL_TEXTURE_COORD_ARRAY_SIZE,
#endif
#ifdef GL_TEXTURE_COORD_ARRAY_STRIDE
    texture_coord_array_stride = GL_TEXTURE_COORD_ARRAY_STRIDE,
#endif
#ifdef GL_TEXTURE_COORD_ARRAY_TYPE
    texture_coord_array_type = GL_TEXTURE_COORD_ARRAY_TYPE,
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
#ifdef GL_TEXTURE_GEN_STR_OES
    texture_gen_str_oes = GL_TEXTURE_GEN_STR_OES,
#endif
#ifdef GL_TEXTURE_GEN_T
    texture_gen_t = GL_TEXTURE_GEN_T,
#endif
#ifdef GL_TEXTURE_MATRIX
    texture_matrix = GL_TEXTURE_MATRIX,
#endif
#ifdef GL_TEXTURE_STACK_DEPTH
    texture_stack_depth = GL_TEXTURE_STACK_DEPTH,
#endif
#ifdef GL_TIMESTAMP
    timestamp = GL_TIMESTAMP,
#endif
#ifdef GL_TIMESTAMP_EXT
    timestamp_ext = GL_TIMESTAMP_EXT,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BUFFER_BINDING
    transform_feedback_buffer_binding = GL_TRANSFORM_FEEDBACK_BUFFER_BINDING,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BUFFER_SIZE
    transform_feedback_buffer_size = GL_TRANSFORM_FEEDBACK_BUFFER_SIZE,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BUFFER_START
    transform_feedback_buffer_start = GL_TRANSFORM_FEEDBACK_BUFFER_START,
#endif
#ifdef GL_UNIFORM_BUFFER_BINDING
    uniform_buffer_binding = GL_UNIFORM_BUFFER_BINDING,
#endif
#ifdef GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT
    uniform_buffer_offset_alignment = GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT,
#endif
#ifdef GL_UNIFORM_BUFFER_SIZE
    uniform_buffer_size = GL_UNIFORM_BUFFER_SIZE,
#endif
#ifdef GL_UNIFORM_BUFFER_START
    uniform_buffer_start = GL_UNIFORM_BUFFER_START,
#endif
#ifdef GL_UNPACK_ALIGNMENT
    unpack_alignment = GL_UNPACK_ALIGNMENT,
#endif
#ifdef GL_UNPACK_CMYK_HINT_EXT
    unpack_cmyk_hint_ext = GL_UNPACK_CMYK_HINT_EXT,
#endif
#ifdef GL_UNPACK_IMAGE_DEPTH_SGIS
    unpack_image_depth_sgis = GL_UNPACK_IMAGE_DEPTH_SGIS,
#endif
#ifdef GL_UNPACK_IMAGE_HEIGHT
    unpack_image_height = GL_UNPACK_IMAGE_HEIGHT,
#endif
#ifdef GL_UNPACK_IMAGE_HEIGHT_EXT
    unpack_image_height_ext = GL_UNPACK_IMAGE_HEIGHT_EXT,
#endif
#ifdef GL_UNPACK_LSB_FIRST
    unpack_lsb_first = GL_UNPACK_LSB_FIRST,
#endif
#ifdef GL_UNPACK_RESAMPLE_SGIX
    unpack_resample_sgix = GL_UNPACK_RESAMPLE_SGIX,
#endif
#ifdef GL_UNPACK_ROW_LENGTH
    unpack_row_length = GL_UNPACK_ROW_LENGTH,
#endif
#ifdef GL_UNPACK_SKIP_IMAGES
    unpack_skip_images = GL_UNPACK_SKIP_IMAGES,
#endif
#ifdef GL_UNPACK_SKIP_IMAGES_EXT
    unpack_skip_images_ext = GL_UNPACK_SKIP_IMAGES_EXT,
#endif
#ifdef GL_UNPACK_SKIP_PIXELS
    unpack_skip_pixels = GL_UNPACK_SKIP_PIXELS,
#endif
#ifdef GL_UNPACK_SKIP_ROWS
    unpack_skip_rows = GL_UNPACK_SKIP_ROWS,
#endif
#ifdef GL_UNPACK_SKIP_VOLUMES_SGIS
    unpack_skip_volumes_sgis = GL_UNPACK_SKIP_VOLUMES_SGIS,
#endif
#ifdef GL_UNPACK_SUBSAMPLE_RATE_SGIX
    unpack_subsample_rate_sgix = GL_UNPACK_SUBSAMPLE_RATE_SGIX,
#endif
#ifdef GL_UNPACK_SWAP_BYTES
    unpack_swap_bytes = GL_UNPACK_SWAP_BYTES,
#endif
#ifdef GL_VERTEX_ARRAY
    vertex_array = GL_VERTEX_ARRAY,
#endif
#ifdef GL_VERTEX_ARRAY_BINDING
    vertex_array_binding = GL_VERTEX_ARRAY_BINDING,
#endif
#ifdef GL_VERTEX_ARRAY_COUNT_EXT
    vertex_array_count_ext = GL_VERTEX_ARRAY_COUNT_EXT,
#endif
#ifdef GL_VERTEX_ARRAY_SIZE
    vertex_array_size = GL_VERTEX_ARRAY_SIZE,
#endif
#ifdef GL_VERTEX_ARRAY_STRIDE
    vertex_array_stride = GL_VERTEX_ARRAY_STRIDE,
#endif
#ifdef GL_VERTEX_ARRAY_TYPE
    vertex_array_type = GL_VERTEX_ARRAY_TYPE,
#endif
#ifdef GL_VERTEX_BINDING_DIVISOR
    vertex_binding_divisor = GL_VERTEX_BINDING_DIVISOR,
#endif
#ifdef GL_VERTEX_BINDING_OFFSET
    vertex_binding_offset = GL_VERTEX_BINDING_OFFSET,
#endif
#ifdef GL_VERTEX_BINDING_STRIDE
    vertex_binding_stride = GL_VERTEX_BINDING_STRIDE,
#endif
#ifdef GL_VERTEX_PRECLIP_HINT_SGIX
    vertex_preclip_hint_sgix = GL_VERTEX_PRECLIP_HINT_SGIX,
#endif
#ifdef GL_VERTEX_PRECLIP_SGIX
    vertex_preclip_sgix = GL_VERTEX_PRECLIP_SGIX,
#endif
#ifdef GL_VIEWPORT
    viewport = GL_VIEWPORT,
#endif
#ifdef GL_VIEWPORT_BOUNDS_RANGE
    viewport_bounds_range = GL_VIEWPORT_BOUNDS_RANGE,
#endif
#ifdef GL_VIEWPORT_INDEX_PROVOKING_VERTEX
    viewport_index_provoking_vertex = GL_VIEWPORT_INDEX_PROVOKING_VERTEX,
#endif
#ifdef GL_VIEWPORT_SUBPIXEL_BITS
    viewport_subpixel_bits = GL_VIEWPORT_SUBPIXEL_BITS,
#endif
#ifdef GL_ZOOM_X
    zoom_x = GL_ZOOM_X,
#endif
#ifdef GL_ZOOM_Y
    zoom_y = GL_ZOOM_Y,
#endif
}; // enum class get_prop

} // namespace gl::group
