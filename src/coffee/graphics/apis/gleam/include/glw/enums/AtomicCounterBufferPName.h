#pragma once

#include "common.h"

namespace gl::group {

// AtomicCounterBufferPName
enum class atomic_counter_buffer_prop : ::libc_types::u32
{
#ifdef GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTERS
    atomic_counter_buffer_active_atomic_counters =
        GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTERS,
#endif
#ifdef GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTER_INDICES
    atomic_counter_buffer_active_atomic_counter_indices =
        GL_ATOMIC_COUNTER_BUFFER_ACTIVE_ATOMIC_COUNTER_INDICES,
#endif
#ifdef GL_ATOMIC_COUNTER_BUFFER_BINDING
    atomic_counter_buffer_binding = GL_ATOMIC_COUNTER_BUFFER_BINDING,
#endif
#ifdef GL_ATOMIC_COUNTER_BUFFER_DATA_SIZE
    atomic_counter_buffer_data_size = GL_ATOMIC_COUNTER_BUFFER_DATA_SIZE,
#endif
#ifdef GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_COMPUTE_SHADER
    atomic_counter_buffer_referenced_by_compute_shader =
        GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_COMPUTE_SHADER,
#endif
#ifdef GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_FRAGMENT_SHADER
    atomic_counter_buffer_referenced_by_fragment_shader =
        GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_FRAGMENT_SHADER,
#endif
#ifdef GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_GEOMETRY_SHADER
    atomic_counter_buffer_referenced_by_geometry_shader =
        GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_GEOMETRY_SHADER,
#endif
#ifdef GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_CONTROL_SHADER
    atomic_counter_buffer_referenced_by_tess_control_shader =
        GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_CONTROL_SHADER,
#endif
#ifdef GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_EVALUATION_SHADER
    atomic_counter_buffer_referenced_by_tess_evaluation_shader =
        GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TESS_EVALUATION_SHADER,
#endif
#ifdef GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_VERTEX_SHADER
    atomic_counter_buffer_referenced_by_vertex_shader =
        GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_VERTEX_SHADER,
#endif
}; // enum class atomic_counter_buffer_prop

} // namespace gl::group
