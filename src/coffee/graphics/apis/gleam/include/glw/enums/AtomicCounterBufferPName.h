#pragma once

#include "common.h"

namespace gl::group {

// AtomicCounterBufferPName
enum class atomic_counter_buffer_prop : u32
{
    atomic_counter_buffer_active_atomic_counters =
        0x92C5, // GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTERS
    atomic_counter_buffer_active_atomic_counter_indices =
        0x92C6, // GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTER_INDICES
    atomic_counter_buffer_binding = 0x92C1, // GL_ATOMIC_COUNTER_BUFFER_BINDING
    atomic_counter_buffer_data_size =
        0x92C4, // GL_ATOMIC_COUNTER_BUFFER_DATA_SIZE
    atomic_counter_buffer_referenced_by_compute_shader =
        0x90ED, // GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_COMPUTE_SHADER
    atomic_counter_buffer_referenced_by_fragment_shader =
        0x92CB, // GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_FRAGMENT_SHADER
    atomic_counter_buffer_referenced_by_geometry_shader =
        0x92CA, // GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_GEOMETRY_SHADER
    atomic_counter_buffer_referenced_by_tess_control_shader =
        0x92C8, // GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_CONTROL_SHADER
    atomic_counter_buffer_referenced_by_tess_evaluation_shader =
        0x92C9, // GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_EVALUATION_SHADER
    atomic_counter_buffer_referenced_by_vertex_shader =
        0x92C7, // GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_VERTEX_SHADER
}; // enum class atomic_counter_buffer_prop

} // namespace gl::group
