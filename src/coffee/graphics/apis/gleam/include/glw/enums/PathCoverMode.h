#pragma once

#include "common.h"

namespace gl::group {

// PathCoverMode
enum class path_cover_mode : ::libc_types::u32
{
#ifdef GL_BOUNDING_BOX_NV
    bounding_box_nv = GL_BOUNDING_BOX_NV,
#endif
#ifdef GL_BOUNDING_BOX_OF_BOUNDING_BOXES_NV
    bounding_box_of_bounding_boxes_nv = GL_BOUNDING_BOX_OF_BOUNDING_BOXES_NV,
#endif
#ifdef GL_CONVEX_HULL_NV
    convex_hull_nv = GL_CONVEX_HULL_NV,
#endif
#ifdef GL_PATH_FILL_COVER_MODE_NV
    path_fill_cover_mode_nv = GL_PATH_FILL_COVER_MODE_NV,
#endif
}; // enum class path_cover_mode

} // namespace gl::group
