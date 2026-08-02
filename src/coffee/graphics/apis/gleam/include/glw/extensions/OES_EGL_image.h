#pragma once

#ifdef GL_OES_EGL_image
namespace gl::oes::egl_image {
namespace values {
} // namespace values

/*!
 * \brief Part of GL_OES_EGL_image
 * \param target GLenum
 * \param image GLeglImageOES
 * \return void
 */
STATICINLINE void egl_image_target_renderbuffer_storage(
    GLenum        target,
    GLeglImageOES image,
    error_check   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(EGLImageTargetRenderbufferStorageOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glEGLImageTargetRenderbufferStorageOES, target, image);
#endif
    glEGLImageTargetRenderbufferStorageOES(target, image);
    detail::error_check("EGLImageTargetRenderbufferStorageOES"sv, check_errors);
}

/*!
 * \brief Part of GL_OES_EGL_image
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

} // namespace gl::oes::egl_image
#endif // GL_OES_EGL_image
namespace gl::oes::egl_image {
constexpr auto name = "GL_OES_EGL_image";
}
