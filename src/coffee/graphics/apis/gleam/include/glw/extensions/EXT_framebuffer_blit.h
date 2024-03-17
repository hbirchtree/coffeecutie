#pragma once

#ifdef GL_EXT_framebuffer_blit
namespace gl::ext::framebuffer_blit {
namespace values {
constexpr u32 draw_framebuffer_binding = 0x8CA6;
constexpr u32 read_framebuffer         = 0x8CA8;
constexpr u32 draw_framebuffer         = 0x8CA9;
constexpr u32 read_framebuffer_binding = 0x8CAA;
} // namespace values

/*!
 * \brief Part of GL_EXT_framebuffer_blit
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
STATICINLINE void blit_framebuffer(
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
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlitFramebufferEXT)
    }
    glBlitFramebufferEXT(
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
    detail::error_check("BlitFramebufferEXT"sv, check_errors);
}

} // namespace gl::ext::framebuffer_blit
#endif // GL_EXT_framebuffer_blit
namespace gl::ext::framebuffer_blit {
constexpr auto name = "GL_EXT_framebuffer_blit";
}
