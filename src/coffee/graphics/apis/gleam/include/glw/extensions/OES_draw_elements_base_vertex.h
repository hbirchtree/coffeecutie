#pragma once

#ifdef GL_OES_draw_elements_base_vertex
namespace gl::oes::draw_elements_base_vertex {
namespace values {
} // namespace values
STATICINLINE void draw_elements_base_vertex(
    group::primitive_type     mode,
    i32                       count,
    group::draw_elements_type type,
    ptroff                    indices,
    i32                       basevertex)
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
    detail::error_check("DrawElementsBaseVertexOES"sv);
}

STATICINLINE void draw_elements_instanced_base_vertex(
    group::primitive_type     mode,
    i32                       count,
    group::draw_elements_type type,
    ptroff                    indices,
    i32                       instancecount,
    i32                       basevertex)
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
    detail::error_check("DrawElementsInstancedBaseVertexOES"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void draw_range_elements_base_vertex(
        group::primitive_type     mode,
        u32                       start,
        u32                       end,
        i32                       count,
        group::draw_elements_type type,
        span_const_void const&    indices,
        i32                       basevertex)
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
    detail::error_check("DrawRangeElementsBaseVertexOES"sv);
}

template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void multi_draw_elements_base_vertex_ext(
        group::primitive_type     mode,
        span_const_i32            count,
        group::draw_elements_type type,
        ptroff                    indices,
        span_const_i32            basevertex)
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
    detail::error_check("MultiDrawElementsBaseVertexEXT"sv);
}

} // namespace gl::oes::draw_elements_base_vertex
#endif // GL_OES_draw_elements_base_vertex
namespace gl::oes::draw_elements_base_vertex {
constexpr auto name = "GL_OES_draw_elements_base_vertex";
}
