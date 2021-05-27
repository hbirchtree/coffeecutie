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
