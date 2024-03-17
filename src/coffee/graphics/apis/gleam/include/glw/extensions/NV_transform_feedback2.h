#pragma once

#ifdef GL_NV_transform_feedback2
namespace gl::nv::transform_feedback2 {
namespace values {
constexpr u32 transform_feedback               = 0x8E22;
constexpr u32 transform_feedback_buffer_paused = 0x8E23;
constexpr u32 transform_feedback_buffer_active = 0x8E24;
constexpr u32 transform_feedback_binding       = 0x8E25;
} // namespace values

/*!
 * \brief Part of GL_NV_transform_feedback2
 * \param target GLenum
 * \param id GLuint
 * \return void
 */
STATICINLINE void bind_transform_feedback(
    group::buffer_target_arb target,
    u32                      id,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindTransformFeedbackNV)
    }
    glBindTransformFeedbackNV(static_cast<GLenum>(target), id);
    detail::error_check("BindTransformFeedbackNV"sv, check_errors);
}

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_NV_transform_feedback2
 * \param n GLsizei
 * \param ids const GLuint *
 * \return void
 */
STATICINLINE void delete_transform_feedbacks(
    span_const_u32 const& ids, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteTransformFeedbacksNV)
    }
    glDeleteTransformFeedbacksNV(
        ids.size(),
        ids.size() ? reinterpret_cast<const GLuint*>(ids.data()) : nullptr);
    detail::error_check("DeleteTransformFeedbacksNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_transform_feedback2
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
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawTransformFeedbackNV)
    }
    glDrawTransformFeedbackNV(static_cast<GLenum>(mode), id);
    detail::error_check("DrawTransformFeedbackNV"sv, check_errors);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_NV_transform_feedback2
 * \param n GLsizei
 * \param ids GLuint *
 * \return void
 */
STATICINLINE void gen_transform_feedbacks(
    span_u32 ids, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenTransformFeedbacksNV)
    }
    glGenTransformFeedbacksNV(
        ids.size(),
        ids.size() ? reinterpret_cast<GLuint*>(ids.data()) : nullptr);
    detail::error_check("GenTransformFeedbacksNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_transform_feedback2
 * \param id GLuint
 * \return Boolean
 */
STATICINLINE bool is_transform_feedback(
    u32 id, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsTransformFeedbackNV)
    }
    auto out = glIsTransformFeedbackNV(id);
    detail::error_check("IsTransformFeedbackNV"sv, check_errors);
    return out == GL_TRUE ? true : false;
}

/*!
 * \brief Part of GL_NV_transform_feedback2

 * \return void
 */
STATICINLINE void pause_transform_feedback(
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PauseTransformFeedbackNV)
    }
    glPauseTransformFeedbackNV();
    detail::error_check("PauseTransformFeedbackNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_transform_feedback2

 * \return void
 */
STATICINLINE void resume_transform_feedback(
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ResumeTransformFeedbackNV)
    }
    glResumeTransformFeedbackNV();
    detail::error_check("ResumeTransformFeedbackNV"sv, check_errors);
}

} // namespace gl::nv::transform_feedback2
#endif // GL_NV_transform_feedback2
namespace gl::nv::transform_feedback2 {
constexpr auto name = "GL_NV_transform_feedback2";
}
