#pragma once

#include "common.h"

namespace gl::group {

// CombinerScaleNV
enum class combiner_scale_nv : u32
{
    none                 = 0,      // GL_NONE
    scale_by_four_nv     = 0x853F, // GL_SCALE_BY_FOUR_NV
    scale_by_one_half_nv = 0x8540, // GL_SCALE_BY_ONE_HALF_NV
    scale_by_two_nv      = 0x853E, // GL_SCALE_BY_TWO_NV
}; // enum class combiner_scale_nv

} // namespace gl::group
