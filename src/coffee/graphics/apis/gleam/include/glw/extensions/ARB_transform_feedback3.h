#pragma once

#ifdef GL_ARB_transform_feedback3
namespace gl::arb::transform_feedback3 {
namespace values {
constexpr u32 max_transform_feedback_buffers = 0x8E70;
constexpr u32 max_vertex_streams             = 0x8E71;
} // namespace values

/*!
 * \brief Part of GL_ARB_transform_feedback3
 * \param target GLenum
 * \param index GLuint
 * \param id GLuint
 * \return void
 */
STATICINLINE void begin_query_indexed(
    group::query_target target,
    u32                 index,
    u32                 id,
    error_check         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BeginQueryIndexed)
    }
    glBeginQueryIndexed(static_cast<GLenum>(target), index, id);
    detail::error_check("BeginQueryIndexed"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_transform_feedback3
 * \param mode GLenum
 * \param id GLuint
 * \param stream GLuint
 * \return void
 */
STATICINLINE void draw_transform_feedback_stream(
    group::primitive_type mode,
    u32                   id,
    u32                   stream,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawTransformFeedbackStream)
    }
    glDrawTransformFeedbackStream(static_cast<GLenum>(mode), id, stream);
    detail::error_check("DrawTransformFeedbackStream"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_transform_feedback3
 * \param target GLenum
 * \param index GLuint
 * \return void
 */
STATICINLINE void end_query_indexed(
    group::query_target target,
    u32                 index,
    error_check         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EndQueryIndexed)
    }
    glEndQueryIndexed(static_cast<GLenum>(target), index);
    detail::error_check("EndQueryIndexed"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_ARB_transform_feedback3
 * \param target GLenum
 * \param index GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_query_indexediv(
    group::query_target         target,
    u32                         index,
    group::query_parameter_name pname,
    span_i32                    params,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryIndexediv)
    }
    glGetQueryIndexediv(
        static_cast<GLenum>(target),
        index,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetQueryIndexediv"sv, check_errors);
}

} // namespace gl::arb::transform_feedback3
#endif // GL_ARB_transform_feedback3
namespace gl::arb::transform_feedback3 {
constexpr auto name = "GL_ARB_transform_feedback3";
}
