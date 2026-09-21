#pragma once

#include "common.h"

namespace gl::group {

// GetMinmaxParameterPNameEXT
enum class get_minmax_parameter_prop_ext : u32
{
    minmax_format     = 0x802F, // GL_MINMAX_FORMAT
    minmax_sink       = 0x8030, // GL_MINMAX_SINK
    minmax_format_ext = 0x802F, // GL_MINMAX_FORMAT_EXT
    minmax_sink_ext   = 0x8030, // GL_MINMAX_SINK_EXT
}; // enum class get_minmax_parameter_prop_ext

} // namespace gl::group
