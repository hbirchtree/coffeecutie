#pragma once

#include "common.h"

namespace gl::group {

// PathListMode
enum class path_list_mode : ::libc_types::u32
{
#ifdef GL_ACCUM_ADJACENT_PAIRS_NV
    accum_adjacent_pairs_nv = GL_ACCUM_ADJACENT_PAIRS_NV,
#endif
#ifdef GL_ADJACENT_PAIRS_NV
    adjacent_pairs_nv = GL_ADJACENT_PAIRS_NV,
#endif
#ifdef GL_FIRST_TO_REST_NV
    first_to_rest_nv = GL_FIRST_TO_REST_NV,
#endif
}; // enum class path_list_mode

} // namespace gl::group
