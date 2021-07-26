#pragma once

#ifdef GL_OES_surfaceless_context
namespace gl::oes::surfaceless_context {
namespace values {
constexpr libc_types::u32 framebuffer_undefined = 0x8219;
} // namespace values
} // namespace gl::oes::surfaceless_context
#endif // GL_OES_surfaceless_context
namespace gl::oes::surfaceless_context {
constexpr auto name = "GL_OES_surfaceless_context";
}
