#pragma once

#include "common.h"

namespace gl::group {

// OcclusionQueryParameterNameNV
enum class occlusion_query_parameter_name_nv : u32
{
#ifdef GL_PIXEL_COUNT_AVAILABLE_NV
    pixel_count_available_nv = GL_PIXEL_COUNT_AVAILABLE_NV,
#endif
#ifdef GL_PIXEL_COUNT_NV
    pixel_count_nv = GL_PIXEL_COUNT_NV,
#endif
}; // enum class occlusion_query_parameter_name_nv

} // namespace gl::group
