#pragma once

#include "common.h"

namespace gl::group {

// PixelDataRangeTargetNV
enum class pixel_data_range_target_nv : u32
{
    read_pixel_data_range_nv  = 0x8879, // GL_READ_PIXEL_DATA_RANGE_NV
    write_pixel_data_range_nv = 0x8878, // GL_WRITE_PIXEL_DATA_RANGE_NV
}; // enum class pixel_data_range_target_nv

} // namespace gl::group
