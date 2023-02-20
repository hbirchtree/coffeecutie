#pragma once

#ifdef GL_ARB_vertex_array_object
#include "../enums/GetPName.h"
namespace gl::arb::vertex_array_object {
using gl::group::get_prop;
namespace values {
} // namespace values
/*!
 * \brief Part of GL_ARB_vertex_array_object
 * \param array GLuint
 * \return void
 */
STATICINLINE void bind_vertex_array(u32 array)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindVertexArray)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(array);
#endif
    }
    glBindVertexArray(array);
    detail::error_check("BindVertexArray"sv);
}

template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_ARB_vertex_array_object
     * \param n GLsizei
     * \param arrays const GLuint *
     * \return void
     */
    STATICINLINE void delete_vertex_arrays(span_const_u32 const& arrays)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteVertexArrays)
    }
    glDeleteVertexArrays(
        arrays.size(),
        arrays.size() ? reinterpret_cast<const GLuint*>(arrays.data())
                      : nullptr);
    detail::error_check("DeleteVertexArrays"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_ARB_vertex_array_object
     * \param n GLsizei
     * \param arrays GLuint *
     * \return void
     */
    STATICINLINE void gen_vertex_arrays(span_u32 arrays)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenVertexArrays)
    }
    glGenVertexArrays(
        arrays.size(),
        arrays.size() ? reinterpret_cast<GLuint*>(arrays.data()) : nullptr);
    detail::error_check("GenVertexArrays"sv);
}

/*!
 * \brief Part of GL_ARB_vertex_array_object
 * \param array GLuint
 * \return Boolean
 */
STATICINLINE GLboolean is_vertex_array(u32 array)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsVertexArray)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsVertexArray)
            glIsVertexArray(array);
#endif
    }
    auto out = glIsVertexArray(array);
    detail::error_check("IsVertexArray"sv);
    return out;
}

} // namespace gl::arb::vertex_array_object
#endif // GL_ARB_vertex_array_object
namespace gl::arb::vertex_array_object {
constexpr auto name = "GL_ARB_vertex_array_object";
}
