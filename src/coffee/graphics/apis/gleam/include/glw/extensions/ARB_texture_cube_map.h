#pragma once

#ifdef GL_ARB_texture_cube_map
#include "../enums/EnableCap.h"
#include "../enums/GetPName.h"
#include "../enums/GetTextureParameter.h"
#include "../enums/TextureTarget.h"
namespace gl::arb::texture_cube_map {
using gl::group::enable_cap;
using gl::group::get_prop;
using gl::group::get_texture_parameter;
using gl::group::texture_target;
namespace values {
} // namespace values
} // namespace gl::arb::texture_cube_map
#endif // GL_ARB_texture_cube_map
namespace gl::arb::texture_cube_map {
constexpr auto name = "GL_ARB_texture_cube_map";
}
