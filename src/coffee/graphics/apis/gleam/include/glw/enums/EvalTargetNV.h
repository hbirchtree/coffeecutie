#pragma once

#include "common.h"

namespace gl::group {

// EvalTargetNV
enum class eval_target_nv : u32
{
#ifdef GL_EVAL_2D_NV
    eval_2d_nv = GL_EVAL_2D_NV,
#endif
#ifdef GL_EVAL_TRIANGULAR_2D_NV
    eval_triangular_2d_nv = GL_EVAL_TRIANGULAR_2D_NV,
#endif
}; // enum class eval_target_nv

} // namespace gl::group
