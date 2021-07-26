#pragma once

#ifdef GL_ARB_explicit_uniform_location
#include "../enums/GetPName.h"
namespace gl::arb::explicit_uniform_location {
using gl::group::get_prop;
namespace values {
} // namespace values
} // namespace gl::arb::explicit_uniform_location
#endif // GL_ARB_explicit_uniform_location
namespace gl::arb::explicit_uniform_location {
constexpr auto name = "GL_ARB_explicit_uniform_location";
}
