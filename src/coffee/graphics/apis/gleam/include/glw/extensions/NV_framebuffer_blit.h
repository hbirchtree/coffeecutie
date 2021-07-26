#pragma once

#ifdef GL_NV_framebuffer_blit
namespace gl::nv::framebuffer_blit {
namespace values {
constexpr libc_types::u32 draw_framebuffer_binding = 0x8CA6;
constexpr libc_types::u32 read_framebuffer         = 0x8CA8;
constexpr libc_types::u32 draw_framebuffer         = 0x8CA9;
constexpr libc_types::u32 read_framebuffer_binding = 0x8CAA;
} // namespace values
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
    group::blit_framebuffer_filter filter)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlitFramebufferNV)
    }
    glBlitFramebufferNV(
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
    detail::error_check("BlitFramebufferNV"sv);
}

} // namespace gl::nv::framebuffer_blit
#endif // GL_NV_framebuffer_blit
namespace gl::nv::framebuffer_blit {
constexpr auto name = "GL_NV_framebuffer_blit";
}
