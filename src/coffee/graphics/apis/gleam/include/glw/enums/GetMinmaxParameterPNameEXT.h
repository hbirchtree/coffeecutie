#pragma once

#include "common.h"

namespace gl::groups {

// GetMinmaxParameterPNameEXT
enum class get_minmax_parameter_prop_ext : ::libc_types::u32 {
#ifdef GL_MINMAX_FORMAT
    minmax_format = GL_MINMAX_FORMAT,
#endif
#ifdef GL_MINMAX_FORMAT_EXT
    minmax_format_ext = GL_MINMAX_FORMAT_EXT,
#endif
#ifdef GL_MINMAX_SINK
    minmax_sink = GL_MINMAX_SINK,
#endif
#ifdef GL_MINMAX_SINK_EXT
    minmax_sink_ext = GL_MINMAX_SINK_EXT,
#endif
}; // enum class get_minmax_parameter_prop_ext

} // namespace gl::groups
