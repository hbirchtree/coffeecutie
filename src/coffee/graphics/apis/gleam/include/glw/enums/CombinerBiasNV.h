#pragma once

#include "common.h"

namespace gl::group {

// CombinerBiasNV
enum class combiner_bias_nv : u32
{
    none                         = 0,      // GL_NONE
    bias_by_negative_one_half_nv = 0x8541, // GL_BIAS_BY_NEGATIVE_ONE_HALF_NV
}; // enum class combiner_bias_nv

} // namespace gl::group
