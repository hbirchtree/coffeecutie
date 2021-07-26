#pragma once

#ifdef GL_ARB_stencil_texturing
#include "../enums/TextureParameterName.h"
namespace gl::arb::stencil_texturing {
using gl::group::texture_parameter_name;
namespace values {
} // namespace values
} // namespace gl::arb::stencil_texturing
#endif // GL_ARB_stencil_texturing
namespace gl::arb::stencil_texturing {
constexpr auto name = "GL_ARB_stencil_texturing";
}
