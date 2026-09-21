#pragma once

#include "common.h"

namespace gl::group {

// SemaphoreParameterName
enum class semaphore_parameter_name : u32
{
    d3d12_fence_value_ext       = 0x9595, // GL_D3D12_FENCE_VALUE_EXT
    semaphore_type_binary_nv    = 0x95B4, // GL_SEMAPHORE_TYPE_BINARY_NV
    semaphore_type_nv           = 0x95B3, // GL_SEMAPHORE_TYPE_NV
    semaphore_type_timeline_nv  = 0x95B5, // GL_SEMAPHORE_TYPE_TIMELINE_NV
    timeline_semaphore_value_nv = 0x9595, // GL_TIMELINE_SEMAPHORE_VALUE_NV
}; // enum class semaphore_parameter_name

} // namespace gl::group
