#pragma once

#ifdef GL_OES_EGL_image_external
namespace gl::oes::egl_image_external {
namespace values {
constexpr u32 texture_external = 0x8D65;
#if defined(GL_ES_VERSION_2_0)
constexpr u32 sampler_external = 0x8D66;
#endif
constexpr u32 texture_binding_external     = 0x8D67;
constexpr u32 required_texture_image_units = 0x8D68;
} // namespace values
} // namespace gl::oes::egl_image_external
#endif // GL_OES_EGL_image_external
namespace gl::oes::egl_image_external {
constexpr auto name = "GL_OES_EGL_image_external";
}
