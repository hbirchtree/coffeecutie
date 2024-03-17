#pragma once

#ifdef GL_OES_draw_elements_base_vertex
namespace gl::oes::draw_elements_base_vertex {
namespace values {
} // namespace values

/*!
 * \brief Part of GL_OES_draw_elements_base_vertex
 * \param mode GLenum
 * \param count GLsizei
 * \param type GLenum
 * \param indices const void *
 * \param basevertex GLint
 * \return void
 */
STATICINLINE void draw_elements_base_vertex(
    group::primitive_type     mode,
    i32                       count,
    group::draw_elements_type type,
    intptr_t                  indices,
    i32                       basevertex,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawElementsBaseVertexOES)
    }
    glDrawElementsBaseVertexOES(
        static_cast<GLenum>(mode),
        count,
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indices),
        basevertex);
    detail::error_check("DrawElementsBaseVertexOES"sv, check_errors);
}

/*!
 * \brief Part of GL_OES_draw_elements_base_vertex
 * \param mode GLenum
 * \param count GLsizei
 * \param type GLenum
 * \param indices const void *
 * \param instancecount GLsizei
 * \param basevertex GLint
 * \return void
 */
STATICINLINE void draw_elements_instanced_base_vertex(
    group::primitive_type     mode,
    i32                       count,
    group::draw_elements_type type,
    intptr_t                  indices,
    i32                       instancecount,
    i32                       basevertex,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawElementsInstancedBaseVertexOES)
    }
    glDrawElementsInstancedBaseVertexOES(
        static_cast<GLenum>(mode),
        count,
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indices),
        instancecount,
        basevertex);
    detail::error_check("DrawElementsInstancedBaseVertexOES"sv, check_errors);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_OES_draw_elements_base_vertex
 * \param mode GLenum
 * \param start GLuint
 * \param end GLuint
 * \param count GLsizei
 * \param type GLenum
 * \param indices const void *
 * \param basevertex GLint
 * \return void
 */
STATICINLINE void draw_range_elements_base_vertex(
    group::primitive_type     mode,
    u32                       start,
    u32                       end,
    i32                       count,
    group::draw_elements_type type,
    span_const_void const&    indices,
    i32                       basevertex,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawRangeElementsBaseVertexOES)
    }
    glDrawRangeElementsBaseVertexOES(
        static_cast<GLenum>(mode),
        start,
        end,
        count,
        static_cast<GLenum>(type),
        indices.size() ? reinterpret_cast<const void*>(indices.data())
                       : nullptr,
        basevertex);
    detail::error_check("DrawRangeElementsBaseVertexOES"sv, check_errors);
}

template<class span_const_i32>
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_OES_draw_elements_base_vertex
 * \param mode GLenum
 * \param count const GLsizei *
 * \param type GLenum
 * \param indices const void *const*
 * \param drawcount GLsizei
 * \param basevertex const GLint *
 * \return void
 */
STATICINLINE void multi_draw_elements_base_vertex_ext(
    group::primitive_type     mode,
    span_const_i32            count,
    group::draw_elements_type type,
    intptr_t                  indices,
    span_const_i32            basevertex,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiDrawElementsBaseVertexEXT)
    }
    GLsizei drawcount = count.size();
    detail::assert_equal(count.size(), drawcount);
    detail::assert_equal(basevertex.size(), drawcount);
    glMultiDrawElementsBaseVertexEXT(
        static_cast<GLenum>(mode),
        count.data(),
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indices),
        basevertex.data());
    detail::error_check("MultiDrawElementsBaseVertexEXT"sv, check_errors);
}

} // namespace gl::oes::draw_elements_base_vertex
#endif // GL_OES_draw_elements_base_vertex
namespace gl::oes::draw_elements_base_vertex {
constexpr auto name = "GL_OES_draw_elements_base_vertex";
}
