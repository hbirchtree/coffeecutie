#pragma once

#ifdef GL_OES_vertex_array_object
namespace gl::oes::vertex_array_object {
namespace values {
constexpr u32 vertex_array_binding = 0x85B5;
} // namespace values
/*!
 * \brief Part of GL_OES_vertex_array_object
 * \param array GLuint
 * \return void
 */
STATICINLINE void bind_vertex_array(u32 array)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindVertexArrayOES)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(array);
#endif
    }
    glBindVertexArrayOES(array);
    detail::error_check("BindVertexArrayOES"sv);
}

template<class span_const_u32>
requires(concepts::span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_OES_vertex_array_object
     * \param n GLsizei
     * \param arrays const GLuint *
     * \return void
     */
    STATICINLINE void delete_vertex_arrays(span_const_u32 const& arrays)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteVertexArraysOES)
    }
    glDeleteVertexArraysOES(
        arrays.size(),
        arrays.size() ? reinterpret_cast<const GLuint*>(arrays.data())
                      : nullptr);
    detail::error_check("DeleteVertexArraysOES"sv);
}

template<class span_u32>
requires(concepts::span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_OES_vertex_array_object
     * \param n GLsizei
     * \param arrays GLuint *
     * \return void
     */
    STATICINLINE void gen_vertex_arrays(span_u32 arrays)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenVertexArraysOES)
    }
    glGenVertexArraysOES(
        arrays.size(),
        arrays.size() ? reinterpret_cast<GLuint*>(arrays.data()) : nullptr);
    detail::error_check("GenVertexArraysOES"sv);
}

/*!
 * \brief Part of GL_OES_vertex_array_object
 * \param array GLuint
 * \return Boolean
 */
STATICINLINE bool is_vertex_array(u32 array)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsVertexArrayOES)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(array);
#endif
    }
    auto out = glIsVertexArrayOES(array);
    detail::error_check("IsVertexArrayOES"sv);
    return out == GL_TRUE ? true : false;
}

} // namespace gl::oes::vertex_array_object
#endif // GL_OES_vertex_array_object
namespace gl::oes::vertex_array_object {
constexpr auto name = "GL_OES_vertex_array_object";
}
