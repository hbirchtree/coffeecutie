#pragma once

#include "common.h"

namespace gl::groups {

// HistogramTargetEXT
enum class histogram_target_ext : ::libc_types::u32 {
#ifdef GL_HISTOGRAM
    histogram = GL_HISTOGRAM,
#endif
#ifdef GL_HISTOGRAM_EXT
    histogram_ext = GL_HISTOGRAM_EXT,
#endif
#ifdef GL_PROXY_HISTOGRAM
    proxy_histogram = GL_PROXY_HISTOGRAM,
#endif
#ifdef GL_PROXY_HISTOGRAM_EXT
    proxy_histogram_ext = GL_PROXY_HISTOGRAM_EXT,
#endif
}; // enum class histogram_target_ext

} // namespace gl::groups
