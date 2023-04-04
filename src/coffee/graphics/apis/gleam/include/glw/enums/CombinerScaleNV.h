#pragma once

#include "common.h"

namespace gl::group {

// CombinerScaleNV
enum class combiner_scale_nv : u32
{
#ifdef GL_NONE
    none = GL_NONE,
#endif
#ifdef GL_SCALE_BY_FOUR_NV
    scale_by_four_nv = GL_SCALE_BY_FOUR_NV,
#endif
#ifdef GL_SCALE_BY_ONE_HALF_NV
    scale_by_one_half_nv = GL_SCALE_BY_ONE_HALF_NV,
#endif
#ifdef GL_SCALE_BY_TWO_NV
    scale_by_two_nv = GL_SCALE_BY_TWO_NV,
#endif
}; // enum class combiner_scale_nv

} // namespace gl::group
