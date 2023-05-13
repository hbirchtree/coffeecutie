#pragma once

#ifdef GL_ARB_invalidate_subdata
namespace gl::arb::invalidate_subdata {
namespace values {
} // namespace values
/*!
 * \brief Part of GL_ARB_invalidate_subdata
 * \param buffer GLuint
 * \return void
 */
STATICINLINE void invalidate_buffer_data(u32 buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InvalidateBufferData)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glInvalidateBufferData(buffer);
    detail::error_check("InvalidateBufferData"sv);
}

/*!
 * \brief Part of GL_ARB_invalidate_subdata
 * \param buffer GLuint
 * \param offset GLintptr
 * \param length GLsizeiptr
 * \return void
 */
STATICINLINE void invalidate_buffer_sub_data(
    u32 buffer, GLintptr offset, GLsizeiptr length)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InvalidateBufferSubData)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glInvalidateBufferSubData(buffer, offset, length);
    detail::error_check("InvalidateBufferSubData"sv);
}

template<class span_const_invalidate_framebuffer_attachment>
requires(
    concepts::span<span_const_invalidate_framebuffer_attachment>&&
        std::is_same_v<
            std::decay_t<typename span_const_invalidate_framebuffer_attachment::
                             value_type>,
            std::decay_t<group::invalidate_framebuffer_attachment>>)
    /*!
     * \brief Part of GL_ARB_invalidate_subdata
     * \param target GLenum
     * \param numAttachments GLsizei
     * \param attachments const GLenum *
     * \return void
     */
    STATICINLINE void invalidate_framebuffer(
        group::framebuffer_target                           target,
        span_const_invalidate_framebuffer_attachment const& attachments)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InvalidateFramebuffer)
    }
    glInvalidateFramebuffer(
        static_cast<GLenum>(target),
        attachments.size(),
        attachments.size() ? reinterpret_cast<const GLenum*>(attachments.data())
                           : nullptr);
    detail::error_check("InvalidateFramebuffer"sv);
}

template<
    class size_2_i32,
    class span_const_invalidate_framebuffer_attachment,
    class vec_2_i32>
requires(
    concepts::span<span_const_invalidate_framebuffer_attachment>&&
        std::is_same_v<
            std::decay_t<typename span_const_invalidate_framebuffer_attachment::
                             value_type>,
            std::decay_t<group::invalidate_framebuffer_attachment>>&&
                concepts::vector<vec_2_i32, i32, 2>&&
                concepts::size_2d<size_2_i32, i32>)
    /*!
     * \brief Part of GL_ARB_invalidate_subdata
     * \param target GLenum
     * \param numAttachments GLsizei
     * \param attachments const GLenum *
     * \param x GLint
     * \param y GLint
     * \param width GLsizei
     * \param height GLsizei
     * \return void
     */
    STATICINLINE void invalidate_sub_framebuffer(
        group::framebuffer_target                           target,
        span_const_invalidate_framebuffer_attachment const& attachments,
        vec_2_i32 const&                                    x,
        size_2_i32 const&                                   width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InvalidateSubFramebuffer)
    }
    glInvalidateSubFramebuffer(
        static_cast<GLenum>(target),
        attachments.size(),
        attachments.size() ? reinterpret_cast<const GLenum*>(attachments.data())
                           : nullptr,
        x[0],
        x[1],
        width[0],
        width[1]);
    detail::error_check("InvalidateSubFramebuffer"sv);
}

/*!
 * \brief Part of GL_ARB_invalidate_subdata
 * \param texture GLuint
 * \param level GLint
 * \return void
 */
STATICINLINE void invalidate_tex_image(u32 texture, i32 level)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InvalidateTexImage)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glInvalidateTexImage(texture, level);
    detail::error_check("InvalidateTexImage"sv);
}

template<class size_3_i32, class vec_3_i32>
requires(
    concepts::vector<vec_3_i32, i32, 3>&& concepts::size_2d<size_3_i32, i32>)
    /*!
     * \brief Part of GL_ARB_invalidate_subdata
     * \param texture GLuint
     * \param level GLint
     * \param xoffset GLint
     * \param yoffset GLint
     * \param zoffset GLint
     * \param width GLsizei
     * \param height GLsizei
     * \param depth GLsizei
     * \return void
     */
    STATICINLINE void invalidate_tex_sub_image(
        u32               texture,
        i32               level,
        vec_3_i32 const&  xoffset,
        size_3_i32 const& width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InvalidateTexSubImage)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glInvalidateTexSubImage(
        texture,
        level,
        xoffset[0],
        xoffset[1],
        xoffset[2],
        width[0],
        width[1],
        width[2]);
    detail::error_check("InvalidateTexSubImage"sv);
}

} // namespace gl::arb::invalidate_subdata
#endif // GL_ARB_invalidate_subdata
namespace gl::arb::invalidate_subdata {
constexpr auto name = "GL_ARB_invalidate_subdata";
}
