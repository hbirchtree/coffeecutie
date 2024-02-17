#pragma once

#ifdef GL_ARB_map_buffer_alignment
#include "../enums/GetPName.h"

namespace gl::arb::map_buffer_alignment {
using gl::group::get_prop;

namespace values {
} // namespace values
} // namespace gl::arb::map_buffer_alignment
#endif // GL_ARB_map_buffer_alignment
namespace gl::arb::map_buffer_alignment {
constexpr auto name = "GL_ARB_map_buffer_alignment";
}
