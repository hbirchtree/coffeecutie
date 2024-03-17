#pragma once

#ifdef GL_INTEL_framebuffer_CMAA
namespace gl::intel::framebuffer_cmaa {
namespace values {
} // namespace values

/*!
 * \brief Part of GL_INTEL_framebuffer_CMAA

 * \return void
 */
STATICINLINE void apply_framebuffer_attachment_cmaa(
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ApplyFramebufferAttachmentCMAAINTEL)
    }
    glApplyFramebufferAttachmentCMAAINTEL();
    detail::error_check("ApplyFramebufferAttachmentCMAAINTEL"sv, check_errors);
}

} // namespace gl::intel::framebuffer_cmaa
#endif // GL_INTEL_framebuffer_CMAA
namespace gl::intel::framebuffer_cmaa {
constexpr auto name = "GL_INTEL_framebuffer_CMAA";
}
