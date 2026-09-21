#pragma once

#include "common.h"

namespace gl::group {

// InstancedPathCoverMode
enum class instanced_path_cover_mode : u32
{
    bounding_box_nv = 0x908D, // GL_BOUNDING_BOX_NV
    bounding_box_of_bounding_boxes_nv =
        0x909C,                       // GL_BOUNDING_BOX_OF_BOUNDING_BOXES_NV
    convex_hull_nv          = 0x908B, // GL_CONVEX_HULL_NV
    path_fill_cover_mode_nv = 0x9082, // GL_PATH_FILL_COVER_MODE_NV
}; // enum class instanced_path_cover_mode

} // namespace gl::group
