#pragma once

#include "common.h"

namespace gl::group {

// PathListMode
enum class path_list_mode : u32
{
    accum_adjacent_pairs_nv = 0x90AD, // GL_ACCUM_ADJACENT_PAIRS_NV
    adjacent_pairs_nv       = 0x90AE, // GL_ADJACENT_PAIRS_NV
    first_to_rest_nv        = 0x90AF, // GL_FIRST_TO_REST_NV
}; // enum class path_list_mode

} // namespace gl::group
