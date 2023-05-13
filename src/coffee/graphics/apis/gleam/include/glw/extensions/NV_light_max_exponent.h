#pragma once

#ifdef GL_NV_light_max_exponent
namespace gl::nv::light_max_exponent {
namespace values {
constexpr u32 max_shininess     = 0x8504;
constexpr u32 max_spot_exponent = 0x8505;
} // namespace values
} // namespace gl::nv::light_max_exponent
#endif // GL_NV_light_max_exponent
namespace gl::nv::light_max_exponent {
constexpr auto name = "GL_NV_light_max_exponent";
}
