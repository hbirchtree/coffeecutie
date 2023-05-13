#pragma once

#ifdef GL_OES_query_matrix
namespace gl::oes::query_matrix {
namespace values {
} // namespace values
template<class span_GLfixed, class span_i32>
requires(concepts::span<span_GLfixed>&& std::is_same_v<
         std::decay_t<typename span_GLfixed::value_type>,
         std::decay_t<GLfixed>>&& concepts::span<span_i32>&&
                                        std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Part of GL_OES_query_matrix
     * \param mantissa GLfixed *
     * \param exponent GLint *
     * \return GLbitfield
     */
    STATICINLINE GLbitfield
    query_matrixx(span_GLfixed mantissa, span_i32 exponent)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(QueryMatrixxOES)
    }
    auto out = glQueryMatrixxOES(
        mantissa.size() ? reinterpret_cast<GLfixed*>(mantissa.data()) : nullptr,
        exponent.size() ? reinterpret_cast<GLint*>(exponent.data()) : nullptr);
    detail::error_check("QueryMatrixxOES"sv);
    return out;
}

} // namespace gl::oes::query_matrix
#endif // GL_OES_query_matrix
namespace gl::oes::query_matrix {
constexpr auto name = "GL_OES_query_matrix";
}
