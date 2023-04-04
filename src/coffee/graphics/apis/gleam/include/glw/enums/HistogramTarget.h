#pragma once

#include "common.h"

namespace gl::group {

// HistogramTarget
enum class histogram_target : u32
{
#ifdef GL_HISTOGRAM
    histogram = GL_HISTOGRAM,
#endif
#ifdef GL_PROXY_HISTOGRAM
    proxy_histogram = GL_PROXY_HISTOGRAM,
#endif
}; // enum class histogram_target

} // namespace gl::group
