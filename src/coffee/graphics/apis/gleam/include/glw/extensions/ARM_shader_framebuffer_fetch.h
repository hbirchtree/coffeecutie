#pragma once

#ifdef GL_ARM_shader_framebuffer_fetch
#include "../enums/EnableCap.h"
#include "../enums/GetPName.h"
namespace gl::arm::shader_framebuffer_fetch {
using gl::group::enable_cap;
using gl::group::get_prop;
namespace values {
} // namespace values
} // namespace gl::arm::shader_framebuffer_fetch
#endif // GL_ARM_shader_framebuffer_fetch
namespace gl::arm::shader_framebuffer_fetch {
constexpr auto name = "GL_ARM_shader_framebuffer_fetch";
}
