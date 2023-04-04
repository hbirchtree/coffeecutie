#pragma once

#include "common.h"

namespace gl::group {

// SemaphoreParameterName
enum class semaphore_parameter_name : u32
{
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

} // namespace gl::group
