#pragma once

#ifdef GL_EXT_draw_range_elements
namespace gl::ext::draw_range_elements {
namespace values {
constexpr libc_types::u32 max_elements_vertices = 0x80E8;
constexpr libc_types::u32 max_elements_indices  = 0x80E9;
} // namespace values
template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>)
/*!
 * \brief Part of GL_EXT_draw_range_elements
 * \param mode GLenum
 * \param start GLuint
 * \param end GLuint
 * \param count GLsizei
 * \param type GLenum
 * \param indices const void *
 * \return void
 */
STATICINLINE void draw_range_elements(
    group::primitive_type     mode,
    u32                       start,
    u32                       end,
    i32                       count,
    group::draw_elements_type type,
    span_const_void const&    indices)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawRangeElementsEXT)
    }
    glDrawRangeElementsEXT(
        static_cast<GLenum>(mode),
        start,
        end,
        count,
        static_cast<GLenum>(type),
        indices.size() ? reinterpret_cast<const void*>(indices.data())
                       : nullptr);
    detail::error_check("DrawRangeElementsEXT"sv);
}

} // namespace gl::ext::draw_range_elements
#endif // GL_EXT_draw_range_elements
namespace gl::ext::draw_range_elements {
constexpr auto name = "GL_EXT_draw_range_elements";
}
