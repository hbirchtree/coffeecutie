#pragma once

#ifdef GL_ARB_framebuffer_no_attachments
#include "../enums/FramebufferParameterName.h"
#include "../enums/GetFramebufferParameter.h"
#include "../enums/GetPName.h"
namespace gl::arb::framebuffer_no_attachments {
using gl::group::framebuffer_parameter_name;
using gl::group::get_framebuffer_parameter;
using gl::group::get_prop;
namespace values {
} // namespace values
/*!
 * \brief Part of GL_ARB_framebuffer_no_attachments
 * \param target GLenum
 * \param pname GLenum
 * \param param GLint
 * \return void
 */
STATICINLINE void framebuffer_parameter(
    group::framebuffer_target         target,
    group::framebuffer_parameter_name pname,
    i32                               param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferParameteri)
    }
    glFramebufferParameteri(
        static_cast<GLenum>(target), static_cast<GLenum>(pname), param);
    detail::error_check("FramebufferParameteri"sv);
}

template<class span_i32>
requires(
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_ARB_framebuffer_no_attachments
 * \param target GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_framebuffer_parameter(
    group::framebuffer_target                    target,
    group::framebuffer_attachment_parameter_name pname,
    span_i32                                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFramebufferParameteriv)
    }
    glGetFramebufferParameteriv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetFramebufferParameteriv"sv);
}

} // namespace gl::arb::framebuffer_no_attachments
#endif // GL_ARB_framebuffer_no_attachments
namespace gl::arb::framebuffer_no_attachments {
constexpr auto name = "GL_ARB_framebuffer_no_attachments";
}
