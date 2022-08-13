#pragma once

#ifdef GL_ARB_draw_buffers
namespace gl::arb::draw_buffers {
namespace values {
constexpr libc_types::u32 max_draw_buffers = 0x8824;
constexpr libc_types::u32 draw_buffer0     = 0x8825;
constexpr libc_types::u32 draw_buffer1     = 0x8826;
constexpr libc_types::u32 draw_buffer2     = 0x8827;
constexpr libc_types::u32 draw_buffer3     = 0x8828;
constexpr libc_types::u32 draw_buffer4     = 0x8829;
constexpr libc_types::u32 draw_buffer5     = 0x882A;
constexpr libc_types::u32 draw_buffer6     = 0x882B;
constexpr libc_types::u32 draw_buffer7     = 0x882C;
constexpr libc_types::u32 draw_buffer8     = 0x882D;
constexpr libc_types::u32 draw_buffer9     = 0x882E;
constexpr libc_types::u32 draw_buffer10    = 0x882F;
constexpr libc_types::u32 draw_buffer11    = 0x8830;
constexpr libc_types::u32 draw_buffer12    = 0x8831;
constexpr libc_types::u32 draw_buffer13    = 0x8832;
constexpr libc_types::u32 draw_buffer14    = 0x8833;
constexpr libc_types::u32 draw_buffer15    = 0x8834;
} // namespace values
template<class span_const_draw_buffer_mode>
requires(semantic::concepts::Span<span_const_draw_buffer_mode>&& std::is_same_v<
         std::decay_t<typename span_const_draw_buffer_mode::value_type>,
         std::decay_t<group::draw_buffer_mode>>) STATICINLINE
    void draw_buffers(span_const_draw_buffer_mode const& bufs)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawBuffersARB)
    }
    glDrawBuffersARB(
        bufs.size(),
        bufs.size() ? reinterpret_cast<const GLenum*>(bufs.data()) : nullptr);
    detail::error_check("DrawBuffersARB"sv);
}

} // namespace gl::arb::draw_buffers
#endif // GL_ARB_draw_buffers
namespace gl::arb::draw_buffers {
constexpr auto name = "GL_ARB_draw_buffers";
}