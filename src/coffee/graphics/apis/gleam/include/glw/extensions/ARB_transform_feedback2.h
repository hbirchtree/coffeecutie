#pragma once

#ifdef GL_ARB_transform_feedback2
#include "../enums/BindTransformFeedbackTarget.h"
#include "../enums/ObjectIdentifier.h"
namespace gl::arb::transform_feedback2 {
using gl::group::bind_transform_feedback_target;
using gl::group::object_identifier;
namespace values {
constexpr libc_types::u32 transform_feedback_buffer_paused = 0x8E23;
constexpr libc_types::u32 transform_feedback_buffer_active = 0x8E24;
constexpr libc_types::u32 transform_feedback_binding       = 0x8E25;
} // namespace values
STATICINLINE void bind_transform_feedback(
    group::bind_transform_feedback_target target, u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindTransformFeedback)
    }
    glBindTransformFeedback(static_cast<GLenum>(target), id);
    detail::error_check("BindTransformFeedback"sv);
}

template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void delete_transform_feedbacks(span_const_u32 const& ids)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteTransformFeedbacks)
    }
    glDeleteTransformFeedbacks(
        ids.size(),
        ids.size() ? reinterpret_cast<const GLuint*>(ids.data()) : nullptr);
    detail::error_check("DeleteTransformFeedbacks"sv);
}

STATICINLINE void draw_transform_feedback(group::primitive_type mode, u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawTransformFeedback)
    }
    glDrawTransformFeedback(static_cast<GLenum>(mode), id);
    detail::error_check("DrawTransformFeedback"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void gen_transform_feedbacks(span_u32 ids)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenTransformFeedbacks)
    }
    glGenTransformFeedbacks(
        ids.size(),
        ids.size() ? reinterpret_cast<GLuint*>(ids.data()) : nullptr);
    detail::error_check("GenTransformFeedbacks"sv);
}

STATICINLINE GLboolean is_transform_feedback(u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsTransformFeedback)
    }
    auto out = glIsTransformFeedback(id);
    detail::error_check("IsTransformFeedback"sv);
    return out;
}

STATICINLINE void pause_transform_feedback()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PauseTransformFeedback)
    }
    glPauseTransformFeedback();
    detail::error_check("PauseTransformFeedback"sv);
}

STATICINLINE void resume_transform_feedback()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ResumeTransformFeedback)
    }
    glResumeTransformFeedback();
    detail::error_check("ResumeTransformFeedback"sv);
}

} // namespace gl::arb::transform_feedback2
#endif // GL_ARB_transform_feedback2
namespace gl::arb::transform_feedback2 {
constexpr auto name = "GL_ARB_transform_feedback2";
}
