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
STATICINLINE void draw_transform_feedback(group::primitive_type mode, u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawTransformFeedbackEXT)
    }
    glDrawTransformFeedbackEXT(static_cast<GLenum>(mode), id);
    detail::error_check("DrawTransformFeedbackEXT"sv);
}

/*!
 * \brief Part of GL_EXT_draw_transform_feedback
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
        GLW_FPTR_CHECK(DrawTransformFeedbackInstancedEXT)
    }
    glDrawTransformFeedbackInstancedEXT(
        static_cast<GLenum>(mode), id, instancecount);
    detail::error_check("DrawTransformFeedbackInstancedEXT"sv);
}

} // namespace gl::ext::draw_transform_feedback
#endif // GL_EXT_draw_transform_feedback
namespace gl::ext::draw_transform_feedback {
constexpr auto name = "GL_EXT_draw_transform_feedback";
}
