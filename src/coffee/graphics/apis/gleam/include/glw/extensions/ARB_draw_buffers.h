#pragma once

#ifdef GL_ARB_draw_buffers
namespace gl::arb::draw_buffers {
namespace values {
constexpr u32 max_draw_buffers = 0x8824;
constexpr u32 draw_buffer0     = 0x8825;
constexpr u32 draw_buffer1     = 0x8826;
constexpr u32 draw_buffer2     = 0x8827;
constexpr u32 draw_buffer3     = 0x8828;
constexpr u32 draw_buffer4     = 0x8829;
constexpr u32 draw_buffer5     = 0x882A;
constexpr u32 draw_buffer6     = 0x882B;
constexpr u32 draw_buffer7     = 0x882C;
constexpr u32 draw_buffer8     = 0x882D;
constexpr u32 draw_buffer9     = 0x882E;
constexpr u32 draw_buffer10    = 0x882F;
constexpr u32 draw_buffer11    = 0x8830;
constexpr u32 draw_buffer12    = 0x8831;
constexpr u32 draw_buffer13    = 0x8832;
constexpr u32 draw_buffer14    = 0x8833;
constexpr u32 draw_buffer15    = 0x8834;
} // namespace values
template<class span_const_draw_buffer_mode>
requires(concepts::span<span_const_draw_buffer_mode>&& std::is_same_v<
         std::decay_t<typename span_const_draw_buffer_mode::value_type>,
         std::decay_t<group::draw_buffer_mode>>)
    /*!
     * \brief Part of GL_ARB_draw_buffers
     * \param n GLsizei
     * \param bufs const GLenum *
     * \return void
     */
    STATICINLINE void draw_buffers(span_const_draw_buffer_mode const& bufs)
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
