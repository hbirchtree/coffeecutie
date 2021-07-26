#pragma once

#include "common.h"

namespace gl::group {

// FenceParameterNameNV
enum class fence_parameter_name_nv : ::libc_types::u32
{
#ifdef GL_FENCE_CONDITION_NV
    fence_condition_nv = GL_FENCE_CONDITION_NV,
#endif
#ifdef GL_FENCE_STATUS_NV
    fence_status_nv = GL_FENCE_STATUS_NV,
#endif
}; // enum class fence_parameter_name_nv

} // namespace gl::group
