#pragma once

#ifdef GL_ARB_invalidate_subdata
namespace gl::arb::invalidate_subdata {
namespace values {
} // namespace values
STATICINLINE void invalidate_buffer_data(u32 buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InvalidateBufferData)
        glIsBuffer(buffer);
    }
    glInvalidateBufferData(buffer);
    detail::error_check("InvalidateBufferData"sv);
}

STATICINLINE void invalidate_buffer_sub_data(
    u32 buffer, GLintptr offset, GLsizeiptr length)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InvalidateBufferSubData)
        glIsBuffer(buffer);
    }
    glInvalidateBufferSubData(buffer, offset, length);
    detail::error_check("InvalidateBufferSubData"sv);
}

template<class span_const_invalidate_framebuffer_attachment>
requires(
    semantic::concepts::Span<span_const_invalidate_framebuffer_attachment>&&
        std::is_same_v<
            std::decay_t<typename span_const_invalidate_framebuffer_attachment::
                             value_type>,
            std::decay_t<group::invalidate_framebuffer_attachment>>)
    STATICINLINE void invalidate_framebuffer(
        group::framebuffer_target                           target,
        span_const_invalidate_framebuffer_attachment const& attachments)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InvalidateFramebuffer)
    }
    glInvalidateFramebuffer(
        static_cast<GLenum>(target),
        attachments.size(),
        attachments.size() ? reinterpret_cast<const GLenum*>(attachments.data())
                           : nullptr);
    detail::error_check("InvalidateFramebuffer"sv);
}

template<
    class size_2_i32,
    class span_const_invalidate_framebuffer_attachment,
    class vec_2_i32>
requires(
    semantic::concepts::Span<span_const_invalidate_framebuffer_attachment>&&
        std::is_same_v<
            std::decay_t<typename span_const_invalidate_framebuffer_attachment::
                             value_type>,
            std::decay_t<group::invalidate_framebuffer_attachment>>&&
                semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void invalidate_sub_framebuffer(
        group::framebuffer_target                           target,
        span_const_invalidate_framebuffer_attachment const& attachments,
        vec_2_i32 const&                                    x,
        size_2_i32 const&                                   width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InvalidateSubFramebuffer)
    }
    glInvalidateSubFramebuffer(
        static_cast<GLenum>(target),
        attachments.size(),
        attachments.size() ? reinterpret_cast<const GLenum*>(attachments.data())
                           : nullptr,
        x.x(),
        x.y(),
        width[0],
        width[1]);
    detail::error_check("InvalidateSubFramebuffer"sv);
}

STATICINLINE void invalidate_tex_image(u32 texture, i32 level)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InvalidateTexImage)
        glIsTexture(texture);
    }
    glInvalidateTexImage(texture, level);
    detail::error_check("InvalidateTexImage"sv);
}

template<class size_3_i32, class vec_3_i32>
requires(semantic::concepts::Vector<vec_3_i32, i32, 3>&&
             semantic::concepts::Size2D<size_3_i32, i32>) STATICINLINE
    void invalidate_tex_sub_image(
        u32               texture,
        i32               level,
        vec_3_i32 const&  xoffset,
        size_3_i32 const& width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(InvalidateTexSubImage)
        glIsTexture(texture);
    }
    glInvalidateTexSubImage(
        texture,
        level,
        xoffset.x(),
        xoffset.y(),
        xoffset.z(),
        width[0],
        width[1],
        width[2]);
    detail::error_check("InvalidateTexSubImage"sv);
}

} // namespace gl::arb::invalidate_subdata
#endif // GL_ARB_invalidate_subdata
namespace gl::arb::invalidate_subdata {
constexpr auto name = "GL_ARB_invalidate_subdata";
}
