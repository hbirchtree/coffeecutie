#pragma once

#include "common.h"

namespace gl::group {

// HistogramTargetEXT
enum class histogram_target_ext : u32
{
    histogram           = 0x8024, // GL_HISTOGRAM
    proxy_histogram     = 0x8025, // GL_PROXY_HISTOGRAM
    histogram_ext       = 0x8024, // GL_HISTOGRAM_EXT
    proxy_histogram_ext = 0x8025, // GL_PROXY_HISTOGRAM_EXT
}; // enum class histogram_target_ext

} // namespace gl::group
