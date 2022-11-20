#pragma once

#ifdef GL_ARB_transform_feedback_instanced
namespace gl::arb::transform_feedback_instanced {
namespace values {
} // namespace values
/*!
 * \brief Part of GL_ARB_transform_feedback_instanced
 * \param mode GLenum
 * \param id GLuint
 * \param instancecount GLsizei
 * \return void
 */
STATICINLINE void draw_transform_feedback_instanced(
    group::primitive_type mode, u32 id, i32 instancecount)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawTransformFeedbackInstanced)
    }
    glDrawTransformFeedbackInstanced(
        static_cast<GLenum>(mode), id, instancecount);
    detail::error_check("DrawTransformFeedbackInstanced"sv);
}

/*!
 * \brief Part of GL_ARB_transform_feedback_instanced
 * \param mode GLenum
 * \param id GLuint
 * \param stream GLuint
 * \param instancecount GLsizei
 * \return void
 */
STATICINLINE void draw_transform_feedback_stream_instanced(
    group::primitive_type mode, u32 id, u32 stream, i32 instancecount)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawTransformFeedbackStreamInstanced)
    }
    glDrawTransformFeedbackStreamInstanced(
        static_cast<GLenum>(mode), id, stream, instancecount);
    detail::error_check("DrawTransformFeedbackStreamInstanced"sv);
}

} // namespace gl::arb::transform_feedback_instanced
#endif // GL_ARB_transform_feedback_instanced
namespace gl::arb::transform_feedback_instanced {
constexpr auto name = "GL_ARB_transform_feedback_instanced";
}
