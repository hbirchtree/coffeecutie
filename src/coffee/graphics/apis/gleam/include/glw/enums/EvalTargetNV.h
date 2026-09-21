#pragma once

#include "common.h"

namespace gl::group {

// EvalTargetNV
enum class eval_target_nv : u32
{
    eval_2d_nv            = 0x86C0, // GL_EVAL_2D_NV
    eval_triangular_2d_nv = 0x86C1, // GL_EVAL_TRIANGULAR_2D_NV
}; // enum class eval_target_nv

} // namespace gl::group
