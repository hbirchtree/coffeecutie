#pragma once

#include "common.h"

namespace gl::group {

// EvalMapsModeNV
enum class eval_maps_mode_nv : u32
{
#ifdef GL_FILL_NV
    fill_nv = GL_FILL_NV,
#endif
}; // enum class eval_maps_mode_nv

} // namespace gl::group
