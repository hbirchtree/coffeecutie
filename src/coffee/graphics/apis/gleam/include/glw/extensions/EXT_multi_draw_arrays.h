#pragma once

#ifdef GL_EXT_multi_draw_arrays
namespace gl::ext::multi_draw_arrays {
namespace values {
} // namespace values
template<class span_const_i32>
requires(concepts::span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_EXT_multi_draw_arrays
     * \param mode GLenum
     * \param first const GLint *
     * \param count const GLsizei *
     * \param primcount GLsizei
     * \return void
     */
    STATICINLINE void multi_draw_arrays(
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
requires(concepts::span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_EXT_multi_draw_arrays
     * \param mode GLenum
     * \param count const GLsizei *
     * \param type GLenum
     * \param indices const void *const*
     * \param primcount GLsizei
     * \return void
     */
    STATICINLINE void multi_draw_elements(
        group::primitive_type     mode,
        span_const_i32            count,
        group::draw_elements_type type,
        intptr_t                  indices)
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
