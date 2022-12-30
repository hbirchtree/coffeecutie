#pragma once

#ifdef GL_EXT_framebuffer_multisample
#include "../enums/RenderbufferParameterName.h"
namespace gl::ext::framebuffer_multisample {
using gl::group::renderbuffer_parameter_name;
namespace values {
constexpr libc_types::u32 framebuffer_incomplete_multisample = 0x8D56;
constexpr libc_types::u32 max_samples                        = 0x8D57;
} // namespace values
template<class size_2_i32>
requires(semantic::concepts::Size2D<size_2_i32, i32>)
/*!
 * \brief Part of GL_EXT_framebuffer_multisample
 * \param target GLenum
 * \param samples GLsizei
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \return void
 */
STATICINLINE void renderbuffer_storage_multisample(
    group::renderbuffer_target target,
    i32                        samples,
    group::internal_format     internalformat,
    size_2_i32 const&          width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RenderbufferStorageMultisampleEXT)
    }
    glRenderbufferStorageMultisampleEXT(
        static_cast<GLenum>(target),
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("RenderbufferStorageMultisampleEXT"sv);
}

} // namespace gl::ext::framebuffer_multisample
#endif // GL_EXT_framebuffer_multisample
namespace gl::ext::framebuffer_multisample {
constexpr auto name = "GL_EXT_framebuffer_multisample";
}
