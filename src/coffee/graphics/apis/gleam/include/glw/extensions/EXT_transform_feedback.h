#pragma once

#ifdef GL_EXT_transform_feedback
namespace gl::ext::transform_feedback {
namespace values {
constexpr libc_types::u32 transform_feedback_varying_max_length      = 0x8C76;
constexpr libc_types::u32 transform_feedback_buffer_mode             = 0x8C7F;
constexpr libc_types::u32 max_transform_feedback_separate_components = 0x8C80;
constexpr libc_types::u32 transform_feedback_varyings                = 0x8C83;
constexpr libc_types::u32 transform_feedback_buffer_start            = 0x8C84;
constexpr libc_types::u32 transform_feedback_buffer_size             = 0x8C85;
constexpr libc_types::u32 primitives_generated                       = 0x8C87;
constexpr libc_types::u32 transform_feedback_primitives_written      = 0x8C88;
constexpr libc_types::u32 rasterizer_discard                         = 0x8C89;
constexpr libc_types::u32 max_transform_feedback_interleaved_components =
    0x8C8A;
constexpr libc_types::u32 max_transform_feedback_separate_attribs = 0x8C8B;
constexpr libc_types::u32 interleaved_attribs                     = 0x8C8C;
constexpr libc_types::u32 separate_attribs                        = 0x8C8D;
constexpr libc_types::u32 transform_feedback_buffer               = 0x8C8E;
constexpr libc_types::u32 transform_feedback_buffer_binding       = 0x8C8F;
} // namespace values
STATICINLINE void begin_transform_feedback(group::primitive_type primitiveMode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BeginTransformFeedbackEXT)
    }
    glBeginTransformFeedbackEXT(static_cast<GLenum>(primitiveMode));
    detail::error_check("BeginTransformFeedbackEXT"sv);
}

STATICINLINE void bind_buffer_base(
    group::buffer_target_arb target, u32 index, u32 buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindBufferBaseEXT)
        glIsBuffer(buffer);
    }
    glBindBufferBaseEXT(static_cast<GLenum>(target), index, buffer);
    detail::error_check("BindBufferBaseEXT"sv);
}

STATICINLINE void bind_buffer_offset(
    group::buffer_target_arb target, u32 index, u32 buffer, GLintptr offset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindBufferOffsetEXT)
        glIsBuffer(buffer);
    }
    glBindBufferOffsetEXT(static_cast<GLenum>(target), index, buffer, offset);
    detail::error_check("BindBufferOffsetEXT"sv);
}

STATICINLINE void bind_buffer_range(
    group::buffer_target_arb target,
    u32                      index,
    u32                      buffer,
    GLintptr                 offset,
    GLsizeiptr               size)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindBufferRangeEXT)
        glIsBuffer(buffer);
    }
    glBindBufferRangeEXT(
        static_cast<GLenum>(target), index, buffer, offset, size);
    detail::error_check("BindBufferRangeEXT"sv);
}

STATICINLINE void end_transform_feedback()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EndTransformFeedbackEXT)
    }
    glEndTransformFeedbackEXT();
    detail::error_check("EndTransformFeedbackEXT"sv);
}

template<class span_GLchar>
requires(semantic::concepts::Span<span_GLchar>&& std::is_same_v<
         std::decay_t<typename span_GLchar::value_type>,
         std::decay_t<GLchar>>) STATICINLINE
    void get_transform_feedback_varying(
        u32         program,
        u32         index,
        i32&        length,
        i32&        size,
        GLenum&     type,
        span_GLchar name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTransformFeedbackVaryingEXT)
        glIsProgram(program);
    }
    glGetTransformFeedbackVaryingEXT(
        program, index, name.size(), &length, &size, &type, name.data());
    detail::error_check("GetTransformFeedbackVaryingEXT"sv);
}

STATICINLINE void transform_feedback_varyings(
    u32 program, std::vector<std::string_view> varyings, GLenum bufferMode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TransformFeedbackVaryingsEXT)
        glIsProgram(program);
    }
    auto [varyings_lens, varyings_cstr, varyings_store] =
        detail::transform_strings(varyings);
    glTransformFeedbackVaryingsEXT(
        program, varyings_cstr.size(), varyings_cstr.data(), bufferMode);
    detail::error_check("TransformFeedbackVaryingsEXT"sv);
}

} // namespace gl::ext::transform_feedback
#endif // GL_EXT_transform_feedback
namespace gl::ext::transform_feedback {
constexpr auto name = "GL_EXT_transform_feedback";
}
