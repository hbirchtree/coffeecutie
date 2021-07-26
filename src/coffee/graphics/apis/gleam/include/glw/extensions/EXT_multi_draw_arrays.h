#pragma once

#ifdef GL_EXT_multi_draw_arrays
namespace gl::ext::multi_draw_arrays {
namespace values {
} // namespace values
template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void multi_draw_arrays(
        group::primitive_type mode, span_const_i32 first, span_const_i32 count)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawArraysEXT)
    }
    GLsizei primcount = first.size();
    detail::assert_equal(first.size(), primcount);
    detail::assert_equal(count.size(), primcount);
    glMultiDrawArraysEXT(static_cast<GLenum>(mode), first.data(), count.data());
    detail::error_check("MultiDrawArraysEXT"sv);
}

template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void multi_draw_elements(
        group::primitive_type     mode,
        span_const_i32            count,
        group::draw_elements_type type,
        ptroff                    indices)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawElementsEXT)
    }
    GLsizei primcount = count.size();
    detail::assert_equal(count.size(), primcount);
    glMultiDrawElementsEXT(
        static_cast<GLenum>(mode),
        count.data(),
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indices));
    detail::error_check("MultiDrawElementsEXT"sv);
}

} // namespace gl::ext::multi_draw_arrays
#endif // GL_EXT_multi_draw_arrays
namespace gl::ext::multi_draw_arrays {
constexpr auto name = "GL_EXT_multi_draw_arrays";
}
