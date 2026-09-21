#pragma once

#include "common.h"

namespace gl::group {

// FenceParameterNameNV
enum class fence_parameter_name_nv : u32
{
    fence_condition_nv = 0x84F4, // GL_FENCE_CONDITION_NV
    fence_status_nv    = 0x84F3, // GL_FENCE_STATUS_NV
}; // enum class fence_parameter_name_nv

} // namespace gl::group
