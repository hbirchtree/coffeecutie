#pragma once

#ifdef GL_ARB_transform_feedback2
#include "../enums/BindTransformFeedbackTarget.h"
#include "../enums/ObjectIdentifier.h"

namespace gl::arb::transform_feedback2 {
using gl::group::bind_transform_feedback_target;
using gl::group::object_identifier;

namespace values {
constexpr u32 transform_feedback_buffer_paused = 0x8E23;
constexpr u32 transform_feedback_buffer_active = 0x8E24;
constexpr u32 transform_feedback_binding       = 0x8E25;
} // namespace values

/*!
 * \brief Part of GL_ARB_transform_feedback2
 * \param target GLenum
 * \param id GLuint
 * \return void
 */
STATICINLINE void bind_transform_feedback(
    group::bind_transform_feedback_target target,
    u32                                   id,
    error_check                           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindTransformFeedback)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBindTransformFeedback, target, id);
#endif
    glBindTransformFeedback(static_cast<GLenum>(target), id);
    detail::error_check("BindTransformFeedback"sv, check_errors);
}

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_ARB_transform_feedback2
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
        GLW_FPTR_CHECK(DeleteTransformFeedbacks)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glDeleteTransformFeedbacks,
        gsl::span<const char>(
            reinterpret_cast<const char*>(ids.data()), ids.size_bytes()));
#endif
    glDeleteTransformFeedbacks(
        ids.size(),
        ids.size() ? reinterpret_cast<const GLuint*>(ids.data()) : nullptr);
    detail::error_check("DeleteTransformFeedbacks"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_transform_feedback2
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
        GLW_FPTR_CHECK(DrawTransformFeedback)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glDrawTransformFeedback, mode, id);
#endif
    glDrawTransformFeedback(static_cast<GLenum>(mode), id);
    detail::error_check("DrawTransformFeedback"sv, check_errors);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_ARB_transform_feedback2
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
        GLW_FPTR_CHECK(GenTransformFeedbacks)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGenTransformFeedbacks,
        gsl::span<char>(reinterpret_cast<char*>(ids.data()), ids.size_bytes()));
#endif
    glGenTransformFeedbacks(
        ids.size(),
        ids.size() ? reinterpret_cast<GLuint*>(ids.data()) : nullptr);
    detail::error_check("GenTransformFeedbacks"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_transform_feedback2
 * \param id GLuint
 * \return GLboolean
 */
STATICINLINE GLboolean
is_transform_feedback(u32 id, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsTransformFeedback)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glIsTransformFeedback, id);
#endif
    auto out = glIsTransformFeedback(id);
    detail::error_check("IsTransformFeedback"sv, check_errors);
    return out;
}

/*!
 * \brief Part of GL_ARB_transform_feedback2

 * \return void
 */
STATICINLINE void pause_transform_feedback(
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PauseTransformFeedback)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glPauseTransformFeedback);
#endif
    glPauseTransformFeedback();
    detail::error_check("PauseTransformFeedback"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_transform_feedback2

 * \return void
 */
STATICINLINE void resume_transform_feedback(
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ResumeTransformFeedback)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glResumeTransformFeedback);
#endif
    glResumeTransformFeedback();
    detail::error_check("ResumeTransformFeedback"sv, check_errors);
}

} // namespace gl::arb::transform_feedback2
#endif // GL_ARB_transform_feedback2
namespace gl::arb::transform_feedback2 {
constexpr auto name = "GL_ARB_transform_feedback2";
}
