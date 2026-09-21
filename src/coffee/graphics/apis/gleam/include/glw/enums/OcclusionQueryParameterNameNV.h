#pragma once

#include "common.h"

namespace gl::group {

// OcclusionQueryParameterNameNV
enum class occlusion_query_parameter_name_nv : u32
{
    pixel_count_available_nv = 0x8867, // GL_PIXEL_COUNT_AVAILABLE_NV
    pixel_count_nv           = 0x8866, // GL_PIXEL_COUNT_NV
}; // enum class occlusion_query_parameter_name_nv

} // namespace gl::group
