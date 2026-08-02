#pragma once

#ifdef GL_EXT_draw_transform_feedback
namespace gl::ext::draw_transform_feedback {
namespace values {
} // namespace values

/*!
 * \brief Part of GL_EXT_draw_transform_feedback
 * \param mode GLenum
 * \param id GLuint
 * \return void
 */
STATICINLINE void draw_transform_feedback(
    group::primitive_type mode,
    u32                   id,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(DrawTransformFeedbackEXT)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glDrawTransformFeedbackEXT, mode, id);
#endif
    glDrawTransformFeedbackEXT(static_cast<GLenum>(mode), id);
    detail::error_check("DrawTransformFeedbackEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_draw_transform_feedback
 * \param mode GLenum
 * \param id GLuint
 * \param instancecount GLsizei
 * \return void
 */
STATICINLINE void draw_transform_feedback_instanced(
    group::primitive_type mode,
    u32                   id,
    i32                   instancecount,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(DrawTransformFeedbackInstancedEXT)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glDrawTransformFeedbackInstancedEXT, mode, id, instancecount);
#endif
    glDrawTransformFeedbackInstancedEXT(
        static_cast<GLenum>(mode), id, instancecount);
    detail::error_check("DrawTransformFeedbackInstancedEXT"sv, check_errors);
}

} // namespace gl::ext::draw_transform_feedback
#endif // GL_EXT_draw_transform_feedback
namespace gl::ext::draw_transform_feedback {
constexpr auto name = "GL_EXT_draw_transform_feedback";
}
