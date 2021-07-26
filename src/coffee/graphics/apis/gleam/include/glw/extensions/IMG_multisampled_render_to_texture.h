#pragma once

#ifdef GL_IMG_multisampled_render_to_texture
#include "../enums/RenderbufferParameterName.h"
namespace gl::img::multisampled_render_to_texture {
using gl::group::renderbuffer_parameter_name;
namespace values {
constexpr libc_types::u32 framebuffer_incomplete_multisample = 0x9134;
constexpr libc_types::u32 max_samples                        = 0x9135;
constexpr libc_types::u32 texture_samples                    = 0x9136;
} // namespace values
STATICINLINE void framebuffer_texture_2d_multisample(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    group::texture_target         textarget,
    u32                           texture,
    i32                           level,
    i32                           samples)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTexture2DMultisampleIMG)
        glIsTexture(texture);
    }
    glFramebufferTexture2DMultisampleIMG(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(textarget),
        texture,
        level,
        samples);
    detail::error_check("FramebufferTexture2DMultisampleIMG"sv);
}

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
        GLW_FPTR_CHECK(RenderbufferStorageMultisampleIMG)
    }
    glRenderbufferStorageMultisampleIMG(
        static_cast<GLenum>(target),
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("RenderbufferStorageMultisampleIMG"sv);
}

} // namespace gl::img::multisampled_render_to_texture
#endif // GL_IMG_multisampled_render_to_texture
namespace gl::img::multisampled_render_to_texture {
constexpr auto name = "GL_IMG_multisampled_render_to_texture";
}
