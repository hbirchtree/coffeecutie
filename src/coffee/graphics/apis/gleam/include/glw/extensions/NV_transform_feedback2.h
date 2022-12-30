#pragma once

#ifdef GL_NV_transform_feedback2
namespace gl::nv::transform_feedback2 {
namespace values {
constexpr libc_types::u32 transform_feedback               = 0x8E22;
constexpr libc_types::u32 transform_feedback_buffer_paused = 0x8E23;
constexpr libc_types::u32 transform_feedback_buffer_active = 0x8E24;
constexpr libc_types::u32 transform_feedback_binding       = 0x8E25;
} // namespace values
/*!
 * \brief Part of GL_NV_transform_feedback2
 * \param target GLenum
 * \param id GLuint
 * \return void
 */
STATICINLINE void bind_transform_feedback(
    group::buffer_target_arb target, u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindTransformFeedbackNV)
    }
    glBindTransformFeedbackNV(static_cast<GLenum>(target), id);
    detail::error_check("BindTransformFeedbackNV"sv);
}

template<class span_const_u32>
requires(
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_NV_transform_feedback2
 * \param n GLsizei
 * \param ids const GLuint *
 * \return void
 */
STATICINLINE void delete_transform_feedbacks(span_const_u32 const& ids)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteTransformFeedbacksNV)
    }
    glDeleteTransformFeedbacksNV(
        ids.size(),
        ids.size() ? reinterpret_cast<const GLuint*>(ids.data()) : nullptr);
    detail::error_check("DeleteTransformFeedbacksNV"sv);
}

/*!
 * \brief Part of GL_NV_transform_feedback2
 * \param mode GLenum
 * \param id GLuint
 * \return void
 */
STATICINLINE void draw_transform_feedback(group::primitive_type mode, u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawTransformFeedbackNV)
    }
    glDrawTransformFeedbackNV(static_cast<GLenum>(mode), id);
    detail::error_check("DrawTransformFeedbackNV"sv);
}

template<class span_u32>
requires(
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_NV_transform_feedback2
 * \param n GLsizei
 * \param ids GLuint *
 * \return void
 */
STATICINLINE void gen_transform_feedbacks(span_u32 ids)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenTransformFeedbacksNV)
    }
    glGenTransformFeedbacksNV(
        ids.size(),
        ids.size() ? reinterpret_cast<GLuint*>(ids.data()) : nullptr);
    detail::error_check("GenTransformFeedbacksNV"sv);
}

/*!
 * \brief Part of GL_NV_transform_feedback2
 * \param id GLuint
 * \return Boolean
 */
STATICINLINE GLboolean is_transform_feedback(u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsTransformFeedbackNV)
    }
    auto out = glIsTransformFeedbackNV(id);
    detail::error_check("IsTransformFeedbackNV"sv);
    return out;
}

/*!
 * \brief Part of GL_NV_transform_feedback2

 * \return void
 */
STATICINLINE void pause_transform_feedback()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PauseTransformFeedbackNV)
    }
    glPauseTransformFeedbackNV();
    detail::error_check("PauseTransformFeedbackNV"sv);
}

/*!
 * \brief Part of GL_NV_transform_feedback2

 * \return void
 */
STATICINLINE void resume_transform_feedback()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ResumeTransformFeedbackNV)
    }
    glResumeTransformFeedbackNV();
    detail::error_check("ResumeTransformFeedbackNV"sv);
}

} // namespace gl::nv::transform_feedback2
#endif // GL_NV_transform_feedback2
namespace gl::nv::transform_feedback2 {
constexpr auto name = "GL_NV_transform_feedback2";
}
