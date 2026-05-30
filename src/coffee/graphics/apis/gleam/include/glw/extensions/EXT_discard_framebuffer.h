#pragma once

#ifdef GL_EXT_discard_framebuffer
#include "../enums/PixelCopyType.h"

namespace gl::ext::discard_framebuffer {
using gl::group::pixel_copy_type;

namespace values {
} // namespace values

template<class span_const_invalidate_framebuffer_attachment>
requires(
    concepts::span<span_const_invalidate_framebuffer_attachment> &&
    std::is_same_v<
        std::decay_t<
            typename span_const_invalidate_framebuffer_attachment::value_type>,
        std::decay_t<group::invalidate_framebuffer_attachment>>)
/*!
 * \brief Part of GL_EXT_discard_framebuffer
 * \param target GLenum
 * \param numAttachments GLsizei
 * \param attachments const GLenum *
 * \return void
 */
STATICINLINE void discard_framebuffer(
    group::framebuffer_target                           target,
    span_const_invalidate_framebuffer_attachment const& attachments,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DiscardFramebufferEXT)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glDiscardFramebufferEXT,
        gsl::span<const char>(
            reinterpret_cast<const char*>(attachments.data()),
            attachments.size_bytes()),
        target);
#endif
    glDiscardFramebufferEXT(
        static_cast<GLenum>(target),
        attachments.size(),
        attachments.size() ? reinterpret_cast<const GLenum*>(attachments.data())
                           : nullptr);
    detail::error_check("DiscardFramebufferEXT"sv, check_errors);
}

} // namespace gl::ext::discard_framebuffer
#endif // GL_EXT_discard_framebuffer
namespace gl::ext::discard_framebuffer {
constexpr auto name = "GL_EXT_discard_framebuffer";
}
