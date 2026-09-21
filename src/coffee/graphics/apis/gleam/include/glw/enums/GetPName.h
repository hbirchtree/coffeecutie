#pragma once

#include "common.h"

namespace gl::group {

// GetPName
enum class get_prop : u32
{
    accum_alpha_bits              = 0x0D5B, // GL_ACCUM_ALPHA_BITS
    accum_blue_bits               = 0x0D5A, // GL_ACCUM_BLUE_BITS
    accum_clear_value             = 0x0B80, // GL_ACCUM_CLEAR_VALUE
    accum_green_bits              = 0x0D59, // GL_ACCUM_GREEN_BITS
    accum_red_bits                = 0x0D58, // GL_ACCUM_RED_BITS
    active_texture                = 0x84E0, // GL_ACTIVE_TEXTURE
    aliased_line_width_range      = 0x846E, // GL_ALIASED_LINE_WIDTH_RANGE
    aliased_point_size_range      = 0x846D, // GL_ALIASED_POINT_SIZE_RANGE
    alpha_bias                    = 0x0D1D, // GL_ALPHA_BIAS
    alpha_bits                    = 0x0D55, // GL_ALPHA_BITS
    alpha_scale                   = 0x0D1C, // GL_ALPHA_SCALE
    alpha_test                    = 0x0BC0, // GL_ALPHA_TEST
    alpha_test_func               = 0x0BC1, // GL_ALPHA_TEST_FUNC
    alpha_test_ref                = 0x0BC2, // GL_ALPHA_TEST_REF
    array_buffer_binding          = 0x8894, // GL_ARRAY_BUFFER_BINDING
    attrib_stack_depth            = 0x0BB0, // GL_ATTRIB_STACK_DEPTH
    auto_normal                   = 0x0D80, // GL_AUTO_NORMAL
    aux_buffers                   = 0x0C00, // GL_AUX_BUFFERS
    blend                         = 0x0BE2, // GL_BLEND
    blend_color                   = 0x8005, // GL_BLEND_COLOR
    blend_dst                     = 0x0BE0, // GL_BLEND_DST
    blend_dst_alpha               = 0x80CA, // GL_BLEND_DST_ALPHA
    blend_dst_rgb                 = 0x80C8, // GL_BLEND_DST_RGB
    blend_equation                = 0x8009, // GL_BLEND_EQUATION
    blend_equation_alpha          = 0x883D, // GL_BLEND_EQUATION_ALPHA
    blend_equation_rgb            = 0x8009, // GL_BLEND_EQUATION_RGB
    blend_src                     = 0x0BE1, // GL_BLEND_SRC
    blend_src_alpha               = 0x80CB, // GL_BLEND_SRC_ALPHA
    blend_src_rgb                 = 0x80C9, // GL_BLEND_SRC_RGB
    blue_bias                     = 0x0D1B, // GL_BLUE_BIAS
    blue_bits                     = 0x0D54, // GL_BLUE_BITS
    blue_scale                    = 0x0D1A, // GL_BLUE_SCALE
    client_attrib_stack_depth     = 0x0BB1, // GL_CLIENT_ATTRIB_STACK_DEPTH
    clip_plane0                   = 0x3000, // GL_CLIP_PLANE0
    clip_plane1                   = 0x3001, // GL_CLIP_PLANE1
    clip_plane2                   = 0x3002, // GL_CLIP_PLANE2
    clip_plane3                   = 0x3003, // GL_CLIP_PLANE3
    clip_plane4                   = 0x3004, // GL_CLIP_PLANE4
    clip_plane5                   = 0x3005, // GL_CLIP_PLANE5
    color_array                   = 0x8076, // GL_COLOR_ARRAY
    color_array_size              = 0x8081, // GL_COLOR_ARRAY_SIZE
    color_array_stride            = 0x8083, // GL_COLOR_ARRAY_STRIDE
    color_array_type              = 0x8082, // GL_COLOR_ARRAY_TYPE
    color_clear_value             = 0x0C22, // GL_COLOR_CLEAR_VALUE
    color_logic_op                = 0x0BF2, // GL_COLOR_LOGIC_OP
    color_material                = 0x0B57, // GL_COLOR_MATERIAL
    color_material_face           = 0x0B55, // GL_COLOR_MATERIAL_FACE
    color_material_parameter      = 0x0B56, // GL_COLOR_MATERIAL_PARAMETER
    color_writemask               = 0x0C23, // GL_COLOR_WRITEMASK
    compressed_texture_formats    = 0x86A3, // GL_COMPRESSED_TEXTURE_FORMATS
    context_flags                 = 0x821E, // GL_CONTEXT_FLAGS
    context_profile_mask          = 0x9126, // GL_CONTEXT_PROFILE_MASK
    cull_face                     = 0x0B44, // GL_CULL_FACE
    cull_face_mode                = 0x0B45, // GL_CULL_FACE_MODE
    current_color                 = 0x0B00, // GL_CURRENT_COLOR
    current_index                 = 0x0B01, // GL_CURRENT_INDEX
    current_normal                = 0x0B02, // GL_CURRENT_NORMAL
    current_program               = 0x8B8D, // GL_CURRENT_PROGRAM
    current_raster_color          = 0x0B04, // GL_CURRENT_RASTER_COLOR
    current_raster_distance       = 0x0B09, // GL_CURRENT_RASTER_DISTANCE
    current_raster_index          = 0x0B05, // GL_CURRENT_RASTER_INDEX
    current_raster_position       = 0x0B07, // GL_CURRENT_RASTER_POSITION
    current_raster_position_valid = 0x0B08, // GL_CURRENT_RASTER_POSITION_VALID
    current_raster_texture_coords = 0x0B06, // GL_CURRENT_RASTER_TEXTURE_COORDS
    current_texture_coords        = 0x0B03, // GL_CURRENT_TEXTURE_COORDS
    debug_group_stack_depth       = 0x826D, // GL_DEBUG_GROUP_STACK_DEPTH
    depth_bias                    = 0x0D1F, // GL_DEPTH_BIAS
    depth_bits                    = 0x0D56, // GL_DEPTH_BITS
    depth_clear_value             = 0x0B73, // GL_DEPTH_CLEAR_VALUE
    depth_func                    = 0x0B74, // GL_DEPTH_FUNC
    depth_range                   = 0x0B70, // GL_DEPTH_RANGE
    depth_scale                   = 0x0D1E, // GL_DEPTH_SCALE
    depth_test                    = 0x0B71, // GL_DEPTH_TEST
    depth_writemask               = 0x0B72, // GL_DEPTH_WRITEMASK
    dispatch_indirect_buffer_binding =
        0x90EF, // GL_DISPATCH_INDIRECT_BUFFER_BINDING
    dither                       = 0x0BD0, // GL_DITHER
    doublebuffer                 = 0x0C32, // GL_DOUBLEBUFFER
    draw_buffer                  = 0x0C01, // GL_DRAW_BUFFER
    draw_framebuffer_binding     = 0x8CA6, // GL_DRAW_FRAMEBUFFER_BINDING
    edge_flag                    = 0x0B43, // GL_EDGE_FLAG
    edge_flag_array              = 0x8079, // GL_EDGE_FLAG_ARRAY
    edge_flag_array_stride       = 0x808C, // GL_EDGE_FLAG_ARRAY_STRIDE
    element_array_buffer_binding = 0x8895, // GL_ELEMENT_ARRAY_BUFFER_BINDING
    feedback_buffer_size         = 0x0DF1, // GL_FEEDBACK_BUFFER_SIZE
    feedback_buffer_type         = 0x0DF2, // GL_FEEDBACK_BUFFER_TYPE
    fog                          = 0x0B60, // GL_FOG
    fog_color                    = 0x0B66, // GL_FOG_COLOR
    fog_density                  = 0x0B62, // GL_FOG_DENSITY
    fog_end                      = 0x0B64, // GL_FOG_END
    fog_hint                     = 0x0C54, // GL_FOG_HINT
    fog_index                    = 0x0B61, // GL_FOG_INDEX
    fog_mode                     = 0x0B65, // GL_FOG_MODE
    fog_start                    = 0x0B63, // GL_FOG_START
    fragment_shader_derivative_hint =
        0x8B8B,                   // GL_FRAGMENT_SHADER_DERIVATIVE_HINT
    framebuffer_binding = 0x8CA6, // GL_FRAMEBUFFER_BINDING
    front_face          = 0x0B46, // GL_FRONT_FACE
    green_bias          = 0x0D19, // GL_GREEN_BIAS
    green_bits          = 0x0D53, // GL_GREEN_BITS
    green_scale         = 0x0D18, // GL_GREEN_SCALE
    implementation_color_read_format =
        0x8B9B, // GL_IMPLEMENTATION_COLOR_READ_FORMAT
    implementation_color_read_type =
        0x8B9A,                             // GL_IMPLEMENTATION_COLOR_READ_TYPE
    index_array                   = 0x8077, // GL_INDEX_ARRAY
    index_array_stride            = 0x8086, // GL_INDEX_ARRAY_STRIDE
    index_array_type              = 0x8085, // GL_INDEX_ARRAY_TYPE
    index_bits                    = 0x0D51, // GL_INDEX_BITS
    index_clear_value             = 0x0C20, // GL_INDEX_CLEAR_VALUE
    index_logic_op                = 0x0BF1, // GL_INDEX_LOGIC_OP
    index_mode                    = 0x0C30, // GL_INDEX_MODE
    index_offset                  = 0x0D13, // GL_INDEX_OFFSET
    index_shift                   = 0x0D12, // GL_INDEX_SHIFT
    index_writemask               = 0x0C21, // GL_INDEX_WRITEMASK
    layer_provoking_vertex        = 0x825E, // GL_LAYER_PROVOKING_VERTEX
    light0                        = 0x4000, // GL_LIGHT0
    light1                        = 0x4001, // GL_LIGHT1
    light2                        = 0x4002, // GL_LIGHT2
    light3                        = 0x4003, // GL_LIGHT3
    light4                        = 0x4004, // GL_LIGHT4
    light5                        = 0x4005, // GL_LIGHT5
    light6                        = 0x4006, // GL_LIGHT6
    light7                        = 0x4007, // GL_LIGHT7
    lighting                      = 0x0B50, // GL_LIGHTING
    light_model_ambient           = 0x0B53, // GL_LIGHT_MODEL_AMBIENT
    light_model_color_control     = 0x81F8, // GL_LIGHT_MODEL_COLOR_CONTROL
    light_model_local_viewer      = 0x0B51, // GL_LIGHT_MODEL_LOCAL_VIEWER
    light_model_two_side          = 0x0B52, // GL_LIGHT_MODEL_TWO_SIDE
    line_smooth                   = 0x0B20, // GL_LINE_SMOOTH
    line_smooth_hint              = 0x0C52, // GL_LINE_SMOOTH_HINT
    line_stipple                  = 0x0B24, // GL_LINE_STIPPLE
    line_stipple_pattern          = 0x0B25, // GL_LINE_STIPPLE_PATTERN
    line_stipple_repeat           = 0x0B26, // GL_LINE_STIPPLE_REPEAT
    line_width                    = 0x0B21, // GL_LINE_WIDTH
    line_width_granularity        = 0x0B23, // GL_LINE_WIDTH_GRANULARITY
    line_width_range              = 0x0B22, // GL_LINE_WIDTH_RANGE
    list_base                     = 0x0B32, // GL_LIST_BASE
    list_index                    = 0x0B33, // GL_LIST_INDEX
    list_mode                     = 0x0B30, // GL_LIST_MODE
    logic_op                      = 0x0BF1, // GL_LOGIC_OP
    logic_op_mode                 = 0x0BF0, // GL_LOGIC_OP_MODE
    major_version                 = 0x821B, // GL_MAJOR_VERSION
    map1_color_4                  = 0x0D90, // GL_MAP1_COLOR_4
    map1_grid_domain              = 0x0DD0, // GL_MAP1_GRID_DOMAIN
    map1_grid_segments            = 0x0DD1, // GL_MAP1_GRID_SEGMENTS
    map1_index                    = 0x0D91, // GL_MAP1_INDEX
    map1_normal                   = 0x0D92, // GL_MAP1_NORMAL
    map1_texture_coord_1          = 0x0D93, // GL_MAP1_TEXTURE_COORD_1
    map1_texture_coord_2          = 0x0D94, // GL_MAP1_TEXTURE_COORD_2
    map1_texture_coord_3          = 0x0D95, // GL_MAP1_TEXTURE_COORD_3
    map1_texture_coord_4          = 0x0D96, // GL_MAP1_TEXTURE_COORD_4
    map1_vertex_3                 = 0x0D97, // GL_MAP1_VERTEX_3
    map1_vertex_4                 = 0x0D98, // GL_MAP1_VERTEX_4
    map2_color_4                  = 0x0DB0, // GL_MAP2_COLOR_4
    map2_grid_domain              = 0x0DD2, // GL_MAP2_GRID_DOMAIN
    map2_grid_segments            = 0x0DD3, // GL_MAP2_GRID_SEGMENTS
    map2_index                    = 0x0DB1, // GL_MAP2_INDEX
    map2_normal                   = 0x0DB2, // GL_MAP2_NORMAL
    map2_texture_coord_1          = 0x0DB3, // GL_MAP2_TEXTURE_COORD_1
    map2_texture_coord_2          = 0x0DB4, // GL_MAP2_TEXTURE_COORD_2
    map2_texture_coord_3          = 0x0DB5, // GL_MAP2_TEXTURE_COORD_3
    map2_texture_coord_4          = 0x0DB6, // GL_MAP2_TEXTURE_COORD_4
    map2_vertex_3                 = 0x0DB7, // GL_MAP2_VERTEX_3
    map2_vertex_4                 = 0x0DB8, // GL_MAP2_VERTEX_4
    map_color                     = 0x0D10, // GL_MAP_COLOR
    map_stencil                   = 0x0D11, // GL_MAP_STENCIL
    matrix_mode                   = 0x0BA0, // GL_MATRIX_MODE
    max_3d_texture_size           = 0x8073, // GL_MAX_3D_TEXTURE_SIZE
    max_array_texture_layers      = 0x88FF, // GL_MAX_ARRAY_TEXTURE_LAYERS
    max_attrib_stack_depth        = 0x0D35, // GL_MAX_ATTRIB_STACK_DEPTH
    max_client_attrib_stack_depth = 0x0D3B, // GL_MAX_CLIENT_ATTRIB_STACK_DEPTH
    max_clip_distances            = 0x0D32, // GL_MAX_CLIP_DISTANCES
    max_clip_planes               = 0x0D32, // GL_MAX_CLIP_PLANES
    max_color_attachments         = 0x8CDF, // GL_MAX_COLOR_ATTACHMENTS
    max_color_texture_samples     = 0x910E, // GL_MAX_COLOR_TEXTURE_SAMPLES
    max_combined_atomic_counters  = 0x92D7, // GL_MAX_COMBINED_ATOMIC_COUNTERS
    max_combined_compute_uniform_components =
        0x8266, // GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS
    max_combined_fragment_uniform_components =
        0x8A33, // GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS
    max_combined_geometry_uniform_components =
        0x8A32, // GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS
    max_combined_shader_storage_blocks =
        0x90DC, // GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS
    max_combined_texture_image_units =
        0x8B4D,                           // GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS
    max_combined_uniform_blocks = 0x8A2E, // GL_MAX_COMBINED_UNIFORM_BLOCKS
    max_combined_vertex_uniform_components =
        0x8A31, // GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS
    max_compute_atomic_counters = 0x8265, // GL_MAX_COMPUTE_ATOMIC_COUNTERS
    max_compute_atomic_counter_buffers =
        0x8264, // GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS
    max_compute_shader_storage_blocks =
        0x90DB, // GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS
    max_compute_texture_image_units =
        0x91BC,                          // GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS
    max_compute_uniform_blocks = 0x91BB, // GL_MAX_COMPUTE_UNIFORM_BLOCKS
    max_compute_uniform_components =
        0x8263,                            // GL_MAX_COMPUTE_UNIFORM_COMPONENTS
    max_compute_work_group_count = 0x91BE, // GL_MAX_COMPUTE_WORK_GROUP_COUNT
    max_compute_work_group_invocations =
        0x90EB, // GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS
    max_compute_work_group_size   = 0x91BF, // GL_MAX_COMPUTE_WORK_GROUP_SIZE
    max_cube_map_texture_size     = 0x851C, // GL_MAX_CUBE_MAP_TEXTURE_SIZE
    max_debug_group_stack_depth   = 0x826C, // GL_MAX_DEBUG_GROUP_STACK_DEPTH
    max_depth_texture_samples     = 0x910F, // GL_MAX_DEPTH_TEXTURE_SAMPLES
    max_draw_buffers              = 0x8824, // GL_MAX_DRAW_BUFFERS
    max_dual_source_draw_buffers  = 0x88FC, // GL_MAX_DUAL_SOURCE_DRAW_BUFFERS
    max_elements_indices          = 0x80E9, // GL_MAX_ELEMENTS_INDICES
    max_elements_vertices         = 0x80E8, // GL_MAX_ELEMENTS_VERTICES
    max_element_index             = 0x8D6B, // GL_MAX_ELEMENT_INDEX
    max_eval_order                = 0x0D30, // GL_MAX_EVAL_ORDER
    max_fragment_atomic_counters  = 0x92D6, // GL_MAX_FRAGMENT_ATOMIC_COUNTERS
    max_fragment_input_components = 0x9125, // GL_MAX_FRAGMENT_INPUT_COMPONENTS
    max_fragment_shader_storage_blocks =
        0x90DA, // GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS
    max_fragment_uniform_blocks = 0x8A2D, // GL_MAX_FRAGMENT_UNIFORM_BLOCKS
    max_fragment_uniform_components =
        0x8B49, // GL_MAX_FRAGMENT_UNIFORM_COMPONENTS
    max_fragment_uniform_vectors  = 0x8DFD, // GL_MAX_FRAGMENT_UNIFORM_VECTORS
    max_framebuffer_height        = 0x9316, // GL_MAX_FRAMEBUFFER_HEIGHT
    max_framebuffer_layers        = 0x9317, // GL_MAX_FRAMEBUFFER_LAYERS
    max_framebuffer_samples       = 0x9318, // GL_MAX_FRAMEBUFFER_SAMPLES
    max_framebuffer_width         = 0x9315, // GL_MAX_FRAMEBUFFER_WIDTH
    max_geometry_atomic_counters  = 0x92D5, // GL_MAX_GEOMETRY_ATOMIC_COUNTERS
    max_geometry_input_components = 0x9123, // GL_MAX_GEOMETRY_INPUT_COMPONENTS
    max_geometry_output_components =
        0x9124, // GL_MAX_GEOMETRY_OUTPUT_COMPONENTS
    max_geometry_shader_storage_blocks =
        0x90D7, // GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS
    max_geometry_texture_image_units =
        0x8C29,                           // GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS
    max_geometry_uniform_blocks = 0x8A2C, // GL_MAX_GEOMETRY_UNIFORM_BLOCKS
    max_geometry_uniform_components =
        0x8DDF,                          // GL_MAX_GEOMETRY_UNIFORM_COMPONENTS
    max_integer_samples        = 0x9110, // GL_MAX_INTEGER_SAMPLES
    max_label_length           = 0x82E8, // GL_MAX_LABEL_LENGTH
    max_lights                 = 0x0D31, // GL_MAX_LIGHTS
    max_list_nesting           = 0x0B31, // GL_MAX_LIST_NESTING
    max_modelview_stack_depth  = 0x0D36, // GL_MAX_MODELVIEW_STACK_DEPTH
    max_name_stack_depth       = 0x0D37, // GL_MAX_NAME_STACK_DEPTH
    max_pixel_map_table        = 0x0D34, // GL_MAX_PIXEL_MAP_TABLE
    max_program_texel_offset   = 0x8905, // GL_MAX_PROGRAM_TEXEL_OFFSET
    max_projection_stack_depth = 0x0D38, // GL_MAX_PROJECTION_STACK_DEPTH
    max_rectangle_texture_size = 0x84F8, // GL_MAX_RECTANGLE_TEXTURE_SIZE
    max_renderbuffer_size      = 0x84E8, // GL_MAX_RENDERBUFFER_SIZE
    max_sample_mask_words      = 0x8E59, // GL_MAX_SAMPLE_MASK_WORDS
    max_server_wait_timeout    = 0x9111, // GL_MAX_SERVER_WAIT_TIMEOUT
    max_shader_storage_buffer_bindings =
        0x90DD, // GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS
    max_tess_control_atomic_counters =
        0x92D3, // GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS
    max_tess_control_shader_storage_blocks =
        0x90D8, // GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS
    max_tess_control_uniform_blocks =
        0x8E89, // GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS
    max_tess_evaluation_atomic_counters =
        0x92D4, // GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS
    max_tess_evaluation_shader_storage_blocks =
        0x90D9, // GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS
    max_tess_evaluation_uniform_blocks =
        0x8E8A, // GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS
    max_texture_buffer_size     = 0x8C2B, // GL_MAX_TEXTURE_BUFFER_SIZE
    max_texture_image_units     = 0x8872, // GL_MAX_TEXTURE_IMAGE_UNITS
    max_texture_lod_bias        = 0x84FD, // GL_MAX_TEXTURE_LOD_BIAS
    max_texture_max_anisotropy  = 0x84FF, // GL_MAX_TEXTURE_MAX_ANISOTROPY
    max_texture_size            = 0x0D33, // GL_MAX_TEXTURE_SIZE
    max_texture_stack_depth     = 0x0D39, // GL_MAX_TEXTURE_STACK_DEPTH
    max_uniform_block_size      = 0x8A30, // GL_MAX_UNIFORM_BLOCK_SIZE
    max_uniform_buffer_bindings = 0x8A2F, // GL_MAX_UNIFORM_BUFFER_BINDINGS
    max_uniform_locations       = 0x826E, // GL_MAX_UNIFORM_LOCATIONS
    max_varying_components      = 0x8B4B, // GL_MAX_VARYING_COMPONENTS
    max_varying_floats          = 0x8B4B, // GL_MAX_VARYING_FLOATS
    max_varying_vectors         = 0x8DFC, // GL_MAX_VARYING_VECTORS
    max_vertex_atomic_counters  = 0x92D2, // GL_MAX_VERTEX_ATOMIC_COUNTERS
    max_vertex_attribs          = 0x8869, // GL_MAX_VERTEX_ATTRIBS
    max_vertex_attrib_bindings  = 0x82DA, // GL_MAX_VERTEX_ATTRIB_BINDINGS
    max_vertex_attrib_relative_offset =
        0x82D9, // GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET
    max_vertex_output_components = 0x9122, // GL_MAX_VERTEX_OUTPUT_COMPONENTS
    max_vertex_shader_storage_blocks =
        0x90D6, // GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS
    max_vertex_texture_image_units =
        0x8B4C,                             // GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS
    max_vertex_uniform_blocks     = 0x8A2B, // GL_MAX_VERTEX_UNIFORM_BLOCKS
    max_vertex_uniform_components = 0x8B4A, // GL_MAX_VERTEX_UNIFORM_COMPONENTS
    max_vertex_uniform_vectors    = 0x8DFB, // GL_MAX_VERTEX_UNIFORM_VECTORS
    max_viewports                 = 0x825B, // GL_MAX_VIEWPORTS
    max_viewport_dims             = 0x0D3A, // GL_MAX_VIEWPORT_DIMS
    minor_version                 = 0x821C, // GL_MINOR_VERSION
    min_map_buffer_alignment      = 0x90BC, // GL_MIN_MAP_BUFFER_ALIGNMENT
    min_program_texel_offset      = 0x8904, // GL_MIN_PROGRAM_TEXEL_OFFSET
    modelview_matrix              = 0x0BA6, // GL_MODELVIEW_MATRIX
    modelview_stack_depth         = 0x0BA3, // GL_MODELVIEW_STACK_DEPTH
    name_stack_depth              = 0x0D70, // GL_NAME_STACK_DEPTH
    normalize                     = 0x0BA1, // GL_NORMALIZE
    normal_array                  = 0x8075, // GL_NORMAL_ARRAY
    normal_array_stride           = 0x807F, // GL_NORMAL_ARRAY_STRIDE
    normal_array_type             = 0x807E, // GL_NORMAL_ARRAY_TYPE
    num_compressed_texture_formats =
        0x86A2,                             // GL_NUM_COMPRESSED_TEXTURE_FORMATS
    num_extensions                = 0x821D, // GL_NUM_EXTENSIONS
    num_program_binary_formats    = 0x87FE, // GL_NUM_PROGRAM_BINARY_FORMATS
    num_shader_binary_formats     = 0x8DF9, // GL_NUM_SHADER_BINARY_FORMATS
    pack_alignment                = 0x0D05, // GL_PACK_ALIGNMENT
    pack_image_height             = 0x806C, // GL_PACK_IMAGE_HEIGHT
    pack_lsb_first                = 0x0D01, // GL_PACK_LSB_FIRST
    pack_row_length               = 0x0D02, // GL_PACK_ROW_LENGTH
    pack_skip_images              = 0x806B, // GL_PACK_SKIP_IMAGES
    pack_skip_pixels              = 0x0D04, // GL_PACK_SKIP_PIXELS
    pack_skip_rows                = 0x0D03, // GL_PACK_SKIP_ROWS
    pack_swap_bytes               = 0x0D00, // GL_PACK_SWAP_BYTES
    perspective_correction_hint   = 0x0C50, // GL_PERSPECTIVE_CORRECTION_HINT
    pixel_map_a_to_a_size         = 0x0CB9, // GL_PIXEL_MAP_A_TO_A_SIZE
    pixel_map_b_to_b_size         = 0x0CB8, // GL_PIXEL_MAP_B_TO_B_SIZE
    pixel_map_g_to_g_size         = 0x0CB7, // GL_PIXEL_MAP_G_TO_G_SIZE
    pixel_map_i_to_a_size         = 0x0CB5, // GL_PIXEL_MAP_I_TO_A_SIZE
    pixel_map_i_to_b_size         = 0x0CB4, // GL_PIXEL_MAP_I_TO_B_SIZE
    pixel_map_i_to_g_size         = 0x0CB3, // GL_PIXEL_MAP_I_TO_G_SIZE
    pixel_map_i_to_i_size         = 0x0CB0, // GL_PIXEL_MAP_I_TO_I_SIZE
    pixel_map_i_to_r_size         = 0x0CB2, // GL_PIXEL_MAP_I_TO_R_SIZE
    pixel_map_r_to_r_size         = 0x0CB6, // GL_PIXEL_MAP_R_TO_R_SIZE
    pixel_map_s_to_s_size         = 0x0CB1, // GL_PIXEL_MAP_S_TO_S_SIZE
    pixel_pack_buffer_binding     = 0x88ED, // GL_PIXEL_PACK_BUFFER_BINDING
    pixel_unpack_buffer_binding   = 0x88EF, // GL_PIXEL_UNPACK_BUFFER_BINDING
    point_distance_attenuation    = 0x8129, // GL_POINT_DISTANCE_ATTENUATION
    point_fade_threshold_size     = 0x8128, // GL_POINT_FADE_THRESHOLD_SIZE
    point_size                    = 0x0B11, // GL_POINT_SIZE
    point_size_granularity        = 0x0B13, // GL_POINT_SIZE_GRANULARITY
    point_size_max                = 0x8127, // GL_POINT_SIZE_MAX
    point_size_min                = 0x8126, // GL_POINT_SIZE_MIN
    point_size_range              = 0x0B12, // GL_POINT_SIZE_RANGE
    point_smooth                  = 0x0B10, // GL_POINT_SMOOTH
    point_smooth_hint             = 0x0C51, // GL_POINT_SMOOTH_HINT
    polygon_mode                  = 0x0B40, // GL_POLYGON_MODE
    polygon_offset_factor         = 0x8038, // GL_POLYGON_OFFSET_FACTOR
    polygon_offset_fill           = 0x8037, // GL_POLYGON_OFFSET_FILL
    polygon_offset_line           = 0x2A02, // GL_POLYGON_OFFSET_LINE
    polygon_offset_point          = 0x2A01, // GL_POLYGON_OFFSET_POINT
    polygon_offset_units          = 0x2A00, // GL_POLYGON_OFFSET_UNITS
    polygon_smooth                = 0x0B41, // GL_POLYGON_SMOOTH
    polygon_smooth_hint           = 0x0C53, // GL_POLYGON_SMOOTH_HINT
    polygon_stipple               = 0x0B42, // GL_POLYGON_STIPPLE
    primitive_restart_index       = 0x8F9E, // GL_PRIMITIVE_RESTART_INDEX
    program_binary_formats        = 0x87FF, // GL_PROGRAM_BINARY_FORMATS
    program_pipeline_binding      = 0x825A, // GL_PROGRAM_PIPELINE_BINDING
    program_point_size            = 0x8642, // GL_PROGRAM_POINT_SIZE
    projection_matrix             = 0x0BA7, // GL_PROJECTION_MATRIX
    projection_stack_depth        = 0x0BA4, // GL_PROJECTION_STACK_DEPTH
    provoking_vertex              = 0x8E4F, // GL_PROVOKING_VERTEX
    read_buffer                   = 0x0C02, // GL_READ_BUFFER
    read_framebuffer_binding      = 0x8CAA, // GL_READ_FRAMEBUFFER_BINDING
    red_bias                      = 0x0D15, // GL_RED_BIAS
    red_bits                      = 0x0D52, // GL_RED_BITS
    red_scale                     = 0x0D14, // GL_RED_SCALE
    renderbuffer_binding          = 0x8CA7, // GL_RENDERBUFFER_BINDING
    render_mode                   = 0x0C40, // GL_RENDER_MODE
    rgba_mode                     = 0x0C31, // GL_RGBA_MODE
    sampler_binding               = 0x8919, // GL_SAMPLER_BINDING
    samples                       = 0x80A9, // GL_SAMPLES
    sample_buffers                = 0x80A8, // GL_SAMPLE_BUFFERS
    sample_coverage_invert        = 0x80AB, // GL_SAMPLE_COVERAGE_INVERT
    sample_coverage_value         = 0x80AA, // GL_SAMPLE_COVERAGE_VALUE
    scissor_box                   = 0x0C10, // GL_SCISSOR_BOX
    scissor_test                  = 0x0C11, // GL_SCISSOR_TEST
    selection_buffer_size         = 0x0DF4, // GL_SELECTION_BUFFER_SIZE
    shader_binary_formats         = 0x8DF8, // GL_SHADER_BINARY_FORMATS
    shader_compiler               = 0x8DFA, // GL_SHADER_COMPILER
    shader_storage_buffer_binding = 0x90D3, // GL_SHADER_STORAGE_BUFFER_BINDING
    shader_storage_buffer_offset_alignment =
        0x90DF, // GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT
    shader_storage_buffer_size    = 0x90D5, // GL_SHADER_STORAGE_BUFFER_SIZE
    shader_storage_buffer_start   = 0x90D4, // GL_SHADER_STORAGE_BUFFER_START
    shade_model                   = 0x0B54, // GL_SHADE_MODEL
    smooth_line_width_granularity = 0x0B23, // GL_SMOOTH_LINE_WIDTH_GRANULARITY
    smooth_line_width_range       = 0x0B22, // GL_SMOOTH_LINE_WIDTH_RANGE
    smooth_point_size_granularity = 0x0B13, // GL_SMOOTH_POINT_SIZE_GRANULARITY
    smooth_point_size_range       = 0x0B12, // GL_SMOOTH_POINT_SIZE_RANGE
    stencil_back_fail             = 0x8801, // GL_STENCIL_BACK_FAIL
    stencil_back_func             = 0x8800, // GL_STENCIL_BACK_FUNC
    stencil_back_pass_depth_fail  = 0x8802, // GL_STENCIL_BACK_PASS_DEPTH_FAIL
    stencil_back_pass_depth_pass  = 0x8803, // GL_STENCIL_BACK_PASS_DEPTH_PASS
    stencil_back_ref              = 0x8CA3, // GL_STENCIL_BACK_REF
    stencil_back_value_mask       = 0x8CA4, // GL_STENCIL_BACK_VALUE_MASK
    stencil_back_writemask        = 0x8CA5, // GL_STENCIL_BACK_WRITEMASK
    stencil_bits                  = 0x0D57, // GL_STENCIL_BITS
    stencil_clear_value           = 0x0B91, // GL_STENCIL_CLEAR_VALUE
    stencil_fail                  = 0x0B94, // GL_STENCIL_FAIL
    stencil_func                  = 0x0B92, // GL_STENCIL_FUNC
    stencil_pass_depth_fail       = 0x0B95, // GL_STENCIL_PASS_DEPTH_FAIL
    stencil_pass_depth_pass       = 0x0B96, // GL_STENCIL_PASS_DEPTH_PASS
    stencil_ref                   = 0x0B97, // GL_STENCIL_REF
    stencil_test                  = 0x0B90, // GL_STENCIL_TEST
    stencil_value_mask            = 0x0B93, // GL_STENCIL_VALUE_MASK
    stencil_writemask             = 0x0B98, // GL_STENCIL_WRITEMASK
    stereo                        = 0x0C33, // GL_STEREO
    subpixel_bits                 = 0x0D50, // GL_SUBPIXEL_BITS
    texture_1d                    = 0x0DE0, // GL_TEXTURE_1D
    texture_2d                    = 0x0DE1, // GL_TEXTURE_2D
    texture_binding_1d            = 0x8068, // GL_TEXTURE_BINDING_1D
    texture_binding_1d_array      = 0x8C1C, // GL_TEXTURE_BINDING_1D_ARRAY
    texture_binding_2d            = 0x8069, // GL_TEXTURE_BINDING_2D
    texture_binding_2d_array      = 0x8C1D, // GL_TEXTURE_BINDING_2D_ARRAY
    texture_binding_2d_multisample =
        0x9104, // GL_TEXTURE_BINDING_2D_MULTISAMPLE
    texture_binding_2d_multisample_array =
        0x9105, // GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY
    texture_binding_3d        = 0x806A, // GL_TEXTURE_BINDING_3D
    texture_binding_buffer    = 0x8C2C, // GL_TEXTURE_BINDING_BUFFER
    texture_binding_cube_map  = 0x8514, // GL_TEXTURE_BINDING_CUBE_MAP
    texture_binding_rectangle = 0x84F6, // GL_TEXTURE_BINDING_RECTANGLE
    texture_buffer_offset_alignment =
        0x919F,                          // GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT
    texture_compression_hint   = 0x84EF, // GL_TEXTURE_COMPRESSION_HINT
    texture_coord_array        = 0x8078, // GL_TEXTURE_COORD_ARRAY
    texture_coord_array_size   = 0x8088, // GL_TEXTURE_COORD_ARRAY_SIZE
    texture_coord_array_stride = 0x808A, // GL_TEXTURE_COORD_ARRAY_STRIDE
    texture_coord_array_type   = 0x8089, // GL_TEXTURE_COORD_ARRAY_TYPE
    texture_gen_q              = 0x0C63, // GL_TEXTURE_GEN_Q
    texture_gen_r              = 0x0C62, // GL_TEXTURE_GEN_R
    texture_gen_s              = 0x0C60, // GL_TEXTURE_GEN_S
    texture_gen_t              = 0x0C61, // GL_TEXTURE_GEN_T
    texture_matrix             = 0x0BA8, // GL_TEXTURE_MATRIX
    texture_stack_depth        = 0x0BA5, // GL_TEXTURE_STACK_DEPTH
    timestamp                  = 0x8E28, // GL_TIMESTAMP
    transform_feedback_buffer_binding =
        0x8C8F, // GL_TRANSFORM_FEEDBACK_BUFFER_BINDING
    transform_feedback_buffer_size =
        0x8C85, // GL_TRANSFORM_FEEDBACK_BUFFER_SIZE
    transform_feedback_buffer_start =
        0x8C84,                      // GL_TRANSFORM_FEEDBACK_BUFFER_START
    uniform_buffer_binding = 0x8A28, // GL_UNIFORM_BUFFER_BINDING
    uniform_buffer_offset_alignment =
        0x8A34,                      // GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT
    uniform_buffer_size    = 0x8A2A, // GL_UNIFORM_BUFFER_SIZE
    uniform_buffer_start   = 0x8A29, // GL_UNIFORM_BUFFER_START
    unpack_alignment       = 0x0CF5, // GL_UNPACK_ALIGNMENT
    unpack_image_height    = 0x806E, // GL_UNPACK_IMAGE_HEIGHT
    unpack_lsb_first       = 0x0CF1, // GL_UNPACK_LSB_FIRST
    unpack_row_length      = 0x0CF2, // GL_UNPACK_ROW_LENGTH
    unpack_skip_images     = 0x806D, // GL_UNPACK_SKIP_IMAGES
    unpack_skip_pixels     = 0x0CF4, // GL_UNPACK_SKIP_PIXELS
    unpack_skip_rows       = 0x0CF3, // GL_UNPACK_SKIP_ROWS
    unpack_swap_bytes      = 0x0CF0, // GL_UNPACK_SWAP_BYTES
    vertex_array           = 0x8074, // GL_VERTEX_ARRAY
    vertex_array_binding   = 0x85B5, // GL_VERTEX_ARRAY_BINDING
    vertex_array_size      = 0x807A, // GL_VERTEX_ARRAY_SIZE
    vertex_array_stride    = 0x807C, // GL_VERTEX_ARRAY_STRIDE
    vertex_array_type      = 0x807B, // GL_VERTEX_ARRAY_TYPE
    vertex_binding_divisor = 0x82D6, // GL_VERTEX_BINDING_DIVISOR
    vertex_binding_offset  = 0x82D7, // GL_VERTEX_BINDING_OFFSET
    vertex_binding_stride  = 0x82D8, // GL_VERTEX_BINDING_STRIDE
    viewport               = 0x0BA2, // GL_VIEWPORT
    viewport_bounds_range  = 0x825D, // GL_VIEWPORT_BOUNDS_RANGE
    viewport_index_provoking_vertex =
        0x825F,                            // GL_VIEWPORT_INDEX_PROVOKING_VERTEX
    viewport_subpixel_bits       = 0x825C, // GL_VIEWPORT_SUBPIXEL_BITS
    zoom_x                       = 0x0D16, // GL_ZOOM_X
    zoom_y                       = 0x0D17, // GL_ZOOM_Y
    alpha_test_func_qcom         = 0x0BC1, // GL_ALPHA_TEST_FUNC_QCOM
    alpha_test_qcom              = 0x0BC0, // GL_ALPHA_TEST_QCOM
    alpha_test_ref_qcom          = 0x0BC2, // GL_ALPHA_TEST_REF_QCOM
    async_draw_pixels_sgix       = 0x835D, // GL_ASYNC_DRAW_PIXELS_SGIX
    async_histogram_sgix         = 0x832C, // GL_ASYNC_HISTOGRAM_SGIX
    async_marker_sgix            = 0x8329, // GL_ASYNC_MARKER_SGIX
    async_read_pixels_sgix       = 0x835E, // GL_ASYNC_READ_PIXELS_SGIX
    async_tex_image_sgix         = 0x835C, // GL_ASYNC_TEX_IMAGE_SGIX
    blend_color_ext              = 0x8005, // GL_BLEND_COLOR_EXT
    blend_equation_ext           = 0x8009, // GL_BLEND_EQUATION_EXT
    blend_equation_oes           = 0x8009, // GL_BLEND_EQUATION_OES
    calligraphic_fragment_sgix   = 0x8183, // GL_CALLIGRAPHIC_FRAGMENT_SGIX
    color_array_count_ext        = 0x8084, // GL_COLOR_ARRAY_COUNT_EXT
    color_matrix_sgi             = 0x80B1, // GL_COLOR_MATRIX_SGI
    color_matrix_stack_depth_sgi = 0x80B2, // GL_COLOR_MATRIX_STACK_DEPTH_SGI
    color_table_sgi              = 0x80D0, // GL_COLOR_TABLE_SGI
    convolution_1d_ext           = 0x8010, // GL_CONVOLUTION_1D_EXT
    convolution_2d_ext           = 0x8011, // GL_CONVOLUTION_2D_EXT
    convolution_hint_sgix        = 0x8316, // GL_CONVOLUTION_HINT_SGIX
    deformations_mask_sgix       = 0x8196, // GL_DEFORMATIONS_MASK_SGIX
    detail_texture_2d_binding_sgis =
        0x8096,                         // GL_DETAIL_TEXTURE_2D_BINDING_SGIS
    device_luid_ext           = 0x9599, // GL_DEVICE_LUID_EXT
    device_node_mask_ext      = 0x959A, // GL_DEVICE_NODE_MASK_EXT
    device_uuid_ext           = 0x9597, // GL_DEVICE_UUID_EXT
    distance_attenuation_ext  = 0x8129, // GL_DISTANCE_ATTENUATION_EXT
    distance_attenuation_sgis = 0x8129, // GL_DISTANCE_ATTENUATION_SGIS
    draw_buffer_ext           = 0x0C01, // GL_DRAW_BUFFER_EXT
    driver_uuid_ext           = 0x9598, // GL_DRIVER_UUID_EXT
    edge_flag_array_count_ext = 0x808D, // GL_EDGE_FLAG_ARRAY_COUNT_EXT
    fetch_per_sample_arm      = 0x8F65, // GL_FETCH_PER_SAMPLE_ARM
    fog_func_points_sgis      = 0x812B, // GL_FOG_FUNC_POINTS_SGIS
    fog_offset_sgix           = 0x8198, // GL_FOG_OFFSET_SGIX
    fog_offset_value_sgix     = 0x8199, // GL_FOG_OFFSET_VALUE_SGIX
    fragment_color_material_face_sgix =
        0x8402, // GL_FRAGMENT_COLOR_MATERIAL_FACE_SGIX
    fragment_color_material_parameter_sgix =
        0x8403, // GL_FRAGMENT_COLOR_MATERIAL_PARAMETER_SGIX
    fragment_color_material_sgix = 0x8401, // GL_FRAGMENT_COLOR_MATERIAL_SGIX
    fragment_light0_sgix         = 0x840C, // GL_FRAGMENT_LIGHT0_SGIX
    fragment_lighting_sgix       = 0x8400, // GL_FRAGMENT_LIGHTING_SGIX
    fragment_light_model_ambient_sgix =
        0x840A, // GL_FRAGMENT_LIGHT_MODEL_AMBIENT_SGIX
    fragment_light_model_local_viewer_sgix =
        0x8408, // GL_FRAGMENT_LIGHT_MODEL_LOCAL_VIEWER_SGIX
    fragment_light_model_normal_interpolation_sgix =
        0x840B, // GL_FRAGMENT_LIGHT_MODEL_NORMAL_INTERPOLATION_SGIX
    fragment_light_model_two_side_sgix =
        0x8409,                   // GL_FRAGMENT_LIGHT_MODEL_TWO_SIDE_SGIX
    fragment_shader_ati = 0x8920, // GL_FRAGMENT_SHADER_ATI
    fragment_shader_framebuffer_fetch_mrt_arm =
        0x8F66, // GL_FRAGMENT_SHADER_FRAMEBUFFER_FETCH_MRT_ARM
    fragment_shading_rate_attachment_with_default_framebuffer_supported_ext =
        0x96DF, // GL_FRAGMENT_SHADING_RATE_ATTACHMENT_WITH_DEFAULT_FRAMEBUFFER_SUPPORTED_EXT
    fragment_shading_rate_non_trivial_combiners_supported_ext =
        0x8F6F, // GL_FRAGMENT_SHADING_RATE_NON_TRIVIAL_COMBINERS_SUPPORTED_EXT
    fragment_shading_rate_primitive_rate_with_multi_viewport_supported_ext =
        0x9780, // GL_FRAGMENT_SHADING_RATE_PRIMITIVE_RATE_WITH_MULTI_VIEWPORT_SUPPORTED_EXT
    fragment_shading_rate_with_sample_mask_supported_ext =
        0x96DE, // GL_FRAGMENT_SHADING_RATE_WITH_SAMPLE_MASK_SUPPORTED_EXT
    fragment_shading_rate_with_shader_depth_stencil_writes_supported_ext =
        0x96DD, // GL_FRAGMENT_SHADING_RATE_WITH_SHADER_DEPTH_STENCIL_WRITES_SUPPORTED_EXT
    framebuffer_fetch_noncoherent_qcom =
        0x96A2, // GL_FRAMEBUFFER_FETCH_NONCOHERENT_QCOM
    framezoom_factor_sgix        = 0x818C, // GL_FRAMEZOOM_FACTOR_SGIX
    framezoom_sgix               = 0x818B, // GL_FRAMEZOOM_SGIX
    generate_mipmap_hint_sgis    = 0x8192, // GL_GENERATE_MIPMAP_HINT_SGIS
    histogram_ext                = 0x8024, // GL_HISTOGRAM_EXT
    index_array_count_ext        = 0x8087, // GL_INDEX_ARRAY_COUNT_EXT
    instrument_measurements_sgix = 0x8181, // GL_INSTRUMENT_MEASUREMENTS_SGIX
    interlace_sgix               = 0x8094, // GL_INTERLACE_SGIX
    ir_instrument1_sgix          = 0x817F, // GL_IR_INSTRUMENT1_SGIX
    light_env_mode_sgix          = 0x8407, // GL_LIGHT_ENV_MODE_SGIX
    max_3d_texture_size_ext      = 0x8073, // GL_MAX_3D_TEXTURE_SIZE_EXT
    max_4d_texture_size_sgis     = 0x8138, // GL_MAX_4D_TEXTURE_SIZE_SGIS
    max_active_lights_sgix       = 0x8405, // GL_MAX_ACTIVE_LIGHTS_SGIX
    max_async_draw_pixels_sgix   = 0x8360, // GL_MAX_ASYNC_DRAW_PIXELS_SGIX
    max_async_histogram_sgix     = 0x832D, // GL_MAX_ASYNC_HISTOGRAM_SGIX
    max_async_read_pixels_sgix   = 0x8361, // GL_MAX_ASYNC_READ_PIXELS_SGIX
    max_async_tex_image_sgix     = 0x835F, // GL_MAX_ASYNC_TEX_IMAGE_SGIX
    max_clipmap_depth_sgix       = 0x8177, // GL_MAX_CLIPMAP_DEPTH_SGIX
    max_clipmap_virtual_depth_sgix =
        0x8178,                         // GL_MAX_CLIPMAP_VIRTUAL_DEPTH_SGIX
    max_color_attachments_ext = 0x8CDF, // GL_MAX_COLOR_ATTACHMENTS_EXT
    max_color_attachments_nv  = 0x8CDF, // GL_MAX_COLOR_ATTACHMENTS_NV
    max_color_matrix_stack_depth_sgi =
        0x80B3, // GL_MAX_COLOR_MATRIX_STACK_DEPTH_SGI
    max_cube_map_texture_size_arb = 0x851C, // GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB
    max_cube_map_texture_size_ext = 0x851C, // GL_MAX_CUBE_MAP_TEXTURE_SIZE_EXT
    max_cube_map_texture_size_oes = 0x851C, // GL_MAX_CUBE_MAP_TEXTURE_SIZE_OES
    max_fog_func_points_sgis      = 0x812C, // GL_MAX_FOG_FUNC_POINTS_SGIS
    max_fragment_lights_sgix      = 0x8404, // GL_MAX_FRAGMENT_LIGHTS_SGIX
    max_fragment_shading_rate_attachment_layers_ext =
        0x96DC, // GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_LAYERS_EXT
    max_fragment_shading_rate_attachment_texel_aspect_ratio_ext =
        0x96DB, // GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_ASPECT_RATIO_EXT
    max_fragment_shading_rate_attachment_texel_height_ext =
        0x96DA, // GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_HEIGHT_EXT
    max_fragment_shading_rate_attachment_texel_width_ext =
        0x96D8, // GL_MAX_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_WIDTH_EXT
    max_framezoom_factor_sgix = 0x818D, // GL_MAX_FRAMEZOOM_FACTOR_SGIX
    max_mesh_output_components_ext =
        0x9749,                          // GL_MAX_MESH_OUTPUT_COMPONENTS_EXT
    max_mesh_output_layers_ext = 0x974A, // GL_MAX_MESH_OUTPUT_LAYERS_EXT
    max_mesh_output_memory_size_ext =
        0x9747, // GL_MAX_MESH_OUTPUT_MEMORY_SIZE_EXT
    max_mesh_output_primitives_ext =
        0x9756, // GL_MAX_MESH_OUTPUT_PRIMITIVES_EXT
    max_mesh_payload_and_output_memory_size_ext =
        0x9748, // GL_MAX_MESH_PAYLOAD_AND_OUTPUT_MEMORY_SIZE_EXT
    max_mesh_payload_and_shared_memory_size_ext =
        0x9746, // GL_MAX_MESH_PAYLOAD_AND_SHARED_MEMORY_SIZE_EXT
    max_mesh_shared_memory_size_ext =
        0x9744, // GL_MAX_MESH_SHARED_MEMORY_SIZE_EXT
    max_mesh_work_group_count_ext = 0x9752, // GL_MAX_MESH_WORK_GROUP_COUNT_EXT
    max_mesh_work_group_invocations_ext =
        0x9757, // GL_MAX_MESH_WORK_GROUP_INVOCATIONS_EXT
    max_mesh_work_group_size_ext = 0x9758, // GL_MAX_MESH_WORK_GROUP_SIZE_EXT
    max_mesh_work_group_total_count_ext =
        0x9741, // GL_MAX_MESH_WORK_GROUP_TOTAL_COUNT_EXT
    max_preferred_mesh_work_group_invocations_ext =
        0x974C, // GL_MAX_PREFERRED_MESH_WORK_GROUP_INVOCATIONS_EXT
    max_preferred_task_work_group_invocations_ext =
        0x974B, // GL_MAX_PREFERRED_TASK_WORK_GROUP_INVOCATIONS_EXT
    max_task_payload_and_shared_memory_size_ext =
        0x9745, // GL_MAX_TASK_PAYLOAD_AND_SHARED_MEMORY_SIZE_EXT
    max_task_payload_size_ext = 0x9742, // GL_MAX_TASK_PAYLOAD_SIZE_EXT
    max_task_shared_memory_size_ext =
        0x9743, // GL_MAX_TASK_SHARED_MEMORY_SIZE_EXT
    max_task_work_group_count_ext = 0x9751, // GL_MAX_TASK_WORK_GROUP_COUNT_EXT
    max_task_work_group_invocations_ext =
        0x9759, // GL_MAX_TASK_WORK_GROUP_INVOCATIONS_EXT
    max_task_work_group_size_ext = 0x975A, // GL_MAX_TASK_WORK_GROUP_SIZE_EXT
    max_task_work_group_total_count_ext =
        0x9740, // GL_MAX_TASK_WORK_GROUP_TOTAL_COUNT_EXT
    max_texture_max_anisotropy_ext =
        0x84FF, // GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
    max_timeline_semaphore_value_difference_nv =
        0x95B6, // GL_MAX_TIMELINE_SEMAPHORE_VALUE_DIFFERENCE_NV
    mesh_prefers_compact_primitive_output_ext =
        0x9750, // GL_MESH_PREFERS_COMPACT_PRIMITIVE_OUTPUT_EXT
    mesh_prefers_compact_vertex_output_ext =
        0x974F, // GL_MESH_PREFERS_COMPACT_VERTEX_OUTPUT_EXT
    mesh_prefers_local_invocation_primitive_output_ext =
        0x974E, // GL_MESH_PREFERS_LOCAL_INVOCATION_PRIMITIVE_OUTPUT_EXT
    mesh_prefers_local_invocation_vertex_output_ext =
        0x974D,          // GL_MESH_PREFERS_LOCAL_INVOCATION_VERTEX_OUTPUT_EXT
    minmax_ext = 0x802E, // GL_MINMAX_EXT
    min_fragment_shading_rate_attachment_texel_height_ext =
        0x96D9, // GL_MIN_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_HEIGHT_EXT
    min_fragment_shading_rate_attachment_texel_width_ext =
        0x96D7, // GL_MIN_FRAGMENT_SHADING_RATE_ATTACHMENT_TEXEL_WIDTH_EXT
    modelview0_matrix_ext      = 0x0BA6, // GL_MODELVIEW0_MATRIX_EXT
    modelview0_stack_depth_ext = 0x0BA3, // GL_MODELVIEW0_STACK_DEPTH_EXT
    motion_estimation_search_block_x_qcom =
        0x8C90, // GL_MOTION_ESTIMATION_SEARCH_BLOCK_X_QCOM
    motion_estimation_search_block_y_qcom =
        0x8C91, // GL_MOTION_ESTIMATION_SEARCH_BLOCK_Y_QCOM
    multisample_sgis         = 0x809D, // GL_MULTISAMPLE_SGIS
    normal_array_count_ext   = 0x8080, // GL_NORMAL_ARRAY_COUNT_EXT
    num_device_uuids_ext     = 0x9596, // GL_NUM_DEVICE_UUIDS_EXT
    pack_cmyk_hint_ext       = 0x800E, // GL_PACK_CMYK_HINT_EXT
    pack_image_depth_sgis    = 0x8131, // GL_PACK_IMAGE_DEPTH_SGIS
    pack_image_height_ext    = 0x806C, // GL_PACK_IMAGE_HEIGHT_EXT
    pack_resample_sgix       = 0x842E, // GL_PACK_RESAMPLE_SGIX
    pack_row_length_nv       = 0x0D02, // GL_PACK_ROW_LENGTH_NV
    pack_skip_images_ext     = 0x806B, // GL_PACK_SKIP_IMAGES_EXT
    pack_skip_pixels_nv      = 0x0D04, // GL_PACK_SKIP_PIXELS_NV
    pack_skip_rows_nv        = 0x0D03, // GL_PACK_SKIP_ROWS_NV
    pack_skip_volumes_sgis   = 0x8130, // GL_PACK_SKIP_VOLUMES_SGIS
    pack_subsample_rate_sgix = 0x85A0, // GL_PACK_SUBSAMPLE_RATE_SGIX
    pixel_texture_sgis       = 0x8353, // GL_PIXEL_TEXTURE_SGIS
    pixel_tex_gen_mode_sgix  = 0x832B, // GL_PIXEL_TEX_GEN_MODE_SGIX
    pixel_tex_gen_sgix       = 0x8139, // GL_PIXEL_TEX_GEN_SGIX
    pixel_tile_best_alignment_sgix =
        0x813E, // GL_PIXEL_TILE_BEST_ALIGNMENT_SGIX
    pixel_tile_cache_increment_sgix =
        0x813F,                           // GL_PIXEL_TILE_CACHE_INCREMENT_SGIX
    pixel_tile_cache_size_sgix  = 0x8145, // GL_PIXEL_TILE_CACHE_SIZE_SGIX
    pixel_tile_grid_depth_sgix  = 0x8144, // GL_PIXEL_TILE_GRID_DEPTH_SGIX
    pixel_tile_grid_height_sgix = 0x8143, // GL_PIXEL_TILE_GRID_HEIGHT_SGIX
    pixel_tile_grid_width_sgix  = 0x8142, // GL_PIXEL_TILE_GRID_WIDTH_SGIX
    pixel_tile_height_sgix      = 0x8141, // GL_PIXEL_TILE_HEIGHT_SGIX
    pixel_tile_width_sgix       = 0x8140, // GL_PIXEL_TILE_WIDTH_SGIX
    point_distance_attenuation_arb =
        0x8129,                             // GL_POINT_DISTANCE_ATTENUATION_ARB
    point_fade_threshold_size_arb = 0x8128, // GL_POINT_FADE_THRESHOLD_SIZE_ARB
    point_fade_threshold_size_ext = 0x8128, // GL_POINT_FADE_THRESHOLD_SIZE_EXT
    point_fade_threshold_size_sgis =
        0x8128,                       // GL_POINT_FADE_THRESHOLD_SIZE_SGIS
    point_size_max_arb      = 0x8127, // GL_POINT_SIZE_MAX_ARB
    point_size_max_ext      = 0x8127, // GL_POINT_SIZE_MAX_EXT
    point_size_max_sgis     = 0x8127, // GL_POINT_SIZE_MAX_SGIS
    point_size_min_arb      = 0x8126, // GL_POINT_SIZE_MIN_ARB
    point_size_min_ext      = 0x8126, // GL_POINT_SIZE_MIN_EXT
    point_size_min_sgis     = 0x8126, // GL_POINT_SIZE_MIN_SGIS
    polygon_offset_bias_ext = 0x8039, // GL_POLYGON_OFFSET_BIAS_EXT
    post_color_matrix_alpha_bias_sgi =
        0x80BB, // GL_POST_COLOR_MATRIX_ALPHA_BIAS_SGI
    post_color_matrix_alpha_scale_sgi =
        0x80B7, // GL_POST_COLOR_MATRIX_ALPHA_SCALE_SGI
    post_color_matrix_blue_bias_sgi =
        0x80BA, // GL_POST_COLOR_MATRIX_BLUE_BIAS_SGI
    post_color_matrix_blue_scale_sgi =
        0x80B6, // GL_POST_COLOR_MATRIX_BLUE_SCALE_SGI
    post_color_matrix_color_table_sgi =
        0x80D2, // GL_POST_COLOR_MATRIX_COLOR_TABLE_SGI
    post_color_matrix_green_bias_sgi =
        0x80B9, // GL_POST_COLOR_MATRIX_GREEN_BIAS_SGI
    post_color_matrix_green_scale_sgi =
        0x80B5, // GL_POST_COLOR_MATRIX_GREEN_SCALE_SGI
    post_color_matrix_red_bias_sgi =
        0x80B8, // GL_POST_COLOR_MATRIX_RED_BIAS_SGI
    post_color_matrix_red_scale_sgi =
        0x80B4, // GL_POST_COLOR_MATRIX_RED_SCALE_SGI
    post_convolution_alpha_bias_ext =
        0x8023, // GL_POST_CONVOLUTION_ALPHA_BIAS_EXT
    post_convolution_alpha_scale_ext =
        0x801F, // GL_POST_CONVOLUTION_ALPHA_SCALE_EXT
    post_convolution_blue_bias_ext =
        0x8022, // GL_POST_CONVOLUTION_BLUE_BIAS_EXT
    post_convolution_blue_scale_ext =
        0x801E, // GL_POST_CONVOLUTION_BLUE_SCALE_EXT
    post_convolution_color_table_sgi =
        0x80D1, // GL_POST_CONVOLUTION_COLOR_TABLE_SGI
    post_convolution_green_bias_ext =
        0x8021, // GL_POST_CONVOLUTION_GREEN_BIAS_EXT
    post_convolution_green_scale_ext =
        0x801D, // GL_POST_CONVOLUTION_GREEN_SCALE_EXT
    post_convolution_red_bias_ext = 0x8020, // GL_POST_CONVOLUTION_RED_BIAS_EXT
    post_convolution_red_scale_ext =
        0x801C, // GL_POST_CONVOLUTION_RED_SCALE_EXT
    post_texture_filter_bias_range_sgix =
        0x817B, // GL_POST_TEXTURE_FILTER_BIAS_RANGE_SGIX
    post_texture_filter_scale_range_sgix =
        0x817C, // GL_POST_TEXTURE_FILTER_SCALE_RANGE_SGIX
    read_buffer_ext               = 0x0C02, // GL_READ_BUFFER_EXT
    read_buffer_nv                = 0x0C02, // GL_READ_BUFFER_NV
    reference_plane_equation_sgix = 0x817E, // GL_REFERENCE_PLANE_EQUATION_SGIX
    reference_plane_sgix          = 0x817D, // GL_REFERENCE_PLANE_SGIX
    rescale_normal_ext            = 0x803A, // GL_RESCALE_NORMAL_EXT
    samples_sgis                  = 0x80A9, // GL_SAMPLES_SGIS
    sample_alpha_to_mask_sgis     = 0x809E, // GL_SAMPLE_ALPHA_TO_MASK_SGIS
    sample_alpha_to_one_sgis      = 0x809F, // GL_SAMPLE_ALPHA_TO_ONE_SGIS
    sample_buffers_sgis           = 0x80A8, // GL_SAMPLE_BUFFERS_SGIS
    sample_mask_invert_sgis       = 0x80AB, // GL_SAMPLE_MASK_INVERT_SGIS
    sample_mask_sgis              = 0x80A0, // GL_SAMPLE_MASK_SGIS
    sample_mask_value_sgis        = 0x80AA, // GL_SAMPLE_MASK_VALUE_SGIS
    sample_pattern_sgis           = 0x80AC, // GL_SAMPLE_PATTERN_SGIS
    separable_2d_ext              = 0x8012, // GL_SEPARABLE_2D_EXT
    shader_core_active_count_arm  = 0x96F1, // GL_SHADER_CORE_ACTIVE_COUNT_ARM
    shader_core_count_arm         = 0x96F0, // GL_SHADER_CORE_COUNT_ARM
    shader_core_fma_rate_arm      = 0x96F6, // GL_SHADER_CORE_FMA_RATE_ARM
    shader_core_max_warp_count_arm =
        0x96F3,                            // GL_SHADER_CORE_MAX_WARP_COUNT_ARM
    shader_core_pixel_rate_arm   = 0x96F4, // GL_SHADER_CORE_PIXEL_RATE_ARM
    shader_core_present_mask_arm = 0x96F2, // GL_SHADER_CORE_PRESENT_MASK_ARM
    shader_core_texel_rate_arm   = 0x96F5, // GL_SHADER_CORE_TEXEL_RATE_ARM
    shading_rate_ext             = 0x96D0, // GL_SHADING_RATE_EXT
    shading_rate_image_palette_count_nv =
        0x95B2, // GL_SHADING_RATE_IMAGE_PALETTE_COUNT_NV
    shading_rate_image_per_primitive_nv =
        0x95B1, // GL_SHADING_RATE_IMAGE_PER_PRIMITIVE_NV
    shading_rate_qcom             = 0x96A4, // GL_SHADING_RATE_QCOM
    shared_texture_palette_ext    = 0x81FB, // GL_SHARED_TEXTURE_PALETTE_EXT
    sprite_axis_sgix              = 0x814A, // GL_SPRITE_AXIS_SGIX
    sprite_mode_sgix              = 0x8149, // GL_SPRITE_MODE_SGIX
    sprite_sgix                   = 0x8148, // GL_SPRITE_SGIX
    sprite_translation_sgix       = 0x814B, // GL_SPRITE_TRANSLATION_SGIX
    texture_3d_binding_ext        = 0x806A, // GL_TEXTURE_3D_BINDING_EXT
    texture_3d_ext                = 0x806F, // GL_TEXTURE_3D_EXT
    texture_4d_binding_sgis       = 0x814F, // GL_TEXTURE_4D_BINDING_SGIS
    texture_4d_sgis               = 0x8134, // GL_TEXTURE_4D_SGIS
    texture_binding_cube_map_arb  = 0x8514, // GL_TEXTURE_BINDING_CUBE_MAP_ARB
    texture_binding_cube_map_ext  = 0x8514, // GL_TEXTURE_BINDING_CUBE_MAP_EXT
    texture_binding_cube_map_oes  = 0x8514, // GL_TEXTURE_BINDING_CUBE_MAP_OES
    texture_binding_rectangle_arb = 0x84F6, // GL_TEXTURE_BINDING_RECTANGLE_ARB
    texture_binding_rectangle_nv  = 0x84F6, // GL_TEXTURE_BINDING_RECTANGLE_NV
    texture_color_table_sgi       = 0x80BC, // GL_TEXTURE_COLOR_TABLE_SGI
    texture_coord_array_count_ext = 0x808B, // GL_TEXTURE_COORD_ARRAY_COUNT_EXT
    texture_gen_str_oes           = 0x8D60, // GL_TEXTURE_GEN_STR_OES
    timestamp_ext                 = 0x8E28, // GL_TIMESTAMP_EXT
    unpack_cmyk_hint_ext          = 0x800F, // GL_UNPACK_CMYK_HINT_EXT
    unpack_image_depth_sgis       = 0x8133, // GL_UNPACK_IMAGE_DEPTH_SGIS
    unpack_image_height_ext       = 0x806E, // GL_UNPACK_IMAGE_HEIGHT_EXT
    unpack_resample_sgix          = 0x842F, // GL_UNPACK_RESAMPLE_SGIX
    unpack_skip_images_ext        = 0x806D, // GL_UNPACK_SKIP_IMAGES_EXT
    unpack_skip_volumes_sgis      = 0x8132, // GL_UNPACK_SKIP_VOLUMES_SGIS
    unpack_subsample_rate_sgix    = 0x85A1, // GL_UNPACK_SUBSAMPLE_RATE_SGIX
    vertex_array_count_ext        = 0x807D, // GL_VERTEX_ARRAY_COUNT_EXT
    vertex_preclip_hint_sgix      = 0x83EF, // GL_VERTEX_PRECLIP_HINT_SGIX
    vertex_preclip_sgix           = 0x83EE, // GL_VERTEX_PRECLIP_SGIX
}; // enum class get_prop

} // namespace gl::group
