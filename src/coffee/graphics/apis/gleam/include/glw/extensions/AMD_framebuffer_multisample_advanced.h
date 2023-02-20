#pragma once

#ifdef GL_AMD_framebuffer_multisample_advanced
#include "../enums/RenderbufferParameterName.h"
namespace gl::amd::framebuffer_multisample_advanced {
using gl::group::renderbuffer_parameter_name;
namespace values {
constexpr libc_types::u32 max_color_framebuffer_samples         = 0x91B3;
constexpr libc_types::u32 max_color_framebuffer_storage_samples = 0x91B4;
constexpr libc_types::u32 max_depth_stencil_framebuffer_samples = 0x91B5;
constexpr libc_types::u32 num_supported_multisample_modes       = 0x91B6;
constexpr libc_types::u32 supported_multisample_modes           = 0x91B7;
} // namespace values
template<class size_2_i32>
requires(semantic::concepts::Size2D<size_2_i32, i32>)
    /*!
     * \brief Part of GL_AMD_framebuffer_multisample_advanced
     * \param renderbuffer GLuint
     * \param samples GLsizei
     * \param storageSamples GLsizei
     * \param internalformat GLenum
     * \param width GLsizei
     * \param height GLsizei
     * \return void
     */
    STATICINLINE void named_renderbuffer_storage_multisample_advanced(
        u32                    renderbuffer,
        i32                    samples,
        i32                    storageSamples,
        group::internal_format internalformat,
        size_2_i32 const&      width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedRenderbufferStorageMultisampleAdvancedAMD)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsRenderbuffer)
            glIsRenderbuffer(renderbuffer);
#endif
    }
    glNamedRenderbufferStorageMultisampleAdvancedAMD(
        renderbuffer,
        samples,
        storageSamples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("NamedRenderbufferStorageMultisampleAdvancedAMD"sv);
}

template<class size_2_i32>
requires(semantic::concepts::Size2D<size_2_i32, i32>)
    /*!
     * \brief Part of GL_AMD_framebuffer_multisample_advanced
     * \param target GLenum
     * \param samples GLsizei
     * \param storageSamples GLsizei
     * \param internalformat GLenum
     * \param width GLsizei
     * \param height GLsizei
     * \return void
     */
    STATICINLINE void renderbuffer_storage_multisample_advanced(
        group::renderbuffer_target target,
        i32                        samples,
        i32                        storageSamples,
        group::internal_format     internalformat,
        size_2_i32 const&          width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RenderbufferStorageMultisampleAdvancedAMD)
    }
    glRenderbufferStorageMultisampleAdvancedAMD(
        static_cast<GLenum>(target),
        samples,
        storageSamples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("RenderbufferStorageMultisampleAdvancedAMD"sv);
}

} // namespace gl::amd::framebuffer_multisample_advanced
#endif // GL_AMD_framebuffer_multisample_advanced
namespace gl::amd::framebuffer_multisample_advanced {
constexpr auto name = "GL_AMD_framebuffer_multisample_advanced";
}
