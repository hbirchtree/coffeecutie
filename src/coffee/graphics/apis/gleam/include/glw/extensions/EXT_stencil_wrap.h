#pragma once

#ifdef GL_EXT_stencil_wrap
namespace gl::ext::stencil_wrap {
namespace values {
constexpr u32 incr_wrap = 0x8507;
constexpr u32 decr_wrap = 0x8508;
} // namespace values
} // namespace gl::ext::stencil_wrap
#endif // GL_EXT_stencil_wrap
namespace gl::ext::stencil_wrap {
constexpr auto name = "GL_EXT_stencil_wrap";
}
