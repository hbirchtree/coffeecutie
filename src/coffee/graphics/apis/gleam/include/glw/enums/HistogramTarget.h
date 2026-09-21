#pragma once

#include "common.h"

namespace gl::group {

// HistogramTarget
enum class histogram_target : u32
{
    histogram       = 0x8024, // GL_HISTOGRAM
    proxy_histogram = 0x8025, // GL_PROXY_HISTOGRAM
}; // enum class histogram_target

} // namespace gl::group
