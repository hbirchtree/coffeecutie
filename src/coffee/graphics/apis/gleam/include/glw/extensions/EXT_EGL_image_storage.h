#pragma once

#ifdef GL_EXT_EGL_image_storage
namespace gl::ext::egl_image_storage {
namespace values {
} // namespace values
template<class span_const_i32>
requires(concepts::span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_EXT_EGL_image_storage
     * \param target GLenum
     * \param image GLeglImageOES
     * \param attrib_list const GLint *
     * \return void
     */
    STATICINLINE void egl_image_target_tex_storage(
        GLenum target, GLeglImageOES image, span_const_i32 const& attrib_list)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EGLImageTargetTexStorageEXT)
    }
    glEGLImageTargetTexStorageEXT(
        target,
        image,
        attrib_list.size() ? reinterpret_cast<const GLint*>(attrib_list.data())
                           : nullptr);
    detail::error_check("EGLImageTargetTexStorageEXT"sv);
}

template<class span_const_i32>
requires(concepts::span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_EXT_EGL_image_storage
     * \param texture GLuint
     * \param image GLeglImageOES
     * \param attrib_list const GLint *
     * \return void
     */
    STATICINLINE void egl_image_target_texture_storage(
        u32 texture, GLeglImageOES image, span_const_i32 const& attrib_list)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EGLImageTargetTextureStorageEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glEGLImageTargetTextureStorageEXT(
        texture,
        image,
        attrib_list.size() ? reinterpret_cast<const GLint*>(attrib_list.data())
                           : nullptr);
    detail::error_check("EGLImageTargetTextureStorageEXT"sv);
}

} // namespace gl::ext::egl_image_storage
#endif // GL_EXT_EGL_image_storage
namespace gl::ext::egl_image_storage {
constexpr auto name = "GL_EXT_EGL_image_storage";
}
