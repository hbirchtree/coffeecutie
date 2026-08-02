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

/*!
 * \brief Part of GL_OES_EGL_image_external
 * \param target GLenum
 * \param image GLeglImageOES
 * \return void
 */
STATICINLINE void egl_image_target_texture_2d(
    GLenum        target,
    GLeglImageOES image,
    error_check   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(EGLImageTargetTexture2DOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glEGLImageTargetTexture2DOES, target, image);
#endif
    glEGLImageTargetTexture2DOES(target, image);
    detail::error_check("EGLImageTargetTexture2DOES"sv, check_errors);
}

} // namespace gl::oes::egl_image_external
#endif // GL_OES_EGL_image_external
namespace gl::oes::egl_image_external {
constexpr auto name = "GL_OES_EGL_image_external";
}
