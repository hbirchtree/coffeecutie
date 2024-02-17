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
    GLenum target, GLeglImageOES image)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EGLImageTargetRenderbufferStorageOES)
    }
    glEGLImageTargetRenderbufferStorageOES(target, image);
    detail::error_check("EGLImageTargetRenderbufferStorageOES"sv);
}

/*!
 * \brief Part of GL_OES_EGL_image
 * \param target GLenum
 * \param image GLeglImageOES
 * \return void
 */
STATICINLINE void egl_image_target_texture_2d(
    GLenum target, GLeglImageOES image)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EGLImageTargetTexture2DOES)
    }
    glEGLImageTargetTexture2DOES(target, image);
    detail::error_check("EGLImageTargetTexture2DOES"sv);
}

} // namespace gl::oes::egl_image
#endif // GL_OES_EGL_image
namespace gl::oes::egl_image {
constexpr auto name = "GL_OES_EGL_image";
}
