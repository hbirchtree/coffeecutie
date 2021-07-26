#pragma once

#ifdef GL_EXT_stencil_wrap
namespace gl::ext::stencil_wrap {
namespace values {
constexpr libc_types::u32 incr_wrap = 0x8507;
constexpr libc_types::u32 decr_wrap = 0x8508;
} // namespace values
} // namespace gl::ext::stencil_wrap
#endif // GL_EXT_stencil_wrap
namespace gl::ext::stencil_wrap {
constexpr auto name = "GL_EXT_stencil_wrap";
}
