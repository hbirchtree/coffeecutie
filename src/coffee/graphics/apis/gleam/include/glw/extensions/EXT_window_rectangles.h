#pragma once

#ifdef GL_EXT_window_rectangles
namespace gl::ext::window_rectangles {
namespace values {
constexpr libc_types::u32 inclusive             = 0x8F10;
constexpr libc_types::u32 exclusive             = 0x8F11;
constexpr libc_types::u32 window_rectangle      = 0x8F12;
constexpr libc_types::u32 window_rectangle_mode = 0x8F13;
constexpr libc_types::u32 max_window_rectangles = 0x8F14;
constexpr libc_types::u32 num_window_rectangles = 0x8F15;
} // namespace values
template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void window_rectangles(GLenum mode, i32 count, span_const_i32 const& box)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WindowRectanglesEXT)
    }
    glWindowRectanglesEXT(
        mode,
        count,
        box.size() ? reinterpret_cast<const GLint*>(box.data()) : nullptr);
    detail::error_check("WindowRectanglesEXT"sv);
}

} // namespace gl::ext::window_rectangles
#endif // GL_EXT_window_rectangles
namespace gl::ext::window_rectangles {
constexpr auto name = "GL_EXT_window_rectangles";
}
