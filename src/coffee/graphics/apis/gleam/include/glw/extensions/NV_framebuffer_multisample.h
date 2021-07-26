#pragma once

#ifdef GL_NV_framebuffer_multisample
#include "../enums/RenderbufferParameterName.h"
namespace gl::nv::framebuffer_multisample {
using gl::group::renderbuffer_parameter_name;
namespace values {
constexpr libc_types::u32 framebuffer_incomplete_multisample = 0x8D56;
constexpr libc_types::u32 max_samples                        = 0x8D57;
} // namespace values
template<class size_2_i32>
requires(semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void renderbuffer_storage_multisample(
        group::renderbuffer_target target,
        i32                        samples,
        group::internal_format     internalformat,
        size_2_i32 const&          width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RenderbufferStorageMultisampleNV)
    }
    glRenderbufferStorageMultisampleNV(
        static_cast<GLenum>(target),
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("RenderbufferStorageMultisampleNV"sv);
}

} // namespace gl::nv::framebuffer_multisample
#endif // GL_NV_framebuffer_multisample
namespace gl::nv::framebuffer_multisample {
constexpr auto name = "GL_NV_framebuffer_multisample";
}
