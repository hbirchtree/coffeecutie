#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ClearBufferMask
enum class clear_buffer_mask : ::libc_types::u32 {
#ifdef GL_ACCUM_BUFFER_BIT
    accum_buffer_bit = GL_ACCUM_BUFFER_BIT,
#endif
#ifdef GL_COLOR_BUFFER_BIT
    color_buffer_bit = GL_COLOR_BUFFER_BIT,
#endif
#ifdef GL_COVERAGE_BUFFER_BIT_NV
    coverage_buffer_bit_nv = GL_COVERAGE_BUFFER_BIT_NV,
#endif
#ifdef GL_DEPTH_BUFFER_BIT
    depth_buffer_bit = GL_DEPTH_BUFFER_BIT,
#endif
#ifdef GL_STENCIL_BUFFER_BIT
    stencil_buffer_bit = GL_STENCIL_BUFFER_BIT,
#endif
}; // enum class clear_buffer_mask
C_FLAGS(clear_buffer_mask, ::libc_types::u32);
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// BufferStorageMask
enum class buffer_storage_mask : ::libc_types::u32 {
#ifdef GL_CLIENT_STORAGE_BIT
    client_storage_bit = GL_CLIENT_STORAGE_BIT,
#endif
#ifdef GL_CLIENT_STORAGE_BIT_EXT
    client_storage_bit_ext = GL_CLIENT_STORAGE_BIT_EXT,
#endif
#ifdef GL_DYNAMIC_STORAGE_BIT
    dynamic_storage_bit = GL_DYNAMIC_STORAGE_BIT,
#endif
#ifdef GL_DYNAMIC_STORAGE_BIT_EXT
    dynamic_storage_bit_ext = GL_DYNAMIC_STORAGE_BIT_EXT,
#endif
#ifdef GL_EXTERNAL_STORAGE_BIT_NVX
    external_storage_bit_nvx = GL_EXTERNAL_STORAGE_BIT_NVX,
#endif
#ifdef GL_LGPU_SEPARATE_STORAGE_BIT_NVX
    lgpu_separate_storage_bit_nvx = GL_LGPU_SEPARATE_STORAGE_BIT_NVX,
#endif
#ifdef GL_MAP_COHERENT_BIT
    map_coherent_bit = GL_MAP_COHERENT_BIT,
#endif
#ifdef GL_MAP_COHERENT_BIT_EXT
    map_coherent_bit_ext = GL_MAP_COHERENT_BIT_EXT,
#endif
#ifdef GL_MAP_PERSISTENT_BIT
    map_persistent_bit = GL_MAP_PERSISTENT_BIT,
#endif
#ifdef GL_MAP_PERSISTENT_BIT_EXT
    map_persistent_bit_ext = GL_MAP_PERSISTENT_BIT_EXT,
#endif
#ifdef GL_MAP_READ_BIT
    map_read_bit = GL_MAP_READ_BIT,
#endif
#ifdef GL_MAP_READ_BIT_EXT
    map_read_bit_ext = GL_MAP_READ_BIT_EXT,
#endif
#ifdef GL_MAP_WRITE_BIT
    map_write_bit = GL_MAP_WRITE_BIT,
#endif
#ifdef GL_MAP_WRITE_BIT_EXT
    map_write_bit_ext = GL_MAP_WRITE_BIT_EXT,
#endif
#ifdef GL_PER_GPU_STORAGE_BIT_NV
    per_gpu_storage_bit_nv = GL_PER_GPU_STORAGE_BIT_NV,
#endif
#ifdef GL_SPARSE_STORAGE_BIT_ARB
    sparse_storage_bit_arb = GL_SPARSE_STORAGE_BIT_ARB,
#endif
}; // enum class buffer_storage_mask
C_FLAGS(buffer_storage_mask, ::libc_types::u32);
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// MapBufferAccessMask
enum class map_buffer_access_mask : ::libc_types::u32 {
#ifdef GL_MAP_COHERENT_BIT
    map_coherent_bit = GL_MAP_COHERENT_BIT,
#endif
#ifdef GL_MAP_COHERENT_BIT_EXT
    map_coherent_bit_ext = GL_MAP_COHERENT_BIT_EXT,
#endif
#ifdef GL_MAP_FLUSH_EXPLICIT_BIT
    map_flush_explicit_bit = GL_MAP_FLUSH_EXPLICIT_BIT,
#endif
#ifdef GL_MAP_FLUSH_EXPLICIT_BIT_EXT
    map_flush_explicit_bit_ext = GL_MAP_FLUSH_EXPLICIT_BIT_EXT,
#endif
#ifdef GL_MAP_INVALIDATE_BUFFER_BIT
    map_invalidate_buffer_bit = GL_MAP_INVALIDATE_BUFFER_BIT,
#endif
#ifdef GL_MAP_INVALIDATE_BUFFER_BIT_EXT
    map_invalidate_buffer_bit_ext = GL_MAP_INVALIDATE_BUFFER_BIT_EXT,
#endif
#ifdef GL_MAP_INVALIDATE_RANGE_BIT
    map_invalidate_range_bit = GL_MAP_INVALIDATE_RANGE_BIT,
#endif
#ifdef GL_MAP_INVALIDATE_RANGE_BIT_EXT
    map_invalidate_range_bit_ext = GL_MAP_INVALIDATE_RANGE_BIT_EXT,
#endif
#ifdef GL_MAP_PERSISTENT_BIT
    map_persistent_bit = GL_MAP_PERSISTENT_BIT,
#endif
#ifdef GL_MAP_PERSISTENT_BIT_EXT
    map_persistent_bit_ext = GL_MAP_PERSISTENT_BIT_EXT,
#endif
#ifdef GL_MAP_READ_BIT
    map_read_bit = GL_MAP_READ_BIT,
#endif
#ifdef GL_MAP_READ_BIT_EXT
    map_read_bit_ext = GL_MAP_READ_BIT_EXT,
#endif
#ifdef GL_MAP_UNSYNCHRONIZED_BIT
    map_unsynchronized_bit = GL_MAP_UNSYNCHRONIZED_BIT,
#endif
#ifdef GL_MAP_UNSYNCHRONIZED_BIT_EXT
    map_unsynchronized_bit_ext = GL_MAP_UNSYNCHRONIZED_BIT_EXT,
#endif
#ifdef GL_MAP_WRITE_BIT
    map_write_bit = GL_MAP_WRITE_BIT,
#endif
#ifdef GL_MAP_WRITE_BIT_EXT
    map_write_bit_ext = GL_MAP_WRITE_BIT_EXT,
#endif
}; // enum class map_buffer_access_mask
C_FLAGS(map_buffer_access_mask, ::libc_types::u32);
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// MemoryBarrierMask
enum class memory_barrier_mask : ::libc_types::u32 {
#ifdef GL_ALL_BARRIER_BITS
    all_barrier_bits = GL_ALL_BARRIER_BITS,
#endif
#ifdef GL_ALL_BARRIER_BITS_EXT
    all_barrier_bits_ext = GL_ALL_BARRIER_BITS_EXT,
#endif
#ifdef GL_ATOMIC_COUNTER_BARRIER_BIT
    atomic_counter_barrier_bit = GL_ATOMIC_COUNTER_BARRIER_BIT,
#endif
#ifdef GL_ATOMIC_COUNTER_BARRIER_BIT_EXT
    atomic_counter_barrier_bit_ext = GL_ATOMIC_COUNTER_BARRIER_BIT_EXT,
#endif
#ifdef GL_BUFFER_UPDATE_BARRIER_BIT
    buffer_update_barrier_bit = GL_BUFFER_UPDATE_BARRIER_BIT,
#endif
#ifdef GL_BUFFER_UPDATE_BARRIER_BIT_EXT
    buffer_update_barrier_bit_ext = GL_BUFFER_UPDATE_BARRIER_BIT_EXT,
#endif
#ifdef GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT
    client_mapped_buffer_barrier_bit = GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT,
#endif
#ifdef GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT_EXT
    client_mapped_buffer_barrier_bit_ext = GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT_EXT,
#endif
#ifdef GL_COMMAND_BARRIER_BIT
    command_barrier_bit = GL_COMMAND_BARRIER_BIT,
#endif
#ifdef GL_COMMAND_BARRIER_BIT_EXT
    command_barrier_bit_ext = GL_COMMAND_BARRIER_BIT_EXT,
#endif
#ifdef GL_ELEMENT_ARRAY_BARRIER_BIT
    element_array_barrier_bit = GL_ELEMENT_ARRAY_BARRIER_BIT,
#endif
#ifdef GL_ELEMENT_ARRAY_BARRIER_BIT_EXT
    element_array_barrier_bit_ext = GL_ELEMENT_ARRAY_BARRIER_BIT_EXT,
#endif
#ifdef GL_FRAMEBUFFER_BARRIER_BIT
    framebuffer_barrier_bit = GL_FRAMEBUFFER_BARRIER_BIT,
#endif
#ifdef GL_FRAMEBUFFER_BARRIER_BIT_EXT
    framebuffer_barrier_bit_ext = GL_FRAMEBUFFER_BARRIER_BIT_EXT,
#endif
#ifdef GL_PIXEL_BUFFER_BARRIER_BIT
    pixel_buffer_barrier_bit = GL_PIXEL_BUFFER_BARRIER_BIT,
#endif
#ifdef GL_PIXEL_BUFFER_BARRIER_BIT_EXT
    pixel_buffer_barrier_bit_ext = GL_PIXEL_BUFFER_BARRIER_BIT_EXT,
#endif
#ifdef GL_QUERY_BUFFER_BARRIER_BIT
    query_buffer_barrier_bit = GL_QUERY_BUFFER_BARRIER_BIT,
#endif
#ifdef GL_SHADER_GLOBAL_ACCESS_BARRIER_BIT_NV
    shader_global_access_barrier_bit_nv = GL_SHADER_GLOBAL_ACCESS_BARRIER_BIT_NV,
#endif
#ifdef GL_SHADER_IMAGE_ACCESS_BARRIER_BIT
    shader_image_access_barrier_bit = GL_SHADER_IMAGE_ACCESS_BARRIER_BIT,
#endif
#ifdef GL_SHADER_IMAGE_ACCESS_BARRIER_BIT_EXT
    shader_image_access_barrier_bit_ext = GL_SHADER_IMAGE_ACCESS_BARRIER_BIT_EXT,
#endif
#ifdef GL_SHADER_STORAGE_BARRIER_BIT
    shader_storage_barrier_bit = GL_SHADER_STORAGE_BARRIER_BIT,
#endif
#ifdef GL_TEXTURE_FETCH_BARRIER_BIT
    texture_fetch_barrier_bit = GL_TEXTURE_FETCH_BARRIER_BIT,
#endif
#ifdef GL_TEXTURE_FETCH_BARRIER_BIT_EXT
    texture_fetch_barrier_bit_ext = GL_TEXTURE_FETCH_BARRIER_BIT_EXT,
#endif
#ifdef GL_TEXTURE_UPDATE_BARRIER_BIT
    texture_update_barrier_bit = GL_TEXTURE_UPDATE_BARRIER_BIT,
#endif
#ifdef GL_TEXTURE_UPDATE_BARRIER_BIT_EXT
    texture_update_barrier_bit_ext = GL_TEXTURE_UPDATE_BARRIER_BIT_EXT,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BARRIER_BIT
    transform_feedback_barrier_bit = GL_TRANSFORM_FEEDBACK_BARRIER_BIT,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BARRIER_BIT_EXT
    transform_feedback_barrier_bit_ext = GL_TRANSFORM_FEEDBACK_BARRIER_BIT_EXT,
#endif
#ifdef GL_UNIFORM_BARRIER_BIT
    uniform_barrier_bit = GL_UNIFORM_BARRIER_BIT,
#endif
#ifdef GL_UNIFORM_BARRIER_BIT_EXT
    uniform_barrier_bit_ext = GL_UNIFORM_BARRIER_BIT_EXT,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT
    vertex_attrib_array_barrier_bit = GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT_EXT
    vertex_attrib_array_barrier_bit_ext = GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT_EXT,
#endif
}; // enum class memory_barrier_mask
C_FLAGS(memory_barrier_mask, ::libc_types::u32);
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// SyncObjectMask
enum class sync_object_mask : ::libc_types::u32 {
#ifdef GL_SYNC_FLUSH_COMMANDS_BIT
    sync_flush_commands_bit = GL_SYNC_FLUSH_COMMANDS_BIT,
#endif
#ifdef GL_SYNC_FLUSH_COMMANDS_BIT_APPLE
    sync_flush_commands_bit_apple = GL_SYNC_FLUSH_COMMANDS_BIT_APPLE,
#endif
}; // enum class sync_object_mask
C_FLAGS(sync_object_mask, ::libc_types::u32);
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// UseProgramStageMask
enum class use_program_stage_mask : ::libc_types::u32 {
#ifdef GL_ALL_SHADER_BITS
    all_shader_bits = GL_ALL_SHADER_BITS,
#endif
#ifdef GL_ALL_SHADER_BITS_EXT
    all_shader_bits_ext = GL_ALL_SHADER_BITS_EXT,
#endif
#ifdef GL_COMPUTE_SHADER_BIT
    compute_shader_bit = GL_COMPUTE_SHADER_BIT,
#endif
#ifdef GL_FRAGMENT_SHADER_BIT
    fragment_shader_bit = GL_FRAGMENT_SHADER_BIT,
#endif
#ifdef GL_FRAGMENT_SHADER_BIT_EXT
    fragment_shader_bit_ext = GL_FRAGMENT_SHADER_BIT_EXT,
#endif
#ifdef GL_GEOMETRY_SHADER_BIT
    geometry_shader_bit = GL_GEOMETRY_SHADER_BIT,
#endif
#ifdef GL_GEOMETRY_SHADER_BIT_EXT
    geometry_shader_bit_ext = GL_GEOMETRY_SHADER_BIT_EXT,
#endif
#ifdef GL_GEOMETRY_SHADER_BIT_OES
    geometry_shader_bit_oes = GL_GEOMETRY_SHADER_BIT_OES,
#endif
#ifdef GL_MESH_SHADER_BIT_NV
    mesh_shader_bit_nv = GL_MESH_SHADER_BIT_NV,
#endif
#ifdef GL_TASK_SHADER_BIT_NV
    task_shader_bit_nv = GL_TASK_SHADER_BIT_NV,
#endif
#ifdef GL_TESS_CONTROL_SHADER_BIT
    tess_control_shader_bit = GL_TESS_CONTROL_SHADER_BIT,
#endif
#ifdef GL_TESS_CONTROL_SHADER_BIT_EXT
    tess_control_shader_bit_ext = GL_TESS_CONTROL_SHADER_BIT_EXT,
#endif
#ifdef GL_TESS_CONTROL_SHADER_BIT_OES
    tess_control_shader_bit_oes = GL_TESS_CONTROL_SHADER_BIT_OES,
#endif
#ifdef GL_TESS_EVALUATION_SHADER_BIT
    tess_evaluation_shader_bit = GL_TESS_EVALUATION_SHADER_BIT,
#endif
#ifdef GL_TESS_EVALUATION_SHADER_BIT_EXT
    tess_evaluation_shader_bit_ext = GL_TESS_EVALUATION_SHADER_BIT_EXT,
#endif
#ifdef GL_TESS_EVALUATION_SHADER_BIT_OES
    tess_evaluation_shader_bit_oes = GL_TESS_EVALUATION_SHADER_BIT_OES,
#endif
#ifdef GL_VERTEX_SHADER_BIT
    vertex_shader_bit = GL_VERTEX_SHADER_BIT,
#endif
#ifdef GL_VERTEX_SHADER_BIT_EXT
    vertex_shader_bit_ext = GL_VERTEX_SHADER_BIT_EXT,
#endif
}; // enum class use_program_stage_mask
C_FLAGS(use_program_stage_mask, ::libc_types::u32);
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// TextureStorageMaskAMD
enum class texture_storage_mask_amd : ::libc_types::u32 {
#ifdef GL_TEXTURE_STORAGE_SPARSE_BIT_AMD
    texture_storage_sparse_bit_amd = GL_TEXTURE_STORAGE_SPARSE_BIT_AMD,
#endif
}; // enum class texture_storage_mask_amd
C_FLAGS(texture_storage_mask_amd, ::libc_types::u32);
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// PathFontStyle
enum class path_font_style : ::libc_types::u32 {
#ifdef GL_BOLD_BIT_NV
    bold_bit_nv = GL_BOLD_BIT_NV,
#endif
#ifdef GL_ITALIC_BIT_NV
    italic_bit_nv = GL_ITALIC_BIT_NV,
#endif
#ifdef GL_NONE
    none = GL_NONE,
#endif
}; // enum class path_font_style
C_FLAGS(path_font_style, ::libc_types::u32);
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// PathMetricMask
enum class path_metric_mask : ::libc_types::u32 {
#ifdef GL_FONT_ASCENDER_BIT_NV
    font_ascender_bit_nv = GL_FONT_ASCENDER_BIT_NV,
#endif
#ifdef GL_FONT_DESCENDER_BIT_NV
    font_descender_bit_nv = GL_FONT_DESCENDER_BIT_NV,
#endif
#ifdef GL_FONT_HAS_KERNING_BIT_NV
    font_has_kerning_bit_nv = GL_FONT_HAS_KERNING_BIT_NV,
#endif
#ifdef GL_FONT_HEIGHT_BIT_NV
    font_height_bit_nv = GL_FONT_HEIGHT_BIT_NV,
#endif
#ifdef GL_FONT_MAX_ADVANCE_HEIGHT_BIT_NV
    font_max_advance_height_bit_nv = GL_FONT_MAX_ADVANCE_HEIGHT_BIT_NV,
#endif
#ifdef GL_FONT_MAX_ADVANCE_WIDTH_BIT_NV
    font_max_advance_width_bit_nv = GL_FONT_MAX_ADVANCE_WIDTH_BIT_NV,
#endif
#ifdef GL_FONT_NUM_GLYPH_INDICES_BIT_NV
    font_num_glyph_indices_bit_nv = GL_FONT_NUM_GLYPH_INDICES_BIT_NV,
#endif
#ifdef GL_FONT_UNDERLINE_POSITION_BIT_NV
    font_underline_position_bit_nv = GL_FONT_UNDERLINE_POSITION_BIT_NV,
#endif
#ifdef GL_FONT_UNDERLINE_THICKNESS_BIT_NV
    font_underline_thickness_bit_nv = GL_FONT_UNDERLINE_THICKNESS_BIT_NV,
#endif
#ifdef GL_FONT_UNITS_PER_EM_BIT_NV
    font_units_per_em_bit_nv = GL_FONT_UNITS_PER_EM_BIT_NV,
#endif
#ifdef GL_FONT_X_MAX_BOUNDS_BIT_NV
    font_x_max_bounds_bit_nv = GL_FONT_X_MAX_BOUNDS_BIT_NV,
#endif
#ifdef GL_FONT_X_MIN_BOUNDS_BIT_NV
    font_x_min_bounds_bit_nv = GL_FONT_X_MIN_BOUNDS_BIT_NV,
#endif
#ifdef GL_FONT_Y_MAX_BOUNDS_BIT_NV
    font_y_max_bounds_bit_nv = GL_FONT_Y_MAX_BOUNDS_BIT_NV,
#endif
#ifdef GL_FONT_Y_MIN_BOUNDS_BIT_NV
    font_y_min_bounds_bit_nv = GL_FONT_Y_MIN_BOUNDS_BIT_NV,
#endif
#ifdef GL_GLYPH_HAS_KERNING_BIT_NV
    glyph_has_kerning_bit_nv = GL_GLYPH_HAS_KERNING_BIT_NV,
#endif
#ifdef GL_GLYPH_HEIGHT_BIT_NV
    glyph_height_bit_nv = GL_GLYPH_HEIGHT_BIT_NV,
#endif
#ifdef GL_GLYPH_HORIZONTAL_BEARING_ADVANCE_BIT_NV
    glyph_horizontal_bearing_advance_bit_nv = GL_GLYPH_HORIZONTAL_BEARING_ADVANCE_BIT_NV,
#endif
#ifdef GL_GLYPH_HORIZONTAL_BEARING_X_BIT_NV
    glyph_horizontal_bearing_x_bit_nv = GL_GLYPH_HORIZONTAL_BEARING_X_BIT_NV,
#endif
#ifdef GL_GLYPH_HORIZONTAL_BEARING_Y_BIT_NV
    glyph_horizontal_bearing_y_bit_nv = GL_GLYPH_HORIZONTAL_BEARING_Y_BIT_NV,
#endif
#ifdef GL_GLYPH_VERTICAL_BEARING_ADVANCE_BIT_NV
    glyph_vertical_bearing_advance_bit_nv = GL_GLYPH_VERTICAL_BEARING_ADVANCE_BIT_NV,
#endif
#ifdef GL_GLYPH_VERTICAL_BEARING_X_BIT_NV
    glyph_vertical_bearing_x_bit_nv = GL_GLYPH_VERTICAL_BEARING_X_BIT_NV,
#endif
#ifdef GL_GLYPH_VERTICAL_BEARING_Y_BIT_NV
    glyph_vertical_bearing_y_bit_nv = GL_GLYPH_VERTICAL_BEARING_Y_BIT_NV,
#endif
#ifdef GL_GLYPH_WIDTH_BIT_NV
    glyph_width_bit_nv = GL_GLYPH_WIDTH_BIT_NV,
#endif
}; // enum class path_metric_mask
C_FLAGS(path_metric_mask, ::libc_types::u32);
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// BufferBitQCOM
enum class buffer_bit_qcom : ::libc_types::u32 {
#ifdef GL_COLOR_BUFFER_BIT0_QCOM
    color_buffer_bit0_qcom = GL_COLOR_BUFFER_BIT0_QCOM,
#endif
#ifdef GL_COLOR_BUFFER_BIT1_QCOM
    color_buffer_bit1_qcom = GL_COLOR_BUFFER_BIT1_QCOM,
#endif
#ifdef GL_COLOR_BUFFER_BIT2_QCOM
    color_buffer_bit2_qcom = GL_COLOR_BUFFER_BIT2_QCOM,
#endif
#ifdef GL_COLOR_BUFFER_BIT3_QCOM
    color_buffer_bit3_qcom = GL_COLOR_BUFFER_BIT3_QCOM,
#endif
#ifdef GL_COLOR_BUFFER_BIT4_QCOM
    color_buffer_bit4_qcom = GL_COLOR_BUFFER_BIT4_QCOM,
#endif
#ifdef GL_COLOR_BUFFER_BIT5_QCOM
    color_buffer_bit5_qcom = GL_COLOR_BUFFER_BIT5_QCOM,
#endif
#ifdef GL_COLOR_BUFFER_BIT6_QCOM
    color_buffer_bit6_qcom = GL_COLOR_BUFFER_BIT6_QCOM,
#endif
#ifdef GL_COLOR_BUFFER_BIT7_QCOM
    color_buffer_bit7_qcom = GL_COLOR_BUFFER_BIT7_QCOM,
#endif
#ifdef GL_DEPTH_BUFFER_BIT0_QCOM
    depth_buffer_bit0_qcom = GL_DEPTH_BUFFER_BIT0_QCOM,
#endif
#ifdef GL_DEPTH_BUFFER_BIT1_QCOM
    depth_buffer_bit1_qcom = GL_DEPTH_BUFFER_BIT1_QCOM,
#endif
#ifdef GL_DEPTH_BUFFER_BIT2_QCOM
    depth_buffer_bit2_qcom = GL_DEPTH_BUFFER_BIT2_QCOM,
#endif
#ifdef GL_DEPTH_BUFFER_BIT3_QCOM
    depth_buffer_bit3_qcom = GL_DEPTH_BUFFER_BIT3_QCOM,
#endif
#ifdef GL_DEPTH_BUFFER_BIT4_QCOM
    depth_buffer_bit4_qcom = GL_DEPTH_BUFFER_BIT4_QCOM,
#endif
#ifdef GL_DEPTH_BUFFER_BIT5_QCOM
    depth_buffer_bit5_qcom = GL_DEPTH_BUFFER_BIT5_QCOM,
#endif
#ifdef GL_DEPTH_BUFFER_BIT6_QCOM
    depth_buffer_bit6_qcom = GL_DEPTH_BUFFER_BIT6_QCOM,
#endif
#ifdef GL_DEPTH_BUFFER_BIT7_QCOM
    depth_buffer_bit7_qcom = GL_DEPTH_BUFFER_BIT7_QCOM,
#endif
#ifdef GL_MULTISAMPLE_BUFFER_BIT0_QCOM
    multisample_buffer_bit0_qcom = GL_MULTISAMPLE_BUFFER_BIT0_QCOM,
#endif
#ifdef GL_MULTISAMPLE_BUFFER_BIT1_QCOM
    multisample_buffer_bit1_qcom = GL_MULTISAMPLE_BUFFER_BIT1_QCOM,
#endif
#ifdef GL_MULTISAMPLE_BUFFER_BIT2_QCOM
    multisample_buffer_bit2_qcom = GL_MULTISAMPLE_BUFFER_BIT2_QCOM,
#endif
#ifdef GL_MULTISAMPLE_BUFFER_BIT3_QCOM
    multisample_buffer_bit3_qcom = GL_MULTISAMPLE_BUFFER_BIT3_QCOM,
#endif
#ifdef GL_MULTISAMPLE_BUFFER_BIT4_QCOM
    multisample_buffer_bit4_qcom = GL_MULTISAMPLE_BUFFER_BIT4_QCOM,
#endif
#ifdef GL_MULTISAMPLE_BUFFER_BIT5_QCOM
    multisample_buffer_bit5_qcom = GL_MULTISAMPLE_BUFFER_BIT5_QCOM,
#endif
#ifdef GL_MULTISAMPLE_BUFFER_BIT6_QCOM
    multisample_buffer_bit6_qcom = GL_MULTISAMPLE_BUFFER_BIT6_QCOM,
#endif
#ifdef GL_MULTISAMPLE_BUFFER_BIT7_QCOM
    multisample_buffer_bit7_qcom = GL_MULTISAMPLE_BUFFER_BIT7_QCOM,
#endif
#ifdef GL_STENCIL_BUFFER_BIT0_QCOM
    stencil_buffer_bit0_qcom = GL_STENCIL_BUFFER_BIT0_QCOM,
#endif
#ifdef GL_STENCIL_BUFFER_BIT1_QCOM
    stencil_buffer_bit1_qcom = GL_STENCIL_BUFFER_BIT1_QCOM,
#endif
#ifdef GL_STENCIL_BUFFER_BIT2_QCOM
    stencil_buffer_bit2_qcom = GL_STENCIL_BUFFER_BIT2_QCOM,
#endif
#ifdef GL_STENCIL_BUFFER_BIT3_QCOM
    stencil_buffer_bit3_qcom = GL_STENCIL_BUFFER_BIT3_QCOM,
#endif
#ifdef GL_STENCIL_BUFFER_BIT4_QCOM
    stencil_buffer_bit4_qcom = GL_STENCIL_BUFFER_BIT4_QCOM,
#endif
#ifdef GL_STENCIL_BUFFER_BIT5_QCOM
    stencil_buffer_bit5_qcom = GL_STENCIL_BUFFER_BIT5_QCOM,
#endif
#ifdef GL_STENCIL_BUFFER_BIT6_QCOM
    stencil_buffer_bit6_qcom = GL_STENCIL_BUFFER_BIT6_QCOM,
#endif
#ifdef GL_STENCIL_BUFFER_BIT7_QCOM
    stencil_buffer_bit7_qcom = GL_STENCIL_BUFFER_BIT7_QCOM,
#endif
}; // enum class buffer_bit_qcom
C_FLAGS(buffer_bit_qcom, ::libc_types::u32);
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// PathCoordType
enum class path_coord_type : ::libc_types::u32 {
#ifdef GL_ARC_TO_NV
    arc_to_nv = GL_ARC_TO_NV,
#endif
#ifdef GL_CIRCULAR_CCW_ARC_TO_NV
    circular_ccw_arc_to_nv = GL_CIRCULAR_CCW_ARC_TO_NV,
#endif
#ifdef GL_CIRCULAR_CW_ARC_TO_NV
    circular_cw_arc_to_nv = GL_CIRCULAR_CW_ARC_TO_NV,
#endif
#ifdef GL_CIRCULAR_TANGENT_ARC_TO_NV
    circular_tangent_arc_to_nv = GL_CIRCULAR_TANGENT_ARC_TO_NV,
#endif
#ifdef GL_CLOSE_PATH_NV
    close_path_nv = GL_CLOSE_PATH_NV,
#endif
#ifdef GL_CONIC_CURVE_TO_NV
    conic_curve_to_nv = GL_CONIC_CURVE_TO_NV,
#endif
#ifdef GL_CUBIC_CURVE_TO_NV
    cubic_curve_to_nv = GL_CUBIC_CURVE_TO_NV,
#endif
#ifdef GL_DUP_FIRST_CUBIC_CURVE_TO_NV
    dup_first_cubic_curve_to_nv = GL_DUP_FIRST_CUBIC_CURVE_TO_NV,
#endif
#ifdef GL_DUP_LAST_CUBIC_CURVE_TO_NV
    dup_last_cubic_curve_to_nv = GL_DUP_LAST_CUBIC_CURVE_TO_NV,
#endif
#ifdef GL_HORIZONTAL_LINE_TO_NV
    horizontal_line_to_nv = GL_HORIZONTAL_LINE_TO_NV,
#endif
#ifdef GL_LARGE_CCW_ARC_TO_NV
    large_ccw_arc_to_nv = GL_LARGE_CCW_ARC_TO_NV,
#endif
#ifdef GL_LARGE_CW_ARC_TO_NV
    large_cw_arc_to_nv = GL_LARGE_CW_ARC_TO_NV,
#endif
#ifdef GL_LINE_TO_NV
    line_to_nv = GL_LINE_TO_NV,
#endif
#ifdef GL_MOVE_TO_NV
    move_to_nv = GL_MOVE_TO_NV,
#endif
#ifdef GL_QUADRATIC_CURVE_TO_NV
    quadratic_curve_to_nv = GL_QUADRATIC_CURVE_TO_NV,
#endif
#ifdef GL_RECT_NV
    rect_nv = GL_RECT_NV,
#endif
#ifdef GL_RELATIVE_ARC_TO_NV
    relative_arc_to_nv = GL_RELATIVE_ARC_TO_NV,
#endif
#ifdef GL_RELATIVE_CONIC_CURVE_TO_NV
    relative_conic_curve_to_nv = GL_RELATIVE_CONIC_CURVE_TO_NV,
#endif
#ifdef GL_RELATIVE_CUBIC_CURVE_TO_NV
    relative_cubic_curve_to_nv = GL_RELATIVE_CUBIC_CURVE_TO_NV,
#endif
#ifdef GL_RELATIVE_HORIZONTAL_LINE_TO_NV
    relative_horizontal_line_to_nv = GL_RELATIVE_HORIZONTAL_LINE_TO_NV,
#endif
#ifdef GL_RELATIVE_LARGE_CCW_ARC_TO_NV
    relative_large_ccw_arc_to_nv = GL_RELATIVE_LARGE_CCW_ARC_TO_NV,
#endif
#ifdef GL_RELATIVE_LARGE_CW_ARC_TO_NV
    relative_large_cw_arc_to_nv = GL_RELATIVE_LARGE_CW_ARC_TO_NV,
#endif
#ifdef GL_RELATIVE_LINE_TO_NV
    relative_line_to_nv = GL_RELATIVE_LINE_TO_NV,
#endif
#ifdef GL_RELATIVE_MOVE_TO_NV
    relative_move_to_nv = GL_RELATIVE_MOVE_TO_NV,
#endif
#ifdef GL_RELATIVE_QUADRATIC_CURVE_TO_NV
    relative_quadratic_curve_to_nv = GL_RELATIVE_QUADRATIC_CURVE_TO_NV,
#endif
#ifdef GL_RELATIVE_RECT_NV
    relative_rect_nv = GL_RELATIVE_RECT_NV,
#endif
#ifdef GL_RELATIVE_ROUNDED_RECT2_NV
    relative_rounded_rect2_nv = GL_RELATIVE_ROUNDED_RECT2_NV,
#endif
#ifdef GL_RELATIVE_ROUNDED_RECT4_NV
    relative_rounded_rect4_nv = GL_RELATIVE_ROUNDED_RECT4_NV,
#endif
#ifdef GL_RELATIVE_ROUNDED_RECT8_NV
    relative_rounded_rect8_nv = GL_RELATIVE_ROUNDED_RECT8_NV,
#endif
#ifdef GL_RELATIVE_ROUNDED_RECT_NV
    relative_rounded_rect_nv = GL_RELATIVE_ROUNDED_RECT_NV,
#endif
#ifdef GL_RELATIVE_SMALL_CCW_ARC_TO_NV
    relative_small_ccw_arc_to_nv = GL_RELATIVE_SMALL_CCW_ARC_TO_NV,
#endif
#ifdef GL_RELATIVE_SMALL_CW_ARC_TO_NV
    relative_small_cw_arc_to_nv = GL_RELATIVE_SMALL_CW_ARC_TO_NV,
#endif
#ifdef GL_RELATIVE_SMOOTH_CUBIC_CURVE_TO_NV
    relative_smooth_cubic_curve_to_nv = GL_RELATIVE_SMOOTH_CUBIC_CURVE_TO_NV,
#endif
#ifdef GL_RELATIVE_SMOOTH_QUADRATIC_CURVE_TO_NV
    relative_smooth_quadratic_curve_to_nv = GL_RELATIVE_SMOOTH_QUADRATIC_CURVE_TO_NV,
#endif
#ifdef GL_RELATIVE_VERTICAL_LINE_TO_NV
    relative_vertical_line_to_nv = GL_RELATIVE_VERTICAL_LINE_TO_NV,
#endif
#ifdef GL_RESTART_PATH_NV
    restart_path_nv = GL_RESTART_PATH_NV,
#endif
#ifdef GL_ROUNDED_RECT2_NV
    rounded_rect2_nv = GL_ROUNDED_RECT2_NV,
#endif
#ifdef GL_ROUNDED_RECT4_NV
    rounded_rect4_nv = GL_ROUNDED_RECT4_NV,
#endif
#ifdef GL_ROUNDED_RECT8_NV
    rounded_rect8_nv = GL_ROUNDED_RECT8_NV,
#endif
#ifdef GL_ROUNDED_RECT_NV
    rounded_rect_nv = GL_ROUNDED_RECT_NV,
#endif
#ifdef GL_SMALL_CCW_ARC_TO_NV
    small_ccw_arc_to_nv = GL_SMALL_CCW_ARC_TO_NV,
#endif
#ifdef GL_SMALL_CW_ARC_TO_NV
    small_cw_arc_to_nv = GL_SMALL_CW_ARC_TO_NV,
#endif
#ifdef GL_SMOOTH_CUBIC_CURVE_TO_NV
    smooth_cubic_curve_to_nv = GL_SMOOTH_CUBIC_CURVE_TO_NV,
#endif
#ifdef GL_SMOOTH_QUADRATIC_CURVE_TO_NV
    smooth_quadratic_curve_to_nv = GL_SMOOTH_QUADRATIC_CURVE_TO_NV,
#endif
#ifdef GL_VERTICAL_LINE_TO_NV
    vertical_line_to_nv = GL_VERTICAL_LINE_TO_NV,
#endif
}; // enum class path_coord_type
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ClampColorModeARB
enum class clamp_color_mode_arb : ::libc_types::u32 {
#ifdef GL_FALSE
    false_ = GL_FALSE,
#endif
#ifdef GL_FIXED_ONLY
    fixed_only = GL_FIXED_ONLY,
#endif
#ifdef GL_FIXED_ONLY_ARB
    fixed_only_arb = GL_FIXED_ONLY_ARB,
#endif
#ifdef GL_TRUE
    true_ = GL_TRUE,
#endif
}; // enum class clamp_color_mode_arb
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// StencilOp
enum class stencil_op : ::libc_types::u32 {
#ifdef GL_DECR
    decr = GL_DECR,
#endif
#ifdef GL_DECR_WRAP
    decr_wrap = GL_DECR_WRAP,
#endif
#ifdef GL_INCR
    incr = GL_INCR,
#endif
#ifdef GL_INCR_WRAP
    incr_wrap = GL_INCR_WRAP,
#endif
#ifdef GL_INVERT
    invert = GL_INVERT,
#endif
#ifdef GL_KEEP
    keep = GL_KEEP,
#endif
#ifdef GL_REPLACE
    replace = GL_REPLACE,
#endif
#ifdef GL_ZERO
    zero = GL_ZERO,
#endif
}; // enum class stencil_op
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// BlendingFactor
enum class blending_factor : ::libc_types::u32 {
#ifdef GL_CONSTANT_ALPHA
    constant_alpha = GL_CONSTANT_ALPHA,
#endif
#ifdef GL_CONSTANT_COLOR
    constant_color = GL_CONSTANT_COLOR,
#endif
#ifdef GL_DST_ALPHA
    dst_alpha = GL_DST_ALPHA,
#endif
#ifdef GL_DST_COLOR
    dst_color = GL_DST_COLOR,
#endif
#ifdef GL_ONE
    one = GL_ONE,
#endif
#ifdef GL_ONE_MINUS_CONSTANT_ALPHA
    one_minus_constant_alpha = GL_ONE_MINUS_CONSTANT_ALPHA,
#endif
#ifdef GL_ONE_MINUS_CONSTANT_COLOR
    one_minus_constant_color = GL_ONE_MINUS_CONSTANT_COLOR,
#endif
#ifdef GL_ONE_MINUS_DST_ALPHA
    one_minus_dst_alpha = GL_ONE_MINUS_DST_ALPHA,
#endif
#ifdef GL_ONE_MINUS_DST_COLOR
    one_minus_dst_color = GL_ONE_MINUS_DST_COLOR,
#endif
#ifdef GL_ONE_MINUS_SRC1_ALPHA
    one_minus_src1_alpha = GL_ONE_MINUS_SRC1_ALPHA,
#endif
#ifdef GL_ONE_MINUS_SRC1_COLOR
    one_minus_src1_color = GL_ONE_MINUS_SRC1_COLOR,
#endif
#ifdef GL_ONE_MINUS_SRC_ALPHA
    one_minus_src_alpha = GL_ONE_MINUS_SRC_ALPHA,
#endif
#ifdef GL_ONE_MINUS_SRC_COLOR
    one_minus_src_color = GL_ONE_MINUS_SRC_COLOR,
#endif
#ifdef GL_SRC1_ALPHA
    src1_alpha = GL_SRC1_ALPHA,
#endif
#ifdef GL_SRC1_COLOR
    src1_color = GL_SRC1_COLOR,
#endif
#ifdef GL_SRC_ALPHA
    src_alpha = GL_SRC_ALPHA,
#endif
#ifdef GL_SRC_ALPHA_SATURATE
    src_alpha_saturate = GL_SRC_ALPHA_SATURATE,
#endif
#ifdef GL_SRC_COLOR
    src_color = GL_SRC_COLOR,
#endif
#ifdef GL_ZERO
    zero = GL_ZERO,
#endif
}; // enum class blending_factor
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// SyncBehaviorFlags
enum class sync_behavior_flags : ::libc_types::u32 {
#ifdef GL_NONE
    none = GL_NONE,
#endif
}; // enum class sync_behavior_flags
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// PathColorFormat
enum class path_color_format : ::libc_types::u32 {
#ifdef GL_ALPHA
    alpha = GL_ALPHA,
#endif
#ifdef GL_INTENSITY
    intensity = GL_INTENSITY,
#endif
#ifdef GL_LUMINANCE
    luminance = GL_LUMINANCE,
#endif
#ifdef GL_LUMINANCE_ALPHA
    luminance_alpha = GL_LUMINANCE_ALPHA,
#endif
#ifdef GL_NONE
    none = GL_NONE,
#endif
#ifdef GL_RGB
    rgb = GL_RGB,
#endif
#ifdef GL_RGBA
    rgba = GL_RGBA,
#endif
}; // enum class path_color_format
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// CombinerBiasNV
enum class combiner_bias_nv : ::libc_types::u32 {
#ifdef GL_BIAS_BY_NEGATIVE_ONE_HALF_NV
    bias_by_negative_one_half_nv = GL_BIAS_BY_NEGATIVE_ONE_HALF_NV,
#endif
#ifdef GL_NONE
    none = GL_NONE,
#endif
}; // enum class combiner_bias_nv
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// CombinerScaleNV
enum class combiner_scale_nv : ::libc_types::u32 {
#ifdef GL_NONE
    none = GL_NONE,
#endif
#ifdef GL_SCALE_BY_FOUR_NV
    scale_by_four_nv = GL_SCALE_BY_FOUR_NV,
#endif
#ifdef GL_SCALE_BY_ONE_HALF_NV
    scale_by_one_half_nv = GL_SCALE_BY_ONE_HALF_NV,
#endif
#ifdef GL_SCALE_BY_TWO_NV
    scale_by_two_nv = GL_SCALE_BY_TWO_NV,
#endif
}; // enum class combiner_scale_nv
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// DrawBufferMode
enum class draw_buffer_mode : ::libc_types::u32 {
#ifdef GL_AUX0
    aux0 = GL_AUX0,
#endif
#ifdef GL_AUX1
    aux1 = GL_AUX1,
#endif
#ifdef GL_AUX2
    aux2 = GL_AUX2,
#endif
#ifdef GL_AUX3
    aux3 = GL_AUX3,
#endif
#ifdef GL_BACK
    back = GL_BACK,
#endif
#ifdef GL_BACK_LEFT
    back_left = GL_BACK_LEFT,
#endif
#ifdef GL_BACK_RIGHT
    back_right = GL_BACK_RIGHT,
#endif
#ifdef GL_COLOR_ATTACHMENT0
    color_attachment0 = GL_COLOR_ATTACHMENT0,
#endif
#ifdef GL_COLOR_ATTACHMENT1
    color_attachment1 = GL_COLOR_ATTACHMENT1,
#endif
#ifdef GL_COLOR_ATTACHMENT10
    color_attachment10 = GL_COLOR_ATTACHMENT10,
#endif
#ifdef GL_COLOR_ATTACHMENT11
    color_attachment11 = GL_COLOR_ATTACHMENT11,
#endif
#ifdef GL_COLOR_ATTACHMENT12
    color_attachment12 = GL_COLOR_ATTACHMENT12,
#endif
#ifdef GL_COLOR_ATTACHMENT13
    color_attachment13 = GL_COLOR_ATTACHMENT13,
#endif
#ifdef GL_COLOR_ATTACHMENT14
    color_attachment14 = GL_COLOR_ATTACHMENT14,
#endif
#ifdef GL_COLOR_ATTACHMENT15
    color_attachment15 = GL_COLOR_ATTACHMENT15,
#endif
#ifdef GL_COLOR_ATTACHMENT16
    color_attachment16 = GL_COLOR_ATTACHMENT16,
#endif
#ifdef GL_COLOR_ATTACHMENT17
    color_attachment17 = GL_COLOR_ATTACHMENT17,
#endif
#ifdef GL_COLOR_ATTACHMENT18
    color_attachment18 = GL_COLOR_ATTACHMENT18,
#endif
#ifdef GL_COLOR_ATTACHMENT19
    color_attachment19 = GL_COLOR_ATTACHMENT19,
#endif
#ifdef GL_COLOR_ATTACHMENT2
    color_attachment2 = GL_COLOR_ATTACHMENT2,
#endif
#ifdef GL_COLOR_ATTACHMENT20
    color_attachment20 = GL_COLOR_ATTACHMENT20,
#endif
#ifdef GL_COLOR_ATTACHMENT21
    color_attachment21 = GL_COLOR_ATTACHMENT21,
#endif
#ifdef GL_COLOR_ATTACHMENT22
    color_attachment22 = GL_COLOR_ATTACHMENT22,
#endif
#ifdef GL_COLOR_ATTACHMENT23
    color_attachment23 = GL_COLOR_ATTACHMENT23,
#endif
#ifdef GL_COLOR_ATTACHMENT24
    color_attachment24 = GL_COLOR_ATTACHMENT24,
#endif
#ifdef GL_COLOR_ATTACHMENT25
    color_attachment25 = GL_COLOR_ATTACHMENT25,
#endif
#ifdef GL_COLOR_ATTACHMENT26
    color_attachment26 = GL_COLOR_ATTACHMENT26,
#endif
#ifdef GL_COLOR_ATTACHMENT27
    color_attachment27 = GL_COLOR_ATTACHMENT27,
#endif
#ifdef GL_COLOR_ATTACHMENT28
    color_attachment28 = GL_COLOR_ATTACHMENT28,
#endif
#ifdef GL_COLOR_ATTACHMENT29
    color_attachment29 = GL_COLOR_ATTACHMENT29,
#endif
#ifdef GL_COLOR_ATTACHMENT3
    color_attachment3 = GL_COLOR_ATTACHMENT3,
#endif
#ifdef GL_COLOR_ATTACHMENT30
    color_attachment30 = GL_COLOR_ATTACHMENT30,
#endif
#ifdef GL_COLOR_ATTACHMENT31
    color_attachment31 = GL_COLOR_ATTACHMENT31,
#endif
#ifdef GL_COLOR_ATTACHMENT4
    color_attachment4 = GL_COLOR_ATTACHMENT4,
#endif
#ifdef GL_COLOR_ATTACHMENT5
    color_attachment5 = GL_COLOR_ATTACHMENT5,
#endif
#ifdef GL_COLOR_ATTACHMENT6
    color_attachment6 = GL_COLOR_ATTACHMENT6,
#endif
#ifdef GL_COLOR_ATTACHMENT7
    color_attachment7 = GL_COLOR_ATTACHMENT7,
#endif
#ifdef GL_COLOR_ATTACHMENT8
    color_attachment8 = GL_COLOR_ATTACHMENT8,
#endif
#ifdef GL_COLOR_ATTACHMENT9
    color_attachment9 = GL_COLOR_ATTACHMENT9,
#endif
#ifdef GL_FRONT
    front = GL_FRONT,
#endif
#ifdef GL_FRONT_AND_BACK
    front_and_back = GL_FRONT_AND_BACK,
#endif
#ifdef GL_FRONT_LEFT
    front_left = GL_FRONT_LEFT,
#endif
#ifdef GL_FRONT_RIGHT
    front_right = GL_FRONT_RIGHT,
#endif
#ifdef GL_LEFT
    left = GL_LEFT,
#endif
#ifdef GL_NONE
    none = GL_NONE,
#endif
#ifdef GL_NONE_OES
    none_oes = GL_NONE_OES,
#endif
#ifdef GL_RIGHT
    right = GL_RIGHT,
#endif
}; // enum class draw_buffer_mode
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ReadBufferMode
enum class read_buffer_mode : ::libc_types::u32 {
#ifdef GL_AUX0
    aux0 = GL_AUX0,
#endif
#ifdef GL_AUX1
    aux1 = GL_AUX1,
#endif
#ifdef GL_AUX2
    aux2 = GL_AUX2,
#endif
#ifdef GL_AUX3
    aux3 = GL_AUX3,
#endif
#ifdef GL_BACK
    back = GL_BACK,
#endif
#ifdef GL_BACK_LEFT
    back_left = GL_BACK_LEFT,
#endif
#ifdef GL_BACK_RIGHT
    back_right = GL_BACK_RIGHT,
#endif
#ifdef GL_COLOR_ATTACHMENT0
    color_attachment0 = GL_COLOR_ATTACHMENT0,
#endif
#ifdef GL_COLOR_ATTACHMENT1
    color_attachment1 = GL_COLOR_ATTACHMENT1,
#endif
#ifdef GL_COLOR_ATTACHMENT10
    color_attachment10 = GL_COLOR_ATTACHMENT10,
#endif
#ifdef GL_COLOR_ATTACHMENT11
    color_attachment11 = GL_COLOR_ATTACHMENT11,
#endif
#ifdef GL_COLOR_ATTACHMENT12
    color_attachment12 = GL_COLOR_ATTACHMENT12,
#endif
#ifdef GL_COLOR_ATTACHMENT13
    color_attachment13 = GL_COLOR_ATTACHMENT13,
#endif
#ifdef GL_COLOR_ATTACHMENT14
    color_attachment14 = GL_COLOR_ATTACHMENT14,
#endif
#ifdef GL_COLOR_ATTACHMENT15
    color_attachment15 = GL_COLOR_ATTACHMENT15,
#endif
#ifdef GL_COLOR_ATTACHMENT2
    color_attachment2 = GL_COLOR_ATTACHMENT2,
#endif
#ifdef GL_COLOR_ATTACHMENT3
    color_attachment3 = GL_COLOR_ATTACHMENT3,
#endif
#ifdef GL_COLOR_ATTACHMENT4
    color_attachment4 = GL_COLOR_ATTACHMENT4,
#endif
#ifdef GL_COLOR_ATTACHMENT5
    color_attachment5 = GL_COLOR_ATTACHMENT5,
#endif
#ifdef GL_COLOR_ATTACHMENT6
    color_attachment6 = GL_COLOR_ATTACHMENT6,
#endif
#ifdef GL_COLOR_ATTACHMENT7
    color_attachment7 = GL_COLOR_ATTACHMENT7,
#endif
#ifdef GL_COLOR_ATTACHMENT8
    color_attachment8 = GL_COLOR_ATTACHMENT8,
#endif
#ifdef GL_COLOR_ATTACHMENT9
    color_attachment9 = GL_COLOR_ATTACHMENT9,
#endif
#ifdef GL_FRONT
    front = GL_FRONT,
#endif
#ifdef GL_FRONT_LEFT
    front_left = GL_FRONT_LEFT,
#endif
#ifdef GL_FRONT_RIGHT
    front_right = GL_FRONT_RIGHT,
#endif
#ifdef GL_LEFT
    left = GL_LEFT,
#endif
#ifdef GL_NONE
    none = GL_NONE,
#endif
#ifdef GL_NONE_OES
    none_oes = GL_NONE_OES,
#endif
#ifdef GL_RIGHT
    right = GL_RIGHT,
#endif
}; // enum class read_buffer_mode
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ColorBuffer
enum class color_buffer : ::libc_types::u32 {
#ifdef GL_BACK
    back = GL_BACK,
#endif
#ifdef GL_BACK_LEFT
    back_left = GL_BACK_LEFT,
#endif
#ifdef GL_BACK_RIGHT
    back_right = GL_BACK_RIGHT,
#endif
#ifdef GL_COLOR_ATTACHMENT0
    color_attachment0 = GL_COLOR_ATTACHMENT0,
#endif
#ifdef GL_COLOR_ATTACHMENT1
    color_attachment1 = GL_COLOR_ATTACHMENT1,
#endif
#ifdef GL_COLOR_ATTACHMENT10
    color_attachment10 = GL_COLOR_ATTACHMENT10,
#endif
#ifdef GL_COLOR_ATTACHMENT11
    color_attachment11 = GL_COLOR_ATTACHMENT11,
#endif
#ifdef GL_COLOR_ATTACHMENT12
    color_attachment12 = GL_COLOR_ATTACHMENT12,
#endif
#ifdef GL_COLOR_ATTACHMENT13
    color_attachment13 = GL_COLOR_ATTACHMENT13,
#endif
#ifdef GL_COLOR_ATTACHMENT14
    color_attachment14 = GL_COLOR_ATTACHMENT14,
#endif
#ifdef GL_COLOR_ATTACHMENT15
    color_attachment15 = GL_COLOR_ATTACHMENT15,
#endif
#ifdef GL_COLOR_ATTACHMENT16
    color_attachment16 = GL_COLOR_ATTACHMENT16,
#endif
#ifdef GL_COLOR_ATTACHMENT17
    color_attachment17 = GL_COLOR_ATTACHMENT17,
#endif
#ifdef GL_COLOR_ATTACHMENT18
    color_attachment18 = GL_COLOR_ATTACHMENT18,
#endif
#ifdef GL_COLOR_ATTACHMENT19
    color_attachment19 = GL_COLOR_ATTACHMENT19,
#endif
#ifdef GL_COLOR_ATTACHMENT2
    color_attachment2 = GL_COLOR_ATTACHMENT2,
#endif
#ifdef GL_COLOR_ATTACHMENT20
    color_attachment20 = GL_COLOR_ATTACHMENT20,
#endif
#ifdef GL_COLOR_ATTACHMENT21
    color_attachment21 = GL_COLOR_ATTACHMENT21,
#endif
#ifdef GL_COLOR_ATTACHMENT22
    color_attachment22 = GL_COLOR_ATTACHMENT22,
#endif
#ifdef GL_COLOR_ATTACHMENT23
    color_attachment23 = GL_COLOR_ATTACHMENT23,
#endif
#ifdef GL_COLOR_ATTACHMENT24
    color_attachment24 = GL_COLOR_ATTACHMENT24,
#endif
#ifdef GL_COLOR_ATTACHMENT25
    color_attachment25 = GL_COLOR_ATTACHMENT25,
#endif
#ifdef GL_COLOR_ATTACHMENT26
    color_attachment26 = GL_COLOR_ATTACHMENT26,
#endif
#ifdef GL_COLOR_ATTACHMENT27
    color_attachment27 = GL_COLOR_ATTACHMENT27,
#endif
#ifdef GL_COLOR_ATTACHMENT28
    color_attachment28 = GL_COLOR_ATTACHMENT28,
#endif
#ifdef GL_COLOR_ATTACHMENT29
    color_attachment29 = GL_COLOR_ATTACHMENT29,
#endif
#ifdef GL_COLOR_ATTACHMENT3
    color_attachment3 = GL_COLOR_ATTACHMENT3,
#endif
#ifdef GL_COLOR_ATTACHMENT30
    color_attachment30 = GL_COLOR_ATTACHMENT30,
#endif
#ifdef GL_COLOR_ATTACHMENT31
    color_attachment31 = GL_COLOR_ATTACHMENT31,
#endif
#ifdef GL_COLOR_ATTACHMENT4
    color_attachment4 = GL_COLOR_ATTACHMENT4,
#endif
#ifdef GL_COLOR_ATTACHMENT5
    color_attachment5 = GL_COLOR_ATTACHMENT5,
#endif
#ifdef GL_COLOR_ATTACHMENT6
    color_attachment6 = GL_COLOR_ATTACHMENT6,
#endif
#ifdef GL_COLOR_ATTACHMENT7
    color_attachment7 = GL_COLOR_ATTACHMENT7,
#endif
#ifdef GL_COLOR_ATTACHMENT8
    color_attachment8 = GL_COLOR_ATTACHMENT8,
#endif
#ifdef GL_COLOR_ATTACHMENT9
    color_attachment9 = GL_COLOR_ATTACHMENT9,
#endif
#ifdef GL_FRONT
    front = GL_FRONT,
#endif
#ifdef GL_FRONT_AND_BACK
    front_and_back = GL_FRONT_AND_BACK,
#endif
#ifdef GL_FRONT_LEFT
    front_left = GL_FRONT_LEFT,
#endif
#ifdef GL_FRONT_RIGHT
    front_right = GL_FRONT_RIGHT,
#endif
#ifdef GL_LEFT
    left = GL_LEFT,
#endif
#ifdef GL_NONE
    none = GL_NONE,
#endif
#ifdef GL_RIGHT
    right = GL_RIGHT,
#endif
}; // enum class color_buffer
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// PathGenMode
enum class path_gen_mode : ::libc_types::u32 {
#ifdef GL_CONSTANT
    constant = GL_CONSTANT,
#endif
#ifdef GL_EYE_LINEAR
    eye_linear = GL_EYE_LINEAR,
#endif
#ifdef GL_NONE
    none = GL_NONE,
#endif
#ifdef GL_OBJECT_LINEAR
    object_linear = GL_OBJECT_LINEAR,
#endif
#ifdef GL_PATH_OBJECT_BOUNDING_BOX_NV
    path_object_bounding_box_nv = GL_PATH_OBJECT_BOUNDING_BOX_NV,
#endif
}; // enum class path_gen_mode
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// PathTransformType
enum class path_transform_type : ::libc_types::u32 {
#ifdef GL_AFFINE_2D_NV
    affine_2d_nv = GL_AFFINE_2D_NV,
#endif
#ifdef GL_AFFINE_3D_NV
    affine_3d_nv = GL_AFFINE_3D_NV,
#endif
#ifdef GL_NONE
    none = GL_NONE,
#endif
#ifdef GL_TRANSLATE_2D_NV
    translate_2d_nv = GL_TRANSLATE_2D_NV,
#endif
#ifdef GL_TRANSLATE_3D_NV
    translate_3d_nv = GL_TRANSLATE_3D_NV,
#endif
#ifdef GL_TRANSLATE_X_NV
    translate_x_nv = GL_TRANSLATE_X_NV,
#endif
#ifdef GL_TRANSLATE_Y_NV
    translate_y_nv = GL_TRANSLATE_Y_NV,
#endif
#ifdef GL_TRANSPOSE_AFFINE_2D_NV
    transpose_affine_2d_nv = GL_TRANSPOSE_AFFINE_2D_NV,
#endif
#ifdef GL_TRANSPOSE_AFFINE_3D_NV
    transpose_affine_3d_nv = GL_TRANSPOSE_AFFINE_3D_NV,
#endif
}; // enum class path_transform_type
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// PrimitiveType
enum class primitive_type : ::libc_types::u32 {
#ifdef GL_LINES
    lines = GL_LINES,
#endif
#ifdef GL_LINES_ADJACENCY
    lines_adjacency = GL_LINES_ADJACENCY,
#endif
#ifdef GL_LINES_ADJACENCY_ARB
    lines_adjacency_arb = GL_LINES_ADJACENCY_ARB,
#endif
#ifdef GL_LINES_ADJACENCY_EXT
    lines_adjacency_ext = GL_LINES_ADJACENCY_EXT,
#endif
#ifdef GL_LINE_LOOP
    line_loop = GL_LINE_LOOP,
#endif
#ifdef GL_LINE_STRIP
    line_strip = GL_LINE_STRIP,
#endif
#ifdef GL_LINE_STRIP_ADJACENCY
    line_strip_adjacency = GL_LINE_STRIP_ADJACENCY,
#endif
#ifdef GL_LINE_STRIP_ADJACENCY_ARB
    line_strip_adjacency_arb = GL_LINE_STRIP_ADJACENCY_ARB,
#endif
#ifdef GL_LINE_STRIP_ADJACENCY_EXT
    line_strip_adjacency_ext = GL_LINE_STRIP_ADJACENCY_EXT,
#endif
#ifdef GL_PATCHES
    patches = GL_PATCHES,
#endif
#ifdef GL_PATCHES_EXT
    patches_ext = GL_PATCHES_EXT,
#endif
#ifdef GL_POINTS
    points = GL_POINTS,
#endif
#ifdef GL_POLYGON
    polygon = GL_POLYGON,
#endif
#ifdef GL_QUADS
    quads = GL_QUADS,
#endif
#ifdef GL_QUADS_EXT
    quads_ext = GL_QUADS_EXT,
#endif
#ifdef GL_QUAD_STRIP
    quad_strip = GL_QUAD_STRIP,
#endif
#ifdef GL_TRIANGLES
    triangles = GL_TRIANGLES,
#endif
#ifdef GL_TRIANGLES_ADJACENCY
    triangles_adjacency = GL_TRIANGLES_ADJACENCY,
#endif
#ifdef GL_TRIANGLES_ADJACENCY_ARB
    triangles_adjacency_arb = GL_TRIANGLES_ADJACENCY_ARB,
#endif
#ifdef GL_TRIANGLES_ADJACENCY_EXT
    triangles_adjacency_ext = GL_TRIANGLES_ADJACENCY_EXT,
#endif
#ifdef GL_TRIANGLE_FAN
    triangle_fan = GL_TRIANGLE_FAN,
#endif
#ifdef GL_TRIANGLE_STRIP
    triangle_strip = GL_TRIANGLE_STRIP,
#endif
#ifdef GL_TRIANGLE_STRIP_ADJACENCY
    triangle_strip_adjacency = GL_TRIANGLE_STRIP_ADJACENCY,
#endif
#ifdef GL_TRIANGLE_STRIP_ADJACENCY_ARB
    triangle_strip_adjacency_arb = GL_TRIANGLE_STRIP_ADJACENCY_ARB,
#endif
#ifdef GL_TRIANGLE_STRIP_ADJACENCY_EXT
    triangle_strip_adjacency_ext = GL_TRIANGLE_STRIP_ADJACENCY_EXT,
#endif
}; // enum class primitive_type
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// StencilFunction
enum class stencil_function : ::libc_types::u32 {
#ifdef GL_ALWAYS
    always = GL_ALWAYS,
#endif
#ifdef GL_EQUAL
    equal = GL_EQUAL,
#endif
#ifdef GL_GEQUAL
    gequal = GL_GEQUAL,
#endif
#ifdef GL_GREATER
    greater = GL_GREATER,
#endif
#ifdef GL_LEQUAL
    lequal = GL_LEQUAL,
#endif
#ifdef GL_LESS
    less = GL_LESS,
#endif
#ifdef GL_NEVER
    never = GL_NEVER,
#endif
#ifdef GL_NOTEQUAL
    notequal = GL_NOTEQUAL,
#endif
}; // enum class stencil_function
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// AlphaFunction
enum class alpha_function : ::libc_types::u32 {
#ifdef GL_ALWAYS
    always = GL_ALWAYS,
#endif
#ifdef GL_EQUAL
    equal = GL_EQUAL,
#endif
#ifdef GL_GEQUAL
    gequal = GL_GEQUAL,
#endif
#ifdef GL_GREATER
    greater = GL_GREATER,
#endif
#ifdef GL_LEQUAL
    lequal = GL_LEQUAL,
#endif
#ifdef GL_LESS
    less = GL_LESS,
#endif
#ifdef GL_NEVER
    never = GL_NEVER,
#endif
#ifdef GL_NOTEQUAL
    notequal = GL_NOTEQUAL,
#endif
}; // enum class alpha_function
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// DepthFunction
enum class depth_function : ::libc_types::u32 {
#ifdef GL_ALWAYS
    always = GL_ALWAYS,
#endif
#ifdef GL_EQUAL
    equal = GL_EQUAL,
#endif
#ifdef GL_GEQUAL
    gequal = GL_GEQUAL,
#endif
#ifdef GL_GREATER
    greater = GL_GREATER,
#endif
#ifdef GL_LEQUAL
    lequal = GL_LEQUAL,
#endif
#ifdef GL_LESS
    less = GL_LESS,
#endif
#ifdef GL_NEVER
    never = GL_NEVER,
#endif
#ifdef GL_NOTEQUAL
    notequal = GL_NOTEQUAL,
#endif
}; // enum class depth_function
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// CullFaceMode
enum class cull_face_mode : ::libc_types::u32 {
#ifdef GL_BACK
    back = GL_BACK,
#endif
#ifdef GL_FRONT
    front = GL_FRONT,
#endif
#ifdef GL_FRONT_AND_BACK
    front_and_back = GL_FRONT_AND_BACK,
#endif
}; // enum class cull_face_mode
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// StencilFaceDirection
enum class stencil_face_direction : ::libc_types::u32 {
#ifdef GL_BACK
    back = GL_BACK,
#endif
#ifdef GL_FRONT
    front = GL_FRONT,
#endif
#ifdef GL_FRONT_AND_BACK
    front_and_back = GL_FRONT_AND_BACK,
#endif
}; // enum class stencil_face_direction
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// MaterialFace
enum class material_face : ::libc_types::u32 {
#ifdef GL_BACK
    back = GL_BACK,
#endif
#ifdef GL_FRONT
    front = GL_FRONT,
#endif
#ifdef GL_FRONT_AND_BACK
    front_and_back = GL_FRONT_AND_BACK,
#endif
}; // enum class material_face
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// FrontFaceDirection
enum class front_face_direction : ::libc_types::u32 {
#ifdef GL_CCW
    ccw = GL_CCW,
#endif
#ifdef GL_CW
    cw = GL_CW,
#endif
}; // enum class front_face_direction
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// MapQuery
enum class map_query : ::libc_types::u32 {
#ifdef GL_COEFF
    coeff = GL_COEFF,
#endif
#ifdef GL_DOMAIN
    domain = GL_DOMAIN,
#endif
#ifdef GL_ORDER
    order = GL_ORDER,
#endif
}; // enum class map_query
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// GetMapQuery
enum class get_map_query : ::libc_types::u32 {
#ifdef GL_COEFF
    coeff = GL_COEFF,
#endif
#ifdef GL_DOMAIN
    domain = GL_DOMAIN,
#endif
#ifdef GL_ORDER
    order = GL_ORDER,
#endif
}; // enum class get_map_query
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// GetPName
enum class get_prop : ::libc_types::u32 {
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
#ifdef GL_BLEND_EQUATION_ALPHA
    blend_equation_alpha = GL_BLEND_EQUATION_ALPHA,
#endif
#ifdef GL_BLEND_EQUATION_EXT
    blend_equation_ext = GL_BLEND_EQUATION_EXT,
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
    fragment_color_material_parameter_sgix = GL_FRAGMENT_COLOR_MATERIAL_PARAMETER_SGIX,
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
    fragment_light_model_local_viewer_sgix = GL_FRAGMENT_LIGHT_MODEL_LOCAL_VIEWER_SGIX,
#endif
#ifdef GL_FRAGMENT_LIGHT_MODEL_NORMAL_INTERPOLATION_SGIX
    fragment_light_model_normal_interpolation_sgix = GL_FRAGMENT_LIGHT_MODEL_NORMAL_INTERPOLATION_SGIX,
#endif
#ifdef GL_FRAGMENT_LIGHT_MODEL_TWO_SIDE_SGIX
    fragment_light_model_two_side_sgix = GL_FRAGMENT_LIGHT_MODEL_TWO_SIDE_SGIX,
#endif
#ifdef GL_FRAGMENT_SHADER_DERIVATIVE_HINT
    fragment_shader_derivative_hint = GL_FRAGMENT_SHADER_DERIVATIVE_HINT,
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
    max_combined_compute_uniform_components = GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS,
#endif
#ifdef GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS
    max_combined_fragment_uniform_components = GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS,
#endif
#ifdef GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS
    max_combined_geometry_uniform_components = GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS,
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
    max_combined_vertex_uniform_components = GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS,
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
    max_tess_control_shader_storage_blocks = GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS,
#endif
#ifdef GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS
    max_tess_control_uniform_blocks = GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS,
#endif
#ifdef GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS
    max_tess_evaluation_atomic_counters = GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS,
#endif
#ifdef GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS
    max_tess_evaluation_shader_storage_blocks = GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS,
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
#ifdef GL_MAX_TEXTURE_SIZE
    max_texture_size = GL_MAX_TEXTURE_SIZE,
#endif
#ifdef GL_MAX_TEXTURE_STACK_DEPTH
    max_texture_stack_depth = GL_MAX_TEXTURE_STACK_DEPTH,
#endif
#ifdef GL_MAX_TIMELINE_SEMAPHORE_VALUE_DIFFERENCE_NV
    max_timeline_semaphore_value_difference_nv = GL_MAX_TIMELINE_SEMAPHORE_VALUE_DIFFERENCE_NV,
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
    motion_estimation_search_block_x_qcom = GL_MOTION_ESTIMATION_SEARCH_BLOCK_X_QCOM,
#endif
#ifdef GL_MOTION_ESTIMATION_SEARCH_BLOCK_Y_QCOM
    motion_estimation_search_block_y_qcom = GL_MOTION_ESTIMATION_SEARCH_BLOCK_Y_QCOM,
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
#ifdef GL_POINT_FADE_THRESHOLD_SIZE
    point_fade_threshold_size = GL_POINT_FADE_THRESHOLD_SIZE,
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
#ifdef GL_POINT_SIZE_MAX_SGIS
    point_size_max_sgis = GL_POINT_SIZE_MAX_SGIS,
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
    post_texture_filter_bias_range_sgix = GL_POST_TEXTURE_FILTER_BIAS_RANGE_SGIX,
#endif
#ifdef GL_POST_TEXTURE_FILTER_SCALE_RANGE_SGIX
    post_texture_filter_scale_range_sgix = GL_POST_TEXTURE_FILTER_SCALE_RANGE_SGIX,
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
    shader_storage_buffer_offset_alignment = GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT,
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
#ifdef GL_SHADING_RATE_IMAGE_PALETTE_COUNT_NV
    shading_rate_image_palette_count_nv = GL_SHADING_RATE_IMAGE_PALETTE_COUNT_NV,
#endif
#ifdef GL_SHADING_RATE_IMAGE_PER_PRIMITIVE_NV
    shading_rate_image_per_primitive_nv = GL_SHADING_RATE_IMAGE_PER_PRIMITIVE_NV,
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
    texture_binding_2d_multisample_array = GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY,
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
#ifdef GL_TEXTURE_BINDING_RECTANGLE
    texture_binding_rectangle = GL_TEXTURE_BINDING_RECTANGLE,
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
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
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
#pragma once

#include "common.h"

namespace gl::groups {

// LightModelParameter
enum class light_model_parameter : ::libc_types::u32 {
#ifdef GL_LIGHT_MODEL_AMBIENT
    light_model_ambient = GL_LIGHT_MODEL_AMBIENT,
#endif
#ifdef GL_LIGHT_MODEL_COLOR_CONTROL
    light_model_color_control = GL_LIGHT_MODEL_COLOR_CONTROL,
#endif
#ifdef GL_LIGHT_MODEL_COLOR_CONTROL_EXT
    light_model_color_control_ext = GL_LIGHT_MODEL_COLOR_CONTROL_EXT,
#endif
#ifdef GL_LIGHT_MODEL_LOCAL_VIEWER
    light_model_local_viewer = GL_LIGHT_MODEL_LOCAL_VIEWER,
#endif
#ifdef GL_LIGHT_MODEL_TWO_SIDE
    light_model_two_side = GL_LIGHT_MODEL_TWO_SIDE,
#endif
}; // enum class light_model_parameter
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// FogPName
enum class fog_prop : ::libc_types::u32 {
#ifdef GL_FOG_COORD_SRC
    fog_coord_src = GL_FOG_COORD_SRC,
#endif
#ifdef GL_FOG_DENSITY
    fog_density = GL_FOG_DENSITY,
#endif
#ifdef GL_FOG_END
    fog_end = GL_FOG_END,
#endif
#ifdef GL_FOG_INDEX
    fog_index = GL_FOG_INDEX,
#endif
#ifdef GL_FOG_MODE
    fog_mode = GL_FOG_MODE,
#endif
#ifdef GL_FOG_START
    fog_start = GL_FOG_START,
#endif
}; // enum class fog_prop
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// GetFramebufferParameter
enum class get_framebuffer_parameter : ::libc_types::u32 {
#ifdef GL_DOUBLEBUFFER
    doublebuffer = GL_DOUBLEBUFFER,
#endif
#ifdef GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS
    framebuffer_default_fixed_sample_locations = GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS,
#endif
#ifdef GL_FRAMEBUFFER_DEFAULT_HEIGHT
    framebuffer_default_height = GL_FRAMEBUFFER_DEFAULT_HEIGHT,
#endif
#ifdef GL_FRAMEBUFFER_DEFAULT_LAYERS
    framebuffer_default_layers = GL_FRAMEBUFFER_DEFAULT_LAYERS,
#endif
#ifdef GL_FRAMEBUFFER_DEFAULT_SAMPLES
    framebuffer_default_samples = GL_FRAMEBUFFER_DEFAULT_SAMPLES,
#endif
#ifdef GL_FRAMEBUFFER_DEFAULT_WIDTH
    framebuffer_default_width = GL_FRAMEBUFFER_DEFAULT_WIDTH,
#endif
#ifdef GL_IMPLEMENTATION_COLOR_READ_FORMAT
    implementation_color_read_format = GL_IMPLEMENTATION_COLOR_READ_FORMAT,
#endif
#ifdef GL_IMPLEMENTATION_COLOR_READ_TYPE
    implementation_color_read_type = GL_IMPLEMENTATION_COLOR_READ_TYPE,
#endif
#ifdef GL_SAMPLES
    samples = GL_SAMPLES,
#endif
#ifdef GL_SAMPLE_BUFFERS
    sample_buffers = GL_SAMPLE_BUFFERS,
#endif
#ifdef GL_STEREO
    stereo = GL_STEREO,
#endif
}; // enum class get_framebuffer_parameter
} // namespace gl::groups
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
#pragma once

#include "common.h"

namespace gl::groups {

// PixelMap
enum class pixel_map : ::libc_types::u32 {
#ifdef GL_PIXEL_MAP_A_TO_A
    a_to_a = GL_PIXEL_MAP_A_TO_A,
#endif
#ifdef GL_PIXEL_MAP_B_TO_B
    b_to_b = GL_PIXEL_MAP_B_TO_B,
#endif
#ifdef GL_PIXEL_MAP_G_TO_G
    g_to_g = GL_PIXEL_MAP_G_TO_G,
#endif
#ifdef GL_PIXEL_MAP_I_TO_A
    i_to_a = GL_PIXEL_MAP_I_TO_A,
#endif
#ifdef GL_PIXEL_MAP_I_TO_B
    i_to_b = GL_PIXEL_MAP_I_TO_B,
#endif
#ifdef GL_PIXEL_MAP_I_TO_G
    i_to_g = GL_PIXEL_MAP_I_TO_G,
#endif
#ifdef GL_PIXEL_MAP_I_TO_I
    i_to_i = GL_PIXEL_MAP_I_TO_I,
#endif
#ifdef GL_PIXEL_MAP_I_TO_R
    i_to_r = GL_PIXEL_MAP_I_TO_R,
#endif
#ifdef GL_PIXEL_MAP_R_TO_R
    r_to_r = GL_PIXEL_MAP_R_TO_R,
#endif
#ifdef GL_PIXEL_MAP_S_TO_S
    s_to_s = GL_PIXEL_MAP_S_TO_S,
#endif
}; // enum class pixel_map
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// PixelStoreParameter
enum class pixel_store_parameter : ::libc_types::u32 {
#ifdef GL_PACK_ALIGNMENT
    pack_alignment = GL_PACK_ALIGNMENT,
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
#ifdef GL_PACK_RESAMPLE_OML
    pack_resample_oml = GL_PACK_RESAMPLE_OML,
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
#ifdef GL_UNPACK_ALIGNMENT
    unpack_alignment = GL_UNPACK_ALIGNMENT,
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
#ifdef GL_UNPACK_RESAMPLE_OML
    unpack_resample_oml = GL_UNPACK_RESAMPLE_OML,
#endif
#ifdef GL_UNPACK_RESAMPLE_SGIX
    unpack_resample_sgix = GL_UNPACK_RESAMPLE_SGIX,
#endif
#ifdef GL_UNPACK_ROW_LENGTH
    unpack_row_length = GL_UNPACK_ROW_LENGTH,
#endif
#ifdef GL_UNPACK_ROW_LENGTH_EXT
    unpack_row_length_ext = GL_UNPACK_ROW_LENGTH_EXT,
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
#ifdef GL_UNPACK_SKIP_PIXELS_EXT
    unpack_skip_pixels_ext = GL_UNPACK_SKIP_PIXELS_EXT,
#endif
#ifdef GL_UNPACK_SKIP_ROWS
    unpack_skip_rows = GL_UNPACK_SKIP_ROWS,
#endif
#ifdef GL_UNPACK_SKIP_ROWS_EXT
    unpack_skip_rows_ext = GL_UNPACK_SKIP_ROWS_EXT,
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
}; // enum class pixel_store_parameter
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// PixelTransferParameter
enum class pixel_transfer_parameter : ::libc_types::u32 {
#ifdef GL_ALPHA_BIAS
    alpha_bias = GL_ALPHA_BIAS,
#endif
#ifdef GL_ALPHA_SCALE
    alpha_scale = GL_ALPHA_SCALE,
#endif
#ifdef GL_BLUE_BIAS
    blue_bias = GL_BLUE_BIAS,
#endif
#ifdef GL_BLUE_SCALE
    blue_scale = GL_BLUE_SCALE,
#endif
#ifdef GL_DEPTH_BIAS
    depth_bias = GL_DEPTH_BIAS,
#endif
#ifdef GL_DEPTH_SCALE
    depth_scale = GL_DEPTH_SCALE,
#endif
#ifdef GL_GREEN_BIAS
    green_bias = GL_GREEN_BIAS,
#endif
#ifdef GL_GREEN_SCALE
    green_scale = GL_GREEN_SCALE,
#endif
#ifdef GL_INDEX_OFFSET
    index_offset = GL_INDEX_OFFSET,
#endif
#ifdef GL_INDEX_SHIFT
    index_shift = GL_INDEX_SHIFT,
#endif
#ifdef GL_MAP_COLOR
    map_color = GL_MAP_COLOR,
#endif
#ifdef GL_MAP_STENCIL
    map_stencil = GL_MAP_STENCIL,
#endif
#ifdef GL_POST_COLOR_MATRIX_ALPHA_BIAS
    post_color_matrix_alpha_bias = GL_POST_COLOR_MATRIX_ALPHA_BIAS,
#endif
#ifdef GL_POST_COLOR_MATRIX_ALPHA_BIAS_SGI
    post_color_matrix_alpha_bias_sgi = GL_POST_COLOR_MATRIX_ALPHA_BIAS_SGI,
#endif
#ifdef GL_POST_COLOR_MATRIX_ALPHA_SCALE
    post_color_matrix_alpha_scale = GL_POST_COLOR_MATRIX_ALPHA_SCALE,
#endif
#ifdef GL_POST_COLOR_MATRIX_ALPHA_SCALE_SGI
    post_color_matrix_alpha_scale_sgi = GL_POST_COLOR_MATRIX_ALPHA_SCALE_SGI,
#endif
#ifdef GL_POST_COLOR_MATRIX_BLUE_BIAS
    post_color_matrix_blue_bias = GL_POST_COLOR_MATRIX_BLUE_BIAS,
#endif
#ifdef GL_POST_COLOR_MATRIX_BLUE_BIAS_SGI
    post_color_matrix_blue_bias_sgi = GL_POST_COLOR_MATRIX_BLUE_BIAS_SGI,
#endif
#ifdef GL_POST_COLOR_MATRIX_BLUE_SCALE
    post_color_matrix_blue_scale = GL_POST_COLOR_MATRIX_BLUE_SCALE,
#endif
#ifdef GL_POST_COLOR_MATRIX_BLUE_SCALE_SGI
    post_color_matrix_blue_scale_sgi = GL_POST_COLOR_MATRIX_BLUE_SCALE_SGI,
#endif
#ifdef GL_POST_COLOR_MATRIX_GREEN_BIAS
    post_color_matrix_green_bias = GL_POST_COLOR_MATRIX_GREEN_BIAS,
#endif
#ifdef GL_POST_COLOR_MATRIX_GREEN_BIAS_SGI
    post_color_matrix_green_bias_sgi = GL_POST_COLOR_MATRIX_GREEN_BIAS_SGI,
#endif
#ifdef GL_POST_COLOR_MATRIX_GREEN_SCALE
    post_color_matrix_green_scale = GL_POST_COLOR_MATRIX_GREEN_SCALE,
#endif
#ifdef GL_POST_COLOR_MATRIX_GREEN_SCALE_SGI
    post_color_matrix_green_scale_sgi = GL_POST_COLOR_MATRIX_GREEN_SCALE_SGI,
#endif
#ifdef GL_POST_COLOR_MATRIX_RED_BIAS
    post_color_matrix_red_bias = GL_POST_COLOR_MATRIX_RED_BIAS,
#endif
#ifdef GL_POST_COLOR_MATRIX_RED_BIAS_SGI
    post_color_matrix_red_bias_sgi = GL_POST_COLOR_MATRIX_RED_BIAS_SGI,
#endif
#ifdef GL_POST_COLOR_MATRIX_RED_SCALE
    post_color_matrix_red_scale = GL_POST_COLOR_MATRIX_RED_SCALE,
#endif
#ifdef GL_POST_COLOR_MATRIX_RED_SCALE_SGI
    post_color_matrix_red_scale_sgi = GL_POST_COLOR_MATRIX_RED_SCALE_SGI,
#endif
#ifdef GL_POST_CONVOLUTION_ALPHA_BIAS
    post_convolution_alpha_bias = GL_POST_CONVOLUTION_ALPHA_BIAS,
#endif
#ifdef GL_POST_CONVOLUTION_ALPHA_BIAS_EXT
    post_convolution_alpha_bias_ext = GL_POST_CONVOLUTION_ALPHA_BIAS_EXT,
#endif
#ifdef GL_POST_CONVOLUTION_ALPHA_SCALE
    post_convolution_alpha_scale = GL_POST_CONVOLUTION_ALPHA_SCALE,
#endif
#ifdef GL_POST_CONVOLUTION_ALPHA_SCALE_EXT
    post_convolution_alpha_scale_ext = GL_POST_CONVOLUTION_ALPHA_SCALE_EXT,
#endif
#ifdef GL_POST_CONVOLUTION_BLUE_BIAS
    post_convolution_blue_bias = GL_POST_CONVOLUTION_BLUE_BIAS,
#endif
#ifdef GL_POST_CONVOLUTION_BLUE_BIAS_EXT
    post_convolution_blue_bias_ext = GL_POST_CONVOLUTION_BLUE_BIAS_EXT,
#endif
#ifdef GL_POST_CONVOLUTION_BLUE_SCALE
    post_convolution_blue_scale = GL_POST_CONVOLUTION_BLUE_SCALE,
#endif
#ifdef GL_POST_CONVOLUTION_BLUE_SCALE_EXT
    post_convolution_blue_scale_ext = GL_POST_CONVOLUTION_BLUE_SCALE_EXT,
#endif
#ifdef GL_POST_CONVOLUTION_GREEN_BIAS
    post_convolution_green_bias = GL_POST_CONVOLUTION_GREEN_BIAS,
#endif
#ifdef GL_POST_CONVOLUTION_GREEN_BIAS_EXT
    post_convolution_green_bias_ext = GL_POST_CONVOLUTION_GREEN_BIAS_EXT,
#endif
#ifdef GL_POST_CONVOLUTION_GREEN_SCALE
    post_convolution_green_scale = GL_POST_CONVOLUTION_GREEN_SCALE,
#endif
#ifdef GL_POST_CONVOLUTION_GREEN_SCALE_EXT
    post_convolution_green_scale_ext = GL_POST_CONVOLUTION_GREEN_SCALE_EXT,
#endif
#ifdef GL_POST_CONVOLUTION_RED_BIAS
    post_convolution_red_bias = GL_POST_CONVOLUTION_RED_BIAS,
#endif
#ifdef GL_POST_CONVOLUTION_RED_BIAS_EXT
    post_convolution_red_bias_ext = GL_POST_CONVOLUTION_RED_BIAS_EXT,
#endif
#ifdef GL_POST_CONVOLUTION_RED_SCALE
    post_convolution_red_scale = GL_POST_CONVOLUTION_RED_SCALE,
#endif
#ifdef GL_POST_CONVOLUTION_RED_SCALE_EXT
    post_convolution_red_scale_ext = GL_POST_CONVOLUTION_RED_SCALE_EXT,
#endif
#ifdef GL_RED_BIAS
    red_bias = GL_RED_BIAS,
#endif
#ifdef GL_RED_SCALE
    red_scale = GL_RED_SCALE,
#endif
}; // enum class pixel_transfer_parameter
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// MapTarget
enum class map_target : ::libc_types::u32 {
#ifdef GL_GEOMETRY_DEFORMATION_SGIX
    geometry_deformation_sgix = GL_GEOMETRY_DEFORMATION_SGIX,
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
#ifdef GL_TEXTURE_DEFORMATION_SGIX
    texture_deformation_sgix = GL_TEXTURE_DEFORMATION_SGIX,
#endif
}; // enum class map_target
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// CopyImageSubDataTarget
enum class copy_image_sub_data_target : ::libc_types::u32 {
#ifdef GL_RENDERBUFFER
    renderbuffer = GL_RENDERBUFFER,
#endif
#ifdef GL_TEXTURE_1D
    texture_1d = GL_TEXTURE_1D,
#endif
#ifdef GL_TEXTURE_1D_ARRAY
    texture_1d_array = GL_TEXTURE_1D_ARRAY,
#endif
#ifdef GL_TEXTURE_2D
    texture_2d = GL_TEXTURE_2D,
#endif
#ifdef GL_TEXTURE_2D_ARRAY
    texture_2d_array = GL_TEXTURE_2D_ARRAY,
#endif
#ifdef GL_TEXTURE_2D_MULTISAMPLE
    texture_2d_multisample = GL_TEXTURE_2D_MULTISAMPLE,
#endif
#ifdef GL_TEXTURE_2D_MULTISAMPLE_ARRAY
    texture_2d_multisample_array = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
#endif
#ifdef GL_TEXTURE_3D
    texture_3d = GL_TEXTURE_3D,
#endif
#ifdef GL_TEXTURE_CUBE_MAP
    texture_cube_map = GL_TEXTURE_CUBE_MAP,
#endif
#ifdef GL_TEXTURE_CUBE_MAP_ARRAY
    texture_cube_map_array = GL_TEXTURE_CUBE_MAP_ARRAY,
#endif
#ifdef GL_TEXTURE_RECTANGLE
    texture_rectangle = GL_TEXTURE_RECTANGLE,
#endif
}; // enum class copy_image_sub_data_target
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// TextureTarget
enum class texture_target : ::libc_types::u32 {
#ifdef GL_DETAIL_TEXTURE_2D_SGIS
    detail_texture_2d_sgis = GL_DETAIL_TEXTURE_2D_SGIS,
#endif
#ifdef GL_PROXY_TEXTURE_1D
    proxy_texture_1d = GL_PROXY_TEXTURE_1D,
#endif
#ifdef GL_PROXY_TEXTURE_1D_ARRAY
    proxy_texture_1d_array = GL_PROXY_TEXTURE_1D_ARRAY,
#endif
#ifdef GL_PROXY_TEXTURE_1D_ARRAY_EXT
    proxy_texture_1d_array_ext = GL_PROXY_TEXTURE_1D_ARRAY_EXT,
#endif
#ifdef GL_PROXY_TEXTURE_1D_EXT
    proxy_texture_1d_ext = GL_PROXY_TEXTURE_1D_EXT,
#endif
#ifdef GL_PROXY_TEXTURE_2D
    proxy_texture_2d = GL_PROXY_TEXTURE_2D,
#endif
#ifdef GL_PROXY_TEXTURE_2D_ARRAY
    proxy_texture_2d_array = GL_PROXY_TEXTURE_2D_ARRAY,
#endif
#ifdef GL_PROXY_TEXTURE_2D_ARRAY_EXT
    proxy_texture_2d_array_ext = GL_PROXY_TEXTURE_2D_ARRAY_EXT,
#endif
#ifdef GL_PROXY_TEXTURE_2D_EXT
    proxy_texture_2d_ext = GL_PROXY_TEXTURE_2D_EXT,
#endif
#ifdef GL_PROXY_TEXTURE_2D_MULTISAMPLE
    proxy_texture_2d_multisample = GL_PROXY_TEXTURE_2D_MULTISAMPLE,
#endif
#ifdef GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY
    proxy_texture_2d_multisample_array = GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY,
#endif
#ifdef GL_PROXY_TEXTURE_3D
    proxy_texture_3d = GL_PROXY_TEXTURE_3D,
#endif
#ifdef GL_PROXY_TEXTURE_3D_EXT
    proxy_texture_3d_ext = GL_PROXY_TEXTURE_3D_EXT,
#endif
#ifdef GL_PROXY_TEXTURE_4D_SGIS
    proxy_texture_4d_sgis = GL_PROXY_TEXTURE_4D_SGIS,
#endif
#ifdef GL_PROXY_TEXTURE_CUBE_MAP
    proxy_texture_cube_map = GL_PROXY_TEXTURE_CUBE_MAP,
#endif
#ifdef GL_PROXY_TEXTURE_CUBE_MAP_ARB
    proxy_texture_cube_map_arb = GL_PROXY_TEXTURE_CUBE_MAP_ARB,
#endif
#ifdef GL_PROXY_TEXTURE_CUBE_MAP_ARRAY
    proxy_texture_cube_map_array = GL_PROXY_TEXTURE_CUBE_MAP_ARRAY,
#endif
#ifdef GL_PROXY_TEXTURE_CUBE_MAP_ARRAY_ARB
    proxy_texture_cube_map_array_arb = GL_PROXY_TEXTURE_CUBE_MAP_ARRAY_ARB,
#endif
#ifdef GL_PROXY_TEXTURE_CUBE_MAP_EXT
    proxy_texture_cube_map_ext = GL_PROXY_TEXTURE_CUBE_MAP_EXT,
#endif
#ifdef GL_PROXY_TEXTURE_RECTANGLE
    proxy_texture_rectangle = GL_PROXY_TEXTURE_RECTANGLE,
#endif
#ifdef GL_PROXY_TEXTURE_RECTANGLE_ARB
    proxy_texture_rectangle_arb = GL_PROXY_TEXTURE_RECTANGLE_ARB,
#endif
#ifdef GL_PROXY_TEXTURE_RECTANGLE_NV
    proxy_texture_rectangle_nv = GL_PROXY_TEXTURE_RECTANGLE_NV,
#endif
#ifdef GL_TEXTURE_1D
    texture_1d = GL_TEXTURE_1D,
#endif
#ifdef GL_TEXTURE_1D_ARRAY
    texture_1d_array = GL_TEXTURE_1D_ARRAY,
#endif
#ifdef GL_TEXTURE_2D
    texture_2d = GL_TEXTURE_2D,
#endif
#ifdef GL_TEXTURE_2D_ARRAY
    texture_2d_array = GL_TEXTURE_2D_ARRAY,
#endif
#ifdef GL_TEXTURE_2D_MULTISAMPLE
    texture_2d_multisample = GL_TEXTURE_2D_MULTISAMPLE,
#endif
#ifdef GL_TEXTURE_2D_MULTISAMPLE_ARRAY
    texture_2d_multisample_array = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
#endif
#ifdef GL_TEXTURE_3D
    texture_3d = GL_TEXTURE_3D,
#endif
#ifdef GL_TEXTURE_3D_EXT
    texture_3d_ext = GL_TEXTURE_3D_EXT,
#endif
#ifdef GL_TEXTURE_3D_OES
    texture_3d_oes = GL_TEXTURE_3D_OES,
#endif
#ifdef GL_TEXTURE_4D_SGIS
    texture_4d_sgis = GL_TEXTURE_4D_SGIS,
#endif
#ifdef GL_TEXTURE_BUFFER
    texture_buffer = GL_TEXTURE_BUFFER,
#endif
#ifdef GL_TEXTURE_CUBE_MAP
    texture_cube_map = GL_TEXTURE_CUBE_MAP,
#endif
#ifdef GL_TEXTURE_CUBE_MAP_ARRAY
    texture_cube_map_array = GL_TEXTURE_CUBE_MAP_ARRAY,
#endif
#ifdef GL_TEXTURE_CUBE_MAP_ARRAY_ARB
    texture_cube_map_array_arb = GL_TEXTURE_CUBE_MAP_ARRAY_ARB,
#endif
#ifdef GL_TEXTURE_CUBE_MAP_ARRAY_EXT
    texture_cube_map_array_ext = GL_TEXTURE_CUBE_MAP_ARRAY_EXT,
#endif
#ifdef GL_TEXTURE_CUBE_MAP_ARRAY_OES
    texture_cube_map_array_oes = GL_TEXTURE_CUBE_MAP_ARRAY_OES,
#endif
#ifdef GL_TEXTURE_CUBE_MAP_NEGATIVE_X
    texture_cube_map_negative_x = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
#endif
#ifdef GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
    texture_cube_map_negative_y = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
#endif
#ifdef GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    texture_cube_map_negative_z = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
#endif
#ifdef GL_TEXTURE_CUBE_MAP_POSITIVE_X
    texture_cube_map_positive_x = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
#endif
#ifdef GL_TEXTURE_CUBE_MAP_POSITIVE_Y
    texture_cube_map_positive_y = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
#endif
#ifdef GL_TEXTURE_CUBE_MAP_POSITIVE_Z
    texture_cube_map_positive_z = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
#endif
#ifdef GL_TEXTURE_RECTANGLE
    texture_rectangle = GL_TEXTURE_RECTANGLE,
#endif
}; // enum class texture_target
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// GetPointervPName
enum class get_pointerv_prop : ::libc_types::u32 {
#ifdef GL_COLOR_ARRAY_POINTER
    color_array_pointer = GL_COLOR_ARRAY_POINTER,
#endif
#ifdef GL_COLOR_ARRAY_POINTER_EXT
    color_array_pointer_ext = GL_COLOR_ARRAY_POINTER_EXT,
#endif
#ifdef GL_DEBUG_CALLBACK_FUNCTION
    debug_callback_function = GL_DEBUG_CALLBACK_FUNCTION,
#endif
#ifdef GL_DEBUG_CALLBACK_USER_PARAM
    debug_callback_user_param = GL_DEBUG_CALLBACK_USER_PARAM,
#endif
#ifdef GL_EDGE_FLAG_ARRAY_POINTER
    edge_flag_array_pointer = GL_EDGE_FLAG_ARRAY_POINTER,
#endif
#ifdef GL_EDGE_FLAG_ARRAY_POINTER_EXT
    edge_flag_array_pointer_ext = GL_EDGE_FLAG_ARRAY_POINTER_EXT,
#endif
#ifdef GL_FEEDBACK_BUFFER_POINTER
    feedback_buffer_pointer = GL_FEEDBACK_BUFFER_POINTER,
#endif
#ifdef GL_INDEX_ARRAY_POINTER
    index_array_pointer = GL_INDEX_ARRAY_POINTER,
#endif
#ifdef GL_INDEX_ARRAY_POINTER_EXT
    index_array_pointer_ext = GL_INDEX_ARRAY_POINTER_EXT,
#endif
#ifdef GL_INSTRUMENT_BUFFER_POINTER_SGIX
    instrument_buffer_pointer_sgix = GL_INSTRUMENT_BUFFER_POINTER_SGIX,
#endif
#ifdef GL_NORMAL_ARRAY_POINTER
    normal_array_pointer = GL_NORMAL_ARRAY_POINTER,
#endif
#ifdef GL_NORMAL_ARRAY_POINTER_EXT
    normal_array_pointer_ext = GL_NORMAL_ARRAY_POINTER_EXT,
#endif
#ifdef GL_SELECTION_BUFFER_POINTER
    selection_buffer_pointer = GL_SELECTION_BUFFER_POINTER,
#endif
#ifdef GL_TEXTURE_COORD_ARRAY_POINTER
    texture_coord_array_pointer = GL_TEXTURE_COORD_ARRAY_POINTER,
#endif
#ifdef GL_TEXTURE_COORD_ARRAY_POINTER_EXT
    texture_coord_array_pointer_ext = GL_TEXTURE_COORD_ARRAY_POINTER_EXT,
#endif
#ifdef GL_VERTEX_ARRAY_POINTER
    vertex_array_pointer = GL_VERTEX_ARRAY_POINTER,
#endif
#ifdef GL_VERTEX_ARRAY_POINTER_EXT
    vertex_array_pointer_ext = GL_VERTEX_ARRAY_POINTER_EXT,
#endif
}; // enum class get_pointerv_prop
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// TextureParameterName
enum class texture_parameter_name : ::libc_types::u32 {
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
    texture_foveated_cutoff_density_qcom = GL_TEXTURE_FOVEATED_CUTOFF_DENSITY_QCOM,
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
    texture_unnormalized_coordinates_arm = GL_TEXTURE_UNNORMALIZED_COORDINATES_ARM,
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
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// GetTextureParameter
enum class get_texture_parameter : ::libc_types::u32 {
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
#ifdef GL_TEXTURE_MAX_LEVEL_SGIS
    texture_max_level_sgis = GL_TEXTURE_MAX_LEVEL_SGIS,
#endif
#ifdef GL_TEXTURE_MAX_LOD_SGIS
    texture_max_lod_sgis = GL_TEXTURE_MAX_LOD_SGIS,
#endif
#ifdef GL_TEXTURE_MIN_FILTER
    texture_min_filter = GL_TEXTURE_MIN_FILTER,
#endif
#ifdef GL_TEXTURE_MIN_LOD_SGIS
    texture_min_lod_sgis = GL_TEXTURE_MIN_LOD_SGIS,
#endif
#ifdef GL_TEXTURE_PRIORITY
    texture_priority = GL_TEXTURE_PRIORITY,
#endif
#ifdef GL_TEXTURE_RED_SIZE
    texture_red_size = GL_TEXTURE_RED_SIZE,
#endif
#ifdef GL_TEXTURE_RESIDENT
    texture_resident = GL_TEXTURE_RESIDENT,
#endif
#ifdef GL_TEXTURE_UNNORMALIZED_COORDINATES_ARM
    texture_unnormalized_coordinates_arm = GL_TEXTURE_UNNORMALIZED_COORDINATES_ARM,
#endif
#ifdef GL_TEXTURE_WIDTH
    texture_width = GL_TEXTURE_WIDTH,
#endif
#ifdef GL_TEXTURE_WRAP_Q_SGIS
    texture_wrap_q_sgis = GL_TEXTURE_WRAP_Q_SGIS,
#endif
#ifdef GL_TEXTURE_WRAP_R_EXT
    texture_wrap_r_ext = GL_TEXTURE_WRAP_R_EXT,
#endif
#ifdef GL_TEXTURE_WRAP_S
    texture_wrap_s = GL_TEXTURE_WRAP_S,
#endif
#ifdef GL_TEXTURE_WRAP_T
    texture_wrap_t = GL_TEXTURE_WRAP_T,
#endif
}; // enum class get_texture_parameter
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// SamplerParameterF
enum class sampler_parameter_f : ::libc_types::u32 {
#ifdef GL_TEXTURE_BORDER_COLOR
    texture_border_color = GL_TEXTURE_BORDER_COLOR,
#endif
#ifdef GL_TEXTURE_LOD_BIAS
    texture_lod_bias = GL_TEXTURE_LOD_BIAS,
#endif
#ifdef GL_TEXTURE_MAX_ANISOTROPY
    texture_max_anisotropy = GL_TEXTURE_MAX_ANISOTROPY,
#endif
#ifdef GL_TEXTURE_MAX_LOD
    texture_max_lod = GL_TEXTURE_MAX_LOD,
#endif
#ifdef GL_TEXTURE_MIN_LOD
    texture_min_lod = GL_TEXTURE_MIN_LOD,
#endif
#ifdef GL_TEXTURE_UNNORMALIZED_COORDINATES_ARM
    texture_unnormalized_coordinates_arm = GL_TEXTURE_UNNORMALIZED_COORDINATES_ARM,
#endif
}; // enum class sampler_parameter_f
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// DebugSeverity
enum class debug_severity : ::libc_types::u32 {
#ifdef GL_DEBUG_SEVERITY_HIGH
    high = GL_DEBUG_SEVERITY_HIGH,
#endif
#ifdef GL_DEBUG_SEVERITY_LOW
    low = GL_DEBUG_SEVERITY_LOW,
#endif
#ifdef GL_DEBUG_SEVERITY_MEDIUM
    medium = GL_DEBUG_SEVERITY_MEDIUM,
#endif
#ifdef GL_DEBUG_SEVERITY_NOTIFICATION
    notification = GL_DEBUG_SEVERITY_NOTIFICATION,
#endif
#ifdef GL_DONT_CARE
    dont_care = GL_DONT_CARE,
#endif
}; // enum class debug_severity
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// HintMode
enum class hint_mode : ::libc_types::u32 {
#ifdef GL_DONT_CARE
    dont_care = GL_DONT_CARE,
#endif
#ifdef GL_FASTEST
    fastest = GL_FASTEST,
#endif
#ifdef GL_NICEST
    nicest = GL_NICEST,
#endif
}; // enum class hint_mode
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// DebugSource
enum class debug_source : ::libc_types::u32 {
#ifdef GL_DEBUG_SOURCE_API
    api = GL_DEBUG_SOURCE_API,
#endif
#ifdef GL_DEBUG_SOURCE_APPLICATION
    application = GL_DEBUG_SOURCE_APPLICATION,
#endif
#ifdef GL_DEBUG_SOURCE_OTHER
    other = GL_DEBUG_SOURCE_OTHER,
#endif
#ifdef GL_DEBUG_SOURCE_SHADER_COMPILER
    shader_compiler = GL_DEBUG_SOURCE_SHADER_COMPILER,
#endif
#ifdef GL_DEBUG_SOURCE_THIRD_PARTY
    third_party = GL_DEBUG_SOURCE_THIRD_PARTY,
#endif
#ifdef GL_DEBUG_SOURCE_WINDOW_SYSTEM
    window_system = GL_DEBUG_SOURCE_WINDOW_SYSTEM,
#endif
#ifdef GL_DONT_CARE
    dont_care = GL_DONT_CARE,
#endif
}; // enum class debug_source
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// DebugType
enum class debug_type : ::libc_types::u32 {
#ifdef GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR
    deprecated_behavior = GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR,
#endif
#ifdef GL_DEBUG_TYPE_ERROR
    error = GL_DEBUG_TYPE_ERROR,
#endif
#ifdef GL_DEBUG_TYPE_MARKER
    marker = GL_DEBUG_TYPE_MARKER,
#endif
#ifdef GL_DEBUG_TYPE_OTHER
    other = GL_DEBUG_TYPE_OTHER,
#endif
#ifdef GL_DEBUG_TYPE_PERFORMANCE
    performance = GL_DEBUG_TYPE_PERFORMANCE,
#endif
#ifdef GL_DEBUG_TYPE_POP_GROUP
    pop_group = GL_DEBUG_TYPE_POP_GROUP,
#endif
#ifdef GL_DEBUG_TYPE_PORTABILITY
    portability = GL_DEBUG_TYPE_PORTABILITY,
#endif
#ifdef GL_DEBUG_TYPE_PUSH_GROUP
    push_group = GL_DEBUG_TYPE_PUSH_GROUP,
#endif
#ifdef GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR
    undefined_behavior = GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR,
#endif
#ifdef GL_DONT_CARE
    dont_care = GL_DONT_CARE,
#endif
}; // enum class debug_type
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// MaterialParameter
enum class material_parameter : ::libc_types::u32 {
#ifdef GL_AMBIENT
    ambient = GL_AMBIENT,
#endif
#ifdef GL_AMBIENT_AND_DIFFUSE
    ambient_and_diffuse = GL_AMBIENT_AND_DIFFUSE,
#endif
#ifdef GL_COLOR_INDEXES
    color_indexes = GL_COLOR_INDEXES,
#endif
#ifdef GL_DIFFUSE
    diffuse = GL_DIFFUSE,
#endif
#ifdef GL_EMISSION
    emission = GL_EMISSION,
#endif
#ifdef GL_SHININESS
    shininess = GL_SHININESS,
#endif
#ifdef GL_SPECULAR
    specular = GL_SPECULAR,
#endif
}; // enum class material_parameter
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// LightParameter
enum class light_parameter : ::libc_types::u32 {
#ifdef GL_CONSTANT_ATTENUATION
    constant_attenuation = GL_CONSTANT_ATTENUATION,
#endif
#ifdef GL_LINEAR_ATTENUATION
    linear_attenuation = GL_LINEAR_ATTENUATION,
#endif
#ifdef GL_POSITION
    position = GL_POSITION,
#endif
#ifdef GL_QUADRATIC_ATTENUATION
    quadratic_attenuation = GL_QUADRATIC_ATTENUATION,
#endif
#ifdef GL_SPOT_CUTOFF
    spot_cutoff = GL_SPOT_CUTOFF,
#endif
#ifdef GL_SPOT_DIRECTION
    spot_direction = GL_SPOT_DIRECTION,
#endif
#ifdef GL_SPOT_EXPONENT
    spot_exponent = GL_SPOT_EXPONENT,
#endif
}; // enum class light_parameter
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// VertexAttribIType
enum class vertex_attrib_int : ::libc_types::u32 {
#ifdef GL_BYTE
    byte_ = GL_BYTE,
#endif
#ifdef GL_INT
    int_ = GL_INT,
#endif
#ifdef GL_SHORT
    short_ = GL_SHORT,
#endif
#ifdef GL_UNSIGNED_BYTE
    unsigned_byte = GL_UNSIGNED_BYTE,
#endif
#ifdef GL_UNSIGNED_INT
    unsigned_int = GL_UNSIGNED_INT,
#endif
#ifdef GL_UNSIGNED_SHORT
    unsigned_short = GL_UNSIGNED_SHORT,
#endif
}; // enum class vertex_attrib_int
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// WeightPointerTypeARB
enum class weight_pointer_type_arb : ::libc_types::u32 {
#ifdef GL_BYTE
    byte_ = GL_BYTE,
#endif
#ifdef GL_DOUBLE
    double_ = GL_DOUBLE,
#endif
#ifdef GL_FLOAT
    float_ = GL_FLOAT,
#endif
#ifdef GL_INT
    int_ = GL_INT,
#endif
#ifdef GL_SHORT
    short_ = GL_SHORT,
#endif
#ifdef GL_UNSIGNED_BYTE
    unsigned_byte = GL_UNSIGNED_BYTE,
#endif
#ifdef GL_UNSIGNED_INT
    unsigned_int = GL_UNSIGNED_INT,
#endif
#ifdef GL_UNSIGNED_SHORT
    unsigned_short = GL_UNSIGNED_SHORT,
#endif
}; // enum class weight_pointer_type_arb
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ColorPointerType
enum class color_pointer_type : ::libc_types::u32 {
#ifdef GL_BYTE
    byte_ = GL_BYTE,
#endif
#ifdef GL_UNSIGNED_BYTE
    unsigned_byte = GL_UNSIGNED_BYTE,
#endif
#ifdef GL_UNSIGNED_INT
    unsigned_int = GL_UNSIGNED_INT,
#endif
#ifdef GL_UNSIGNED_SHORT
    unsigned_short = GL_UNSIGNED_SHORT,
#endif
}; // enum class color_pointer_type
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// NormalPointerType
enum class normal_pointer_type : ::libc_types::u32 {
#ifdef GL_BYTE
    byte_ = GL_BYTE,
#endif
#ifdef GL_DOUBLE
    double_ = GL_DOUBLE,
#endif
#ifdef GL_FLOAT
    float_ = GL_FLOAT,
#endif
#ifdef GL_INT
    int_ = GL_INT,
#endif
#ifdef GL_SHORT
    short_ = GL_SHORT,
#endif
}; // enum class normal_pointer_type
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// PixelType
enum class pixel_type : ::libc_types::u32 {
#ifdef GL_BITMAP
    bitmap = GL_BITMAP,
#endif
#ifdef GL_BYTE
    byte_ = GL_BYTE,
#endif
#ifdef GL_FLOAT
    float_ = GL_FLOAT,
#endif
#ifdef GL_INT
    int_ = GL_INT,
#endif
#ifdef GL_SHORT
    short_ = GL_SHORT,
#endif
#ifdef GL_UNSIGNED_BYTE
    unsigned_byte = GL_UNSIGNED_BYTE,
#endif
#ifdef GL_UNSIGNED_BYTE_3_3_2
    unsigned_byte_3_3_2 = GL_UNSIGNED_BYTE_3_3_2,
#endif
#ifdef GL_UNSIGNED_BYTE_3_3_2_EXT
    unsigned_byte_3_3_2_ext = GL_UNSIGNED_BYTE_3_3_2_EXT,
#endif
#ifdef GL_UNSIGNED_INT
    unsigned_int = GL_UNSIGNED_INT,
#endif
#ifdef GL_UNSIGNED_INT_10_10_10_2
    unsigned_int_10_10_10_2 = GL_UNSIGNED_INT_10_10_10_2,
#endif
#ifdef GL_UNSIGNED_INT_10_10_10_2_EXT
    unsigned_int_10_10_10_2_ext = GL_UNSIGNED_INT_10_10_10_2_EXT,
#endif
#ifdef GL_UNSIGNED_INT_8_8_8_8
    unsigned_int_8_8_8_8 = GL_UNSIGNED_INT_8_8_8_8,
#endif
#ifdef GL_UNSIGNED_INT_8_8_8_8_EXT
    unsigned_int_8_8_8_8_ext = GL_UNSIGNED_INT_8_8_8_8_EXT,
#endif
#ifdef GL_UNSIGNED_SHORT
    unsigned_short = GL_UNSIGNED_SHORT,
#endif
#ifdef GL_UNSIGNED_SHORT_4_4_4_4
    unsigned_short_4_4_4_4 = GL_UNSIGNED_SHORT_4_4_4_4,
#endif
#ifdef GL_UNSIGNED_SHORT_4_4_4_4_EXT
    unsigned_short_4_4_4_4_ext = GL_UNSIGNED_SHORT_4_4_4_4_EXT,
#endif
#ifdef GL_UNSIGNED_SHORT_5_5_5_1
    unsigned_short_5_5_5_1 = GL_UNSIGNED_SHORT_5_5_5_1,
#endif
#ifdef GL_UNSIGNED_SHORT_5_5_5_1_EXT
    unsigned_short_5_5_5_1_ext = GL_UNSIGNED_SHORT_5_5_5_1_EXT,
#endif
}; // enum class pixel_type
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// VertexAttribType
enum class vertex_attrib_type : ::libc_types::u32 {
#ifdef GL_BYTE
    byte_ = GL_BYTE,
#endif
#ifdef GL_DOUBLE
    double_ = GL_DOUBLE,
#endif
#ifdef GL_FIXED
    fixed = GL_FIXED,
#endif
#ifdef GL_FLOAT
    float_ = GL_FLOAT,
#endif
#ifdef GL_HALF_FLOAT
    half_float = GL_HALF_FLOAT,
#endif
#ifdef GL_INT
    int_ = GL_INT,
#endif
#ifdef GL_INT_2_10_10_10_REV
    int_2_10_10_10_rev = GL_INT_2_10_10_10_REV,
#endif
#ifdef GL_SHORT
    short_ = GL_SHORT,
#endif
#ifdef GL_UNSIGNED_BYTE
    unsigned_byte = GL_UNSIGNED_BYTE,
#endif
#ifdef GL_UNSIGNED_INT
    unsigned_int = GL_UNSIGNED_INT,
#endif
#ifdef GL_UNSIGNED_INT_10F_11F_11F_REV
    unsigned_int_10f_11f_11f_rev = GL_UNSIGNED_INT_10F_11F_11F_REV,
#endif
#ifdef GL_UNSIGNED_INT_2_10_10_10_REV
    unsigned_int_2_10_10_10_rev = GL_UNSIGNED_INT_2_10_10_10_REV,
#endif
#ifdef GL_UNSIGNED_SHORT
    unsigned_short = GL_UNSIGNED_SHORT,
#endif
}; // enum class vertex_attrib_type
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// VertexAttribPointerType
enum class vertex_attrib_pointer_type : ::libc_types::u32 {
#ifdef GL_BYTE
    byte_ = GL_BYTE,
#endif
#ifdef GL_DOUBLE
    double_ = GL_DOUBLE,
#endif
#ifdef GL_FIXED
    fixed = GL_FIXED,
#endif
#ifdef GL_FLOAT
    float_ = GL_FLOAT,
#endif
#ifdef GL_HALF_FLOAT
    half_float = GL_HALF_FLOAT,
#endif
#ifdef GL_INT
    int_ = GL_INT,
#endif
#ifdef GL_INT64_ARB
    int64_arb = GL_INT64_ARB,
#endif
#ifdef GL_INT64_NV
    int64_nv = GL_INT64_NV,
#endif
#ifdef GL_INT_2_10_10_10_REV
    int_2_10_10_10_rev = GL_INT_2_10_10_10_REV,
#endif
#ifdef GL_SHORT
    short_ = GL_SHORT,
#endif
#ifdef GL_UNSIGNED_BYTE
    unsigned_byte = GL_UNSIGNED_BYTE,
#endif
#ifdef GL_UNSIGNED_INT
    unsigned_int = GL_UNSIGNED_INT,
#endif
#ifdef GL_UNSIGNED_INT64_ARB
    unsigned_int64_arb = GL_UNSIGNED_INT64_ARB,
#endif
#ifdef GL_UNSIGNED_INT64_NV
    unsigned_int64_nv = GL_UNSIGNED_INT64_NV,
#endif
#ifdef GL_UNSIGNED_INT_10F_11F_11F_REV
    unsigned_int_10f_11f_11f_rev = GL_UNSIGNED_INT_10F_11F_11F_REV,
#endif
#ifdef GL_UNSIGNED_INT_2_10_10_10_REV
    unsigned_int_2_10_10_10_rev = GL_UNSIGNED_INT_2_10_10_10_REV,
#endif
#ifdef GL_UNSIGNED_SHORT
    unsigned_short = GL_UNSIGNED_SHORT,
#endif
}; // enum class vertex_attrib_pointer_type
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// MatrixIndexPointerTypeARB
enum class matrix_index_pointer_type_arb : ::libc_types::u32 {
#ifdef GL_UNSIGNED_BYTE
    unsigned_byte = GL_UNSIGNED_BYTE,
#endif
#ifdef GL_UNSIGNED_INT
    unsigned_int = GL_UNSIGNED_INT,
#endif
#ifdef GL_UNSIGNED_SHORT
    unsigned_short = GL_UNSIGNED_SHORT,
#endif
}; // enum class matrix_index_pointer_type_arb
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// DrawElementsType
enum class draw_elements_type : ::libc_types::u32 {
#ifdef GL_UNSIGNED_BYTE
    unsigned_byte = GL_UNSIGNED_BYTE,
#endif
#ifdef GL_UNSIGNED_INT
    unsigned_int = GL_UNSIGNED_INT,
#endif
#ifdef GL_UNSIGNED_SHORT
    unsigned_short = GL_UNSIGNED_SHORT,
#endif
}; // enum class draw_elements_type
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// TexCoordPointerType
enum class tex_coord_pointer_type : ::libc_types::u32 {
#ifdef GL_DOUBLE
    double_ = GL_DOUBLE,
#endif
#ifdef GL_FLOAT
    float_ = GL_FLOAT,
#endif
#ifdef GL_INT
    int_ = GL_INT,
#endif
#ifdef GL_SHORT
    short_ = GL_SHORT,
#endif
}; // enum class tex_coord_pointer_type
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// VertexPointerType
enum class vertex_pointer_type : ::libc_types::u32 {
#ifdef GL_DOUBLE
    double_ = GL_DOUBLE,
#endif
#ifdef GL_FLOAT
    float_ = GL_FLOAT,
#endif
#ifdef GL_INT
    int_ = GL_INT,
#endif
#ifdef GL_SHORT
    short_ = GL_SHORT,
#endif
}; // enum class vertex_pointer_type
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// PixelFormat
enum class pixel_format : ::libc_types::u32 {
#ifdef GL_ABGR_EXT
    abgr_ext = GL_ABGR_EXT,
#endif
#ifdef GL_ALPHA
    alpha = GL_ALPHA,
#endif
#ifdef GL_BGR
    bgr = GL_BGR,
#endif
#ifdef GL_BGRA
    bgra = GL_BGRA,
#endif
#ifdef GL_BGRA_INTEGER
    bgra_integer = GL_BGRA_INTEGER,
#endif
#ifdef GL_BGR_INTEGER
    bgr_integer = GL_BGR_INTEGER,
#endif
#ifdef GL_BLUE
    blue = GL_BLUE,
#endif
#ifdef GL_BLUE_INTEGER
    blue_integer = GL_BLUE_INTEGER,
#endif
#ifdef GL_CMYKA_EXT
    cmyka_ext = GL_CMYKA_EXT,
#endif
#ifdef GL_CMYK_EXT
    cmyk_ext = GL_CMYK_EXT,
#endif
#ifdef GL_COLOR_INDEX
    color_index = GL_COLOR_INDEX,
#endif
#ifdef GL_DEPTH_COMPONENT
    depth_component = GL_DEPTH_COMPONENT,
#endif
#ifdef GL_DEPTH_STENCIL
    depth_stencil = GL_DEPTH_STENCIL,
#endif
#ifdef GL_GREEN
    green = GL_GREEN,
#endif
#ifdef GL_GREEN_INTEGER
    green_integer = GL_GREEN_INTEGER,
#endif
#ifdef GL_LUMINANCE
    luminance = GL_LUMINANCE,
#endif
#ifdef GL_LUMINANCE_ALPHA
    luminance_alpha = GL_LUMINANCE_ALPHA,
#endif
#ifdef GL_RED
    red = GL_RED,
#endif
#ifdef GL_RED_EXT
    red_ext = GL_RED_EXT,
#endif
#ifdef GL_RED_INTEGER
    red_integer = GL_RED_INTEGER,
#endif
#ifdef GL_RG
    rg = GL_RG,
#endif
#ifdef GL_RGB
    rgb = GL_RGB,
#endif
#ifdef GL_RGBA
    rgba = GL_RGBA,
#endif
#ifdef GL_RGBA_INTEGER
    rgba_integer = GL_RGBA_INTEGER,
#endif
#ifdef GL_RGB_INTEGER
    rgb_integer = GL_RGB_INTEGER,
#endif
#ifdef GL_RG_INTEGER
    rg_integer = GL_RG_INTEGER,
#endif
#ifdef GL_STENCIL_INDEX
    stencil_index = GL_STENCIL_INDEX,
#endif
#ifdef GL_UNSIGNED_INT
    unsigned_int = GL_UNSIGNED_INT,
#endif
#ifdef GL_UNSIGNED_SHORT
    unsigned_short = GL_UNSIGNED_SHORT,
#endif
#ifdef GL_YCRCB_422_SGIX
    ycrcb_422_sgix = GL_YCRCB_422_SGIX,
#endif
#ifdef GL_YCRCB_444_SGIX
    ycrcb_444_sgix = GL_YCRCB_444_SGIX,
#endif
}; // enum class pixel_format
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// MapTypeNV
enum class map_type_nv : ::libc_types::u32 {
#ifdef GL_DOUBLE
    double_ = GL_DOUBLE,
#endif
#ifdef GL_FLOAT
    float_ = GL_FLOAT,
#endif
}; // enum class map_type_nv
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// VertexAttribLType
enum class vertex_attrib_long : ::libc_types::u32 {
#ifdef GL_DOUBLE
    double_ = GL_DOUBLE,
#endif
}; // enum class vertex_attrib_long
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// LogicOp
enum class logic_op : ::libc_types::u32 {
#ifdef GL_AND
    and_ = GL_AND,
#endif
#ifdef GL_AND_INVERTED
    and_inverted = GL_AND_INVERTED,
#endif
#ifdef GL_AND_REVERSE
    and_reverse = GL_AND_REVERSE,
#endif
#ifdef GL_CLEAR
    clear = GL_CLEAR,
#endif
#ifdef GL_COPY
    copy = GL_COPY,
#endif
#ifdef GL_COPY_INVERTED
    copy_inverted = GL_COPY_INVERTED,
#endif
#ifdef GL_EQUIV
    equiv = GL_EQUIV,
#endif
#ifdef GL_INVERT
    invert = GL_INVERT,
#endif
#ifdef GL_NAND
    nand = GL_NAND,
#endif
#ifdef GL_NOOP
    noop = GL_NOOP,
#endif
#ifdef GL_NOR
    nor = GL_NOR,
#endif
#ifdef GL_OR
    or_ = GL_OR,
#endif
#ifdef GL_OR_INVERTED
    or_inverted = GL_OR_INVERTED,
#endif
#ifdef GL_OR_REVERSE
    or_reverse = GL_OR_REVERSE,
#endif
#ifdef GL_SET
    set = GL_SET,
#endif
#ifdef GL_XOR
    xor_ = GL_XOR,
#endif
}; // enum class logic_op
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// PathFillMode
enum class path_fill_mode : ::libc_types::u32 {
#ifdef GL_COUNT_DOWN_NV
    count_down_nv = GL_COUNT_DOWN_NV,
#endif
#ifdef GL_COUNT_UP_NV
    count_up_nv = GL_COUNT_UP_NV,
#endif
#ifdef GL_INVERT
    invert = GL_INVERT,
#endif
#ifdef GL_PATH_FILL_MODE_NV
    nv = GL_PATH_FILL_MODE_NV,
#endif
}; // enum class path_fill_mode
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// MatrixMode
enum class matrix_mode : ::libc_types::u32 {
#ifdef GL_MODELVIEW
    modelview = GL_MODELVIEW,
#endif
#ifdef GL_MODELVIEW0_EXT
    modelview0_ext = GL_MODELVIEW0_EXT,
#endif
#ifdef GL_PROJECTION
    projection = GL_PROJECTION,
#endif
#ifdef GL_TEXTURE
    texture = GL_TEXTURE,
#endif
}; // enum class matrix_mode
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ObjectIdentifier
enum class object_identifier : ::libc_types::u32 {
#ifdef GL_BUFFER
    buffer = GL_BUFFER,
#endif
#ifdef GL_FRAMEBUFFER
    framebuffer = GL_FRAMEBUFFER,
#endif
#ifdef GL_PROGRAM
    program = GL_PROGRAM,
#endif
#ifdef GL_PROGRAM_PIPELINE
    program_pipeline = GL_PROGRAM_PIPELINE,
#endif
#ifdef GL_QUERY
    query = GL_QUERY,
#endif
#ifdef GL_RENDERBUFFER
    renderbuffer = GL_RENDERBUFFER,
#endif
#ifdef GL_SAMPLER
    sampler = GL_SAMPLER,
#endif
#ifdef GL_SHADER
    shader = GL_SHADER,
#endif
#ifdef GL_TEXTURE
    texture = GL_TEXTURE,
#endif
#ifdef GL_TRANSFORM_FEEDBACK
    transform_feedback = GL_TRANSFORM_FEEDBACK,
#endif
#ifdef GL_VERTEX_ARRAY
    vertex_array = GL_VERTEX_ARRAY,
#endif
}; // enum class object_identifier
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// Buffer
enum class buffer : ::libc_types::u32 {
#ifdef GL_COLOR
    color = GL_COLOR,
#endif
#ifdef GL_DEPTH
    depth = GL_DEPTH,
#endif
#ifdef GL_STENCIL
    stencil = GL_STENCIL,
#endif
}; // enum class buffer
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// InternalFormat
enum class internal_format : ::libc_types::u32 {
#ifdef GL_ALPHA12
    alpha12 = GL_ALPHA12,
#endif
#ifdef GL_ALPHA12_EXT
    alpha12_ext = GL_ALPHA12_EXT,
#endif
#ifdef GL_ALPHA16
    alpha16 = GL_ALPHA16,
#endif
#ifdef GL_ALPHA16I_EXT
    alpha16i_ext = GL_ALPHA16I_EXT,
#endif
#ifdef GL_ALPHA16UI_EXT
    alpha16ui_ext = GL_ALPHA16UI_EXT,
#endif
#ifdef GL_ALPHA16_EXT
    alpha16_ext = GL_ALPHA16_EXT,
#endif
#ifdef GL_ALPHA32I_EXT
    alpha32i_ext = GL_ALPHA32I_EXT,
#endif
#ifdef GL_ALPHA32UI_EXT
    alpha32ui_ext = GL_ALPHA32UI_EXT,
#endif
#ifdef GL_ALPHA4
    alpha4 = GL_ALPHA4,
#endif
#ifdef GL_ALPHA4_EXT
    alpha4_ext = GL_ALPHA4_EXT,
#endif
#ifdef GL_ALPHA8
    alpha8 = GL_ALPHA8,
#endif
#ifdef GL_ALPHA8I_EXT
    alpha8i_ext = GL_ALPHA8I_EXT,
#endif
#ifdef GL_ALPHA8UI_EXT
    alpha8ui_ext = GL_ALPHA8UI_EXT,
#endif
#ifdef GL_ALPHA8_EXT
    alpha8_ext = GL_ALPHA8_EXT,
#endif
#ifdef GL_ALPHA8_OES
    alpha8_oes = GL_ALPHA8_OES,
#endif
#ifdef GL_COMPRESSED_R11_EAC
    compressed_r11_eac = GL_COMPRESSED_R11_EAC,
#endif
#ifdef GL_COMPRESSED_R11_EAC_OES
    compressed_r11_eac_oes = GL_COMPRESSED_R11_EAC_OES,
#endif
#ifdef GL_COMPRESSED_RED
    compressed_red = GL_COMPRESSED_RED,
#endif
#ifdef GL_COMPRESSED_RED_GREEN_RGTC2_EXT
    compressed_red_green_rgtc2_ext = GL_COMPRESSED_RED_GREEN_RGTC2_EXT,
#endif
#ifdef GL_COMPRESSED_RED_RGTC1
    compressed_red_rgtc1 = GL_COMPRESSED_RED_RGTC1,
#endif
#ifdef GL_COMPRESSED_RED_RGTC1_EXT
    compressed_red_rgtc1_ext = GL_COMPRESSED_RED_RGTC1_EXT,
#endif
#ifdef GL_COMPRESSED_RG
    compressed_rg = GL_COMPRESSED_RG,
#endif
#ifdef GL_COMPRESSED_RG11_EAC
    compressed_rg11_eac = GL_COMPRESSED_RG11_EAC,
#endif
#ifdef GL_COMPRESSED_RG11_EAC_OES
    compressed_rg11_eac_oes = GL_COMPRESSED_RG11_EAC_OES,
#endif
#ifdef GL_COMPRESSED_RGB
    compressed_rgb = GL_COMPRESSED_RGB,
#endif
#ifdef GL_COMPRESSED_RGB8_ETC2
    compressed_rgb8_etc2 = GL_COMPRESSED_RGB8_ETC2,
#endif
#ifdef GL_COMPRESSED_RGB8_ETC2_OES
    compressed_rgb8_etc2_oes = GL_COMPRESSED_RGB8_ETC2_OES,
#endif
#ifdef GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2
    compressed_rgb8_punchthrough_alpha1_etc2 = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,
#endif
#ifdef GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2_OES
    compressed_rgb8_punchthrough_alpha1_etc2_oes = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2_OES,
#endif
#ifdef GL_COMPRESSED_RGBA
    compressed_rgba = GL_COMPRESSED_RGBA,
#endif
#ifdef GL_COMPRESSED_RGBA8_ETC2_EAC
    compressed_rgba8_etc2_eac = GL_COMPRESSED_RGBA8_ETC2_EAC,
#endif
#ifdef GL_COMPRESSED_RGBA8_ETC2_EAC_OES
    compressed_rgba8_etc2_eac_oes = GL_COMPRESSED_RGBA8_ETC2_EAC_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_10x10
    compressed_rgba_astc_10x10 = GL_COMPRESSED_RGBA_ASTC_10x10,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_10x10_KHR
    compressed_rgba_astc_10x10_khr = GL_COMPRESSED_RGBA_ASTC_10x10_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_10x5
    compressed_rgba_astc_10x5 = GL_COMPRESSED_RGBA_ASTC_10x5,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_10x5_KHR
    compressed_rgba_astc_10x5_khr = GL_COMPRESSED_RGBA_ASTC_10x5_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_10x6
    compressed_rgba_astc_10x6 = GL_COMPRESSED_RGBA_ASTC_10x6,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_10x6_KHR
    compressed_rgba_astc_10x6_khr = GL_COMPRESSED_RGBA_ASTC_10x6_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_10x8
    compressed_rgba_astc_10x8 = GL_COMPRESSED_RGBA_ASTC_10x8,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_10x8_KHR
    compressed_rgba_astc_10x8_khr = GL_COMPRESSED_RGBA_ASTC_10x8_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_12x10
    compressed_rgba_astc_12x10 = GL_COMPRESSED_RGBA_ASTC_12x10,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_12x10_KHR
    compressed_rgba_astc_12x10_khr = GL_COMPRESSED_RGBA_ASTC_12x10_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_12x12
    compressed_rgba_astc_12x12 = GL_COMPRESSED_RGBA_ASTC_12x12,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_12x12_KHR
    compressed_rgba_astc_12x12_khr = GL_COMPRESSED_RGBA_ASTC_12x12_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_3x3x3_OES
    compressed_rgba_astc_3x3x3_oes = GL_COMPRESSED_RGBA_ASTC_3x3x3_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_4x3x3_OES
    compressed_rgba_astc_4x3x3_oes = GL_COMPRESSED_RGBA_ASTC_4x3x3_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_4x4
    compressed_rgba_astc_4x4 = GL_COMPRESSED_RGBA_ASTC_4x4,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_4x4_KHR
    compressed_rgba_astc_4x4_khr = GL_COMPRESSED_RGBA_ASTC_4x4_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_4x4x3_OES
    compressed_rgba_astc_4x4x3_oes = GL_COMPRESSED_RGBA_ASTC_4x4x3_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_4x4x4_OES
    compressed_rgba_astc_4x4x4_oes = GL_COMPRESSED_RGBA_ASTC_4x4x4_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_5x4
    compressed_rgba_astc_5x4 = GL_COMPRESSED_RGBA_ASTC_5x4,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_5x4_KHR
    compressed_rgba_astc_5x4_khr = GL_COMPRESSED_RGBA_ASTC_5x4_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_5x4x4_OES
    compressed_rgba_astc_5x4x4_oes = GL_COMPRESSED_RGBA_ASTC_5x4x4_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_5x5
    compressed_rgba_astc_5x5 = GL_COMPRESSED_RGBA_ASTC_5x5,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_5x5_KHR
    compressed_rgba_astc_5x5_khr = GL_COMPRESSED_RGBA_ASTC_5x5_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_5x5x4_OES
    compressed_rgba_astc_5x5x4_oes = GL_COMPRESSED_RGBA_ASTC_5x5x4_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_5x5x5_OES
    compressed_rgba_astc_5x5x5_oes = GL_COMPRESSED_RGBA_ASTC_5x5x5_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_6x5
    compressed_rgba_astc_6x5 = GL_COMPRESSED_RGBA_ASTC_6x5,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_6x5_KHR
    compressed_rgba_astc_6x5_khr = GL_COMPRESSED_RGBA_ASTC_6x5_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_6x5x5_OES
    compressed_rgba_astc_6x5x5_oes = GL_COMPRESSED_RGBA_ASTC_6x5x5_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_6x6
    compressed_rgba_astc_6x6 = GL_COMPRESSED_RGBA_ASTC_6x6,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_6x6_KHR
    compressed_rgba_astc_6x6_khr = GL_COMPRESSED_RGBA_ASTC_6x6_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_6x6x5_OES
    compressed_rgba_astc_6x6x5_oes = GL_COMPRESSED_RGBA_ASTC_6x6x5_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_6x6x6_OES
    compressed_rgba_astc_6x6x6_oes = GL_COMPRESSED_RGBA_ASTC_6x6x6_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_8x5
    compressed_rgba_astc_8x5 = GL_COMPRESSED_RGBA_ASTC_8x5,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_8x5_KHR
    compressed_rgba_astc_8x5_khr = GL_COMPRESSED_RGBA_ASTC_8x5_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_8x6
    compressed_rgba_astc_8x6 = GL_COMPRESSED_RGBA_ASTC_8x6,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_8x6_KHR
    compressed_rgba_astc_8x6_khr = GL_COMPRESSED_RGBA_ASTC_8x6_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_8x8
    compressed_rgba_astc_8x8 = GL_COMPRESSED_RGBA_ASTC_8x8,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_8x8_KHR
    compressed_rgba_astc_8x8_khr = GL_COMPRESSED_RGBA_ASTC_8x8_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_BPTC_UNORM
    compressed_rgba_bptc_unorm = GL_COMPRESSED_RGBA_BPTC_UNORM,
#endif
#ifdef GL_COMPRESSED_RGBA_BPTC_UNORM_ARB
    compressed_rgba_bptc_unorm_arb = GL_COMPRESSED_RGBA_BPTC_UNORM_ARB,
#endif
#ifdef GL_COMPRESSED_RGBA_BPTC_UNORM_EXT
    compressed_rgba_bptc_unorm_ext = GL_COMPRESSED_RGBA_BPTC_UNORM_EXT,
#endif
#ifdef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
    compressed_rgba_s3tc_dxt1_ext = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
#endif
#ifdef GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE
    compressed_rgba_s3tc_dxt3_angle = GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE,
#endif
#ifdef GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
    compressed_rgba_s3tc_dxt3_ext = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
#endif
#ifdef GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE
    compressed_rgba_s3tc_dxt5_angle = GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE,
#endif
#ifdef GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
    compressed_rgba_s3tc_dxt5_ext = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
#endif
#ifdef GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT
    compressed_rgb_bptc_signed_float = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT,
#endif
#ifdef GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB
    compressed_rgb_bptc_signed_float_arb = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB,
#endif
#ifdef GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_EXT
    compressed_rgb_bptc_signed_float_ext = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_EXT,
#endif
#ifdef GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT
    compressed_rgb_bptc_unsigned_float = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,
#endif
#ifdef GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB
    compressed_rgb_bptc_unsigned_float_arb = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB,
#endif
#ifdef GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_EXT
    compressed_rgb_bptc_unsigned_float_ext = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_EXT,
#endif
#ifdef GL_COMPRESSED_RGB_S3TC_DXT1_EXT
    compressed_rgb_s3tc_dxt1_ext = GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
#endif
#ifdef GL_COMPRESSED_RG_RGTC2
    compressed_rg_rgtc2 = GL_COMPRESSED_RG_RGTC2,
#endif
#ifdef GL_COMPRESSED_SIGNED_R11_EAC
    compressed_signed_r11_eac = GL_COMPRESSED_SIGNED_R11_EAC,
#endif
#ifdef GL_COMPRESSED_SIGNED_R11_EAC_OES
    compressed_signed_r11_eac_oes = GL_COMPRESSED_SIGNED_R11_EAC_OES,
#endif
#ifdef GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT
    compressed_signed_red_green_rgtc2_ext = GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT,
#endif
#ifdef GL_COMPRESSED_SIGNED_RED_RGTC1
    compressed_signed_red_rgtc1 = GL_COMPRESSED_SIGNED_RED_RGTC1,
#endif
#ifdef GL_COMPRESSED_SIGNED_RED_RGTC1_EXT
    compressed_signed_red_rgtc1_ext = GL_COMPRESSED_SIGNED_RED_RGTC1_EXT,
#endif
#ifdef GL_COMPRESSED_SIGNED_RG11_EAC
    compressed_signed_rg11_eac = GL_COMPRESSED_SIGNED_RG11_EAC,
#endif
#ifdef GL_COMPRESSED_SIGNED_RG11_EAC_OES
    compressed_signed_rg11_eac_oes = GL_COMPRESSED_SIGNED_RG11_EAC_OES,
#endif
#ifdef GL_COMPRESSED_SIGNED_RG_RGTC2
    compressed_signed_rg_rgtc2 = GL_COMPRESSED_SIGNED_RG_RGTC2,
#endif
#ifdef GL_COMPRESSED_SRGB
    compressed_srgb = GL_COMPRESSED_SRGB,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10
    compressed_srgb8_alpha8_astc_10x10 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR
    compressed_srgb8_alpha8_astc_10x10_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5
    compressed_srgb8_alpha8_astc_10x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR
    compressed_srgb8_alpha8_astc_10x5_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6
    compressed_srgb8_alpha8_astc_10x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR
    compressed_srgb8_alpha8_astc_10x6_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8
    compressed_srgb8_alpha8_astc_10x8 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR
    compressed_srgb8_alpha8_astc_10x8_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10
    compressed_srgb8_alpha8_astc_12x10 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR
    compressed_srgb8_alpha8_astc_12x10_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12
    compressed_srgb8_alpha8_astc_12x12 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR
    compressed_srgb8_alpha8_astc_12x12_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_3x3x3_OES
    compressed_srgb8_alpha8_astc_3x3x3_oes = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_3x3x3_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x3x3_OES
    compressed_srgb8_alpha8_astc_4x3x3_oes = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x3x3_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4
    compressed_srgb8_alpha8_astc_4x4 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR
    compressed_srgb8_alpha8_astc_4x4_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x3_OES
    compressed_srgb8_alpha8_astc_4x4x3_oes = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x3_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x4_OES
    compressed_srgb8_alpha8_astc_4x4x4_oes = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x4_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4
    compressed_srgb8_alpha8_astc_5x4 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR
    compressed_srgb8_alpha8_astc_5x4_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4x4_OES
    compressed_srgb8_alpha8_astc_5x4x4_oes = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4x4_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5
    compressed_srgb8_alpha8_astc_5x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR
    compressed_srgb8_alpha8_astc_5x5_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x4_OES
    compressed_srgb8_alpha8_astc_5x5x4_oes = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x4_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x5_OES
    compressed_srgb8_alpha8_astc_5x5x5_oes = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x5_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5
    compressed_srgb8_alpha8_astc_6x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR
    compressed_srgb8_alpha8_astc_6x5_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5x5_OES
    compressed_srgb8_alpha8_astc_6x5x5_oes = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5x5_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6
    compressed_srgb8_alpha8_astc_6x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR
    compressed_srgb8_alpha8_astc_6x6_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x5_OES
    compressed_srgb8_alpha8_astc_6x6x5_oes = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x5_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x6_OES
    compressed_srgb8_alpha8_astc_6x6x6_oes = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x6_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5
    compressed_srgb8_alpha8_astc_8x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR
    compressed_srgb8_alpha8_astc_8x5_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6
    compressed_srgb8_alpha8_astc_8x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR
    compressed_srgb8_alpha8_astc_8x6_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8
    compressed_srgb8_alpha8_astc_8x8 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR
    compressed_srgb8_alpha8_astc_8x8_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC
    compressed_srgb8_alpha8_etc2_eac = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC_OES
    compressed_srgb8_alpha8_etc2_eac_oes = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ETC2
    compressed_srgb8_etc2 = GL_COMPRESSED_SRGB8_ETC2,
#endif
#ifdef GL_COMPRESSED_SRGB8_ETC2_OES
    compressed_srgb8_etc2_oes = GL_COMPRESSED_SRGB8_ETC2_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2
    compressed_srgb8_punchthrough_alpha1_etc2 = GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,
#endif
#ifdef GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2_OES
    compressed_srgb8_punchthrough_alpha1_etc2_oes = GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2_OES,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA
    compressed_srgb_alpha = GL_COMPRESSED_SRGB_ALPHA,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM
    compressed_srgb_alpha_bptc_unorm = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB
    compressed_srgb_alpha_bptc_unorm_arb = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_EXT
    compressed_srgb_alpha_bptc_unorm_ext = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_EXT,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT
    compressed_srgb_alpha_s3tc_dxt1_ext = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_NV
    compressed_srgb_alpha_s3tc_dxt1_nv = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_NV,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT
    compressed_srgb_alpha_s3tc_dxt3_ext = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_NV
    compressed_srgb_alpha_s3tc_dxt3_nv = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_NV,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT
    compressed_srgb_alpha_s3tc_dxt5_ext = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_NV
    compressed_srgb_alpha_s3tc_dxt5_nv = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_NV,
#endif
#ifdef GL_COMPRESSED_SRGB_S3TC_DXT1_EXT
    compressed_srgb_s3tc_dxt1_ext = GL_COMPRESSED_SRGB_S3TC_DXT1_EXT,
#endif
#ifdef GL_COMPRESSED_SRGB_S3TC_DXT1_NV
    compressed_srgb_s3tc_dxt1_nv = GL_COMPRESSED_SRGB_S3TC_DXT1_NV,
#endif
#ifdef GL_DEPTH24_STENCIL8
    depth24_stencil8 = GL_DEPTH24_STENCIL8,
#endif
#ifdef GL_DEPTH24_STENCIL8_EXT
    depth24_stencil8_ext = GL_DEPTH24_STENCIL8_EXT,
#endif
#ifdef GL_DEPTH24_STENCIL8_OES
    depth24_stencil8_oes = GL_DEPTH24_STENCIL8_OES,
#endif
#ifdef GL_DEPTH32F_STENCIL8
    depth32f_stencil8 = GL_DEPTH32F_STENCIL8,
#endif
#ifdef GL_DEPTH32F_STENCIL8_NV
    depth32f_stencil8_nv = GL_DEPTH32F_STENCIL8_NV,
#endif
#ifdef GL_DEPTH_COMPONENT
    depth_component = GL_DEPTH_COMPONENT,
#endif
#ifdef GL_DEPTH_COMPONENT16
    depth_component16 = GL_DEPTH_COMPONENT16,
#endif
#ifdef GL_DEPTH_COMPONENT16_ARB
    depth_component16_arb = GL_DEPTH_COMPONENT16_ARB,
#endif
#ifdef GL_DEPTH_COMPONENT16_OES
    depth_component16_oes = GL_DEPTH_COMPONENT16_OES,
#endif
#ifdef GL_DEPTH_COMPONENT16_SGIX
    depth_component16_sgix = GL_DEPTH_COMPONENT16_SGIX,
#endif
#ifdef GL_DEPTH_COMPONENT24
    depth_component24 = GL_DEPTH_COMPONENT24,
#endif
#ifdef GL_DEPTH_COMPONENT24_ARB
    depth_component24_arb = GL_DEPTH_COMPONENT24_ARB,
#endif
#ifdef GL_DEPTH_COMPONENT24_OES
    depth_component24_oes = GL_DEPTH_COMPONENT24_OES,
#endif
#ifdef GL_DEPTH_COMPONENT24_SGIX
    depth_component24_sgix = GL_DEPTH_COMPONENT24_SGIX,
#endif
#ifdef GL_DEPTH_COMPONENT32
    depth_component32 = GL_DEPTH_COMPONENT32,
#endif
#ifdef GL_DEPTH_COMPONENT32F
    depth_component32f = GL_DEPTH_COMPONENT32F,
#endif
#ifdef GL_DEPTH_COMPONENT32F_NV
    depth_component32f_nv = GL_DEPTH_COMPONENT32F_NV,
#endif
#ifdef GL_DEPTH_COMPONENT32_ARB
    depth_component32_arb = GL_DEPTH_COMPONENT32_ARB,
#endif
#ifdef GL_DEPTH_COMPONENT32_OES
    depth_component32_oes = GL_DEPTH_COMPONENT32_OES,
#endif
#ifdef GL_DEPTH_COMPONENT32_SGIX
    depth_component32_sgix = GL_DEPTH_COMPONENT32_SGIX,
#endif
#ifdef GL_DEPTH_STENCIL
    depth_stencil = GL_DEPTH_STENCIL,
#endif
#ifdef GL_DEPTH_STENCIL_EXT
    depth_stencil_ext = GL_DEPTH_STENCIL_EXT,
#endif
#ifdef GL_DEPTH_STENCIL_MESA
    depth_stencil_mesa = GL_DEPTH_STENCIL_MESA,
#endif
#ifdef GL_DEPTH_STENCIL_NV
    depth_stencil_nv = GL_DEPTH_STENCIL_NV,
#endif
#ifdef GL_DEPTH_STENCIL_OES
    depth_stencil_oes = GL_DEPTH_STENCIL_OES,
#endif
#ifdef GL_DUAL_ALPHA12_SGIS
    dual_alpha12_sgis = GL_DUAL_ALPHA12_SGIS,
#endif
#ifdef GL_DUAL_ALPHA16_SGIS
    dual_alpha16_sgis = GL_DUAL_ALPHA16_SGIS,
#endif
#ifdef GL_DUAL_ALPHA4_SGIS
    dual_alpha4_sgis = GL_DUAL_ALPHA4_SGIS,
#endif
#ifdef GL_DUAL_ALPHA8_SGIS
    dual_alpha8_sgis = GL_DUAL_ALPHA8_SGIS,
#endif
#ifdef GL_DUAL_INTENSITY12_SGIS
    dual_intensity12_sgis = GL_DUAL_INTENSITY12_SGIS,
#endif
#ifdef GL_DUAL_INTENSITY16_SGIS
    dual_intensity16_sgis = GL_DUAL_INTENSITY16_SGIS,
#endif
#ifdef GL_DUAL_INTENSITY4_SGIS
    dual_intensity4_sgis = GL_DUAL_INTENSITY4_SGIS,
#endif
#ifdef GL_DUAL_INTENSITY8_SGIS
    dual_intensity8_sgis = GL_DUAL_INTENSITY8_SGIS,
#endif
#ifdef GL_DUAL_LUMINANCE12_SGIS
    dual_luminance12_sgis = GL_DUAL_LUMINANCE12_SGIS,
#endif
#ifdef GL_DUAL_LUMINANCE16_SGIS
    dual_luminance16_sgis = GL_DUAL_LUMINANCE16_SGIS,
#endif
#ifdef GL_DUAL_LUMINANCE4_SGIS
    dual_luminance4_sgis = GL_DUAL_LUMINANCE4_SGIS,
#endif
#ifdef GL_DUAL_LUMINANCE8_SGIS
    dual_luminance8_sgis = GL_DUAL_LUMINANCE8_SGIS,
#endif
#ifdef GL_DUAL_LUMINANCE_ALPHA4_SGIS
    dual_luminance_alpha4_sgis = GL_DUAL_LUMINANCE_ALPHA4_SGIS,
#endif
#ifdef GL_DUAL_LUMINANCE_ALPHA8_SGIS
    dual_luminance_alpha8_sgis = GL_DUAL_LUMINANCE_ALPHA8_SGIS,
#endif
#ifdef GL_INTENSITY
    intensity = GL_INTENSITY,
#endif
#ifdef GL_INTENSITY12
    intensity12 = GL_INTENSITY12,
#endif
#ifdef GL_INTENSITY12_EXT
    intensity12_ext = GL_INTENSITY12_EXT,
#endif
#ifdef GL_INTENSITY16
    intensity16 = GL_INTENSITY16,
#endif
#ifdef GL_INTENSITY16I_EXT
    intensity16i_ext = GL_INTENSITY16I_EXT,
#endif
#ifdef GL_INTENSITY16UI_EXT
    intensity16ui_ext = GL_INTENSITY16UI_EXT,
#endif
#ifdef GL_INTENSITY16_EXT
    intensity16_ext = GL_INTENSITY16_EXT,
#endif
#ifdef GL_INTENSITY32I_EXT
    intensity32i_ext = GL_INTENSITY32I_EXT,
#endif
#ifdef GL_INTENSITY32UI_EXT
    intensity32ui_ext = GL_INTENSITY32UI_EXT,
#endif
#ifdef GL_INTENSITY4
    intensity4 = GL_INTENSITY4,
#endif
#ifdef GL_INTENSITY4_EXT
    intensity4_ext = GL_INTENSITY4_EXT,
#endif
#ifdef GL_INTENSITY8
    intensity8 = GL_INTENSITY8,
#endif
#ifdef GL_INTENSITY8I_EXT
    intensity8i_ext = GL_INTENSITY8I_EXT,
#endif
#ifdef GL_INTENSITY8UI_EXT
    intensity8ui_ext = GL_INTENSITY8UI_EXT,
#endif
#ifdef GL_INTENSITY8_EXT
    intensity8_ext = GL_INTENSITY8_EXT,
#endif
#ifdef GL_LUMINANCE12
    luminance12 = GL_LUMINANCE12,
#endif
#ifdef GL_LUMINANCE12_ALPHA12
    luminance12_alpha12 = GL_LUMINANCE12_ALPHA12,
#endif
#ifdef GL_LUMINANCE12_ALPHA12_EXT
    luminance12_alpha12_ext = GL_LUMINANCE12_ALPHA12_EXT,
#endif
#ifdef GL_LUMINANCE12_ALPHA4
    luminance12_alpha4 = GL_LUMINANCE12_ALPHA4,
#endif
#ifdef GL_LUMINANCE12_ALPHA4_EXT
    luminance12_alpha4_ext = GL_LUMINANCE12_ALPHA4_EXT,
#endif
#ifdef GL_LUMINANCE12_EXT
    luminance12_ext = GL_LUMINANCE12_EXT,
#endif
#ifdef GL_LUMINANCE16
    luminance16 = GL_LUMINANCE16,
#endif
#ifdef GL_LUMINANCE16I_EXT
    luminance16i_ext = GL_LUMINANCE16I_EXT,
#endif
#ifdef GL_LUMINANCE16UI_EXT
    luminance16ui_ext = GL_LUMINANCE16UI_EXT,
#endif
#ifdef GL_LUMINANCE16_ALPHA16
    luminance16_alpha16 = GL_LUMINANCE16_ALPHA16,
#endif
#ifdef GL_LUMINANCE16_ALPHA16_EXT
    luminance16_alpha16_ext = GL_LUMINANCE16_ALPHA16_EXT,
#endif
#ifdef GL_LUMINANCE16_EXT
    luminance16_ext = GL_LUMINANCE16_EXT,
#endif
#ifdef GL_LUMINANCE32I_EXT
    luminance32i_ext = GL_LUMINANCE32I_EXT,
#endif
#ifdef GL_LUMINANCE32UI_EXT
    luminance32ui_ext = GL_LUMINANCE32UI_EXT,
#endif
#ifdef GL_LUMINANCE4
    luminance4 = GL_LUMINANCE4,
#endif
#ifdef GL_LUMINANCE4_ALPHA4
    luminance4_alpha4 = GL_LUMINANCE4_ALPHA4,
#endif
#ifdef GL_LUMINANCE4_ALPHA4_EXT
    luminance4_alpha4_ext = GL_LUMINANCE4_ALPHA4_EXT,
#endif
#ifdef GL_LUMINANCE4_ALPHA4_OES
    luminance4_alpha4_oes = GL_LUMINANCE4_ALPHA4_OES,
#endif
#ifdef GL_LUMINANCE4_EXT
    luminance4_ext = GL_LUMINANCE4_EXT,
#endif
#ifdef GL_LUMINANCE6_ALPHA2
    luminance6_alpha2 = GL_LUMINANCE6_ALPHA2,
#endif
#ifdef GL_LUMINANCE6_ALPHA2_EXT
    luminance6_alpha2_ext = GL_LUMINANCE6_ALPHA2_EXT,
#endif
#ifdef GL_LUMINANCE8
    luminance8 = GL_LUMINANCE8,
#endif
#ifdef GL_LUMINANCE8I_EXT
    luminance8i_ext = GL_LUMINANCE8I_EXT,
#endif
#ifdef GL_LUMINANCE8UI_EXT
    luminance8ui_ext = GL_LUMINANCE8UI_EXT,
#endif
#ifdef GL_LUMINANCE8_ALPHA8
    luminance8_alpha8 = GL_LUMINANCE8_ALPHA8,
#endif
#ifdef GL_LUMINANCE8_ALPHA8_EXT
    luminance8_alpha8_ext = GL_LUMINANCE8_ALPHA8_EXT,
#endif
#ifdef GL_LUMINANCE8_ALPHA8_OES
    luminance8_alpha8_oes = GL_LUMINANCE8_ALPHA8_OES,
#endif
#ifdef GL_LUMINANCE8_EXT
    luminance8_ext = GL_LUMINANCE8_EXT,
#endif
#ifdef GL_LUMINANCE8_OES
    luminance8_oes = GL_LUMINANCE8_OES,
#endif
#ifdef GL_LUMINANCE_ALPHA16I_EXT
    luminance_alpha16i_ext = GL_LUMINANCE_ALPHA16I_EXT,
#endif
#ifdef GL_LUMINANCE_ALPHA16UI_EXT
    luminance_alpha16ui_ext = GL_LUMINANCE_ALPHA16UI_EXT,
#endif
#ifdef GL_LUMINANCE_ALPHA32I_EXT
    luminance_alpha32i_ext = GL_LUMINANCE_ALPHA32I_EXT,
#endif
#ifdef GL_LUMINANCE_ALPHA32UI_EXT
    luminance_alpha32ui_ext = GL_LUMINANCE_ALPHA32UI_EXT,
#endif
#ifdef GL_LUMINANCE_ALPHA8I_EXT
    luminance_alpha8i_ext = GL_LUMINANCE_ALPHA8I_EXT,
#endif
#ifdef GL_LUMINANCE_ALPHA8UI_EXT
    luminance_alpha8ui_ext = GL_LUMINANCE_ALPHA8UI_EXT,
#endif
#ifdef GL_QUAD_ALPHA4_SGIS
    quad_alpha4_sgis = GL_QUAD_ALPHA4_SGIS,
#endif
#ifdef GL_QUAD_ALPHA8_SGIS
    quad_alpha8_sgis = GL_QUAD_ALPHA8_SGIS,
#endif
#ifdef GL_QUAD_INTENSITY4_SGIS
    quad_intensity4_sgis = GL_QUAD_INTENSITY4_SGIS,
#endif
#ifdef GL_QUAD_INTENSITY8_SGIS
    quad_intensity8_sgis = GL_QUAD_INTENSITY8_SGIS,
#endif
#ifdef GL_QUAD_LUMINANCE4_SGIS
    quad_luminance4_sgis = GL_QUAD_LUMINANCE4_SGIS,
#endif
#ifdef GL_QUAD_LUMINANCE8_SGIS
    quad_luminance8_sgis = GL_QUAD_LUMINANCE8_SGIS,
#endif
#ifdef GL_R11F_G11F_B10F
    r11f_g11f_b10f = GL_R11F_G11F_B10F,
#endif
#ifdef GL_R11F_G11F_B10F_APPLE
    r11f_g11f_b10f_apple = GL_R11F_G11F_B10F_APPLE,
#endif
#ifdef GL_R11F_G11F_B10F_EXT
    r11f_g11f_b10f_ext = GL_R11F_G11F_B10F_EXT,
#endif
#ifdef GL_R16
    r16 = GL_R16,
#endif
#ifdef GL_R16F
    r16f = GL_R16F,
#endif
#ifdef GL_R16F_EXT
    r16f_ext = GL_R16F_EXT,
#endif
#ifdef GL_R16I
    r16i = GL_R16I,
#endif
#ifdef GL_R16UI
    r16ui = GL_R16UI,
#endif
#ifdef GL_R16_EXT
    r16_ext = GL_R16_EXT,
#endif
#ifdef GL_R16_SNORM
    r16_snorm = GL_R16_SNORM,
#endif
#ifdef GL_R16_SNORM_EXT
    r16_snorm_ext = GL_R16_SNORM_EXT,
#endif
#ifdef GL_R32F
    r32f = GL_R32F,
#endif
#ifdef GL_R32F_EXT
    r32f_ext = GL_R32F_EXT,
#endif
#ifdef GL_R32I
    r32i = GL_R32I,
#endif
#ifdef GL_R32UI
    r32ui = GL_R32UI,
#endif
#ifdef GL_R3_G3_B2
    r3_g3_b2 = GL_R3_G3_B2,
#endif
#ifdef GL_R8
    r8 = GL_R8,
#endif
#ifdef GL_R8I
    r8i = GL_R8I,
#endif
#ifdef GL_R8UI
    r8ui = GL_R8UI,
#endif
#ifdef GL_R8_EXT
    r8_ext = GL_R8_EXT,
#endif
#ifdef GL_R8_SNORM
    r8_snorm = GL_R8_SNORM,
#endif
#ifdef GL_RED
    red = GL_RED,
#endif
#ifdef GL_RED_EXT
    red_ext = GL_RED_EXT,
#endif
#ifdef GL_RG
    rg = GL_RG,
#endif
#ifdef GL_RG16
    rg16 = GL_RG16,
#endif
#ifdef GL_RG16F
    rg16f = GL_RG16F,
#endif
#ifdef GL_RG16F_EXT
    rg16f_ext = GL_RG16F_EXT,
#endif
#ifdef GL_RG16I
    rg16i = GL_RG16I,
#endif
#ifdef GL_RG16UI
    rg16ui = GL_RG16UI,
#endif
#ifdef GL_RG16_EXT
    rg16_ext = GL_RG16_EXT,
#endif
#ifdef GL_RG16_SNORM
    rg16_snorm = GL_RG16_SNORM,
#endif
#ifdef GL_RG16_SNORM_EXT
    rg16_snorm_ext = GL_RG16_SNORM_EXT,
#endif
#ifdef GL_RG32F
    rg32f = GL_RG32F,
#endif
#ifdef GL_RG32F_EXT
    rg32f_ext = GL_RG32F_EXT,
#endif
#ifdef GL_RG32I
    rg32i = GL_RG32I,
#endif
#ifdef GL_RG32UI
    rg32ui = GL_RG32UI,
#endif
#ifdef GL_RG8
    rg8 = GL_RG8,
#endif
#ifdef GL_RG8I
    rg8i = GL_RG8I,
#endif
#ifdef GL_RG8UI
    rg8ui = GL_RG8UI,
#endif
#ifdef GL_RG8_EXT
    rg8_ext = GL_RG8_EXT,
#endif
#ifdef GL_RG8_SNORM
    rg8_snorm = GL_RG8_SNORM,
#endif
#ifdef GL_RGB
    rgb = GL_RGB,
#endif
#ifdef GL_RGB10
    rgb10 = GL_RGB10,
#endif
#ifdef GL_RGB10_A2
    rgb10_a2 = GL_RGB10_A2,
#endif
#ifdef GL_RGB10_A2UI
    rgb10_a2ui = GL_RGB10_A2UI,
#endif
#ifdef GL_RGB10_A2_EXT
    rgb10_a2_ext = GL_RGB10_A2_EXT,
#endif
#ifdef GL_RGB10_EXT
    rgb10_ext = GL_RGB10_EXT,
#endif
#ifdef GL_RGB12
    rgb12 = GL_RGB12,
#endif
#ifdef GL_RGB12_EXT
    rgb12_ext = GL_RGB12_EXT,
#endif
#ifdef GL_RGB16
    rgb16 = GL_RGB16,
#endif
#ifdef GL_RGB16F
    rgb16f = GL_RGB16F,
#endif
#ifdef GL_RGB16F_ARB
    rgb16f_arb = GL_RGB16F_ARB,
#endif
#ifdef GL_RGB16F_EXT
    rgb16f_ext = GL_RGB16F_EXT,
#endif
#ifdef GL_RGB16I
    rgb16i = GL_RGB16I,
#endif
#ifdef GL_RGB16I_EXT
    rgb16i_ext = GL_RGB16I_EXT,
#endif
#ifdef GL_RGB16UI
    rgb16ui = GL_RGB16UI,
#endif
#ifdef GL_RGB16UI_EXT
    rgb16ui_ext = GL_RGB16UI_EXT,
#endif
#ifdef GL_RGB16_EXT
    rgb16_ext = GL_RGB16_EXT,
#endif
#ifdef GL_RGB16_SNORM
    rgb16_snorm = GL_RGB16_SNORM,
#endif
#ifdef GL_RGB16_SNORM_EXT
    rgb16_snorm_ext = GL_RGB16_SNORM_EXT,
#endif
#ifdef GL_RGB2_EXT
    rgb2_ext = GL_RGB2_EXT,
#endif
#ifdef GL_RGB32F
    rgb32f = GL_RGB32F,
#endif
#ifdef GL_RGB32F_ARB
    rgb32f_arb = GL_RGB32F_ARB,
#endif
#ifdef GL_RGB32F_EXT
    rgb32f_ext = GL_RGB32F_EXT,
#endif
#ifdef GL_RGB32I
    rgb32i = GL_RGB32I,
#endif
#ifdef GL_RGB32I_EXT
    rgb32i_ext = GL_RGB32I_EXT,
#endif
#ifdef GL_RGB32UI
    rgb32ui = GL_RGB32UI,
#endif
#ifdef GL_RGB32UI_EXT
    rgb32ui_ext = GL_RGB32UI_EXT,
#endif
#ifdef GL_RGB4
    rgb4 = GL_RGB4,
#endif
#ifdef GL_RGB4_EXT
    rgb4_ext = GL_RGB4_EXT,
#endif
#ifdef GL_RGB5
    rgb5 = GL_RGB5,
#endif
#ifdef GL_RGB5_A1
    rgb5_a1 = GL_RGB5_A1,
#endif
#ifdef GL_RGB5_A1_EXT
    rgb5_a1_ext = GL_RGB5_A1_EXT,
#endif
#ifdef GL_RGB5_A1_OES
    rgb5_a1_oes = GL_RGB5_A1_OES,
#endif
#ifdef GL_RGB5_EXT
    rgb5_ext = GL_RGB5_EXT,
#endif
#ifdef GL_RGB8
    rgb8 = GL_RGB8,
#endif
#ifdef GL_RGB8I
    rgb8i = GL_RGB8I,
#endif
#ifdef GL_RGB8I_EXT
    rgb8i_ext = GL_RGB8I_EXT,
#endif
#ifdef GL_RGB8UI
    rgb8ui = GL_RGB8UI,
#endif
#ifdef GL_RGB8UI_EXT
    rgb8ui_ext = GL_RGB8UI_EXT,
#endif
#ifdef GL_RGB8_EXT
    rgb8_ext = GL_RGB8_EXT,
#endif
#ifdef GL_RGB8_OES
    rgb8_oes = GL_RGB8_OES,
#endif
#ifdef GL_RGB8_SNORM
    rgb8_snorm = GL_RGB8_SNORM,
#endif
#ifdef GL_RGB9_E5
    rgb9_e5 = GL_RGB9_E5,
#endif
#ifdef GL_RGB9_E5_APPLE
    rgb9_e5_apple = GL_RGB9_E5_APPLE,
#endif
#ifdef GL_RGB9_E5_EXT
    rgb9_e5_ext = GL_RGB9_E5_EXT,
#endif
#ifdef GL_RGBA
    rgba = GL_RGBA,
#endif
#ifdef GL_RGBA12
    rgba12 = GL_RGBA12,
#endif
#ifdef GL_RGBA12_EXT
    rgba12_ext = GL_RGBA12_EXT,
#endif
#ifdef GL_RGBA16
    rgba16 = GL_RGBA16,
#endif
#ifdef GL_RGBA16F
    rgba16f = GL_RGBA16F,
#endif
#ifdef GL_RGBA16F_ARB
    rgba16f_arb = GL_RGBA16F_ARB,
#endif
#ifdef GL_RGBA16F_EXT
    rgba16f_ext = GL_RGBA16F_EXT,
#endif
#ifdef GL_RGBA16I
    rgba16i = GL_RGBA16I,
#endif
#ifdef GL_RGBA16I_EXT
    rgba16i_ext = GL_RGBA16I_EXT,
#endif
#ifdef GL_RGBA16UI
    rgba16ui = GL_RGBA16UI,
#endif
#ifdef GL_RGBA16UI_EXT
    rgba16ui_ext = GL_RGBA16UI_EXT,
#endif
#ifdef GL_RGBA16_EXT
    rgba16_ext = GL_RGBA16_EXT,
#endif
#ifdef GL_RGBA16_SNORM
    rgba16_snorm = GL_RGBA16_SNORM,
#endif
#ifdef GL_RGBA16_SNORM_EXT
    rgba16_snorm_ext = GL_RGBA16_SNORM_EXT,
#endif
#ifdef GL_RGBA2
    rgba2 = GL_RGBA2,
#endif
#ifdef GL_RGBA2_EXT
    rgba2_ext = GL_RGBA2_EXT,
#endif
#ifdef GL_RGBA32F
    rgba32f = GL_RGBA32F,
#endif
#ifdef GL_RGBA32F_ARB
    rgba32f_arb = GL_RGBA32F_ARB,
#endif
#ifdef GL_RGBA32F_EXT
    rgba32f_ext = GL_RGBA32F_EXT,
#endif
#ifdef GL_RGBA32I
    rgba32i = GL_RGBA32I,
#endif
#ifdef GL_RGBA32I_EXT
    rgba32i_ext = GL_RGBA32I_EXT,
#endif
#ifdef GL_RGBA32UI
    rgba32ui = GL_RGBA32UI,
#endif
#ifdef GL_RGBA32UI_EXT
    rgba32ui_ext = GL_RGBA32UI_EXT,
#endif
#ifdef GL_RGBA4
    rgba4 = GL_RGBA4,
#endif
#ifdef GL_RGBA4_EXT
    rgba4_ext = GL_RGBA4_EXT,
#endif
#ifdef GL_RGBA4_OES
    rgba4_oes = GL_RGBA4_OES,
#endif
#ifdef GL_RGBA8
    rgba8 = GL_RGBA8,
#endif
#ifdef GL_RGBA8I
    rgba8i = GL_RGBA8I,
#endif
#ifdef GL_RGBA8I_EXT
    rgba8i_ext = GL_RGBA8I_EXT,
#endif
#ifdef GL_RGBA8UI
    rgba8ui = GL_RGBA8UI,
#endif
#ifdef GL_RGBA8UI_EXT
    rgba8ui_ext = GL_RGBA8UI_EXT,
#endif
#ifdef GL_RGBA8_EXT
    rgba8_ext = GL_RGBA8_EXT,
#endif
#ifdef GL_RGBA8_OES
    rgba8_oes = GL_RGBA8_OES,
#endif
#ifdef GL_RGBA8_SNORM
    rgba8_snorm = GL_RGBA8_SNORM,
#endif
#ifdef GL_SR8_EXT
    sr8_ext = GL_SR8_EXT,
#endif
#ifdef GL_SRG8_EXT
    srg8_ext = GL_SRG8_EXT,
#endif
#ifdef GL_SRGB
    srgb = GL_SRGB,
#endif
#ifdef GL_SRGB8
    srgb8 = GL_SRGB8,
#endif
#ifdef GL_SRGB8_ALPHA8
    srgb8_alpha8 = GL_SRGB8_ALPHA8,
#endif
#ifdef GL_SRGB8_ALPHA8_EXT
    srgb8_alpha8_ext = GL_SRGB8_ALPHA8_EXT,
#endif
#ifdef GL_SRGB8_EXT
    srgb8_ext = GL_SRGB8_EXT,
#endif
#ifdef GL_SRGB8_NV
    srgb8_nv = GL_SRGB8_NV,
#endif
#ifdef GL_SRGB_ALPHA
    srgb_alpha = GL_SRGB_ALPHA,
#endif
#ifdef GL_SRGB_ALPHA_EXT
    srgb_alpha_ext = GL_SRGB_ALPHA_EXT,
#endif
#ifdef GL_SRGB_EXT
    srgb_ext = GL_SRGB_EXT,
#endif
#ifdef GL_STENCIL_INDEX
    stencil_index = GL_STENCIL_INDEX,
#endif
#ifdef GL_STENCIL_INDEX1
    stencil_index1 = GL_STENCIL_INDEX1,
#endif
#ifdef GL_STENCIL_INDEX16
    stencil_index16 = GL_STENCIL_INDEX16,
#endif
#ifdef GL_STENCIL_INDEX16_EXT
    stencil_index16_ext = GL_STENCIL_INDEX16_EXT,
#endif
#ifdef GL_STENCIL_INDEX1_EXT
    stencil_index1_ext = GL_STENCIL_INDEX1_EXT,
#endif
#ifdef GL_STENCIL_INDEX1_OES
    stencil_index1_oes = GL_STENCIL_INDEX1_OES,
#endif
#ifdef GL_STENCIL_INDEX4
    stencil_index4 = GL_STENCIL_INDEX4,
#endif
#ifdef GL_STENCIL_INDEX4_EXT
    stencil_index4_ext = GL_STENCIL_INDEX4_EXT,
#endif
#ifdef GL_STENCIL_INDEX4_OES
    stencil_index4_oes = GL_STENCIL_INDEX4_OES,
#endif
#ifdef GL_STENCIL_INDEX8
    stencil_index8 = GL_STENCIL_INDEX8,
#endif
#ifdef GL_STENCIL_INDEX8_EXT
    stencil_index8_ext = GL_STENCIL_INDEX8_EXT,
#endif
#ifdef GL_STENCIL_INDEX8_OES
    stencil_index8_oes = GL_STENCIL_INDEX8_OES,
#endif
#ifdef GL_STENCIL_INDEX_OES
    stencil_index_oes = GL_STENCIL_INDEX_OES,
#endif
}; // enum class internal_format
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// CombinerComponentUsageNV
enum class combiner_component_usage_nv : ::libc_types::u32 {
#ifdef GL_ALPHA
    alpha = GL_ALPHA,
#endif
#ifdef GL_BLUE
    blue = GL_BLUE,
#endif
#ifdef GL_RGB
    rgb = GL_RGB,
#endif
}; // enum class combiner_component_usage_nv
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// CombinerPortionNV
enum class combiner_portion_nv : ::libc_types::u32 {
#ifdef GL_ALPHA
    alpha = GL_ALPHA,
#endif
#ifdef GL_RGB
    rgb = GL_RGB,
#endif
}; // enum class combiner_portion_nv
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// PolygonMode
enum class polygon_mode : ::libc_types::u32 {
#ifdef GL_FILL
    fill = GL_FILL,
#endif
#ifdef GL_LINE
    line = GL_LINE,
#endif
#ifdef GL_POINT
    point = GL_POINT,
#endif
}; // enum class polygon_mode
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// EvalMapsModeNV
enum class eval_maps_mode_nv : ::libc_types::u32 {
#ifdef GL_FILL_NV
    fill_nv = GL_FILL_NV,
#endif
}; // enum class eval_maps_mode_nv
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// StringName
enum class string_name : ::libc_types::u32 {
#ifdef GL_EXTENSIONS
    extensions = GL_EXTENSIONS,
#endif
#ifdef GL_RENDERER
    renderer = GL_RENDERER,
#endif
#ifdef GL_SHADING_LANGUAGE_VERSION
    shading_language_version = GL_SHADING_LANGUAGE_VERSION,
#endif
#ifdef GL_VENDOR
    vendor = GL_VENDOR,
#endif
#ifdef GL_VERSION
    version = GL_VERSION,
#endif
}; // enum class string_name
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// TextureCoordName
enum class texture_coord_name : ::libc_types::u32 {
#ifdef GL_Q
    q = GL_Q,
#endif
#ifdef GL_R
    r = GL_R,
#endif
#ifdef GL_S
    s = GL_S,
#endif
#ifdef GL_T
    t = GL_T,
#endif
}; // enum class texture_coord_name
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// TextureEnvParameter
enum class texture_env_parameter : ::libc_types::u32 {
#ifdef GL_TEXTURE_ENV_COLOR
    texture_env_color = GL_TEXTURE_ENV_COLOR,
#endif
#ifdef GL_TEXTURE_ENV_MODE
    texture_env_mode = GL_TEXTURE_ENV_MODE,
#endif
}; // enum class texture_env_parameter
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// TextureEnvTarget
enum class texture_env_target : ::libc_types::u32 {
#ifdef GL_TEXTURE_ENV
    texture_env = GL_TEXTURE_ENV,
#endif
}; // enum class texture_env_target
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// TextureGenParameter
enum class texture_gen_parameter : ::libc_types::u32 {
#ifdef GL_EYE_LINE_SGIS
    eye_line_sgis = GL_EYE_LINE_SGIS,
#endif
#ifdef GL_EYE_PLANE
    eye_plane = GL_EYE_PLANE,
#endif
#ifdef GL_EYE_POINT_SGIS
    eye_point_sgis = GL_EYE_POINT_SGIS,
#endif
#ifdef GL_OBJECT_LINE_SGIS
    object_line_sgis = GL_OBJECT_LINE_SGIS,
#endif
#ifdef GL_OBJECT_PLANE
    object_plane = GL_OBJECT_PLANE,
#endif
#ifdef GL_OBJECT_POINT_SGIS
    object_point_sgis = GL_OBJECT_POINT_SGIS,
#endif
#ifdef GL_TEXTURE_GEN_MODE
    texture_gen_mode = GL_TEXTURE_GEN_MODE,
#endif
}; // enum class texture_gen_parameter
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// BlitFramebufferFilter
enum class blit_framebuffer_filter : ::libc_types::u32 {
#ifdef GL_LINEAR
    linear = GL_LINEAR,
#endif
#ifdef GL_NEAREST
    nearest = GL_NEAREST,
#endif
}; // enum class blit_framebuffer_filter
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// SamplerParameterI
enum class sampler_parameter_i : ::libc_types::u32 {
#ifdef GL_TEXTURE_COMPARE_FUNC
    texture_compare_func = GL_TEXTURE_COMPARE_FUNC,
#endif
#ifdef GL_TEXTURE_COMPARE_MODE
    texture_compare_mode = GL_TEXTURE_COMPARE_MODE,
#endif
#ifdef GL_TEXTURE_MAG_FILTER
    texture_mag_filter = GL_TEXTURE_MAG_FILTER,
#endif
#ifdef GL_TEXTURE_MIN_FILTER
    texture_min_filter = GL_TEXTURE_MIN_FILTER,
#endif
#ifdef GL_TEXTURE_UNNORMALIZED_COORDINATES_ARM
    texture_unnormalized_coordinates_arm = GL_TEXTURE_UNNORMALIZED_COORDINATES_ARM,
#endif
#ifdef GL_TEXTURE_WRAP_R
    texture_wrap_r = GL_TEXTURE_WRAP_R,
#endif
#ifdef GL_TEXTURE_WRAP_S
    texture_wrap_s = GL_TEXTURE_WRAP_S,
#endif
#ifdef GL_TEXTURE_WRAP_T
    texture_wrap_t = GL_TEXTURE_WRAP_T,
#endif
}; // enum class sampler_parameter_i
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// SizedInternalFormat
enum class sized_internal_format : ::libc_types::u32 {
#ifdef GL_ALPHA12
    alpha12 = GL_ALPHA12,
#endif
#ifdef GL_ALPHA12_EXT
    alpha12_ext = GL_ALPHA12_EXT,
#endif
#ifdef GL_ALPHA16
    alpha16 = GL_ALPHA16,
#endif
#ifdef GL_ALPHA16I_EXT
    alpha16i_ext = GL_ALPHA16I_EXT,
#endif
#ifdef GL_ALPHA16UI_EXT
    alpha16ui_ext = GL_ALPHA16UI_EXT,
#endif
#ifdef GL_ALPHA16_EXT
    alpha16_ext = GL_ALPHA16_EXT,
#endif
#ifdef GL_ALPHA32I_EXT
    alpha32i_ext = GL_ALPHA32I_EXT,
#endif
#ifdef GL_ALPHA32UI_EXT
    alpha32ui_ext = GL_ALPHA32UI_EXT,
#endif
#ifdef GL_ALPHA4
    alpha4 = GL_ALPHA4,
#endif
#ifdef GL_ALPHA4_EXT
    alpha4_ext = GL_ALPHA4_EXT,
#endif
#ifdef GL_ALPHA8
    alpha8 = GL_ALPHA8,
#endif
#ifdef GL_ALPHA8I_EXT
    alpha8i_ext = GL_ALPHA8I_EXT,
#endif
#ifdef GL_ALPHA8UI_EXT
    alpha8ui_ext = GL_ALPHA8UI_EXT,
#endif
#ifdef GL_ALPHA8_EXT
    alpha8_ext = GL_ALPHA8_EXT,
#endif
#ifdef GL_ALPHA8_OES
    alpha8_oes = GL_ALPHA8_OES,
#endif
#ifdef GL_COMPRESSED_R11_EAC
    compressed_r11_eac = GL_COMPRESSED_R11_EAC,
#endif
#ifdef GL_COMPRESSED_R11_EAC_OES
    compressed_r11_eac_oes = GL_COMPRESSED_R11_EAC_OES,
#endif
#ifdef GL_COMPRESSED_RED_GREEN_RGTC2_EXT
    compressed_red_green_rgtc2_ext = GL_COMPRESSED_RED_GREEN_RGTC2_EXT,
#endif
#ifdef GL_COMPRESSED_RED_RGTC1
    compressed_red_rgtc1 = GL_COMPRESSED_RED_RGTC1,
#endif
#ifdef GL_COMPRESSED_RED_RGTC1_EXT
    compressed_red_rgtc1_ext = GL_COMPRESSED_RED_RGTC1_EXT,
#endif
#ifdef GL_COMPRESSED_RG11_EAC
    compressed_rg11_eac = GL_COMPRESSED_RG11_EAC,
#endif
#ifdef GL_COMPRESSED_RG11_EAC_OES
    compressed_rg11_eac_oes = GL_COMPRESSED_RG11_EAC_OES,
#endif
#ifdef GL_COMPRESSED_RGB8_ETC2
    compressed_rgb8_etc2 = GL_COMPRESSED_RGB8_ETC2,
#endif
#ifdef GL_COMPRESSED_RGB8_ETC2_OES
    compressed_rgb8_etc2_oes = GL_COMPRESSED_RGB8_ETC2_OES,
#endif
#ifdef GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2
    compressed_rgb8_punchthrough_alpha1_etc2 = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,
#endif
#ifdef GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2_OES
    compressed_rgb8_punchthrough_alpha1_etc2_oes = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2_OES,
#endif
#ifdef GL_COMPRESSED_RGBA8_ETC2_EAC
    compressed_rgba8_etc2_eac = GL_COMPRESSED_RGBA8_ETC2_EAC,
#endif
#ifdef GL_COMPRESSED_RGBA8_ETC2_EAC_OES
    compressed_rgba8_etc2_eac_oes = GL_COMPRESSED_RGBA8_ETC2_EAC_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_10x10
    compressed_rgba_astc_10x10 = GL_COMPRESSED_RGBA_ASTC_10x10,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_10x10_KHR
    compressed_rgba_astc_10x10_khr = GL_COMPRESSED_RGBA_ASTC_10x10_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_10x5
    compressed_rgba_astc_10x5 = GL_COMPRESSED_RGBA_ASTC_10x5,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_10x5_KHR
    compressed_rgba_astc_10x5_khr = GL_COMPRESSED_RGBA_ASTC_10x5_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_10x6
    compressed_rgba_astc_10x6 = GL_COMPRESSED_RGBA_ASTC_10x6,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_10x6_KHR
    compressed_rgba_astc_10x6_khr = GL_COMPRESSED_RGBA_ASTC_10x6_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_10x8
    compressed_rgba_astc_10x8 = GL_COMPRESSED_RGBA_ASTC_10x8,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_10x8_KHR
    compressed_rgba_astc_10x8_khr = GL_COMPRESSED_RGBA_ASTC_10x8_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_12x10
    compressed_rgba_astc_12x10 = GL_COMPRESSED_RGBA_ASTC_12x10,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_12x10_KHR
    compressed_rgba_astc_12x10_khr = GL_COMPRESSED_RGBA_ASTC_12x10_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_12x12
    compressed_rgba_astc_12x12 = GL_COMPRESSED_RGBA_ASTC_12x12,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_12x12_KHR
    compressed_rgba_astc_12x12_khr = GL_COMPRESSED_RGBA_ASTC_12x12_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_3x3x3_OES
    compressed_rgba_astc_3x3x3_oes = GL_COMPRESSED_RGBA_ASTC_3x3x3_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_4x3x3_OES
    compressed_rgba_astc_4x3x3_oes = GL_COMPRESSED_RGBA_ASTC_4x3x3_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_4x4
    compressed_rgba_astc_4x4 = GL_COMPRESSED_RGBA_ASTC_4x4,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_4x4_KHR
    compressed_rgba_astc_4x4_khr = GL_COMPRESSED_RGBA_ASTC_4x4_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_4x4x3_OES
    compressed_rgba_astc_4x4x3_oes = GL_COMPRESSED_RGBA_ASTC_4x4x3_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_4x4x4_OES
    compressed_rgba_astc_4x4x4_oes = GL_COMPRESSED_RGBA_ASTC_4x4x4_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_5x4
    compressed_rgba_astc_5x4 = GL_COMPRESSED_RGBA_ASTC_5x4,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_5x4_KHR
    compressed_rgba_astc_5x4_khr = GL_COMPRESSED_RGBA_ASTC_5x4_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_5x4x4_OES
    compressed_rgba_astc_5x4x4_oes = GL_COMPRESSED_RGBA_ASTC_5x4x4_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_5x5
    compressed_rgba_astc_5x5 = GL_COMPRESSED_RGBA_ASTC_5x5,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_5x5_KHR
    compressed_rgba_astc_5x5_khr = GL_COMPRESSED_RGBA_ASTC_5x5_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_5x5x4_OES
    compressed_rgba_astc_5x5x4_oes = GL_COMPRESSED_RGBA_ASTC_5x5x4_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_5x5x5_OES
    compressed_rgba_astc_5x5x5_oes = GL_COMPRESSED_RGBA_ASTC_5x5x5_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_6x5
    compressed_rgba_astc_6x5 = GL_COMPRESSED_RGBA_ASTC_6x5,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_6x5_KHR
    compressed_rgba_astc_6x5_khr = GL_COMPRESSED_RGBA_ASTC_6x5_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_6x5x5_OES
    compressed_rgba_astc_6x5x5_oes = GL_COMPRESSED_RGBA_ASTC_6x5x5_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_6x6
    compressed_rgba_astc_6x6 = GL_COMPRESSED_RGBA_ASTC_6x6,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_6x6_KHR
    compressed_rgba_astc_6x6_khr = GL_COMPRESSED_RGBA_ASTC_6x6_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_6x6x5_OES
    compressed_rgba_astc_6x6x5_oes = GL_COMPRESSED_RGBA_ASTC_6x6x5_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_6x6x6_OES
    compressed_rgba_astc_6x6x6_oes = GL_COMPRESSED_RGBA_ASTC_6x6x6_OES,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_8x5
    compressed_rgba_astc_8x5 = GL_COMPRESSED_RGBA_ASTC_8x5,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_8x5_KHR
    compressed_rgba_astc_8x5_khr = GL_COMPRESSED_RGBA_ASTC_8x5_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_8x6
    compressed_rgba_astc_8x6 = GL_COMPRESSED_RGBA_ASTC_8x6,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_8x6_KHR
    compressed_rgba_astc_8x6_khr = GL_COMPRESSED_RGBA_ASTC_8x6_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_8x8
    compressed_rgba_astc_8x8 = GL_COMPRESSED_RGBA_ASTC_8x8,
#endif
#ifdef GL_COMPRESSED_RGBA_ASTC_8x8_KHR
    compressed_rgba_astc_8x8_khr = GL_COMPRESSED_RGBA_ASTC_8x8_KHR,
#endif
#ifdef GL_COMPRESSED_RGBA_BPTC_UNORM
    compressed_rgba_bptc_unorm = GL_COMPRESSED_RGBA_BPTC_UNORM,
#endif
#ifdef GL_COMPRESSED_RGBA_BPTC_UNORM_ARB
    compressed_rgba_bptc_unorm_arb = GL_COMPRESSED_RGBA_BPTC_UNORM_ARB,
#endif
#ifdef GL_COMPRESSED_RGBA_BPTC_UNORM_EXT
    compressed_rgba_bptc_unorm_ext = GL_COMPRESSED_RGBA_BPTC_UNORM_EXT,
#endif
#ifdef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
    compressed_rgba_s3tc_dxt1_ext = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
#endif
#ifdef GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE
    compressed_rgba_s3tc_dxt3_angle = GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE,
#endif
#ifdef GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
    compressed_rgba_s3tc_dxt3_ext = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
#endif
#ifdef GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE
    compressed_rgba_s3tc_dxt5_angle = GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE,
#endif
#ifdef GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
    compressed_rgba_s3tc_dxt5_ext = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
#endif
#ifdef GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT
    compressed_rgb_bptc_signed_float = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT,
#endif
#ifdef GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB
    compressed_rgb_bptc_signed_float_arb = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB,
#endif
#ifdef GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_EXT
    compressed_rgb_bptc_signed_float_ext = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_EXT,
#endif
#ifdef GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT
    compressed_rgb_bptc_unsigned_float = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,
#endif
#ifdef GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB
    compressed_rgb_bptc_unsigned_float_arb = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB,
#endif
#ifdef GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_EXT
    compressed_rgb_bptc_unsigned_float_ext = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_EXT,
#endif
#ifdef GL_COMPRESSED_RGB_S3TC_DXT1_EXT
    compressed_rgb_s3tc_dxt1_ext = GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
#endif
#ifdef GL_COMPRESSED_RG_RGTC2
    compressed_rg_rgtc2 = GL_COMPRESSED_RG_RGTC2,
#endif
#ifdef GL_COMPRESSED_SIGNED_R11_EAC
    compressed_signed_r11_eac = GL_COMPRESSED_SIGNED_R11_EAC,
#endif
#ifdef GL_COMPRESSED_SIGNED_R11_EAC_OES
    compressed_signed_r11_eac_oes = GL_COMPRESSED_SIGNED_R11_EAC_OES,
#endif
#ifdef GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT
    compressed_signed_red_green_rgtc2_ext = GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT,
#endif
#ifdef GL_COMPRESSED_SIGNED_RED_RGTC1
    compressed_signed_red_rgtc1 = GL_COMPRESSED_SIGNED_RED_RGTC1,
#endif
#ifdef GL_COMPRESSED_SIGNED_RED_RGTC1_EXT
    compressed_signed_red_rgtc1_ext = GL_COMPRESSED_SIGNED_RED_RGTC1_EXT,
#endif
#ifdef GL_COMPRESSED_SIGNED_RG11_EAC
    compressed_signed_rg11_eac = GL_COMPRESSED_SIGNED_RG11_EAC,
#endif
#ifdef GL_COMPRESSED_SIGNED_RG11_EAC_OES
    compressed_signed_rg11_eac_oes = GL_COMPRESSED_SIGNED_RG11_EAC_OES,
#endif
#ifdef GL_COMPRESSED_SIGNED_RG_RGTC2
    compressed_signed_rg_rgtc2 = GL_COMPRESSED_SIGNED_RG_RGTC2,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10
    compressed_srgb8_alpha8_astc_10x10 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR
    compressed_srgb8_alpha8_astc_10x10_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5
    compressed_srgb8_alpha8_astc_10x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR
    compressed_srgb8_alpha8_astc_10x5_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6
    compressed_srgb8_alpha8_astc_10x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR
    compressed_srgb8_alpha8_astc_10x6_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8
    compressed_srgb8_alpha8_astc_10x8 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR
    compressed_srgb8_alpha8_astc_10x8_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10
    compressed_srgb8_alpha8_astc_12x10 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR
    compressed_srgb8_alpha8_astc_12x10_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12
    compressed_srgb8_alpha8_astc_12x12 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR
    compressed_srgb8_alpha8_astc_12x12_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_3x3x3_OES
    compressed_srgb8_alpha8_astc_3x3x3_oes = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_3x3x3_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x3x3_OES
    compressed_srgb8_alpha8_astc_4x3x3_oes = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x3x3_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4
    compressed_srgb8_alpha8_astc_4x4 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR
    compressed_srgb8_alpha8_astc_4x4_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x3_OES
    compressed_srgb8_alpha8_astc_4x4x3_oes = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x3_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x4_OES
    compressed_srgb8_alpha8_astc_4x4x4_oes = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x4_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4
    compressed_srgb8_alpha8_astc_5x4 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR
    compressed_srgb8_alpha8_astc_5x4_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4x4_OES
    compressed_srgb8_alpha8_astc_5x4x4_oes = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4x4_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5
    compressed_srgb8_alpha8_astc_5x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR
    compressed_srgb8_alpha8_astc_5x5_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x4_OES
    compressed_srgb8_alpha8_astc_5x5x4_oes = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x4_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x5_OES
    compressed_srgb8_alpha8_astc_5x5x5_oes = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x5_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5
    compressed_srgb8_alpha8_astc_6x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR
    compressed_srgb8_alpha8_astc_6x5_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5x5_OES
    compressed_srgb8_alpha8_astc_6x5x5_oes = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5x5_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6
    compressed_srgb8_alpha8_astc_6x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR
    compressed_srgb8_alpha8_astc_6x6_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x5_OES
    compressed_srgb8_alpha8_astc_6x6x5_oes = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x5_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x6_OES
    compressed_srgb8_alpha8_astc_6x6x6_oes = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x6_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5
    compressed_srgb8_alpha8_astc_8x5 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR
    compressed_srgb8_alpha8_astc_8x5_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6
    compressed_srgb8_alpha8_astc_8x6 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR
    compressed_srgb8_alpha8_astc_8x6_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8
    compressed_srgb8_alpha8_astc_8x8 = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR
    compressed_srgb8_alpha8_astc_8x8_khr = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC
    compressed_srgb8_alpha8_etc2_eac = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC,
#endif
#ifdef GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC_OES
    compressed_srgb8_alpha8_etc2_eac_oes = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_ETC2
    compressed_srgb8_etc2 = GL_COMPRESSED_SRGB8_ETC2,
#endif
#ifdef GL_COMPRESSED_SRGB8_ETC2_OES
    compressed_srgb8_etc2_oes = GL_COMPRESSED_SRGB8_ETC2_OES,
#endif
#ifdef GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2
    compressed_srgb8_punchthrough_alpha1_etc2 = GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,
#endif
#ifdef GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2_OES
    compressed_srgb8_punchthrough_alpha1_etc2_oes = GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2_OES,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM
    compressed_srgb_alpha_bptc_unorm = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB
    compressed_srgb_alpha_bptc_unorm_arb = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_EXT
    compressed_srgb_alpha_bptc_unorm_ext = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_EXT,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT
    compressed_srgb_alpha_s3tc_dxt1_ext = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_NV
    compressed_srgb_alpha_s3tc_dxt1_nv = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_NV,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT
    compressed_srgb_alpha_s3tc_dxt3_ext = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_NV
    compressed_srgb_alpha_s3tc_dxt3_nv = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_NV,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT
    compressed_srgb_alpha_s3tc_dxt5_ext = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,
#endif
#ifdef GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_NV
    compressed_srgb_alpha_s3tc_dxt5_nv = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_NV,
#endif
#ifdef GL_COMPRESSED_SRGB_S3TC_DXT1_EXT
    compressed_srgb_s3tc_dxt1_ext = GL_COMPRESSED_SRGB_S3TC_DXT1_EXT,
#endif
#ifdef GL_COMPRESSED_SRGB_S3TC_DXT1_NV
    compressed_srgb_s3tc_dxt1_nv = GL_COMPRESSED_SRGB_S3TC_DXT1_NV,
#endif
#ifdef GL_DEPTH24_STENCIL8
    depth24_stencil8 = GL_DEPTH24_STENCIL8,
#endif
#ifdef GL_DEPTH24_STENCIL8_EXT
    depth24_stencil8_ext = GL_DEPTH24_STENCIL8_EXT,
#endif
#ifdef GL_DEPTH24_STENCIL8_OES
    depth24_stencil8_oes = GL_DEPTH24_STENCIL8_OES,
#endif
#ifdef GL_DEPTH32F_STENCIL8
    depth32f_stencil8 = GL_DEPTH32F_STENCIL8,
#endif
#ifdef GL_DEPTH32F_STENCIL8_NV
    depth32f_stencil8_nv = GL_DEPTH32F_STENCIL8_NV,
#endif
#ifdef GL_DEPTH_COMPONENT16
    depth_component16 = GL_DEPTH_COMPONENT16,
#endif
#ifdef GL_DEPTH_COMPONENT16_ARB
    depth_component16_arb = GL_DEPTH_COMPONENT16_ARB,
#endif
#ifdef GL_DEPTH_COMPONENT16_OES
    depth_component16_oes = GL_DEPTH_COMPONENT16_OES,
#endif
#ifdef GL_DEPTH_COMPONENT16_SGIX
    depth_component16_sgix = GL_DEPTH_COMPONENT16_SGIX,
#endif
#ifdef GL_DEPTH_COMPONENT24
    depth_component24 = GL_DEPTH_COMPONENT24,
#endif
#ifdef GL_DEPTH_COMPONENT24_ARB
    depth_component24_arb = GL_DEPTH_COMPONENT24_ARB,
#endif
#ifdef GL_DEPTH_COMPONENT24_OES
    depth_component24_oes = GL_DEPTH_COMPONENT24_OES,
#endif
#ifdef GL_DEPTH_COMPONENT24_SGIX
    depth_component24_sgix = GL_DEPTH_COMPONENT24_SGIX,
#endif
#ifdef GL_DEPTH_COMPONENT32
    depth_component32 = GL_DEPTH_COMPONENT32,
#endif
#ifdef GL_DEPTH_COMPONENT32F
    depth_component32f = GL_DEPTH_COMPONENT32F,
#endif
#ifdef GL_DEPTH_COMPONENT32F_NV
    depth_component32f_nv = GL_DEPTH_COMPONENT32F_NV,
#endif
#ifdef GL_DEPTH_COMPONENT32_ARB
    depth_component32_arb = GL_DEPTH_COMPONENT32_ARB,
#endif
#ifdef GL_DEPTH_COMPONENT32_OES
    depth_component32_oes = GL_DEPTH_COMPONENT32_OES,
#endif
#ifdef GL_DEPTH_COMPONENT32_SGIX
    depth_component32_sgix = GL_DEPTH_COMPONENT32_SGIX,
#endif
#ifdef GL_INTENSITY12
    intensity12 = GL_INTENSITY12,
#endif
#ifdef GL_INTENSITY12_EXT
    intensity12_ext = GL_INTENSITY12_EXT,
#endif
#ifdef GL_INTENSITY16
    intensity16 = GL_INTENSITY16,
#endif
#ifdef GL_INTENSITY16I_EXT
    intensity16i_ext = GL_INTENSITY16I_EXT,
#endif
#ifdef GL_INTENSITY16UI_EXT
    intensity16ui_ext = GL_INTENSITY16UI_EXT,
#endif
#ifdef GL_INTENSITY16_EXT
    intensity16_ext = GL_INTENSITY16_EXT,
#endif
#ifdef GL_INTENSITY32I_EXT
    intensity32i_ext = GL_INTENSITY32I_EXT,
#endif
#ifdef GL_INTENSITY32UI_EXT
    intensity32ui_ext = GL_INTENSITY32UI_EXT,
#endif
#ifdef GL_INTENSITY4
    intensity4 = GL_INTENSITY4,
#endif
#ifdef GL_INTENSITY4_EXT
    intensity4_ext = GL_INTENSITY4_EXT,
#endif
#ifdef GL_INTENSITY8
    intensity8 = GL_INTENSITY8,
#endif
#ifdef GL_INTENSITY8I_EXT
    intensity8i_ext = GL_INTENSITY8I_EXT,
#endif
#ifdef GL_INTENSITY8UI_EXT
    intensity8ui_ext = GL_INTENSITY8UI_EXT,
#endif
#ifdef GL_INTENSITY8_EXT
    intensity8_ext = GL_INTENSITY8_EXT,
#endif
#ifdef GL_LUMINANCE12
    luminance12 = GL_LUMINANCE12,
#endif
#ifdef GL_LUMINANCE12_ALPHA12
    luminance12_alpha12 = GL_LUMINANCE12_ALPHA12,
#endif
#ifdef GL_LUMINANCE12_ALPHA12_EXT
    luminance12_alpha12_ext = GL_LUMINANCE12_ALPHA12_EXT,
#endif
#ifdef GL_LUMINANCE12_ALPHA4
    luminance12_alpha4 = GL_LUMINANCE12_ALPHA4,
#endif
#ifdef GL_LUMINANCE12_ALPHA4_EXT
    luminance12_alpha4_ext = GL_LUMINANCE12_ALPHA4_EXT,
#endif
#ifdef GL_LUMINANCE12_EXT
    luminance12_ext = GL_LUMINANCE12_EXT,
#endif
#ifdef GL_LUMINANCE16
    luminance16 = GL_LUMINANCE16,
#endif
#ifdef GL_LUMINANCE16I_EXT
    luminance16i_ext = GL_LUMINANCE16I_EXT,
#endif
#ifdef GL_LUMINANCE16UI_EXT
    luminance16ui_ext = GL_LUMINANCE16UI_EXT,
#endif
#ifdef GL_LUMINANCE16_ALPHA16
    luminance16_alpha16 = GL_LUMINANCE16_ALPHA16,
#endif
#ifdef GL_LUMINANCE16_ALPHA16_EXT
    luminance16_alpha16_ext = GL_LUMINANCE16_ALPHA16_EXT,
#endif
#ifdef GL_LUMINANCE16_EXT
    luminance16_ext = GL_LUMINANCE16_EXT,
#endif
#ifdef GL_LUMINANCE32I_EXT
    luminance32i_ext = GL_LUMINANCE32I_EXT,
#endif
#ifdef GL_LUMINANCE32UI_EXT
    luminance32ui_ext = GL_LUMINANCE32UI_EXT,
#endif
#ifdef GL_LUMINANCE4
    luminance4 = GL_LUMINANCE4,
#endif
#ifdef GL_LUMINANCE4_ALPHA4
    luminance4_alpha4 = GL_LUMINANCE4_ALPHA4,
#endif
#ifdef GL_LUMINANCE4_ALPHA4_EXT
    luminance4_alpha4_ext = GL_LUMINANCE4_ALPHA4_EXT,
#endif
#ifdef GL_LUMINANCE4_ALPHA4_OES
    luminance4_alpha4_oes = GL_LUMINANCE4_ALPHA4_OES,
#endif
#ifdef GL_LUMINANCE4_EXT
    luminance4_ext = GL_LUMINANCE4_EXT,
#endif
#ifdef GL_LUMINANCE6_ALPHA2
    luminance6_alpha2 = GL_LUMINANCE6_ALPHA2,
#endif
#ifdef GL_LUMINANCE6_ALPHA2_EXT
    luminance6_alpha2_ext = GL_LUMINANCE6_ALPHA2_EXT,
#endif
#ifdef GL_LUMINANCE8
    luminance8 = GL_LUMINANCE8,
#endif
#ifdef GL_LUMINANCE8I_EXT
    luminance8i_ext = GL_LUMINANCE8I_EXT,
#endif
#ifdef GL_LUMINANCE8UI_EXT
    luminance8ui_ext = GL_LUMINANCE8UI_EXT,
#endif
#ifdef GL_LUMINANCE8_ALPHA8
    luminance8_alpha8 = GL_LUMINANCE8_ALPHA8,
#endif
#ifdef GL_LUMINANCE8_ALPHA8_EXT
    luminance8_alpha8_ext = GL_LUMINANCE8_ALPHA8_EXT,
#endif
#ifdef GL_LUMINANCE8_ALPHA8_OES
    luminance8_alpha8_oes = GL_LUMINANCE8_ALPHA8_OES,
#endif
#ifdef GL_LUMINANCE8_EXT
    luminance8_ext = GL_LUMINANCE8_EXT,
#endif
#ifdef GL_LUMINANCE8_OES
    luminance8_oes = GL_LUMINANCE8_OES,
#endif
#ifdef GL_LUMINANCE_ALPHA16I_EXT
    luminance_alpha16i_ext = GL_LUMINANCE_ALPHA16I_EXT,
#endif
#ifdef GL_LUMINANCE_ALPHA16UI_EXT
    luminance_alpha16ui_ext = GL_LUMINANCE_ALPHA16UI_EXT,
#endif
#ifdef GL_LUMINANCE_ALPHA32I_EXT
    luminance_alpha32i_ext = GL_LUMINANCE_ALPHA32I_EXT,
#endif
#ifdef GL_LUMINANCE_ALPHA32UI_EXT
    luminance_alpha32ui_ext = GL_LUMINANCE_ALPHA32UI_EXT,
#endif
#ifdef GL_LUMINANCE_ALPHA8I_EXT
    luminance_alpha8i_ext = GL_LUMINANCE_ALPHA8I_EXT,
#endif
#ifdef GL_LUMINANCE_ALPHA8UI_EXT
    luminance_alpha8ui_ext = GL_LUMINANCE_ALPHA8UI_EXT,
#endif
#ifdef GL_R11F_G11F_B10F
    r11f_g11f_b10f = GL_R11F_G11F_B10F,
#endif
#ifdef GL_R11F_G11F_B10F_APPLE
    r11f_g11f_b10f_apple = GL_R11F_G11F_B10F_APPLE,
#endif
#ifdef GL_R11F_G11F_B10F_EXT
    r11f_g11f_b10f_ext = GL_R11F_G11F_B10F_EXT,
#endif
#ifdef GL_R16
    r16 = GL_R16,
#endif
#ifdef GL_R16F
    r16f = GL_R16F,
#endif
#ifdef GL_R16F_EXT
    r16f_ext = GL_R16F_EXT,
#endif
#ifdef GL_R16I
    r16i = GL_R16I,
#endif
#ifdef GL_R16UI
    r16ui = GL_R16UI,
#endif
#ifdef GL_R16_EXT
    r16_ext = GL_R16_EXT,
#endif
#ifdef GL_R16_SNORM
    r16_snorm = GL_R16_SNORM,
#endif
#ifdef GL_R16_SNORM_EXT
    r16_snorm_ext = GL_R16_SNORM_EXT,
#endif
#ifdef GL_R32F
    r32f = GL_R32F,
#endif
#ifdef GL_R32F_EXT
    r32f_ext = GL_R32F_EXT,
#endif
#ifdef GL_R32I
    r32i = GL_R32I,
#endif
#ifdef GL_R32UI
    r32ui = GL_R32UI,
#endif
#ifdef GL_R3_G3_B2
    r3_g3_b2 = GL_R3_G3_B2,
#endif
#ifdef GL_R8
    r8 = GL_R8,
#endif
#ifdef GL_R8I
    r8i = GL_R8I,
#endif
#ifdef GL_R8UI
    r8ui = GL_R8UI,
#endif
#ifdef GL_R8_EXT
    r8_ext = GL_R8_EXT,
#endif
#ifdef GL_R8_SNORM
    r8_snorm = GL_R8_SNORM,
#endif
#ifdef GL_RG16
    rg16 = GL_RG16,
#endif
#ifdef GL_RG16F
    rg16f = GL_RG16F,
#endif
#ifdef GL_RG16F_EXT
    rg16f_ext = GL_RG16F_EXT,
#endif
#ifdef GL_RG16I
    rg16i = GL_RG16I,
#endif
#ifdef GL_RG16UI
    rg16ui = GL_RG16UI,
#endif
#ifdef GL_RG16_EXT
    rg16_ext = GL_RG16_EXT,
#endif
#ifdef GL_RG16_SNORM
    rg16_snorm = GL_RG16_SNORM,
#endif
#ifdef GL_RG16_SNORM_EXT
    rg16_snorm_ext = GL_RG16_SNORM_EXT,
#endif
#ifdef GL_RG32F
    rg32f = GL_RG32F,
#endif
#ifdef GL_RG32F_EXT
    rg32f_ext = GL_RG32F_EXT,
#endif
#ifdef GL_RG32I
    rg32i = GL_RG32I,
#endif
#ifdef GL_RG32UI
    rg32ui = GL_RG32UI,
#endif
#ifdef GL_RG8
    rg8 = GL_RG8,
#endif
#ifdef GL_RG8I
    rg8i = GL_RG8I,
#endif
#ifdef GL_RG8UI
    rg8ui = GL_RG8UI,
#endif
#ifdef GL_RG8_EXT
    rg8_ext = GL_RG8_EXT,
#endif
#ifdef GL_RG8_SNORM
    rg8_snorm = GL_RG8_SNORM,
#endif
#ifdef GL_RGB10
    rgb10 = GL_RGB10,
#endif
#ifdef GL_RGB10_A2
    rgb10_a2 = GL_RGB10_A2,
#endif
#ifdef GL_RGB10_A2UI
    rgb10_a2ui = GL_RGB10_A2UI,
#endif
#ifdef GL_RGB10_A2_EXT
    rgb10_a2_ext = GL_RGB10_A2_EXT,
#endif
#ifdef GL_RGB10_EXT
    rgb10_ext = GL_RGB10_EXT,
#endif
#ifdef GL_RGB12
    rgb12 = GL_RGB12,
#endif
#ifdef GL_RGB12_EXT
    rgb12_ext = GL_RGB12_EXT,
#endif
#ifdef GL_RGB16
    rgb16 = GL_RGB16,
#endif
#ifdef GL_RGB16F
    rgb16f = GL_RGB16F,
#endif
#ifdef GL_RGB16F_ARB
    rgb16f_arb = GL_RGB16F_ARB,
#endif
#ifdef GL_RGB16F_EXT
    rgb16f_ext = GL_RGB16F_EXT,
#endif
#ifdef GL_RGB16I
    rgb16i = GL_RGB16I,
#endif
#ifdef GL_RGB16I_EXT
    rgb16i_ext = GL_RGB16I_EXT,
#endif
#ifdef GL_RGB16UI
    rgb16ui = GL_RGB16UI,
#endif
#ifdef GL_RGB16UI_EXT
    rgb16ui_ext = GL_RGB16UI_EXT,
#endif
#ifdef GL_RGB16_EXT
    rgb16_ext = GL_RGB16_EXT,
#endif
#ifdef GL_RGB16_SNORM
    rgb16_snorm = GL_RGB16_SNORM,
#endif
#ifdef GL_RGB16_SNORM_EXT
    rgb16_snorm_ext = GL_RGB16_SNORM_EXT,
#endif
#ifdef GL_RGB2_EXT
    rgb2_ext = GL_RGB2_EXT,
#endif
#ifdef GL_RGB32F
    rgb32f = GL_RGB32F,
#endif
#ifdef GL_RGB32F_ARB
    rgb32f_arb = GL_RGB32F_ARB,
#endif
#ifdef GL_RGB32F_EXT
    rgb32f_ext = GL_RGB32F_EXT,
#endif
#ifdef GL_RGB32I
    rgb32i = GL_RGB32I,
#endif
#ifdef GL_RGB32I_EXT
    rgb32i_ext = GL_RGB32I_EXT,
#endif
#ifdef GL_RGB32UI
    rgb32ui = GL_RGB32UI,
#endif
#ifdef GL_RGB32UI_EXT
    rgb32ui_ext = GL_RGB32UI_EXT,
#endif
#ifdef GL_RGB4
    rgb4 = GL_RGB4,
#endif
#ifdef GL_RGB4_EXT
    rgb4_ext = GL_RGB4_EXT,
#endif
#ifdef GL_RGB5
    rgb5 = GL_RGB5,
#endif
#ifdef GL_RGB5_A1
    rgb5_a1 = GL_RGB5_A1,
#endif
#ifdef GL_RGB5_A1_EXT
    rgb5_a1_ext = GL_RGB5_A1_EXT,
#endif
#ifdef GL_RGB5_A1_OES
    rgb5_a1_oes = GL_RGB5_A1_OES,
#endif
#ifdef GL_RGB5_EXT
    rgb5_ext = GL_RGB5_EXT,
#endif
#ifdef GL_RGB8
    rgb8 = GL_RGB8,
#endif
#ifdef GL_RGB8I
    rgb8i = GL_RGB8I,
#endif
#ifdef GL_RGB8I_EXT
    rgb8i_ext = GL_RGB8I_EXT,
#endif
#ifdef GL_RGB8UI
    rgb8ui = GL_RGB8UI,
#endif
#ifdef GL_RGB8UI_EXT
    rgb8ui_ext = GL_RGB8UI_EXT,
#endif
#ifdef GL_RGB8_EXT
    rgb8_ext = GL_RGB8_EXT,
#endif
#ifdef GL_RGB8_OES
    rgb8_oes = GL_RGB8_OES,
#endif
#ifdef GL_RGB8_SNORM
    rgb8_snorm = GL_RGB8_SNORM,
#endif
#ifdef GL_RGB9_E5
    rgb9_e5 = GL_RGB9_E5,
#endif
#ifdef GL_RGB9_E5_APPLE
    rgb9_e5_apple = GL_RGB9_E5_APPLE,
#endif
#ifdef GL_RGB9_E5_EXT
    rgb9_e5_ext = GL_RGB9_E5_EXT,
#endif
#ifdef GL_RGBA12
    rgba12 = GL_RGBA12,
#endif
#ifdef GL_RGBA12_EXT
    rgba12_ext = GL_RGBA12_EXT,
#endif
#ifdef GL_RGBA16
    rgba16 = GL_RGBA16,
#endif
#ifdef GL_RGBA16F
    rgba16f = GL_RGBA16F,
#endif
#ifdef GL_RGBA16F_ARB
    rgba16f_arb = GL_RGBA16F_ARB,
#endif
#ifdef GL_RGBA16F_EXT
    rgba16f_ext = GL_RGBA16F_EXT,
#endif
#ifdef GL_RGBA16I
    rgba16i = GL_RGBA16I,
#endif
#ifdef GL_RGBA16I_EXT
    rgba16i_ext = GL_RGBA16I_EXT,
#endif
#ifdef GL_RGBA16UI
    rgba16ui = GL_RGBA16UI,
#endif
#ifdef GL_RGBA16UI_EXT
    rgba16ui_ext = GL_RGBA16UI_EXT,
#endif
#ifdef GL_RGBA16_EXT
    rgba16_ext = GL_RGBA16_EXT,
#endif
#ifdef GL_RGBA16_SNORM
    rgba16_snorm = GL_RGBA16_SNORM,
#endif
#ifdef GL_RGBA16_SNORM_EXT
    rgba16_snorm_ext = GL_RGBA16_SNORM_EXT,
#endif
#ifdef GL_RGBA2
    rgba2 = GL_RGBA2,
#endif
#ifdef GL_RGBA2_EXT
    rgba2_ext = GL_RGBA2_EXT,
#endif
#ifdef GL_RGBA32F
    rgba32f = GL_RGBA32F,
#endif
#ifdef GL_RGBA32F_ARB
    rgba32f_arb = GL_RGBA32F_ARB,
#endif
#ifdef GL_RGBA32F_EXT
    rgba32f_ext = GL_RGBA32F_EXT,
#endif
#ifdef GL_RGBA32I
    rgba32i = GL_RGBA32I,
#endif
#ifdef GL_RGBA32I_EXT
    rgba32i_ext = GL_RGBA32I_EXT,
#endif
#ifdef GL_RGBA32UI
    rgba32ui = GL_RGBA32UI,
#endif
#ifdef GL_RGBA32UI_EXT
    rgba32ui_ext = GL_RGBA32UI_EXT,
#endif
#ifdef GL_RGBA4
    rgba4 = GL_RGBA4,
#endif
#ifdef GL_RGBA4_EXT
    rgba4_ext = GL_RGBA4_EXT,
#endif
#ifdef GL_RGBA4_OES
    rgba4_oes = GL_RGBA4_OES,
#endif
#ifdef GL_RGBA8
    rgba8 = GL_RGBA8,
#endif
#ifdef GL_RGBA8I
    rgba8i = GL_RGBA8I,
#endif
#ifdef GL_RGBA8I_EXT
    rgba8i_ext = GL_RGBA8I_EXT,
#endif
#ifdef GL_RGBA8UI
    rgba8ui = GL_RGBA8UI,
#endif
#ifdef GL_RGBA8UI_EXT
    rgba8ui_ext = GL_RGBA8UI_EXT,
#endif
#ifdef GL_RGBA8_EXT
    rgba8_ext = GL_RGBA8_EXT,
#endif
#ifdef GL_RGBA8_OES
    rgba8_oes = GL_RGBA8_OES,
#endif
#ifdef GL_RGBA8_SNORM
    rgba8_snorm = GL_RGBA8_SNORM,
#endif
#ifdef GL_SRGB8
    srgb8 = GL_SRGB8,
#endif
#ifdef GL_SRGB8_ALPHA8
    srgb8_alpha8 = GL_SRGB8_ALPHA8,
#endif
#ifdef GL_SRGB8_ALPHA8_EXT
    srgb8_alpha8_ext = GL_SRGB8_ALPHA8_EXT,
#endif
#ifdef GL_SRGB8_EXT
    srgb8_ext = GL_SRGB8_EXT,
#endif
#ifdef GL_SRGB8_NV
    srgb8_nv = GL_SRGB8_NV,
#endif
#ifdef GL_STENCIL_INDEX1
    stencil_index1 = GL_STENCIL_INDEX1,
#endif
#ifdef GL_STENCIL_INDEX16
    stencil_index16 = GL_STENCIL_INDEX16,
#endif
#ifdef GL_STENCIL_INDEX16_EXT
    stencil_index16_ext = GL_STENCIL_INDEX16_EXT,
#endif
#ifdef GL_STENCIL_INDEX1_EXT
    stencil_index1_ext = GL_STENCIL_INDEX1_EXT,
#endif
#ifdef GL_STENCIL_INDEX1_OES
    stencil_index1_oes = GL_STENCIL_INDEX1_OES,
#endif
#ifdef GL_STENCIL_INDEX4
    stencil_index4 = GL_STENCIL_INDEX4,
#endif
#ifdef GL_STENCIL_INDEX4_EXT
    stencil_index4_ext = GL_STENCIL_INDEX4_EXT,
#endif
#ifdef GL_STENCIL_INDEX4_OES
    stencil_index4_oes = GL_STENCIL_INDEX4_OES,
#endif
#ifdef GL_STENCIL_INDEX8
    stencil_index8 = GL_STENCIL_INDEX8,
#endif
#ifdef GL_STENCIL_INDEX8_EXT
    stencil_index8_ext = GL_STENCIL_INDEX8_EXT,
#endif
#ifdef GL_STENCIL_INDEX8_OES
    stencil_index8_oes = GL_STENCIL_INDEX8_OES,
#endif
}; // enum class sized_internal_format
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ClipPlaneName
enum class clip_plane_name : ::libc_types::u32 {
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
}; // enum class clip_plane_name
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// LightName
enum class light_name : ::libc_types::u32 {
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
}; // enum class light_name
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// BlendEquationModeEXT
enum class blend_equation_mode_ext : ::libc_types::u32 {
#ifdef GL_ALPHA_MAX_SGIX
    alpha_max_sgix = GL_ALPHA_MAX_SGIX,
#endif
#ifdef GL_ALPHA_MIN_SGIX
    alpha_min_sgix = GL_ALPHA_MIN_SGIX,
#endif
#ifdef GL_FUNC_ADD
    func_add = GL_FUNC_ADD,
#endif
#ifdef GL_FUNC_ADD_EXT
    func_add_ext = GL_FUNC_ADD_EXT,
#endif
#ifdef GL_FUNC_REVERSE_SUBTRACT
    func_reverse_subtract = GL_FUNC_REVERSE_SUBTRACT,
#endif
#ifdef GL_FUNC_REVERSE_SUBTRACT_EXT
    func_reverse_subtract_ext = GL_FUNC_REVERSE_SUBTRACT_EXT,
#endif
#ifdef GL_FUNC_SUBTRACT
    func_subtract = GL_FUNC_SUBTRACT,
#endif
#ifdef GL_FUNC_SUBTRACT_EXT
    func_subtract_ext = GL_FUNC_SUBTRACT_EXT,
#endif
#ifdef GL_MAX
    max = GL_MAX,
#endif
#ifdef GL_MAX_EXT
    max_ext = GL_MAX_EXT,
#endif
#ifdef GL_MIN
    min = GL_MIN,
#endif
#ifdef GL_MIN_EXT
    min_ext = GL_MIN_EXT,
#endif
}; // enum class blend_equation_mode_ext
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ConvolutionTarget
enum class convolution_target : ::libc_types::u32 {
#ifdef GL_CONVOLUTION_1D
    convolution_1d = GL_CONVOLUTION_1D,
#endif
#ifdef GL_CONVOLUTION_2D
    convolution_2d = GL_CONVOLUTION_2D,
#endif
}; // enum class convolution_target
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ConvolutionTargetEXT
enum class convolution_target_ext : ::libc_types::u32 {
#ifdef GL_CONVOLUTION_1D
    convolution_1d = GL_CONVOLUTION_1D,
#endif
#ifdef GL_CONVOLUTION_1D_EXT
    convolution_1d_ext = GL_CONVOLUTION_1D_EXT,
#endif
#ifdef GL_CONVOLUTION_2D
    convolution_2d = GL_CONVOLUTION_2D,
#endif
#ifdef GL_CONVOLUTION_2D_EXT
    convolution_2d_ext = GL_CONVOLUTION_2D_EXT,
#endif
}; // enum class convolution_target_ext
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// SeparableTarget
enum class separable_target : ::libc_types::u32 {
#ifdef GL_SEPARABLE_2D
    separable_2d = GL_SEPARABLE_2D,
#endif
}; // enum class separable_target
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// SeparableTargetEXT
enum class separable_target_ext : ::libc_types::u32 {
#ifdef GL_SEPARABLE_2D
    separable_2d = GL_SEPARABLE_2D,
#endif
#ifdef GL_SEPARABLE_2D_EXT
    separable_2d_ext = GL_SEPARABLE_2D_EXT,
#endif
}; // enum class separable_target_ext
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ConvolutionParameterEXT
enum class convolution_parameter_ext : ::libc_types::u32 {
#ifdef GL_CONVOLUTION_BORDER_MODE
    convolution_border_mode = GL_CONVOLUTION_BORDER_MODE,
#endif
#ifdef GL_CONVOLUTION_BORDER_MODE_EXT
    convolution_border_mode_ext = GL_CONVOLUTION_BORDER_MODE_EXT,
#endif
#ifdef GL_CONVOLUTION_FILTER_BIAS
    convolution_filter_bias = GL_CONVOLUTION_FILTER_BIAS,
#endif
#ifdef GL_CONVOLUTION_FILTER_BIAS_EXT
    convolution_filter_bias_ext = GL_CONVOLUTION_FILTER_BIAS_EXT,
#endif
#ifdef GL_CONVOLUTION_FILTER_SCALE
    convolution_filter_scale = GL_CONVOLUTION_FILTER_SCALE,
#endif
#ifdef GL_CONVOLUTION_FILTER_SCALE_EXT
    convolution_filter_scale_ext = GL_CONVOLUTION_FILTER_SCALE_EXT,
#endif
}; // enum class convolution_parameter_ext
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// HistogramTarget
enum class histogram_target : ::libc_types::u32 {
#ifdef GL_HISTOGRAM
    histogram = GL_HISTOGRAM,
#endif
#ifdef GL_PROXY_HISTOGRAM
    proxy_histogram = GL_PROXY_HISTOGRAM,
#endif
}; // enum class histogram_target
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// HistogramTargetEXT
enum class histogram_target_ext : ::libc_types::u32 {
#ifdef GL_HISTOGRAM
    histogram = GL_HISTOGRAM,
#endif
#ifdef GL_HISTOGRAM_EXT
    histogram_ext = GL_HISTOGRAM_EXT,
#endif
#ifdef GL_PROXY_HISTOGRAM
    proxy_histogram = GL_PROXY_HISTOGRAM,
#endif
#ifdef GL_PROXY_HISTOGRAM_EXT
    proxy_histogram_ext = GL_PROXY_HISTOGRAM_EXT,
#endif
}; // enum class histogram_target_ext
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// GetHistogramParameterPNameEXT
enum class get_histogram_parameter_prop_ext : ::libc_types::u32 {
#ifdef GL_HISTOGRAM_ALPHA_SIZE
    histogram_alpha_size = GL_HISTOGRAM_ALPHA_SIZE,
#endif
#ifdef GL_HISTOGRAM_ALPHA_SIZE_EXT
    histogram_alpha_size_ext = GL_HISTOGRAM_ALPHA_SIZE_EXT,
#endif
#ifdef GL_HISTOGRAM_BLUE_SIZE
    histogram_blue_size = GL_HISTOGRAM_BLUE_SIZE,
#endif
#ifdef GL_HISTOGRAM_BLUE_SIZE_EXT
    histogram_blue_size_ext = GL_HISTOGRAM_BLUE_SIZE_EXT,
#endif
#ifdef GL_HISTOGRAM_FORMAT
    histogram_format = GL_HISTOGRAM_FORMAT,
#endif
#ifdef GL_HISTOGRAM_FORMAT_EXT
    histogram_format_ext = GL_HISTOGRAM_FORMAT_EXT,
#endif
#ifdef GL_HISTOGRAM_GREEN_SIZE
    histogram_green_size = GL_HISTOGRAM_GREEN_SIZE,
#endif
#ifdef GL_HISTOGRAM_GREEN_SIZE_EXT
    histogram_green_size_ext = GL_HISTOGRAM_GREEN_SIZE_EXT,
#endif
#ifdef GL_HISTOGRAM_LUMINANCE_SIZE
    histogram_luminance_size = GL_HISTOGRAM_LUMINANCE_SIZE,
#endif
#ifdef GL_HISTOGRAM_LUMINANCE_SIZE_EXT
    histogram_luminance_size_ext = GL_HISTOGRAM_LUMINANCE_SIZE_EXT,
#endif
#ifdef GL_HISTOGRAM_RED_SIZE
    histogram_red_size = GL_HISTOGRAM_RED_SIZE,
#endif
#ifdef GL_HISTOGRAM_RED_SIZE_EXT
    histogram_red_size_ext = GL_HISTOGRAM_RED_SIZE_EXT,
#endif
#ifdef GL_HISTOGRAM_SINK
    histogram_sink = GL_HISTOGRAM_SINK,
#endif
#ifdef GL_HISTOGRAM_SINK_EXT
    histogram_sink_ext = GL_HISTOGRAM_SINK_EXT,
#endif
#ifdef GL_HISTOGRAM_WIDTH
    histogram_width = GL_HISTOGRAM_WIDTH,
#endif
#ifdef GL_HISTOGRAM_WIDTH_EXT
    histogram_width_ext = GL_HISTOGRAM_WIDTH_EXT,
#endif
}; // enum class get_histogram_parameter_prop_ext
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// MinmaxTarget
enum class minmax_target : ::libc_types::u32 {
#ifdef GL_MINMAX
    minmax = GL_MINMAX,
#endif
}; // enum class minmax_target
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// MinmaxTargetEXT
enum class minmax_target_ext : ::libc_types::u32 {
#ifdef GL_MINMAX
    minmax = GL_MINMAX,
#endif
#ifdef GL_MINMAX_EXT
    minmax_ext = GL_MINMAX_EXT,
#endif
}; // enum class minmax_target_ext
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// GetMinmaxParameterPNameEXT
enum class get_minmax_parameter_prop_ext : ::libc_types::u32 {
#ifdef GL_MINMAX_FORMAT
    minmax_format = GL_MINMAX_FORMAT,
#endif
#ifdef GL_MINMAX_FORMAT_EXT
    minmax_format_ext = GL_MINMAX_FORMAT_EXT,
#endif
#ifdef GL_MINMAX_SINK
    minmax_sink = GL_MINMAX_SINK,
#endif
#ifdef GL_MINMAX_SINK_EXT
    minmax_sink_ext = GL_MINMAX_SINK_EXT,
#endif
}; // enum class get_minmax_parameter_prop_ext
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// InternalFormatPName
enum class internal_format_prop : ::libc_types::u32 {
#ifdef GL_AUTO_GENERATE_MIPMAP
    auto_generate_mipmap = GL_AUTO_GENERATE_MIPMAP,
#endif
#ifdef GL_CLEAR_BUFFER
    clear_buffer = GL_CLEAR_BUFFER,
#endif
#ifdef GL_CLEAR_TEXTURE
    clear_texture = GL_CLEAR_TEXTURE,
#endif
#ifdef GL_COLOR_COMPONENTS
    color_components = GL_COLOR_COMPONENTS,
#endif
#ifdef GL_COLOR_ENCODING
    color_encoding = GL_COLOR_ENCODING,
#endif
#ifdef GL_COLOR_RENDERABLE
    color_renderable = GL_COLOR_RENDERABLE,
#endif
#ifdef GL_COMPUTE_TEXTURE
    compute_texture = GL_COMPUTE_TEXTURE,
#endif
#ifdef GL_DEPTH_RENDERABLE
    depth_renderable = GL_DEPTH_RENDERABLE,
#endif
#ifdef GL_FILTER
    filter = GL_FILTER,
#endif
#ifdef GL_FRAGMENT_TEXTURE
    fragment_texture = GL_FRAGMENT_TEXTURE,
#endif
#ifdef GL_FRAMEBUFFER_BLEND
    framebuffer_blend = GL_FRAMEBUFFER_BLEND,
#endif
#ifdef GL_FRAMEBUFFER_RENDERABLE
    framebuffer_renderable = GL_FRAMEBUFFER_RENDERABLE,
#endif
#ifdef GL_FRAMEBUFFER_RENDERABLE_LAYERED
    framebuffer_renderable_layered = GL_FRAMEBUFFER_RENDERABLE_LAYERED,
#endif
#ifdef GL_GENERATE_MIPMAP
    generate_mipmap = GL_GENERATE_MIPMAP,
#endif
#ifdef GL_GEOMETRY_TEXTURE
    geometry_texture = GL_GEOMETRY_TEXTURE,
#endif
#ifdef GL_GET_TEXTURE_IMAGE_FORMAT
    get_texture_image_format = GL_GET_TEXTURE_IMAGE_FORMAT,
#endif
#ifdef GL_GET_TEXTURE_IMAGE_TYPE
    get_texture_image_type = GL_GET_TEXTURE_IMAGE_TYPE,
#endif
#ifdef GL_IMAGE_COMPATIBILITY_CLASS
    image_compatibility_class = GL_IMAGE_COMPATIBILITY_CLASS,
#endif
#ifdef GL_IMAGE_FORMAT_COMPATIBILITY_TYPE
    image_format_compatibility_type = GL_IMAGE_FORMAT_COMPATIBILITY_TYPE,
#endif
#ifdef GL_IMAGE_PIXEL_FORMAT
    image_pixel_format = GL_IMAGE_PIXEL_FORMAT,
#endif
#ifdef GL_IMAGE_PIXEL_TYPE
    image_pixel_type = GL_IMAGE_PIXEL_TYPE,
#endif
#ifdef GL_IMAGE_TEXEL_SIZE
    image_texel_size = GL_IMAGE_TEXEL_SIZE,
#endif
#ifdef GL_INTERNALFORMAT_ALPHA_SIZE
    internalformat_alpha_size = GL_INTERNALFORMAT_ALPHA_SIZE,
#endif
#ifdef GL_INTERNALFORMAT_ALPHA_TYPE
    internalformat_alpha_type = GL_INTERNALFORMAT_ALPHA_TYPE,
#endif
#ifdef GL_INTERNALFORMAT_BLUE_SIZE
    internalformat_blue_size = GL_INTERNALFORMAT_BLUE_SIZE,
#endif
#ifdef GL_INTERNALFORMAT_BLUE_TYPE
    internalformat_blue_type = GL_INTERNALFORMAT_BLUE_TYPE,
#endif
#ifdef GL_INTERNALFORMAT_DEPTH_SIZE
    internalformat_depth_size = GL_INTERNALFORMAT_DEPTH_SIZE,
#endif
#ifdef GL_INTERNALFORMAT_DEPTH_TYPE
    internalformat_depth_type = GL_INTERNALFORMAT_DEPTH_TYPE,
#endif
#ifdef GL_INTERNALFORMAT_GREEN_SIZE
    internalformat_green_size = GL_INTERNALFORMAT_GREEN_SIZE,
#endif
#ifdef GL_INTERNALFORMAT_GREEN_TYPE
    internalformat_green_type = GL_INTERNALFORMAT_GREEN_TYPE,
#endif
#ifdef GL_INTERNALFORMAT_PREFERRED
    internalformat_preferred = GL_INTERNALFORMAT_PREFERRED,
#endif
#ifdef GL_INTERNALFORMAT_RED_SIZE
    internalformat_red_size = GL_INTERNALFORMAT_RED_SIZE,
#endif
#ifdef GL_INTERNALFORMAT_RED_TYPE
    internalformat_red_type = GL_INTERNALFORMAT_RED_TYPE,
#endif
#ifdef GL_INTERNALFORMAT_SHARED_SIZE
    internalformat_shared_size = GL_INTERNALFORMAT_SHARED_SIZE,
#endif
#ifdef GL_INTERNALFORMAT_STENCIL_SIZE
    internalformat_stencil_size = GL_INTERNALFORMAT_STENCIL_SIZE,
#endif
#ifdef GL_INTERNALFORMAT_STENCIL_TYPE
    internalformat_stencil_type = GL_INTERNALFORMAT_STENCIL_TYPE,
#endif
#ifdef GL_INTERNALFORMAT_SUPPORTED
    internalformat_supported = GL_INTERNALFORMAT_SUPPORTED,
#endif
#ifdef GL_MAX_DEPTH
    max_depth = GL_MAX_DEPTH,
#endif
#ifdef GL_MAX_HEIGHT
    max_height = GL_MAX_HEIGHT,
#endif
#ifdef GL_MAX_LAYERS
    max_layers = GL_MAX_LAYERS,
#endif
#ifdef GL_MAX_WIDTH
    max_width = GL_MAX_WIDTH,
#endif
#ifdef GL_MIPMAP
    mipmap = GL_MIPMAP,
#endif
#ifdef GL_NUM_SAMPLE_COUNTS
    num_sample_counts = GL_NUM_SAMPLE_COUNTS,
#endif
#ifdef GL_READ_PIXELS
    read_pixels = GL_READ_PIXELS,
#endif
#ifdef GL_READ_PIXELS_FORMAT
    read_pixels_format = GL_READ_PIXELS_FORMAT,
#endif
#ifdef GL_READ_PIXELS_TYPE
    read_pixels_type = GL_READ_PIXELS_TYPE,
#endif
#ifdef GL_SAMPLES
    samples = GL_SAMPLES,
#endif
#ifdef GL_SHADER_IMAGE_ATOMIC
    shader_image_atomic = GL_SHADER_IMAGE_ATOMIC,
#endif
#ifdef GL_SHADER_IMAGE_LOAD
    shader_image_load = GL_SHADER_IMAGE_LOAD,
#endif
#ifdef GL_SHADER_IMAGE_STORE
    shader_image_store = GL_SHADER_IMAGE_STORE,
#endif
#ifdef GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_TEST
    simultaneous_texture_and_depth_test = GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_TEST,
#endif
#ifdef GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_WRITE
    simultaneous_texture_and_depth_write = GL_SIMULTANEOUS_TEXTURE_AND_DEPTH_WRITE,
#endif
#ifdef GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_TEST
    simultaneous_texture_and_stencil_test = GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_TEST,
#endif
#ifdef GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_WRITE
    simultaneous_texture_and_stencil_write = GL_SIMULTANEOUS_TEXTURE_AND_STENCIL_WRITE,
#endif
#ifdef GL_SRGB_READ
    srgb_read = GL_SRGB_READ,
#endif
#ifdef GL_SRGB_WRITE
    srgb_write = GL_SRGB_WRITE,
#endif
#ifdef GL_STENCIL_RENDERABLE
    stencil_renderable = GL_STENCIL_RENDERABLE,
#endif
#ifdef GL_TESS_CONTROL_TEXTURE
    tess_control_texture = GL_TESS_CONTROL_TEXTURE,
#endif
#ifdef GL_TESS_EVALUATION_TEXTURE
    tess_evaluation_texture = GL_TESS_EVALUATION_TEXTURE,
#endif
#ifdef GL_TEXTURE_COMPRESSED
    texture_compressed = GL_TEXTURE_COMPRESSED,
#endif
#ifdef GL_TEXTURE_COMPRESSED_BLOCK_HEIGHT
    texture_compressed_block_height = GL_TEXTURE_COMPRESSED_BLOCK_HEIGHT,
#endif
#ifdef GL_TEXTURE_COMPRESSED_BLOCK_SIZE
    texture_compressed_block_size = GL_TEXTURE_COMPRESSED_BLOCK_SIZE,
#endif
#ifdef GL_TEXTURE_COMPRESSED_BLOCK_WIDTH
    texture_compressed_block_width = GL_TEXTURE_COMPRESSED_BLOCK_WIDTH,
#endif
#ifdef GL_TEXTURE_GATHER
    texture_gather = GL_TEXTURE_GATHER,
#endif
#ifdef GL_TEXTURE_GATHER_SHADOW
    texture_gather_shadow = GL_TEXTURE_GATHER_SHADOW,
#endif
#ifdef GL_TEXTURE_IMAGE_FORMAT
    texture_image_format = GL_TEXTURE_IMAGE_FORMAT,
#endif
#ifdef GL_TEXTURE_IMAGE_TYPE
    texture_image_type = GL_TEXTURE_IMAGE_TYPE,
#endif
#ifdef GL_TEXTURE_SHADOW
    texture_shadow = GL_TEXTURE_SHADOW,
#endif
#ifdef GL_TEXTURE_VIEW
    texture_view = GL_TEXTURE_VIEW,
#endif
#ifdef GL_VERTEX_TEXTURE
    vertex_texture = GL_VERTEX_TEXTURE,
#endif
#ifdef GL_VIEW_COMPATIBILITY_CLASS
    view_compatibility_class = GL_VIEW_COMPATIBILITY_CLASS,
#endif
}; // enum class internal_format_prop
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ColorTableTarget
enum class color_table_target : ::libc_types::u32 {
#ifdef GL_COLOR_TABLE
    color_table = GL_COLOR_TABLE,
#endif
#ifdef GL_POST_COLOR_MATRIX_COLOR_TABLE
    post_color_matrix_color_table = GL_POST_COLOR_MATRIX_COLOR_TABLE,
#endif
#ifdef GL_POST_CONVOLUTION_COLOR_TABLE
    post_convolution_color_table = GL_POST_CONVOLUTION_COLOR_TABLE,
#endif
#ifdef GL_PROXY_COLOR_TABLE
    proxy_color_table = GL_PROXY_COLOR_TABLE,
#endif
#ifdef GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE
    proxy_post_color_matrix_color_table = GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE,
#endif
#ifdef GL_PROXY_POST_CONVOLUTION_COLOR_TABLE
    proxy_post_convolution_color_table = GL_PROXY_POST_CONVOLUTION_COLOR_TABLE,
#endif
}; // enum class color_table_target
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// GetColorTableParameterPNameSGI
enum class get_color_table_parameter_prop_sgi : ::libc_types::u32 {
#ifdef GL_COLOR_TABLE_ALPHA_SIZE
    color_table_alpha_size = GL_COLOR_TABLE_ALPHA_SIZE,
#endif
#ifdef GL_COLOR_TABLE_ALPHA_SIZE_SGI
    color_table_alpha_size_sgi = GL_COLOR_TABLE_ALPHA_SIZE_SGI,
#endif
#ifdef GL_COLOR_TABLE_BIAS
    color_table_bias = GL_COLOR_TABLE_BIAS,
#endif
#ifdef GL_COLOR_TABLE_BIAS_SGI
    color_table_bias_sgi = GL_COLOR_TABLE_BIAS_SGI,
#endif
#ifdef GL_COLOR_TABLE_BLUE_SIZE
    color_table_blue_size = GL_COLOR_TABLE_BLUE_SIZE,
#endif
#ifdef GL_COLOR_TABLE_BLUE_SIZE_SGI
    color_table_blue_size_sgi = GL_COLOR_TABLE_BLUE_SIZE_SGI,
#endif
#ifdef GL_COLOR_TABLE_FORMAT
    color_table_format = GL_COLOR_TABLE_FORMAT,
#endif
#ifdef GL_COLOR_TABLE_FORMAT_SGI
    color_table_format_sgi = GL_COLOR_TABLE_FORMAT_SGI,
#endif
#ifdef GL_COLOR_TABLE_GREEN_SIZE
    color_table_green_size = GL_COLOR_TABLE_GREEN_SIZE,
#endif
#ifdef GL_COLOR_TABLE_GREEN_SIZE_SGI
    color_table_green_size_sgi = GL_COLOR_TABLE_GREEN_SIZE_SGI,
#endif
#ifdef GL_COLOR_TABLE_INTENSITY_SIZE
    color_table_intensity_size = GL_COLOR_TABLE_INTENSITY_SIZE,
#endif
#ifdef GL_COLOR_TABLE_INTENSITY_SIZE_SGI
    color_table_intensity_size_sgi = GL_COLOR_TABLE_INTENSITY_SIZE_SGI,
#endif
#ifdef GL_COLOR_TABLE_LUMINANCE_SIZE
    color_table_luminance_size = GL_COLOR_TABLE_LUMINANCE_SIZE,
#endif
#ifdef GL_COLOR_TABLE_LUMINANCE_SIZE_SGI
    color_table_luminance_size_sgi = GL_COLOR_TABLE_LUMINANCE_SIZE_SGI,
#endif
#ifdef GL_COLOR_TABLE_RED_SIZE
    color_table_red_size = GL_COLOR_TABLE_RED_SIZE,
#endif
#ifdef GL_COLOR_TABLE_RED_SIZE_SGI
    color_table_red_size_sgi = GL_COLOR_TABLE_RED_SIZE_SGI,
#endif
#ifdef GL_COLOR_TABLE_SCALE
    color_table_scale = GL_COLOR_TABLE_SCALE,
#endif
#ifdef GL_COLOR_TABLE_SCALE_SGI
    color_table_scale_sgi = GL_COLOR_TABLE_SCALE_SGI,
#endif
#ifdef GL_COLOR_TABLE_WIDTH
    color_table_width = GL_COLOR_TABLE_WIDTH,
#endif
#ifdef GL_COLOR_TABLE_WIDTH_SGI
    color_table_width_sgi = GL_COLOR_TABLE_WIDTH_SGI,
#endif
}; // enum class get_color_table_parameter_prop_sgi
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ColorTableParameterPNameSGI
enum class color_table_parameter_prop_sgi : ::libc_types::u32 {
#ifdef GL_COLOR_TABLE_ALPHA_SIZE
    color_table_alpha_size = GL_COLOR_TABLE_ALPHA_SIZE,
#endif
#ifdef GL_COLOR_TABLE_ALPHA_SIZE_SGI
    color_table_alpha_size_sgi = GL_COLOR_TABLE_ALPHA_SIZE_SGI,
#endif
#ifdef GL_COLOR_TABLE_BIAS
    color_table_bias = GL_COLOR_TABLE_BIAS,
#endif
#ifdef GL_COLOR_TABLE_BIAS_SGI
    color_table_bias_sgi = GL_COLOR_TABLE_BIAS_SGI,
#endif
#ifdef GL_COLOR_TABLE_BLUE_SIZE
    color_table_blue_size = GL_COLOR_TABLE_BLUE_SIZE,
#endif
#ifdef GL_COLOR_TABLE_BLUE_SIZE_SGI
    color_table_blue_size_sgi = GL_COLOR_TABLE_BLUE_SIZE_SGI,
#endif
#ifdef GL_COLOR_TABLE_FORMAT
    color_table_format = GL_COLOR_TABLE_FORMAT,
#endif
#ifdef GL_COLOR_TABLE_FORMAT_SGI
    color_table_format_sgi = GL_COLOR_TABLE_FORMAT_SGI,
#endif
#ifdef GL_COLOR_TABLE_GREEN_SIZE
    color_table_green_size = GL_COLOR_TABLE_GREEN_SIZE,
#endif
#ifdef GL_COLOR_TABLE_GREEN_SIZE_SGI
    color_table_green_size_sgi = GL_COLOR_TABLE_GREEN_SIZE_SGI,
#endif
#ifdef GL_COLOR_TABLE_INTENSITY_SIZE
    color_table_intensity_size = GL_COLOR_TABLE_INTENSITY_SIZE,
#endif
#ifdef GL_COLOR_TABLE_INTENSITY_SIZE_SGI
    color_table_intensity_size_sgi = GL_COLOR_TABLE_INTENSITY_SIZE_SGI,
#endif
#ifdef GL_COLOR_TABLE_LUMINANCE_SIZE
    color_table_luminance_size = GL_COLOR_TABLE_LUMINANCE_SIZE,
#endif
#ifdef GL_COLOR_TABLE_LUMINANCE_SIZE_SGI
    color_table_luminance_size_sgi = GL_COLOR_TABLE_LUMINANCE_SIZE_SGI,
#endif
#ifdef GL_COLOR_TABLE_RED_SIZE
    color_table_red_size = GL_COLOR_TABLE_RED_SIZE,
#endif
#ifdef GL_COLOR_TABLE_RED_SIZE_SGI
    color_table_red_size_sgi = GL_COLOR_TABLE_RED_SIZE_SGI,
#endif
#ifdef GL_COLOR_TABLE_SCALE
    color_table_scale = GL_COLOR_TABLE_SCALE,
#endif
#ifdef GL_COLOR_TABLE_SCALE_SGI
    color_table_scale_sgi = GL_COLOR_TABLE_SCALE_SGI,
#endif
#ifdef GL_COLOR_TABLE_WIDTH
    color_table_width = GL_COLOR_TABLE_WIDTH,
#endif
#ifdef GL_COLOR_TABLE_WIDTH_SGI
    color_table_width_sgi = GL_COLOR_TABLE_WIDTH_SGI,
#endif
}; // enum class color_table_parameter_prop_sgi
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// BufferTargetARB
enum class buffer_target_arb : ::libc_types::u32 {
#ifdef GL_ARRAY_BUFFER
    array_buffer = GL_ARRAY_BUFFER,
#endif
#ifdef GL_ATOMIC_COUNTER_BUFFER
    atomic_counter_buffer = GL_ATOMIC_COUNTER_BUFFER,
#endif
#ifdef GL_COPY_READ_BUFFER
    copy_read_buffer = GL_COPY_READ_BUFFER,
#endif
#ifdef GL_COPY_WRITE_BUFFER
    copy_write_buffer = GL_COPY_WRITE_BUFFER,
#endif
#ifdef GL_DISPATCH_INDIRECT_BUFFER
    dispatch_indirect_buffer = GL_DISPATCH_INDIRECT_BUFFER,
#endif
#ifdef GL_DRAW_INDIRECT_BUFFER
    draw_indirect_buffer = GL_DRAW_INDIRECT_BUFFER,
#endif
#ifdef GL_ELEMENT_ARRAY_BUFFER
    element_array_buffer = GL_ELEMENT_ARRAY_BUFFER,
#endif
#ifdef GL_PARAMETER_BUFFER
    parameter_buffer = GL_PARAMETER_BUFFER,
#endif
#ifdef GL_PIXEL_PACK_BUFFER
    pixel_pack_buffer = GL_PIXEL_PACK_BUFFER,
#endif
#ifdef GL_PIXEL_UNPACK_BUFFER
    pixel_unpack_buffer = GL_PIXEL_UNPACK_BUFFER,
#endif
#ifdef GL_QUERY_BUFFER
    query_buffer = GL_QUERY_BUFFER,
#endif
#ifdef GL_SHADER_STORAGE_BUFFER
    shader_storage_buffer = GL_SHADER_STORAGE_BUFFER,
#endif
#ifdef GL_TEXTURE_BUFFER
    texture_buffer = GL_TEXTURE_BUFFER,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BUFFER
    transform_feedback_buffer = GL_TRANSFORM_FEEDBACK_BUFFER,
#endif
#ifdef GL_UNIFORM_BUFFER
    uniform_buffer = GL_UNIFORM_BUFFER,
#endif
}; // enum class buffer_target_arb
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// PointParameterNameARB
enum class point_parameter_name_arb : ::libc_types::u32 {
#ifdef GL_POINT_FADE_THRESHOLD_SIZE
    point_fade_threshold_size = GL_POINT_FADE_THRESHOLD_SIZE,
#endif
#ifdef GL_POINT_FADE_THRESHOLD_SIZE_EXT
    point_fade_threshold_size_ext = GL_POINT_FADE_THRESHOLD_SIZE_EXT,
#endif
#ifdef GL_POINT_SIZE_MAX_EXT
    point_size_max_ext = GL_POINT_SIZE_MAX_EXT,
#endif
#ifdef GL_POINT_SIZE_MIN_EXT
    point_size_min_ext = GL_POINT_SIZE_MIN_EXT,
#endif
}; // enum class point_parameter_name_arb
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ProgramTarget
enum class program_target : ::libc_types::u32 {
#ifdef GL_COMPUTE_PROGRAM_NV
    compute_program_nv = GL_COMPUTE_PROGRAM_NV,
#endif
#ifdef GL_FRAGMENT_PROGRAM_ARB
    fragment_program_arb = GL_FRAGMENT_PROGRAM_ARB,
#endif
#ifdef GL_GEOMETRY_PROGRAM_NV
    geometry_program_nv = GL_GEOMETRY_PROGRAM_NV,
#endif
#ifdef GL_TESS_CONTROL_PROGRAM_NV
    tess_control_program_nv = GL_TESS_CONTROL_PROGRAM_NV,
#endif
#ifdef GL_TESS_EVALUATION_PROGRAM_NV
    tess_evaluation_program_nv = GL_TESS_EVALUATION_PROGRAM_NV,
#endif
#ifdef GL_TEXT_FRAGMENT_SHADER_ATI
    text_fragment_shader_ati = GL_TEXT_FRAGMENT_SHADER_ATI,
#endif
#ifdef GL_VERTEX_PROGRAM_ARB
    vertex_program_arb = GL_VERTEX_PROGRAM_ARB,
#endif
}; // enum class program_target
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// FramebufferAttachmentParameterName
enum class framebuffer_attachment_parameter_name : ::libc_types::u32 {
#ifdef GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE
    framebuffer_attachment_alpha_size = GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE
    framebuffer_attachment_blue_size = GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING
    framebuffer_attachment_color_encoding = GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING_EXT
    framebuffer_attachment_color_encoding_ext = GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING_EXT,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE
    framebuffer_attachment_component_type = GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE_EXT
    framebuffer_attachment_component_type_ext = GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE_EXT,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE
    framebuffer_attachment_depth_size = GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE
    framebuffer_attachment_green_size = GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_LAYERED
    framebuffer_attachment_layered = GL_FRAMEBUFFER_ATTACHMENT_LAYERED,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_LAYERED_ARB
    framebuffer_attachment_layered_arb = GL_FRAMEBUFFER_ATTACHMENT_LAYERED_ARB,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_LAYERED_EXT
    framebuffer_attachment_layered_ext = GL_FRAMEBUFFER_ATTACHMENT_LAYERED_EXT,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_LAYERED_OES
    framebuffer_attachment_layered_oes = GL_FRAMEBUFFER_ATTACHMENT_LAYERED_OES,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME
    framebuffer_attachment_object_name = GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT
    framebuffer_attachment_object_name_ext = GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_OES
    framebuffer_attachment_object_name_oes = GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_OES,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE
    framebuffer_attachment_object_type = GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT
    framebuffer_attachment_object_type_ext = GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_OES
    framebuffer_attachment_object_type_oes = GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_OES,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE
    framebuffer_attachment_red_size = GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE
    framebuffer_attachment_stencil_size = GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT
    framebuffer_attachment_texture_3d_zoffset_ext = GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_OES
    framebuffer_attachment_texture_3d_zoffset_oes = GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_OES,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_BASE_VIEW_INDEX_OVR
    framebuffer_attachment_texture_base_view_index_ovr = GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_BASE_VIEW_INDEX_OVR,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE
    framebuffer_attachment_texture_cube_map_face = GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT
    framebuffer_attachment_texture_cube_map_face_ext = GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_OES
    framebuffer_attachment_texture_cube_map_face_oes = GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_OES,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER
    framebuffer_attachment_texture_layer = GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER_EXT
    framebuffer_attachment_texture_layer_ext = GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER_EXT,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL
    framebuffer_attachment_texture_level = GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT
    framebuffer_attachment_texture_level_ext = GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_OES
    framebuffer_attachment_texture_level_oes = GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_OES,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_NUM_VIEWS_OVR
    framebuffer_attachment_texture_num_views_ovr = GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_NUM_VIEWS_OVR,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_SAMPLES_EXT
    framebuffer_attachment_texture_samples_ext = GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_SAMPLES_EXT,
#endif
#ifdef GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_SCALE_IMG
    framebuffer_attachment_texture_scale_img = GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_SCALE_IMG,
#endif
}; // enum class framebuffer_attachment_parameter_name
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// BufferPNameARB
enum class buffer_prop_arb : ::libc_types::u32 {
#ifdef GL_BUFFER_ACCESS
    buffer_access = GL_BUFFER_ACCESS,
#endif
#ifdef GL_BUFFER_ACCESS_ARB
    buffer_access_arb = GL_BUFFER_ACCESS_ARB,
#endif
#ifdef GL_BUFFER_ACCESS_FLAGS
    buffer_access_flags = GL_BUFFER_ACCESS_FLAGS,
#endif
#ifdef GL_BUFFER_IMMUTABLE_STORAGE
    buffer_immutable_storage = GL_BUFFER_IMMUTABLE_STORAGE,
#endif
#ifdef GL_BUFFER_MAPPED
    buffer_mapped = GL_BUFFER_MAPPED,
#endif
#ifdef GL_BUFFER_MAPPED_ARB
    buffer_mapped_arb = GL_BUFFER_MAPPED_ARB,
#endif
#ifdef GL_BUFFER_MAP_LENGTH
    buffer_map_length = GL_BUFFER_MAP_LENGTH,
#endif
#ifdef GL_BUFFER_MAP_OFFSET
    buffer_map_offset = GL_BUFFER_MAP_OFFSET,
#endif
#ifdef GL_BUFFER_SIZE
    buffer_size = GL_BUFFER_SIZE,
#endif
#ifdef GL_BUFFER_SIZE_ARB
    buffer_size_arb = GL_BUFFER_SIZE_ARB,
#endif
#ifdef GL_BUFFER_STORAGE_FLAGS
    buffer_storage_flags = GL_BUFFER_STORAGE_FLAGS,
#endif
#ifdef GL_BUFFER_USAGE
    buffer_usage = GL_BUFFER_USAGE,
#endif
#ifdef GL_BUFFER_USAGE_ARB
    buffer_usage_arb = GL_BUFFER_USAGE_ARB,
#endif
}; // enum class buffer_prop_arb
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ProgramParameterPName
enum class program_parameter_prop : ::libc_types::u32 {
#ifdef GL_PROGRAM_BINARY_RETRIEVABLE_HINT
    program_binary_retrievable_hint = GL_PROGRAM_BINARY_RETRIEVABLE_HINT,
#endif
#ifdef GL_PROGRAM_SEPARABLE
    program_separable = GL_PROGRAM_SEPARABLE,
#endif
}; // enum class program_parameter_prop
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// PipelineParameterName
enum class pipeline_parameter_name : ::libc_types::u32 {
#ifdef GL_ACTIVE_PROGRAM
    active_program = GL_ACTIVE_PROGRAM,
#endif
#ifdef GL_FRAGMENT_SHADER
    fragment_shader = GL_FRAGMENT_SHADER,
#endif
#ifdef GL_GEOMETRY_SHADER
    geometry_shader = GL_GEOMETRY_SHADER,
#endif
#ifdef GL_INFO_LOG_LENGTH
    info_log_length = GL_INFO_LOG_LENGTH,
#endif
#ifdef GL_TESS_CONTROL_SHADER
    tess_control_shader = GL_TESS_CONTROL_SHADER,
#endif
#ifdef GL_TESS_EVALUATION_SHADER
    tess_evaluation_shader = GL_TESS_EVALUATION_SHADER,
#endif
#ifdef GL_VERTEX_SHADER
    vertex_shader = GL_VERTEX_SHADER,
#endif
}; // enum class pipeline_parameter_name
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ProgramPropertyARB
enum class program_property_arb : ::libc_types::u32 {
#ifdef GL_ACTIVE_ATOMIC_COUNTER_BUFFERS
    active_atomic_counter_buffers = GL_ACTIVE_ATOMIC_COUNTER_BUFFERS,
#endif
#ifdef GL_ACTIVE_ATTRIBUTES
    active_attributes = GL_ACTIVE_ATTRIBUTES,
#endif
#ifdef GL_ACTIVE_ATTRIBUTE_MAX_LENGTH
    active_attribute_max_length = GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,
#endif
#ifdef GL_ACTIVE_UNIFORMS
    active_uniforms = GL_ACTIVE_UNIFORMS,
#endif
#ifdef GL_ACTIVE_UNIFORM_BLOCKS
    active_uniform_blocks = GL_ACTIVE_UNIFORM_BLOCKS,
#endif
#ifdef GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH
    active_uniform_block_max_name_length = GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH,
#endif
#ifdef GL_ACTIVE_UNIFORM_MAX_LENGTH
    active_uniform_max_length = GL_ACTIVE_UNIFORM_MAX_LENGTH,
#endif
#ifdef GL_ATTACHED_SHADERS
    attached_shaders = GL_ATTACHED_SHADERS,
#endif
#ifdef GL_COMPUTE_WORK_GROUP_SIZE
    compute_work_group_size = GL_COMPUTE_WORK_GROUP_SIZE,
#endif
#ifdef GL_DELETE_STATUS
    delete_status = GL_DELETE_STATUS,
#endif
#ifdef GL_GEOMETRY_INPUT_TYPE
    geometry_input_type = GL_GEOMETRY_INPUT_TYPE,
#endif
#ifdef GL_GEOMETRY_OUTPUT_TYPE
    geometry_output_type = GL_GEOMETRY_OUTPUT_TYPE,
#endif
#ifdef GL_GEOMETRY_VERTICES_OUT
    geometry_vertices_out = GL_GEOMETRY_VERTICES_OUT,
#endif
#ifdef GL_INFO_LOG_LENGTH
    info_log_length = GL_INFO_LOG_LENGTH,
#endif
#ifdef GL_LINK_STATUS
    link_status = GL_LINK_STATUS,
#endif
#ifdef GL_PROGRAM_BINARY_LENGTH
    program_binary_length = GL_PROGRAM_BINARY_LENGTH,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BUFFER_MODE
    transform_feedback_buffer_mode = GL_TRANSFORM_FEEDBACK_BUFFER_MODE,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_VARYINGS
    transform_feedback_varyings = GL_TRANSFORM_FEEDBACK_VARYINGS,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH
    transform_feedback_varying_max_length = GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH,
#endif
#ifdef GL_VALIDATE_STATUS
    validate_status = GL_VALIDATE_STATUS,
#endif
}; // enum class program_property_arb
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// VertexAttribPropertyARB
enum class vertex_attrib_property_arb : ::libc_types::u32 {
#ifdef GL_CURRENT_VERTEX_ATTRIB
    current_vertex_attrib = GL_CURRENT_VERTEX_ATTRIB,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING
    vertex_attrib_array_buffer_binding = GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_DIVISOR
    vertex_attrib_array_divisor = GL_VERTEX_ATTRIB_ARRAY_DIVISOR,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_ENABLED
    vertex_attrib_array_enabled = GL_VERTEX_ATTRIB_ARRAY_ENABLED,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_INTEGER
    vertex_attrib_array_integer = GL_VERTEX_ATTRIB_ARRAY_INTEGER,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_INTEGER_EXT
    vertex_attrib_array_integer_ext = GL_VERTEX_ATTRIB_ARRAY_INTEGER_EXT,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_LONG
    vertex_attrib_array_long = GL_VERTEX_ATTRIB_ARRAY_LONG,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_NORMALIZED
    vertex_attrib_array_normalized = GL_VERTEX_ATTRIB_ARRAY_NORMALIZED,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_SIZE
    vertex_attrib_array_size = GL_VERTEX_ATTRIB_ARRAY_SIZE,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_STRIDE
    vertex_attrib_array_stride = GL_VERTEX_ATTRIB_ARRAY_STRIDE,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_TYPE
    vertex_attrib_array_type = GL_VERTEX_ATTRIB_ARRAY_TYPE,
#endif
#ifdef GL_VERTEX_ATTRIB_BINDING
    vertex_attrib_binding = GL_VERTEX_ATTRIB_BINDING,
#endif
#ifdef GL_VERTEX_ATTRIB_RELATIVE_OFFSET
    vertex_attrib_relative_offset = GL_VERTEX_ATTRIB_RELATIVE_OFFSET,
#endif
}; // enum class vertex_attrib_property_arb
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// VertexArrayPName
enum class vertex_array_prop : ::libc_types::u32 {
#ifdef GL_VERTEX_ATTRIB_ARRAY_DIVISOR
    vertex_attrib_array_divisor = GL_VERTEX_ATTRIB_ARRAY_DIVISOR,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_ENABLED
    vertex_attrib_array_enabled = GL_VERTEX_ATTRIB_ARRAY_ENABLED,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_INTEGER
    vertex_attrib_array_integer = GL_VERTEX_ATTRIB_ARRAY_INTEGER,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_LONG
    vertex_attrib_array_long = GL_VERTEX_ATTRIB_ARRAY_LONG,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_NORMALIZED
    vertex_attrib_array_normalized = GL_VERTEX_ATTRIB_ARRAY_NORMALIZED,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_SIZE
    vertex_attrib_array_size = GL_VERTEX_ATTRIB_ARRAY_SIZE,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_STRIDE
    vertex_attrib_array_stride = GL_VERTEX_ATTRIB_ARRAY_STRIDE,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_TYPE
    vertex_attrib_array_type = GL_VERTEX_ATTRIB_ARRAY_TYPE,
#endif
#ifdef GL_VERTEX_ATTRIB_RELATIVE_OFFSET
    vertex_attrib_relative_offset = GL_VERTEX_ATTRIB_RELATIVE_OFFSET,
#endif
}; // enum class vertex_array_prop
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// QueryObjectParameterName
enum class query_object_parameter_name : ::libc_types::u32 {
#ifdef GL_QUERY_RESULT
    query_result = GL_QUERY_RESULT,
#endif
#ifdef GL_QUERY_RESULT_AVAILABLE
    query_result_available = GL_QUERY_RESULT_AVAILABLE,
#endif
#ifdef GL_QUERY_RESULT_NO_WAIT
    query_result_no_wait = GL_QUERY_RESULT_NO_WAIT,
#endif
#ifdef GL_QUERY_TARGET
    query_target = GL_QUERY_TARGET,
#endif
}; // enum class query_object_parameter_name
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// QueryTarget
enum class query_target : ::libc_types::u32 {
#ifdef GL_ANY_SAMPLES_PASSED
    any_samples_passed = GL_ANY_SAMPLES_PASSED,
#endif
#ifdef GL_ANY_SAMPLES_PASSED_CONSERVATIVE
    any_samples_passed_conservative = GL_ANY_SAMPLES_PASSED_CONSERVATIVE,
#endif
#ifdef GL_PRIMITIVES_GENERATED
    primitives_generated = GL_PRIMITIVES_GENERATED,
#endif
#ifdef GL_PRIMITIVES_SUBMITTED
    primitives_submitted = GL_PRIMITIVES_SUBMITTED,
#endif
#ifdef GL_SAMPLES_PASSED
    samples_passed = GL_SAMPLES_PASSED,
#endif
#ifdef GL_TIME_ELAPSED
    time_elapsed = GL_TIME_ELAPSED,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_OVERFLOW
    transform_feedback_overflow = GL_TRANSFORM_FEEDBACK_OVERFLOW,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN
    transform_feedback_primitives_written = GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,
#endif
#ifdef GL_VERTEX_SHADER_INVOCATIONS
    vertex_shader_invocations = GL_VERTEX_SHADER_INVOCATIONS,
#endif
#ifdef GL_VERTICES_SUBMITTED
    vertices_submitted = GL_VERTICES_SUBMITTED,
#endif
}; // enum class query_target
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// TextureUnit
enum class texture_unit : ::libc_types::u32 {
#ifdef GL_TEXTURE0
    texture0 = GL_TEXTURE0,
#endif
#ifdef GL_TEXTURE1
    texture1 = GL_TEXTURE1,
#endif
#ifdef GL_TEXTURE10
    texture10 = GL_TEXTURE10,
#endif
#ifdef GL_TEXTURE11
    texture11 = GL_TEXTURE11,
#endif
#ifdef GL_TEXTURE12
    texture12 = GL_TEXTURE12,
#endif
#ifdef GL_TEXTURE13
    texture13 = GL_TEXTURE13,
#endif
#ifdef GL_TEXTURE14
    texture14 = GL_TEXTURE14,
#endif
#ifdef GL_TEXTURE15
    texture15 = GL_TEXTURE15,
#endif
#ifdef GL_TEXTURE16
    texture16 = GL_TEXTURE16,
#endif
#ifdef GL_TEXTURE17
    texture17 = GL_TEXTURE17,
#endif
#ifdef GL_TEXTURE18
    texture18 = GL_TEXTURE18,
#endif
#ifdef GL_TEXTURE19
    texture19 = GL_TEXTURE19,
#endif
#ifdef GL_TEXTURE2
    texture2 = GL_TEXTURE2,
#endif
#ifdef GL_TEXTURE20
    texture20 = GL_TEXTURE20,
#endif
#ifdef GL_TEXTURE21
    texture21 = GL_TEXTURE21,
#endif
#ifdef GL_TEXTURE22
    texture22 = GL_TEXTURE22,
#endif
#ifdef GL_TEXTURE23
    texture23 = GL_TEXTURE23,
#endif
#ifdef GL_TEXTURE24
    texture24 = GL_TEXTURE24,
#endif
#ifdef GL_TEXTURE25
    texture25 = GL_TEXTURE25,
#endif
#ifdef GL_TEXTURE26
    texture26 = GL_TEXTURE26,
#endif
#ifdef GL_TEXTURE27
    texture27 = GL_TEXTURE27,
#endif
#ifdef GL_TEXTURE28
    texture28 = GL_TEXTURE28,
#endif
#ifdef GL_TEXTURE29
    texture29 = GL_TEXTURE29,
#endif
#ifdef GL_TEXTURE3
    texture3 = GL_TEXTURE3,
#endif
#ifdef GL_TEXTURE30
    texture30 = GL_TEXTURE30,
#endif
#ifdef GL_TEXTURE31
    texture31 = GL_TEXTURE31,
#endif
#ifdef GL_TEXTURE4
    texture4 = GL_TEXTURE4,
#endif
#ifdef GL_TEXTURE5
    texture5 = GL_TEXTURE5,
#endif
#ifdef GL_TEXTURE6
    texture6 = GL_TEXTURE6,
#endif
#ifdef GL_TEXTURE7
    texture7 = GL_TEXTURE7,
#endif
#ifdef GL_TEXTURE8
    texture8 = GL_TEXTURE8,
#endif
#ifdef GL_TEXTURE9
    texture9 = GL_TEXTURE9,
#endif
}; // enum class texture_unit
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// CombinerRegisterNV
enum class combiner_register_nv : ::libc_types::u32 {
#ifdef GL_DISCARD_NV
    discard_nv = GL_DISCARD_NV,
#endif
#ifdef GL_PRIMARY_COLOR_NV
    primary_color_nv = GL_PRIMARY_COLOR_NV,
#endif
#ifdef GL_SECONDARY_COLOR_NV
    secondary_color_nv = GL_SECONDARY_COLOR_NV,
#endif
#ifdef GL_SPARE0_NV
    spare0_nv = GL_SPARE0_NV,
#endif
#ifdef GL_SPARE1_NV
    spare1_nv = GL_SPARE1_NV,
#endif
#ifdef GL_TEXTURE0_ARB
    texture0_arb = GL_TEXTURE0_ARB,
#endif
#ifdef GL_TEXTURE1_ARB
    texture1_arb = GL_TEXTURE1_ARB,
#endif
}; // enum class combiner_register_nv
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// UniformBlockPName
enum class uniform_block_prop : ::libc_types::u32 {
#ifdef GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS
    uniform_block_active_uniforms = GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS,
#endif
#ifdef GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES
    uniform_block_active_uniform_indices = GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES,
#endif
#ifdef GL_UNIFORM_BLOCK_BINDING
    uniform_block_binding = GL_UNIFORM_BLOCK_BINDING,
#endif
#ifdef GL_UNIFORM_BLOCK_DATA_SIZE
    uniform_block_data_size = GL_UNIFORM_BLOCK_DATA_SIZE,
#endif
#ifdef GL_UNIFORM_BLOCK_NAME_LENGTH
    uniform_block_name_length = GL_UNIFORM_BLOCK_NAME_LENGTH,
#endif
#ifdef GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER
    uniform_block_referenced_by_compute_shader = GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER,
#endif
#ifdef GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER
    uniform_block_referenced_by_fragment_shader = GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER,
#endif
#ifdef GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER
    uniform_block_referenced_by_geometry_shader = GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER,
#endif
#ifdef GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER
    uniform_block_referenced_by_tess_control_shader = GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER,
#endif
#ifdef GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER
    uniform_block_referenced_by_tess_evaluation_shader = GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER,
#endif
#ifdef GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER
    uniform_block_referenced_by_vertex_shader = GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER,
#endif
}; // enum class uniform_block_prop
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// FenceConditionNV
enum class fence_condition_nv : ::libc_types::u32 {
#ifdef GL_ALL_COMPLETED_NV
    all_completed_nv = GL_ALL_COMPLETED_NV,
#endif
}; // enum class fence_condition_nv
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// FenceParameterNameNV
enum class fence_parameter_name_nv : ::libc_types::u32 {
#ifdef GL_FENCE_CONDITION_NV
    fence_condition_nv = GL_FENCE_CONDITION_NV,
#endif
#ifdef GL_FENCE_STATUS_NV
    fence_status_nv = GL_FENCE_STATUS_NV,
#endif
}; // enum class fence_parameter_name_nv
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// CombinerVariableNV
enum class combiner_variable_nv : ::libc_types::u32 {
#ifdef GL_VARIABLE_A_NV
    variable_a_nv = GL_VARIABLE_A_NV,
#endif
#ifdef GL_VARIABLE_B_NV
    variable_b_nv = GL_VARIABLE_B_NV,
#endif
#ifdef GL_VARIABLE_C_NV
    variable_c_nv = GL_VARIABLE_C_NV,
#endif
#ifdef GL_VARIABLE_D_NV
    variable_d_nv = GL_VARIABLE_D_NV,
#endif
#ifdef GL_VARIABLE_E_NV
    variable_e_nv = GL_VARIABLE_E_NV,
#endif
#ifdef GL_VARIABLE_F_NV
    variable_f_nv = GL_VARIABLE_F_NV,
#endif
#ifdef GL_VARIABLE_G_NV
    variable_g_nv = GL_VARIABLE_G_NV,
#endif
}; // enum class combiner_variable_nv
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// PathColor
enum class path_color : ::libc_types::u32 {
#ifdef GL_PRIMARY_COLOR
    primary_color = GL_PRIMARY_COLOR,
#endif
#ifdef GL_PRIMARY_COLOR_NV
    primary_color_nv = GL_PRIMARY_COLOR_NV,
#endif
#ifdef GL_SECONDARY_COLOR_NV
    secondary_color_nv = GL_SECONDARY_COLOR_NV,
#endif
}; // enum class path_color
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// CombinerMappingNV
enum class combiner_mapping_nv : ::libc_types::u32 {
#ifdef GL_EXPAND_NEGATE_NV
    expand_negate_nv = GL_EXPAND_NEGATE_NV,
#endif
#ifdef GL_EXPAND_NORMAL_NV
    expand_normal_nv = GL_EXPAND_NORMAL_NV,
#endif
#ifdef GL_HALF_BIAS_NEGATE_NV
    half_bias_negate_nv = GL_HALF_BIAS_NEGATE_NV,
#endif
#ifdef GL_HALF_BIAS_NORMAL_NV
    half_bias_normal_nv = GL_HALF_BIAS_NORMAL_NV,
#endif
#ifdef GL_SIGNED_IDENTITY_NV
    signed_identity_nv = GL_SIGNED_IDENTITY_NV,
#endif
#ifdef GL_SIGNED_NEGATE_NV
    signed_negate_nv = GL_SIGNED_NEGATE_NV,
#endif
#ifdef GL_UNSIGNED_IDENTITY_NV
    unsigned_identity_nv = GL_UNSIGNED_IDENTITY_NV,
#endif
#ifdef GL_UNSIGNED_INVERT_NV
    unsigned_invert_nv = GL_UNSIGNED_INVERT_NV,
#endif
}; // enum class combiner_mapping_nv
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// CombinerParameterNV
enum class combiner_parameter_nv : ::libc_types::u32 {
#ifdef GL_COMBINER_COMPONENT_USAGE_NV
    combiner_component_usage_nv = GL_COMBINER_COMPONENT_USAGE_NV,
#endif
#ifdef GL_COMBINER_INPUT_NV
    combiner_input_nv = GL_COMBINER_INPUT_NV,
#endif
#ifdef GL_COMBINER_MAPPING_NV
    combiner_mapping_nv = GL_COMBINER_MAPPING_NV,
#endif
}; // enum class combiner_parameter_nv
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// CombinerStageNV
enum class combiner_stage_nv : ::libc_types::u32 {
#ifdef GL_COMBINER0_NV
    combiner0_nv = GL_COMBINER0_NV,
#endif
#ifdef GL_COMBINER1_NV
    combiner1_nv = GL_COMBINER1_NV,
#endif
#ifdef GL_COMBINER2_NV
    combiner2_nv = GL_COMBINER2_NV,
#endif
#ifdef GL_COMBINER3_NV
    combiner3_nv = GL_COMBINER3_NV,
#endif
#ifdef GL_COMBINER4_NV
    combiner4_nv = GL_COMBINER4_NV,
#endif
#ifdef GL_COMBINER5_NV
    combiner5_nv = GL_COMBINER5_NV,
#endif
#ifdef GL_COMBINER6_NV
    combiner6_nv = GL_COMBINER6_NV,
#endif
#ifdef GL_COMBINER7_NV
    combiner7_nv = GL_COMBINER7_NV,
#endif
}; // enum class combiner_stage_nv
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// VertexAttribEnum
enum class vertex_attrib_enum : ::libc_types::u32 {
#ifdef GL_CURRENT_VERTEX_ATTRIB
    current_vertex_attrib = GL_CURRENT_VERTEX_ATTRIB,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING
    vertex_attrib_array_buffer_binding = GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_DIVISOR
    vertex_attrib_array_divisor = GL_VERTEX_ATTRIB_ARRAY_DIVISOR,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_ENABLED
    vertex_attrib_array_enabled = GL_VERTEX_ATTRIB_ARRAY_ENABLED,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_INTEGER
    vertex_attrib_array_integer = GL_VERTEX_ATTRIB_ARRAY_INTEGER,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_NORMALIZED
    vertex_attrib_array_normalized = GL_VERTEX_ATTRIB_ARRAY_NORMALIZED,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_SIZE
    vertex_attrib_array_size = GL_VERTEX_ATTRIB_ARRAY_SIZE,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_STRIDE
    vertex_attrib_array_stride = GL_VERTEX_ATTRIB_ARRAY_STRIDE,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_TYPE
    vertex_attrib_array_type = GL_VERTEX_ATTRIB_ARRAY_TYPE,
#endif
}; // enum class vertex_attrib_enum
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ProgramStringProperty
enum class program_string_property : ::libc_types::u32 {
#ifdef GL_PROGRAM_STRING_ARB
    program_string_arb = GL_PROGRAM_STRING_ARB,
#endif
}; // enum class program_string_property
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// VertexAttribEnumNV
enum class vertex_attrib_enum_nv : ::libc_types::u32 {
#ifdef GL_PROGRAM_PARAMETER_NV
    program_parameter_nv = GL_PROGRAM_PARAMETER_NV,
#endif
}; // enum class vertex_attrib_enum_nv
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// VertexAttribPointerPropertyARB
enum class vertex_attrib_pointer_property_arb : ::libc_types::u32 {
#ifdef GL_VERTEX_ATTRIB_ARRAY_POINTER
    vertex_attrib_array_pointer = GL_VERTEX_ATTRIB_ARRAY_POINTER,
#endif
#ifdef GL_VERTEX_ATTRIB_ARRAY_POINTER_ARB
    vertex_attrib_array_pointer_arb = GL_VERTEX_ATTRIB_ARRAY_POINTER_ARB,
#endif
}; // enum class vertex_attrib_pointer_property_arb
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// EvalTargetNV
enum class eval_target_nv : ::libc_types::u32 {
#ifdef GL_EVAL_2D_NV
    eval_2d_nv = GL_EVAL_2D_NV,
#endif
#ifdef GL_EVAL_TRIANGULAR_2D_NV
    eval_triangular_2d_nv = GL_EVAL_TRIANGULAR_2D_NV,
#endif
}; // enum class eval_target_nv
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// MapParameterNV
enum class map_parameter_nv : ::libc_types::u32 {
#ifdef GL_MAP_TESSELLATION_NV
    map_tessellation_nv = GL_MAP_TESSELLATION_NV,
#endif
}; // enum class map_parameter_nv
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// MapAttribParameterNV
enum class map_attrib_parameter_nv : ::libc_types::u32 {
#ifdef GL_MAP_ATTRIB_U_ORDER_NV
    map_attrib_u_order_nv = GL_MAP_ATTRIB_U_ORDER_NV,
#endif
#ifdef GL_MAP_ATTRIB_V_ORDER_NV
    map_attrib_v_order_nv = GL_MAP_ATTRIB_V_ORDER_NV,
#endif
}; // enum class map_attrib_parameter_nv
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// QueryParameterName
enum class query_parameter_name : ::libc_types::u32 {
#ifdef GL_CURRENT_QUERY
    current_query = GL_CURRENT_QUERY,
#endif
#ifdef GL_QUERY_COUNTER_BITS
    query_counter_bits = GL_QUERY_COUNTER_BITS,
#endif
}; // enum class query_parameter_name
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// OcclusionQueryParameterNameNV
enum class occlusion_query_parameter_name_nv : ::libc_types::u32 {
#ifdef GL_PIXEL_COUNT_AVAILABLE_NV
    pixel_count_available_nv = GL_PIXEL_COUNT_AVAILABLE_NV,
#endif
#ifdef GL_PIXEL_COUNT_NV
    pixel_count_nv = GL_PIXEL_COUNT_NV,
#endif
}; // enum class occlusion_query_parameter_name_nv
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ProgramFormat
enum class program_format : ::libc_types::u32 {
#ifdef GL_PROGRAM_FORMAT_ASCII_ARB
    ascii_arb = GL_PROGRAM_FORMAT_ASCII_ARB,
#endif
}; // enum class program_format
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// PixelDataRangeTargetNV
enum class pixel_data_range_target_nv : ::libc_types::u32 {
#ifdef GL_READ_PIXEL_DATA_RANGE_NV
    read_pixel_data_range_nv = GL_READ_PIXEL_DATA_RANGE_NV,
#endif
#ifdef GL_WRITE_PIXEL_DATA_RANGE_NV
    write_pixel_data_range_nv = GL_WRITE_PIXEL_DATA_RANGE_NV,
#endif
}; // enum class pixel_data_range_target_nv
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// CopyBufferSubDataTarget
enum class copy_buffer_sub_data_target : ::libc_types::u32 {
#ifdef GL_ARRAY_BUFFER
    array_buffer = GL_ARRAY_BUFFER,
#endif
#ifdef GL_ATOMIC_COUNTER_BUFFER
    atomic_counter_buffer = GL_ATOMIC_COUNTER_BUFFER,
#endif
#ifdef GL_COPY_READ_BUFFER
    copy_read_buffer = GL_COPY_READ_BUFFER,
#endif
#ifdef GL_COPY_WRITE_BUFFER
    copy_write_buffer = GL_COPY_WRITE_BUFFER,
#endif
#ifdef GL_DISPATCH_INDIRECT_BUFFER
    dispatch_indirect_buffer = GL_DISPATCH_INDIRECT_BUFFER,
#endif
#ifdef GL_DRAW_INDIRECT_BUFFER
    draw_indirect_buffer = GL_DRAW_INDIRECT_BUFFER,
#endif
#ifdef GL_ELEMENT_ARRAY_BUFFER
    element_array_buffer = GL_ELEMENT_ARRAY_BUFFER,
#endif
#ifdef GL_PIXEL_PACK_BUFFER
    pixel_pack_buffer = GL_PIXEL_PACK_BUFFER,
#endif
#ifdef GL_PIXEL_UNPACK_BUFFER
    pixel_unpack_buffer = GL_PIXEL_UNPACK_BUFFER,
#endif
#ifdef GL_QUERY_BUFFER
    query_buffer = GL_QUERY_BUFFER,
#endif
#ifdef GL_SHADER_STORAGE_BUFFER
    shader_storage_buffer = GL_SHADER_STORAGE_BUFFER,
#endif
#ifdef GL_TEXTURE_BUFFER
    texture_buffer = GL_TEXTURE_BUFFER,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BUFFER
    transform_feedback_buffer = GL_TRANSFORM_FEEDBACK_BUFFER,
#endif
#ifdef GL_UNIFORM_BUFFER
    uniform_buffer = GL_UNIFORM_BUFFER,
#endif
}; // enum class copy_buffer_sub_data_target
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// BufferStorageTarget
enum class buffer_storage_target : ::libc_types::u32 {
#ifdef GL_ARRAY_BUFFER
    array_buffer = GL_ARRAY_BUFFER,
#endif
#ifdef GL_ATOMIC_COUNTER_BUFFER
    atomic_counter_buffer = GL_ATOMIC_COUNTER_BUFFER,
#endif
#ifdef GL_COPY_READ_BUFFER
    copy_read_buffer = GL_COPY_READ_BUFFER,
#endif
#ifdef GL_COPY_WRITE_BUFFER
    copy_write_buffer = GL_COPY_WRITE_BUFFER,
#endif
#ifdef GL_DISPATCH_INDIRECT_BUFFER
    dispatch_indirect_buffer = GL_DISPATCH_INDIRECT_BUFFER,
#endif
#ifdef GL_DRAW_INDIRECT_BUFFER
    draw_indirect_buffer = GL_DRAW_INDIRECT_BUFFER,
#endif
#ifdef GL_ELEMENT_ARRAY_BUFFER
    element_array_buffer = GL_ELEMENT_ARRAY_BUFFER,
#endif
#ifdef GL_PIXEL_PACK_BUFFER
    pixel_pack_buffer = GL_PIXEL_PACK_BUFFER,
#endif
#ifdef GL_PIXEL_UNPACK_BUFFER
    pixel_unpack_buffer = GL_PIXEL_UNPACK_BUFFER,
#endif
#ifdef GL_QUERY_BUFFER
    query_buffer = GL_QUERY_BUFFER,
#endif
#ifdef GL_SHADER_STORAGE_BUFFER
    shader_storage_buffer = GL_SHADER_STORAGE_BUFFER,
#endif
#ifdef GL_TEXTURE_BUFFER
    texture_buffer = GL_TEXTURE_BUFFER,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BUFFER
    transform_feedback_buffer = GL_TRANSFORM_FEEDBACK_BUFFER,
#endif
#ifdef GL_UNIFORM_BUFFER
    uniform_buffer = GL_UNIFORM_BUFFER,
#endif
}; // enum class buffer_storage_target
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// BufferAccessARB
enum class buffer_access_arb : ::libc_types::u32 {
#ifdef GL_READ_ONLY
    read_only = GL_READ_ONLY,
#endif
#ifdef GL_READ_WRITE
    read_write = GL_READ_WRITE,
#endif
#ifdef GL_WRITE_ONLY
    write_only = GL_WRITE_ONLY,
#endif
}; // enum class buffer_access_arb
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// BufferPointerNameARB
enum class buffer_pointer_name_arb : ::libc_types::u32 {
#ifdef GL_BUFFER_MAP_POINTER
    buffer_map_pointer = GL_BUFFER_MAP_POINTER,
#endif
#ifdef GL_BUFFER_MAP_POINTER_ARB
    buffer_map_pointer_arb = GL_BUFFER_MAP_POINTER_ARB,
#endif
}; // enum class buffer_pointer_name_arb
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// VertexBufferObjectUsage
enum class vertex_buffer_object_usage : ::libc_types::u32 {
#ifdef GL_DYNAMIC_COPY
    dynamic_copy = GL_DYNAMIC_COPY,
#endif
#ifdef GL_DYNAMIC_DRAW
    dynamic_draw = GL_DYNAMIC_DRAW,
#endif
#ifdef GL_DYNAMIC_READ
    dynamic_read = GL_DYNAMIC_READ,
#endif
#ifdef GL_STATIC_COPY
    static_copy = GL_STATIC_COPY,
#endif
#ifdef GL_STATIC_DRAW
    static_draw = GL_STATIC_DRAW,
#endif
#ifdef GL_STATIC_READ
    static_read = GL_STATIC_READ,
#endif
#ifdef GL_STREAM_COPY
    stream_copy = GL_STREAM_COPY,
#endif
#ifdef GL_STREAM_DRAW
    stream_draw = GL_STREAM_DRAW,
#endif
#ifdef GL_STREAM_READ
    stream_read = GL_STREAM_READ,
#endif
}; // enum class vertex_buffer_object_usage
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// BufferUsageARB
enum class buffer_usage_arb : ::libc_types::u32 {
#ifdef GL_DYNAMIC_COPY
    dynamic_copy = GL_DYNAMIC_COPY,
#endif
#ifdef GL_DYNAMIC_DRAW
    dynamic_draw = GL_DYNAMIC_DRAW,
#endif
#ifdef GL_DYNAMIC_READ
    dynamic_read = GL_DYNAMIC_READ,
#endif
#ifdef GL_STATIC_COPY
    static_copy = GL_STATIC_COPY,
#endif
#ifdef GL_STATIC_DRAW
    static_draw = GL_STATIC_DRAW,
#endif
#ifdef GL_STATIC_READ
    static_read = GL_STATIC_READ,
#endif
#ifdef GL_STREAM_COPY
    stream_copy = GL_STREAM_COPY,
#endif
#ifdef GL_STREAM_DRAW
    stream_draw = GL_STREAM_DRAW,
#endif
#ifdef GL_STREAM_READ
    stream_read = GL_STREAM_READ,
#endif
}; // enum class buffer_usage_arb
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ClampColorTargetARB
enum class clamp_color_target_arb : ::libc_types::u32 {
#ifdef GL_CLAMP_FRAGMENT_COLOR_ARB
    clamp_fragment_color_arb = GL_CLAMP_FRAGMENT_COLOR_ARB,
#endif
#ifdef GL_CLAMP_READ_COLOR
    clamp_read_color = GL_CLAMP_READ_COLOR,
#endif
#ifdef GL_CLAMP_READ_COLOR_ARB
    clamp_read_color_arb = GL_CLAMP_READ_COLOR_ARB,
#endif
#ifdef GL_CLAMP_VERTEX_COLOR_ARB
    clamp_vertex_color_arb = GL_CLAMP_VERTEX_COLOR_ARB,
#endif
}; // enum class clamp_color_target_arb
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// UniformPName
enum class uniform_prop : ::libc_types::u32 {
#ifdef GL_UNIFORM_ARRAY_STRIDE
    uniform_array_stride = GL_UNIFORM_ARRAY_STRIDE,
#endif
#ifdef GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX
    uniform_atomic_counter_buffer_index = GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX,
#endif
#ifdef GL_UNIFORM_BLOCK_INDEX
    uniform_block_index = GL_UNIFORM_BLOCK_INDEX,
#endif
#ifdef GL_UNIFORM_IS_ROW_MAJOR
    uniform_is_row_major = GL_UNIFORM_IS_ROW_MAJOR,
#endif
#ifdef GL_UNIFORM_MATRIX_STRIDE
    uniform_matrix_stride = GL_UNIFORM_MATRIX_STRIDE,
#endif
#ifdef GL_UNIFORM_NAME_LENGTH
    uniform_name_length = GL_UNIFORM_NAME_LENGTH,
#endif
#ifdef GL_UNIFORM_OFFSET
    uniform_offset = GL_UNIFORM_OFFSET,
#endif
#ifdef GL_UNIFORM_SIZE
    uniform_size = GL_UNIFORM_SIZE,
#endif
#ifdef GL_UNIFORM_TYPE
    uniform_type = GL_UNIFORM_TYPE,
#endif
}; // enum class uniform_prop
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// SubroutineParameterName
enum class subroutine_parameter_name : ::libc_types::u32 {
#ifdef GL_COMPATIBLE_SUBROUTINES
    compatible_subroutines = GL_COMPATIBLE_SUBROUTINES,
#endif
#ifdef GL_NUM_COMPATIBLE_SUBROUTINES
    num_compatible_subroutines = GL_NUM_COMPATIBLE_SUBROUTINES,
#endif
#ifdef GL_UNIFORM_NAME_LENGTH
    uniform_name_length = GL_UNIFORM_NAME_LENGTH,
#endif
#ifdef GL_UNIFORM_SIZE
    uniform_size = GL_UNIFORM_SIZE,
#endif
}; // enum class subroutine_parameter_name
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ShaderType
enum class shader_type : ::libc_types::u32 {
#ifdef GL_COMPUTE_SHADER
    compute_shader = GL_COMPUTE_SHADER,
#endif
#ifdef GL_FRAGMENT_SHADER
    fragment_shader = GL_FRAGMENT_SHADER,
#endif
#ifdef GL_FRAGMENT_SHADER_ARB
    fragment_shader_arb = GL_FRAGMENT_SHADER_ARB,
#endif
#ifdef GL_GEOMETRY_SHADER
    geometry_shader = GL_GEOMETRY_SHADER,
#endif
#ifdef GL_TESS_CONTROL_SHADER
    tess_control_shader = GL_TESS_CONTROL_SHADER,
#endif
#ifdef GL_TESS_EVALUATION_SHADER
    tess_evaluation_shader = GL_TESS_EVALUATION_SHADER,
#endif
#ifdef GL_VERTEX_SHADER
    vertex_shader = GL_VERTEX_SHADER,
#endif
#ifdef GL_VERTEX_SHADER_ARB
    vertex_shader_arb = GL_VERTEX_SHADER_ARB,
#endif
}; // enum class shader_type
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ShaderParameterName
enum class shader_parameter_name : ::libc_types::u32 {
#ifdef GL_COMPILE_STATUS
    compile_status = GL_COMPILE_STATUS,
#endif
#ifdef GL_DELETE_STATUS
    delete_status = GL_DELETE_STATUS,
#endif
#ifdef GL_INFO_LOG_LENGTH
    info_log_length = GL_INFO_LOG_LENGTH,
#endif
#ifdef GL_SHADER_SOURCE_LENGTH
    shader_source_length = GL_SHADER_SOURCE_LENGTH,
#endif
#ifdef GL_SHADER_TYPE
    shader_type = GL_SHADER_TYPE,
#endif
}; // enum class shader_parameter_name
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ShaderBinaryFormat
enum class shader_binary_format : ::libc_types::u32 {
#ifdef GL_GCCSO_SHADER_BINARY_FJ
    gccso_shader_binary_fj = GL_GCCSO_SHADER_BINARY_FJ,
#endif
#ifdef GL_MALI_SHADER_BINARY_ARM
    mali_shader_binary_arm = GL_MALI_SHADER_BINARY_ARM,
#endif
#ifdef GL_SGX_BINARY_IMG
    sgx_binary_img = GL_SGX_BINARY_IMG,
#endif
#ifdef GL_SHADER_BINARY_DMP
    shader_binary_dmp = GL_SHADER_BINARY_DMP,
#endif
#ifdef GL_SHADER_BINARY_FORMAT_SPIR_V
    spir_v = GL_SHADER_BINARY_FORMAT_SPIR_V,
#endif
#ifdef GL_SHADER_BINARY_VIV
    shader_binary_viv = GL_SHADER_BINARY_VIV,
#endif
}; // enum class shader_binary_format
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// TransformFeedbackPName
enum class transform_feedback_prop : ::libc_types::u32 {
#ifdef GL_TRANSFORM_FEEDBACK_ACTIVE
    transform_feedback_active = GL_TRANSFORM_FEEDBACK_ACTIVE,
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
#ifdef GL_TRANSFORM_FEEDBACK_PAUSED
    transform_feedback_paused = GL_TRANSFORM_FEEDBACK_PAUSED,
#endif
}; // enum class transform_feedback_prop
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// TransformFeedbackBufferMode
enum class transform_feedback_buffer_mode : ::libc_types::u32 {
#ifdef GL_INTERLEAVED_ATTRIBS
    interleaved_attribs = GL_INTERLEAVED_ATTRIBS,
#endif
#ifdef GL_SEPARATE_ATTRIBS
    separate_attribs = GL_SEPARATE_ATTRIBS,
#endif
}; // enum class transform_feedback_buffer_mode
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ProgramInterface
enum class program_interface : ::libc_types::u32 {
#ifdef GL_BUFFER_VARIABLE
    buffer_variable = GL_BUFFER_VARIABLE,
#endif
#ifdef GL_COMPUTE_SUBROUTINE
    compute_subroutine = GL_COMPUTE_SUBROUTINE,
#endif
#ifdef GL_COMPUTE_SUBROUTINE_UNIFORM
    compute_subroutine_uniform = GL_COMPUTE_SUBROUTINE_UNIFORM,
#endif
#ifdef GL_FRAGMENT_SUBROUTINE
    fragment_subroutine = GL_FRAGMENT_SUBROUTINE,
#endif
#ifdef GL_FRAGMENT_SUBROUTINE_UNIFORM
    fragment_subroutine_uniform = GL_FRAGMENT_SUBROUTINE_UNIFORM,
#endif
#ifdef GL_GEOMETRY_SUBROUTINE
    geometry_subroutine = GL_GEOMETRY_SUBROUTINE,
#endif
#ifdef GL_GEOMETRY_SUBROUTINE_UNIFORM
    geometry_subroutine_uniform = GL_GEOMETRY_SUBROUTINE_UNIFORM,
#endif
#ifdef GL_PROGRAM_INPUT
    program_input = GL_PROGRAM_INPUT,
#endif
#ifdef GL_PROGRAM_OUTPUT
    program_output = GL_PROGRAM_OUTPUT,
#endif
#ifdef GL_SHADER_STORAGE_BLOCK
    shader_storage_block = GL_SHADER_STORAGE_BLOCK,
#endif
#ifdef GL_TESS_CONTROL_SUBROUTINE
    tess_control_subroutine = GL_TESS_CONTROL_SUBROUTINE,
#endif
#ifdef GL_TESS_CONTROL_SUBROUTINE_UNIFORM
    tess_control_subroutine_uniform = GL_TESS_CONTROL_SUBROUTINE_UNIFORM,
#endif
#ifdef GL_TESS_EVALUATION_SUBROUTINE
    tess_evaluation_subroutine = GL_TESS_EVALUATION_SUBROUTINE,
#endif
#ifdef GL_TESS_EVALUATION_SUBROUTINE_UNIFORM
    tess_evaluation_subroutine_uniform = GL_TESS_EVALUATION_SUBROUTINE_UNIFORM,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BUFFER
    transform_feedback_buffer = GL_TRANSFORM_FEEDBACK_BUFFER,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_VARYING
    transform_feedback_varying = GL_TRANSFORM_FEEDBACK_VARYING,
#endif
#ifdef GL_UNIFORM
    uniform = GL_UNIFORM,
#endif
#ifdef GL_UNIFORM_BLOCK
    uniform_block = GL_UNIFORM_BLOCK,
#endif
#ifdef GL_VERTEX_SUBROUTINE
    vertex_subroutine = GL_VERTEX_SUBROUTINE,
#endif
#ifdef GL_VERTEX_SUBROUTINE_UNIFORM
    vertex_subroutine_uniform = GL_VERTEX_SUBROUTINE_UNIFORM,
#endif
}; // enum class program_interface
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ClipControlOrigin
enum class clip_control_origin : ::libc_types::u32 {
#ifdef GL_LOWER_LEFT
    lower_left = GL_LOWER_LEFT,
#endif
#ifdef GL_UPPER_LEFT
    upper_left = GL_UPPER_LEFT,
#endif
}; // enum class clip_control_origin
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// FramebufferTarget
enum class framebuffer_target : ::libc_types::u32 {
#ifdef GL_DRAW_FRAMEBUFFER
    draw_framebuffer = GL_DRAW_FRAMEBUFFER,
#endif
#ifdef GL_FRAMEBUFFER
    framebuffer = GL_FRAMEBUFFER,
#endif
#ifdef GL_FRAMEBUFFER_OES
    framebuffer_oes = GL_FRAMEBUFFER_OES,
#endif
#ifdef GL_READ_FRAMEBUFFER
    read_framebuffer = GL_READ_FRAMEBUFFER,
#endif
}; // enum class framebuffer_target
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// RenderbufferParameterName
enum class renderbuffer_parameter_name : ::libc_types::u32 {
#ifdef GL_RENDERBUFFER_ALPHA_SIZE
    renderbuffer_alpha_size = GL_RENDERBUFFER_ALPHA_SIZE,
#endif
#ifdef GL_RENDERBUFFER_ALPHA_SIZE_EXT
    renderbuffer_alpha_size_ext = GL_RENDERBUFFER_ALPHA_SIZE_EXT,
#endif
#ifdef GL_RENDERBUFFER_ALPHA_SIZE_OES
    renderbuffer_alpha_size_oes = GL_RENDERBUFFER_ALPHA_SIZE_OES,
#endif
#ifdef GL_RENDERBUFFER_BLUE_SIZE
    renderbuffer_blue_size = GL_RENDERBUFFER_BLUE_SIZE,
#endif
#ifdef GL_RENDERBUFFER_BLUE_SIZE_EXT
    renderbuffer_blue_size_ext = GL_RENDERBUFFER_BLUE_SIZE_EXT,
#endif
#ifdef GL_RENDERBUFFER_BLUE_SIZE_OES
    renderbuffer_blue_size_oes = GL_RENDERBUFFER_BLUE_SIZE_OES,
#endif
#ifdef GL_RENDERBUFFER_COLOR_SAMPLES_NV
    renderbuffer_color_samples_nv = GL_RENDERBUFFER_COLOR_SAMPLES_NV,
#endif
#ifdef GL_RENDERBUFFER_COVERAGE_SAMPLES_NV
    renderbuffer_coverage_samples_nv = GL_RENDERBUFFER_COVERAGE_SAMPLES_NV,
#endif
#ifdef GL_RENDERBUFFER_DEPTH_SIZE
    renderbuffer_depth_size = GL_RENDERBUFFER_DEPTH_SIZE,
#endif
#ifdef GL_RENDERBUFFER_DEPTH_SIZE_EXT
    renderbuffer_depth_size_ext = GL_RENDERBUFFER_DEPTH_SIZE_EXT,
#endif
#ifdef GL_RENDERBUFFER_DEPTH_SIZE_OES
    renderbuffer_depth_size_oes = GL_RENDERBUFFER_DEPTH_SIZE_OES,
#endif
#ifdef GL_RENDERBUFFER_GREEN_SIZE
    renderbuffer_green_size = GL_RENDERBUFFER_GREEN_SIZE,
#endif
#ifdef GL_RENDERBUFFER_GREEN_SIZE_EXT
    renderbuffer_green_size_ext = GL_RENDERBUFFER_GREEN_SIZE_EXT,
#endif
#ifdef GL_RENDERBUFFER_GREEN_SIZE_OES
    renderbuffer_green_size_oes = GL_RENDERBUFFER_GREEN_SIZE_OES,
#endif
#ifdef GL_RENDERBUFFER_HEIGHT
    renderbuffer_height = GL_RENDERBUFFER_HEIGHT,
#endif
#ifdef GL_RENDERBUFFER_HEIGHT_EXT
    renderbuffer_height_ext = GL_RENDERBUFFER_HEIGHT_EXT,
#endif
#ifdef GL_RENDERBUFFER_HEIGHT_OES
    renderbuffer_height_oes = GL_RENDERBUFFER_HEIGHT_OES,
#endif
#ifdef GL_RENDERBUFFER_INTERNAL_FORMAT
    renderbuffer_internal_format = GL_RENDERBUFFER_INTERNAL_FORMAT,
#endif
#ifdef GL_RENDERBUFFER_INTERNAL_FORMAT_EXT
    renderbuffer_internal_format_ext = GL_RENDERBUFFER_INTERNAL_FORMAT_EXT,
#endif
#ifdef GL_RENDERBUFFER_INTERNAL_FORMAT_OES
    renderbuffer_internal_format_oes = GL_RENDERBUFFER_INTERNAL_FORMAT_OES,
#endif
#ifdef GL_RENDERBUFFER_RED_SIZE
    renderbuffer_red_size = GL_RENDERBUFFER_RED_SIZE,
#endif
#ifdef GL_RENDERBUFFER_RED_SIZE_EXT
    renderbuffer_red_size_ext = GL_RENDERBUFFER_RED_SIZE_EXT,
#endif
#ifdef GL_RENDERBUFFER_RED_SIZE_OES
    renderbuffer_red_size_oes = GL_RENDERBUFFER_RED_SIZE_OES,
#endif
#ifdef GL_RENDERBUFFER_SAMPLES
    renderbuffer_samples = GL_RENDERBUFFER_SAMPLES,
#endif
#ifdef GL_RENDERBUFFER_SAMPLES_ANGLE
    renderbuffer_samples_angle = GL_RENDERBUFFER_SAMPLES_ANGLE,
#endif
#ifdef GL_RENDERBUFFER_SAMPLES_APPLE
    renderbuffer_samples_apple = GL_RENDERBUFFER_SAMPLES_APPLE,
#endif
#ifdef GL_RENDERBUFFER_SAMPLES_EXT
    renderbuffer_samples_ext = GL_RENDERBUFFER_SAMPLES_EXT,
#endif
#ifdef GL_RENDERBUFFER_SAMPLES_IMG
    renderbuffer_samples_img = GL_RENDERBUFFER_SAMPLES_IMG,
#endif
#ifdef GL_RENDERBUFFER_SAMPLES_NV
    renderbuffer_samples_nv = GL_RENDERBUFFER_SAMPLES_NV,
#endif
#ifdef GL_RENDERBUFFER_STENCIL_SIZE
    renderbuffer_stencil_size = GL_RENDERBUFFER_STENCIL_SIZE,
#endif
#ifdef GL_RENDERBUFFER_STENCIL_SIZE_EXT
    renderbuffer_stencil_size_ext = GL_RENDERBUFFER_STENCIL_SIZE_EXT,
#endif
#ifdef GL_RENDERBUFFER_STENCIL_SIZE_OES
    renderbuffer_stencil_size_oes = GL_RENDERBUFFER_STENCIL_SIZE_OES,
#endif
#ifdef GL_RENDERBUFFER_STORAGE_SAMPLES_AMD
    renderbuffer_storage_samples_amd = GL_RENDERBUFFER_STORAGE_SAMPLES_AMD,
#endif
#ifdef GL_RENDERBUFFER_WIDTH
    renderbuffer_width = GL_RENDERBUFFER_WIDTH,
#endif
#ifdef GL_RENDERBUFFER_WIDTH_EXT
    renderbuffer_width_ext = GL_RENDERBUFFER_WIDTH_EXT,
#endif
#ifdef GL_RENDERBUFFER_WIDTH_OES
    renderbuffer_width_oes = GL_RENDERBUFFER_WIDTH_OES,
#endif
}; // enum class renderbuffer_parameter_name
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// FramebufferAttachment
enum class framebuffer_attachment : ::libc_types::u32 {
#ifdef GL_COLOR_ATTACHMENT0
    color_attachment0 = GL_COLOR_ATTACHMENT0,
#endif
#ifdef GL_COLOR_ATTACHMENT1
    color_attachment1 = GL_COLOR_ATTACHMENT1,
#endif
#ifdef GL_COLOR_ATTACHMENT10
    color_attachment10 = GL_COLOR_ATTACHMENT10,
#endif
#ifdef GL_COLOR_ATTACHMENT11
    color_attachment11 = GL_COLOR_ATTACHMENT11,
#endif
#ifdef GL_COLOR_ATTACHMENT12
    color_attachment12 = GL_COLOR_ATTACHMENT12,
#endif
#ifdef GL_COLOR_ATTACHMENT13
    color_attachment13 = GL_COLOR_ATTACHMENT13,
#endif
#ifdef GL_COLOR_ATTACHMENT14
    color_attachment14 = GL_COLOR_ATTACHMENT14,
#endif
#ifdef GL_COLOR_ATTACHMENT15
    color_attachment15 = GL_COLOR_ATTACHMENT15,
#endif
#ifdef GL_COLOR_ATTACHMENT16
    color_attachment16 = GL_COLOR_ATTACHMENT16,
#endif
#ifdef GL_COLOR_ATTACHMENT17
    color_attachment17 = GL_COLOR_ATTACHMENT17,
#endif
#ifdef GL_COLOR_ATTACHMENT18
    color_attachment18 = GL_COLOR_ATTACHMENT18,
#endif
#ifdef GL_COLOR_ATTACHMENT19
    color_attachment19 = GL_COLOR_ATTACHMENT19,
#endif
#ifdef GL_COLOR_ATTACHMENT2
    color_attachment2 = GL_COLOR_ATTACHMENT2,
#endif
#ifdef GL_COLOR_ATTACHMENT20
    color_attachment20 = GL_COLOR_ATTACHMENT20,
#endif
#ifdef GL_COLOR_ATTACHMENT21
    color_attachment21 = GL_COLOR_ATTACHMENT21,
#endif
#ifdef GL_COLOR_ATTACHMENT22
    color_attachment22 = GL_COLOR_ATTACHMENT22,
#endif
#ifdef GL_COLOR_ATTACHMENT23
    color_attachment23 = GL_COLOR_ATTACHMENT23,
#endif
#ifdef GL_COLOR_ATTACHMENT24
    color_attachment24 = GL_COLOR_ATTACHMENT24,
#endif
#ifdef GL_COLOR_ATTACHMENT25
    color_attachment25 = GL_COLOR_ATTACHMENT25,
#endif
#ifdef GL_COLOR_ATTACHMENT26
    color_attachment26 = GL_COLOR_ATTACHMENT26,
#endif
#ifdef GL_COLOR_ATTACHMENT27
    color_attachment27 = GL_COLOR_ATTACHMENT27,
#endif
#ifdef GL_COLOR_ATTACHMENT28
    color_attachment28 = GL_COLOR_ATTACHMENT28,
#endif
#ifdef GL_COLOR_ATTACHMENT29
    color_attachment29 = GL_COLOR_ATTACHMENT29,
#endif
#ifdef GL_COLOR_ATTACHMENT3
    color_attachment3 = GL_COLOR_ATTACHMENT3,
#endif
#ifdef GL_COLOR_ATTACHMENT30
    color_attachment30 = GL_COLOR_ATTACHMENT30,
#endif
#ifdef GL_COLOR_ATTACHMENT31
    color_attachment31 = GL_COLOR_ATTACHMENT31,
#endif
#ifdef GL_COLOR_ATTACHMENT4
    color_attachment4 = GL_COLOR_ATTACHMENT4,
#endif
#ifdef GL_COLOR_ATTACHMENT5
    color_attachment5 = GL_COLOR_ATTACHMENT5,
#endif
#ifdef GL_COLOR_ATTACHMENT6
    color_attachment6 = GL_COLOR_ATTACHMENT6,
#endif
#ifdef GL_COLOR_ATTACHMENT7
    color_attachment7 = GL_COLOR_ATTACHMENT7,
#endif
#ifdef GL_COLOR_ATTACHMENT8
    color_attachment8 = GL_COLOR_ATTACHMENT8,
#endif
#ifdef GL_COLOR_ATTACHMENT9
    color_attachment9 = GL_COLOR_ATTACHMENT9,
#endif
#ifdef GL_DEPTH_ATTACHMENT
    depth_attachment = GL_DEPTH_ATTACHMENT,
#endif
#ifdef GL_STENCIL_ATTACHMENT
    stencil_attachment = GL_STENCIL_ATTACHMENT,
#endif
}; // enum class framebuffer_attachment
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// RenderbufferTarget
enum class renderbuffer_target : ::libc_types::u32 {
#ifdef GL_RENDERBUFFER
    renderbuffer = GL_RENDERBUFFER,
#endif
#ifdef GL_RENDERBUFFER_OES
    renderbuffer_oes = GL_RENDERBUFFER_OES,
#endif
}; // enum class renderbuffer_target
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ProgramStagePName
enum class program_stage_prop : ::libc_types::u32 {
#ifdef GL_ACTIVE_SUBROUTINES
    active_subroutines = GL_ACTIVE_SUBROUTINES,
#endif
#ifdef GL_ACTIVE_SUBROUTINE_MAX_LENGTH
    active_subroutine_max_length = GL_ACTIVE_SUBROUTINE_MAX_LENGTH,
#endif
#ifdef GL_ACTIVE_SUBROUTINE_UNIFORMS
    active_subroutine_uniforms = GL_ACTIVE_SUBROUTINE_UNIFORMS,
#endif
#ifdef GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS
    active_subroutine_uniform_locations = GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS,
#endif
#ifdef GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH
    active_subroutine_uniform_max_length = GL_ACTIVE_SUBROUTINE_UNIFORM_MAX_LENGTH,
#endif
}; // enum class program_stage_prop
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// PrecisionType
enum class precision_type : ::libc_types::u32 {
#ifdef GL_HIGH_FLOAT
    high_float = GL_HIGH_FLOAT,
#endif
#ifdef GL_HIGH_INT
    high_int = GL_HIGH_INT,
#endif
#ifdef GL_LOW_FLOAT
    low_float = GL_LOW_FLOAT,
#endif
#ifdef GL_LOW_INT
    low_int = GL_LOW_INT,
#endif
#ifdef GL_MEDIUM_FLOAT
    medium_float = GL_MEDIUM_FLOAT,
#endif
#ifdef GL_MEDIUM_INT
    medium_int = GL_MEDIUM_INT,
#endif
}; // enum class precision_type
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ConditionalRenderMode
enum class conditional_render_mode : ::libc_types::u32 {
#ifdef GL_QUERY_BY_REGION_NO_WAIT
    query_by_region_no_wait = GL_QUERY_BY_REGION_NO_WAIT,
#endif
#ifdef GL_QUERY_BY_REGION_NO_WAIT_INVERTED
    query_by_region_no_wait_inverted = GL_QUERY_BY_REGION_NO_WAIT_INVERTED,
#endif
#ifdef GL_QUERY_BY_REGION_WAIT
    query_by_region_wait = GL_QUERY_BY_REGION_WAIT,
#endif
#ifdef GL_QUERY_BY_REGION_WAIT_INVERTED
    query_by_region_wait_inverted = GL_QUERY_BY_REGION_WAIT_INVERTED,
#endif
#ifdef GL_QUERY_NO_WAIT
    query_no_wait = GL_QUERY_NO_WAIT,
#endif
#ifdef GL_QUERY_NO_WAIT_INVERTED
    query_no_wait_inverted = GL_QUERY_NO_WAIT_INVERTED,
#endif
#ifdef GL_QUERY_WAIT
    query_wait = GL_QUERY_WAIT,
#endif
#ifdef GL_QUERY_WAIT_INVERTED
    query_wait_inverted = GL_QUERY_WAIT_INVERTED,
#endif
}; // enum class conditional_render_mode
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// BindTransformFeedbackTarget
enum class bind_transform_feedback_target : ::libc_types::u32 {
#ifdef GL_TRANSFORM_FEEDBACK
    transform_feedback = GL_TRANSFORM_FEEDBACK,
#endif
}; // enum class bind_transform_feedback_target
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// QueryCounterTarget
enum class query_counter_target : ::libc_types::u32 {
#ifdef GL_TIMESTAMP
    timestamp = GL_TIMESTAMP,
#endif
}; // enum class query_counter_target
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// VertexProvokingMode
enum class vertex_provoking_mode : ::libc_types::u32 {
#ifdef GL_FIRST_VERTEX_CONVENTION
    first_vertex_convention = GL_FIRST_VERTEX_CONVENTION,
#endif
#ifdef GL_LAST_VERTEX_CONVENTION
    last_vertex_convention = GL_LAST_VERTEX_CONVENTION,
#endif
}; // enum class vertex_provoking_mode
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// GetMultisamplePNameNV
enum class get_multisample_prop_nv : ::libc_types::u32 {
#ifdef GL_PROGRAMMABLE_SAMPLE_LOCATION_ARB
    programmable_sample_location_arb = GL_PROGRAMMABLE_SAMPLE_LOCATION_ARB,
#endif
#ifdef GL_SAMPLE_LOCATION_ARB
    sample_location_arb = GL_SAMPLE_LOCATION_ARB,
#endif
#ifdef GL_SAMPLE_POSITION
    sample_position = GL_SAMPLE_POSITION,
#endif
}; // enum class get_multisample_prop_nv
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// PatchParameterName
enum class patch_parameter_name : ::libc_types::u32 {
#ifdef GL_PATCH_DEFAULT_INNER_LEVEL
    patch_default_inner_level = GL_PATCH_DEFAULT_INNER_LEVEL,
#endif
#ifdef GL_PATCH_DEFAULT_OUTER_LEVEL
    patch_default_outer_level = GL_PATCH_DEFAULT_OUTER_LEVEL,
#endif
#ifdef GL_PATCH_VERTICES
    patch_vertices = GL_PATCH_VERTICES,
#endif
}; // enum class patch_parameter_name
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// PathStringFormat
enum class path_string_format : ::libc_types::u32 {
#ifdef GL_PATH_FORMAT_PS_NV
    path_format_ps_nv = GL_PATH_FORMAT_PS_NV,
#endif
#ifdef GL_PATH_FORMAT_SVG_NV
    path_format_svg_nv = GL_PATH_FORMAT_SVG_NV,
#endif
}; // enum class path_string_format
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// PathFontTarget
enum class path_font_target : ::libc_types::u32 {
#ifdef GL_FILE_NAME_NV
    file_name_nv = GL_FILE_NAME_NV,
#endif
#ifdef GL_STANDARD_FONT_NAME_NV
    standard_font_name_nv = GL_STANDARD_FONT_NAME_NV,
#endif
#ifdef GL_SYSTEM_FONT_NAME_NV
    system_font_name_nv = GL_SYSTEM_FONT_NAME_NV,
#endif
}; // enum class path_font_target
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// PathParameter
enum class path_parameter : ::libc_types::u32 {
#ifdef GL_PATH_CLIENT_LENGTH_NV
    path_client_length_nv = GL_PATH_CLIENT_LENGTH_NV,
#endif
#ifdef GL_PATH_COMMAND_COUNT_NV
    path_command_count_nv = GL_PATH_COMMAND_COUNT_NV,
#endif
#ifdef GL_PATH_COMPUTED_LENGTH_NV
    path_computed_length_nv = GL_PATH_COMPUTED_LENGTH_NV,
#endif
#ifdef GL_PATH_COORD_COUNT_NV
    path_coord_count_nv = GL_PATH_COORD_COUNT_NV,
#endif
#ifdef GL_PATH_DASH_ARRAY_COUNT_NV
    path_dash_array_count_nv = GL_PATH_DASH_ARRAY_COUNT_NV,
#endif
#ifdef GL_PATH_DASH_CAPS_NV
    path_dash_caps_nv = GL_PATH_DASH_CAPS_NV,
#endif
#ifdef GL_PATH_DASH_OFFSET_NV
    path_dash_offset_nv = GL_PATH_DASH_OFFSET_NV,
#endif
#ifdef GL_PATH_DASH_OFFSET_RESET_NV
    path_dash_offset_reset_nv = GL_PATH_DASH_OFFSET_RESET_NV,
#endif
#ifdef GL_PATH_END_CAPS_NV
    path_end_caps_nv = GL_PATH_END_CAPS_NV,
#endif
#ifdef GL_PATH_FILL_BOUNDING_BOX_NV
    path_fill_bounding_box_nv = GL_PATH_FILL_BOUNDING_BOX_NV,
#endif
#ifdef GL_PATH_FILL_COVER_MODE_NV
    path_fill_cover_mode_nv = GL_PATH_FILL_COVER_MODE_NV,
#endif
#ifdef GL_PATH_FILL_MASK_NV
    path_fill_mask_nv = GL_PATH_FILL_MASK_NV,
#endif
#ifdef GL_PATH_FILL_MODE_NV
    path_fill_mode_nv = GL_PATH_FILL_MODE_NV,
#endif
#ifdef GL_PATH_INITIAL_DASH_CAP_NV
    path_initial_dash_cap_nv = GL_PATH_INITIAL_DASH_CAP_NV,
#endif
#ifdef GL_PATH_INITIAL_END_CAP_NV
    path_initial_end_cap_nv = GL_PATH_INITIAL_END_CAP_NV,
#endif
#ifdef GL_PATH_JOIN_STYLE_NV
    path_join_style_nv = GL_PATH_JOIN_STYLE_NV,
#endif
#ifdef GL_PATH_MITER_LIMIT_NV
    path_miter_limit_nv = GL_PATH_MITER_LIMIT_NV,
#endif
#ifdef GL_PATH_OBJECT_BOUNDING_BOX_NV
    path_object_bounding_box_nv = GL_PATH_OBJECT_BOUNDING_BOX_NV,
#endif
#ifdef GL_PATH_STROKE_BOUNDING_BOX_NV
    path_stroke_bounding_box_nv = GL_PATH_STROKE_BOUNDING_BOX_NV,
#endif
#ifdef GL_PATH_STROKE_COVER_MODE_NV
    path_stroke_cover_mode_nv = GL_PATH_STROKE_COVER_MODE_NV,
#endif
#ifdef GL_PATH_STROKE_MASK_NV
    path_stroke_mask_nv = GL_PATH_STROKE_MASK_NV,
#endif
#ifdef GL_PATH_STROKE_WIDTH_NV
    path_stroke_width_nv = GL_PATH_STROKE_WIDTH_NV,
#endif
#ifdef GL_PATH_TERMINAL_DASH_CAP_NV
    path_terminal_dash_cap_nv = GL_PATH_TERMINAL_DASH_CAP_NV,
#endif
#ifdef GL_PATH_TERMINAL_END_CAP_NV
    path_terminal_end_cap_nv = GL_PATH_TERMINAL_END_CAP_NV,
#endif
}; // enum class path_parameter
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// PathCoverMode
enum class path_cover_mode : ::libc_types::u32 {
#ifdef GL_BOUNDING_BOX_NV
    bounding_box_nv = GL_BOUNDING_BOX_NV,
#endif
#ifdef GL_BOUNDING_BOX_OF_BOUNDING_BOXES_NV
    bounding_box_of_bounding_boxes_nv = GL_BOUNDING_BOX_OF_BOUNDING_BOXES_NV,
#endif
#ifdef GL_CONVEX_HULL_NV
    convex_hull_nv = GL_CONVEX_HULL_NV,
#endif
#ifdef GL_PATH_FILL_COVER_MODE_NV
    path_fill_cover_mode_nv = GL_PATH_FILL_COVER_MODE_NV,
#endif
}; // enum class path_cover_mode
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// PathElementType
enum class path_element_type : ::libc_types::u32 {
#ifdef GL_UTF16_NV
    utf16_nv = GL_UTF16_NV,
#endif
#ifdef GL_UTF8_NV
    utf8_nv = GL_UTF8_NV,
#endif
}; // enum class path_element_type
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// PathHandleMissingGlyphs
enum class path_handle_missing_glyphs : ::libc_types::u32 {
#ifdef GL_SKIP_MISSING_GLYPH_NV
    skip_missing_glyph_nv = GL_SKIP_MISSING_GLYPH_NV,
#endif
#ifdef GL_USE_MISSING_GLYPH_NV
    use_missing_glyph_nv = GL_USE_MISSING_GLYPH_NV,
#endif
}; // enum class path_handle_missing_glyphs
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// PathListMode
enum class path_list_mode : ::libc_types::u32 {
#ifdef GL_ACCUM_ADJACENT_PAIRS_NV
    accum_adjacent_pairs_nv = GL_ACCUM_ADJACENT_PAIRS_NV,
#endif
#ifdef GL_ADJACENT_PAIRS_NV
    adjacent_pairs_nv = GL_ADJACENT_PAIRS_NV,
#endif
#ifdef GL_FIRST_TO_REST_NV
    first_to_rest_nv = GL_FIRST_TO_REST_NV,
#endif
}; // enum class path_list_mode
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// AtomicCounterBufferPName
enum class atomic_counter_buffer_prop : ::libc_types::u32 {
#ifdef GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTERS
    atomic_counter_buffer_active_atomic_counters = GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTERS,
#endif
#ifdef GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTER_INDICES
    atomic_counter_buffer_active_atomic_counter_indices = GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTER_INDICES,
#endif
#ifdef GL_ATOMIC_COUNTER_BUFFER_BINDING
    atomic_counter_buffer_binding = GL_ATOMIC_COUNTER_BUFFER_BINDING,
#endif
#ifdef GL_ATOMIC_COUNTER_BUFFER_DATA_SIZE
    atomic_counter_buffer_data_size = GL_ATOMIC_COUNTER_BUFFER_DATA_SIZE,
#endif
#ifdef GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_COMPUTE_SHADER
    atomic_counter_buffer_referenced_by_compute_shader = GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_COMPUTE_SHADER,
#endif
#ifdef GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_FRAGMENT_SHADER
    atomic_counter_buffer_referenced_by_fragment_shader = GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_FRAGMENT_SHADER,
#endif
#ifdef GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_GEOMETRY_SHADER
    atomic_counter_buffer_referenced_by_geometry_shader = GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_GEOMETRY_SHADER,
#endif
#ifdef GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_CONTROL_SHADER
    atomic_counter_buffer_referenced_by_tess_control_shader = GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_CONTROL_SHADER,
#endif
#ifdef GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_EVALUATION_SHADER
    atomic_counter_buffer_referenced_by_tess_evaluation_shader = GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_EVALUATION_SHADER,
#endif
#ifdef GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_VERTEX_SHADER
    atomic_counter_buffer_referenced_by_vertex_shader = GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_VERTEX_SHADER,
#endif
}; // enum class atomic_counter_buffer_prop
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// SyncParameterName
enum class sync_parameter_name : ::libc_types::u32 {
#ifdef GL_OBJECT_TYPE
    object_type = GL_OBJECT_TYPE,
#endif
#ifdef GL_SYNC_CONDITION
    sync_condition = GL_SYNC_CONDITION,
#endif
#ifdef GL_SYNC_FLAGS
    sync_flags = GL_SYNC_FLAGS,
#endif
#ifdef GL_SYNC_STATUS
    sync_status = GL_SYNC_STATUS,
#endif
}; // enum class sync_parameter_name
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// SyncCondition
enum class sync_condition : ::libc_types::u32 {
#ifdef GL_SYNC_GPU_COMMANDS_COMPLETE
    sync_gpu_commands_complete = GL_SYNC_GPU_COMMANDS_COMPLETE,
#endif
}; // enum class sync_condition
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ProgramInterfacePName
enum class program_interface_prop : ::libc_types::u32 {
#ifdef GL_ACTIVE_RESOURCES
    active_resources = GL_ACTIVE_RESOURCES,
#endif
#ifdef GL_MAX_NAME_LENGTH
    max_name_length = GL_MAX_NAME_LENGTH,
#endif
#ifdef GL_MAX_NUM_ACTIVE_VARIABLES
    max_num_active_variables = GL_MAX_NUM_ACTIVE_VARIABLES,
#endif
#ifdef GL_MAX_NUM_COMPATIBLE_SUBROUTINES
    max_num_compatible_subroutines = GL_MAX_NUM_COMPATIBLE_SUBROUTINES,
#endif
}; // enum class program_interface_prop
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// FramebufferParameterName
enum class framebuffer_parameter_name : ::libc_types::u32 {
#ifdef GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS
    framebuffer_default_fixed_sample_locations = GL_FRAMEBUFFER_DEFAULT_FIXED_SAMPLE_LOCATIONS,
#endif
#ifdef GL_FRAMEBUFFER_DEFAULT_HEIGHT
    framebuffer_default_height = GL_FRAMEBUFFER_DEFAULT_HEIGHT,
#endif
#ifdef GL_FRAMEBUFFER_DEFAULT_LAYERS
    framebuffer_default_layers = GL_FRAMEBUFFER_DEFAULT_LAYERS,
#endif
#ifdef GL_FRAMEBUFFER_DEFAULT_SAMPLES
    framebuffer_default_samples = GL_FRAMEBUFFER_DEFAULT_SAMPLES,
#endif
#ifdef GL_FRAMEBUFFER_DEFAULT_WIDTH
    framebuffer_default_width = GL_FRAMEBUFFER_DEFAULT_WIDTH,
#endif
}; // enum class framebuffer_parameter_name
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ClipControlDepth
enum class clip_control_depth : ::libc_types::u32 {
#ifdef GL_NEGATIVE_ONE_TO_ONE
    negative_one_to_one = GL_NEGATIVE_ONE_TO_ONE,
#endif
#ifdef GL_ZERO_TO_ONE
    zero_to_one = GL_ZERO_TO_ONE,
#endif
}; // enum class clip_control_depth
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// SemaphoreParameterName
enum class semaphore_parameter_name : ::libc_types::u32 {
#ifdef GL_D3D12_FENCE_VALUE_EXT
    d3d12_fence_value_ext = GL_D3D12_FENCE_VALUE_EXT,
#endif
#ifdef GL_SEMAPHORE_TYPE_BINARY_NV
    semaphore_type_binary_nv = GL_SEMAPHORE_TYPE_BINARY_NV,
#endif
#ifdef GL_SEMAPHORE_TYPE_NV
    semaphore_type_nv = GL_SEMAPHORE_TYPE_NV,
#endif
#ifdef GL_SEMAPHORE_TYPE_TIMELINE_NV
    semaphore_type_timeline_nv = GL_SEMAPHORE_TYPE_TIMELINE_NV,
#endif
#ifdef GL_TIMELINE_SEMAPHORE_VALUE_NV
    timeline_semaphore_value_nv = GL_TIMELINE_SEMAPHORE_VALUE_NV,
#endif
}; // enum class semaphore_parameter_name
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {

// ShadingRateQCOM
enum class shading_rate_qcom : ::libc_types::u32 {
#ifdef GL_SHADING_RATE_1X1_PIXELS_QCOM
    shading_rate_1x1_pixels_qcom = GL_SHADING_RATE_1X1_PIXELS_QCOM,
#endif
#ifdef GL_SHADING_RATE_1X2_PIXELS_QCOM
    shading_rate_1x2_pixels_qcom = GL_SHADING_RATE_1X2_PIXELS_QCOM,
#endif
#ifdef GL_SHADING_RATE_1X4_PIXELS_QCOM
    shading_rate_1x4_pixels_qcom = GL_SHADING_RATE_1X4_PIXELS_QCOM,
#endif
#ifdef GL_SHADING_RATE_2X1_PIXELS_QCOM
    shading_rate_2x1_pixels_qcom = GL_SHADING_RATE_2X1_PIXELS_QCOM,
#endif
#ifdef GL_SHADING_RATE_2X2_PIXELS_QCOM
    shading_rate_2x2_pixels_qcom = GL_SHADING_RATE_2X2_PIXELS_QCOM,
#endif
#ifdef GL_SHADING_RATE_2X4_PIXELS_QCOM
    shading_rate_2x4_pixels_qcom = GL_SHADING_RATE_2X4_PIXELS_QCOM,
#endif
#ifdef GL_SHADING_RATE_4X1_PIXELS_QCOM
    shading_rate_4x1_pixels_qcom = GL_SHADING_RATE_4X1_PIXELS_QCOM,
#endif
#ifdef GL_SHADING_RATE_4X2_PIXELS_QCOM
    shading_rate_4x2_pixels_qcom = GL_SHADING_RATE_4X2_PIXELS_QCOM,
#endif
#ifdef GL_SHADING_RATE_4X4_PIXELS_QCOM
    shading_rate_4x4_pixels_qcom = GL_SHADING_RATE_4X4_PIXELS_QCOM,
#endif
}; // enum class shading_rate_qcom
} // namespace gl::groups
#pragma once

#include "common.h"

namespace gl::groups {
} // namespace gl::groups
