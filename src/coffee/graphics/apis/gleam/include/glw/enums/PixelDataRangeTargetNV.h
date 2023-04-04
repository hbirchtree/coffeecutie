#pragma once

#include "common.h"

namespace gl::group {

// PixelDataRangeTargetNV
enum class pixel_data_range_target_nv : u32
{
#ifdef GL_READ_PIXEL_DATA_RANGE_NV
    read_pixel_data_range_nv = GL_READ_PIXEL_DATA_RANGE_NV,
#endif
#ifdef GL_WRITE_PIXEL_DATA_RANGE_NV
    write_pixel_data_range_nv = GL_WRITE_PIXEL_DATA_RANGE_NV,
#endif
}; // enum class pixel_data_range_target_nv

} // namespace gl::group
