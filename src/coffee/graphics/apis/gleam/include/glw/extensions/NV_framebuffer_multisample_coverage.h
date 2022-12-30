#pragma once

#ifdef GL_NV_framebuffer_multisample_coverage
#include "../enums/RenderbufferParameterName.h"
namespace gl::nv::framebuffer_multisample_coverage {
using gl::group::renderbuffer_parameter_name;
namespace values {
constexpr libc_types::u32 max_multisample_coverage_modes = 0x8E11;
constexpr libc_types::u32 multisample_coverage_modes     = 0x8E12;
} // namespace values
template<class size_2_i32>
requires(semantic::concepts::Size2D<size_2_i32, i32>)
/*!
 * \brief Part of GL_NV_framebuffer_multisample_coverage
 * \param target GLenum
 * \param coverageSamples GLsizei
 * \param colorSamples GLsizei
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \return void
 */
STATICINLINE void renderbuffer_storage_multisample_coverage(
    group::renderbuffer_target target,
    i32                        coverageSamples,
    i32                        colorSamples,
    group::internal_format     internalformat,
    size_2_i32 const&          width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RenderbufferStorageMultisampleCoverageNV)
    }
    glRenderbufferStorageMultisampleCoverageNV(
        static_cast<GLenum>(target),
        coverageSamples,
        colorSamples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("RenderbufferStorageMultisampleCoverageNV"sv);
}

} // namespace gl::nv::framebuffer_multisample_coverage
#endif // GL_NV_framebuffer_multisample_coverage
namespace gl::nv::framebuffer_multisample_coverage {
constexpr auto name = "GL_NV_framebuffer_multisample_coverage";
}
