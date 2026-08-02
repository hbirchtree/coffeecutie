#pragma once

#ifdef GL_EXT_framebuffer_blit_layers
namespace gl::ext::framebuffer_blit_layers {
namespace values {
} // namespace values

/*!
 * \brief Part of GL_EXT_framebuffer_blit_layers
 * \param srcX0 GLint
 * \param srcY0 GLint
 * \param srcX1 GLint
 * \param srcY1 GLint
 * \param srcLayer GLint
 * \param dstX0 GLint
 * \param dstY0 GLint
 * \param dstX1 GLint
 * \param dstY1 GLint
 * \param dstLayer GLint
 * \param mask GLbitfield
 * \param filter GLenum
 * \return void
 */
STATICINLINE void blit_framebuffer_layer(
    i32                            srcX0,
    i32                            srcY0,
    i32                            srcX1,
    i32                            srcY1,
    i32                            srcLayer,
    i32                            dstX0,
    i32                            dstY0,
    i32                            dstX1,
    i32                            dstY1,
    i32                            dstLayer,
    group::clear_buffer_mask       mask,
    group::blit_framebuffer_filter filter,
    error_check                    check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(BlitFramebufferLayerEXT)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glBlitFramebufferLayerEXT,
        srcX0,
        srcY0,
        srcX1,
        srcY1,
        srcLayer,
        dstX0,
        dstY0,
        dstX1,
        dstY1,
        dstLayer,
        mask,
        filter);
#endif
    glBlitFramebufferLayerEXT(
        srcX0,
        srcY0,
        srcX1,
        srcY1,
        srcLayer,
        dstX0,
        dstY0,
        dstX1,
        dstY1,
        dstLayer,
        static_cast<GLenum>(mask),
        static_cast<GLenum>(filter));
    detail::error_check("BlitFramebufferLayerEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_framebuffer_blit_layers
 * \param srcX0 GLint
 * \param srcY0 GLint
 * \param srcX1 GLint
 * \param srcY1 GLint
 * \param dstX0 GLint
 * \param dstY0 GLint
 * \param dstX1 GLint
 * \param dstY1 GLint
 * \param mask GLbitfield
 * \param filter GLenum
 * \return void
 */
STATICINLINE void blit_framebuffer_layers(
    i32                            srcX0,
    i32                            srcY0,
    i32                            srcX1,
    i32                            srcY1,
    i32                            dstX0,
    i32                            dstY0,
    i32                            dstX1,
    i32                            dstY1,
    group::clear_buffer_mask       mask,
    group::blit_framebuffer_filter filter,
    error_check                    check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(BlitFramebufferLayersEXT)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glBlitFramebufferLayersEXT,
        srcX0,
        srcY0,
        srcX1,
        srcY1,
        dstX0,
        dstY0,
        dstX1,
        dstY1,
        mask,
        filter);
#endif
    glBlitFramebufferLayersEXT(
        srcX0,
        srcY0,
        srcX1,
        srcY1,
        dstX0,
        dstY0,
        dstX1,
        dstY1,
        static_cast<GLenum>(mask),
        static_cast<GLenum>(filter));
    detail::error_check("BlitFramebufferLayersEXT"sv, check_errors);
}

} // namespace gl::ext::framebuffer_blit_layers
#endif // GL_EXT_framebuffer_blit_layers
namespace gl::ext::framebuffer_blit_layers {
constexpr auto name = "GL_EXT_framebuffer_blit_layers";
}
