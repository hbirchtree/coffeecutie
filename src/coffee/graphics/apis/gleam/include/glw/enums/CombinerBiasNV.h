#pragma once

#include "common.h"

namespace gl::group {

// CombinerBiasNV
enum class combiner_bias_nv : ::libc_types::u32
{
#ifdef GL_BIAS_BY_NEGATIVE_ONE_HALF_NV
    bias_by_negative_one_half_nv = GL_BIAS_BY_NEGATIVE_ONE_HALF_NV,
#endif
#ifdef GL_NONE
    none = GL_NONE,
#endif
}; // enum class combiner_bias_nv

} // namespace gl::group
