#pragma once

#ifdef GL_EXT_transform_feedback
namespace gl::ext::transform_feedback {
namespace values {
constexpr u32 transform_feedback_varying_max_length         = 0x8C76;
constexpr u32 transform_feedback_buffer_mode                = 0x8C7F;
constexpr u32 max_transform_feedback_separate_components    = 0x8C80;
constexpr u32 transform_feedback_varyings                   = 0x8C83;
constexpr u32 transform_feedback_buffer_start               = 0x8C84;
constexpr u32 transform_feedback_buffer_size                = 0x8C85;
constexpr u32 primitives_generated                          = 0x8C87;
constexpr u32 transform_feedback_primitives_written         = 0x8C88;
constexpr u32 rasterizer_discard                            = 0x8C89;
constexpr u32 max_transform_feedback_interleaved_components = 0x8C8A;
constexpr u32 max_transform_feedback_separate_attribs       = 0x8C8B;
constexpr u32 interleaved_attribs                           = 0x8C8C;
constexpr u32 separate_attribs                              = 0x8C8D;
constexpr u32 transform_feedback_buffer                     = 0x8C8E;
constexpr u32 transform_feedback_buffer_binding             = 0x8C8F;
} // namespace values

/*!
 * \brief Part of GL_EXT_transform_feedback
 * \param primitiveMode GLenum
 * \return void
 */
STATICINLINE void begin_transform_feedback(
    group::primitive_type primitiveMode,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BeginTransformFeedbackEXT)
    }
    glBeginTransformFeedbackEXT(static_cast<GLenum>(primitiveMode));
    detail::error_check("BeginTransformFeedbackEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_transform_feedback
 * \param target GLenum
 * \param index GLuint
 * \param buffer GLuint
 * \return void
 */
STATICINLINE void bind_buffer_base(
    group::buffer_target_arb target,
    u32                      index,
    u32                      buffer,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindBufferBaseEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glBindBufferBaseEXT(static_cast<GLenum>(target), index, buffer);
    detail::error_check("BindBufferBaseEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_transform_feedback
 * \param target GLenum
 * \param index GLuint
 * \param buffer GLuint
 * \param offset GLintptr
 * \return void
 */
STATICINLINE void bind_buffer_offset(
    group::buffer_target_arb target,
    u32                      index,
    u32                      buffer,
    GLintptr                 offset,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindBufferOffsetEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glBindBufferOffsetEXT(static_cast<GLenum>(target), index, buffer, offset);
    detail::error_check("BindBufferOffsetEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_transform_feedback
 * \param target GLenum
 * \param index GLuint
 * \param buffer GLuint
 * \param offset GLintptr
 * \param size GLsizeiptr
 * \return void
 */
STATICINLINE void bind_buffer_range(
    group::buffer_target_arb target,
    u32                      index,
    u32                      buffer,
    GLintptr                 offset,
    GLsizeiptr               size,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindBufferRangeEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glBindBufferRangeEXT(
        static_cast<GLenum>(target), index, buffer, offset, size);
    detail::error_check("BindBufferRangeEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_transform_feedback

 * \return void
 */
STATICINLINE void end_transform_feedback(
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EndTransformFeedbackEXT)
    }
    glEndTransformFeedbackEXT();
    detail::error_check("EndTransformFeedbackEXT"sv, check_errors);
}

template<class span_GLchar>
requires(
    concepts::span<span_GLchar> &&
    std::is_same_v<
        std::decay_t<typename span_GLchar::value_type>,
        std::decay_t<GLchar>>)
/*!
 * \brief Part of GL_EXT_transform_feedback
 * \param program GLuint
 * \param index GLuint
 * \param bufSize GLsizei
 * \param length GLsizei *
 * \param size GLsizei *
 * \param type GLenum *
 * \param name GLchar *
 * \return void
 */
STATICINLINE void get_transform_feedback_varying(
    u32         program,
    u32         index,
    i32&        length,
    i32&        size,
    GLenum&     type,
    span_GLchar name,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTransformFeedbackVaryingEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetTransformFeedbackVaryingEXT(
        program, index, name.size(), &length, &size, &type, name.data());
    detail::error_check("GetTransformFeedbackVaryingEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_transform_feedback
 * \param program GLuint
 * \param count GLsizei
 * \param varyings const GLchar *const*
 * \param bufferMode GLenum
 * \return void
 */
STATICINLINE void transform_feedback_varyings(
    u32                                   program,
    std::vector<std::string_view>         varyings,
    group::transform_feedback_buffer_mode bufferMode,
    error_check                           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TransformFeedbackVaryingsEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    auto [varyings_lens, varyings_cstr, varyings_store] =
        detail::transform_strings(varyings);
    glTransformFeedbackVaryingsEXT(
        program,
        varyings_cstr.size(),
        varyings_cstr.data(),
        static_cast<GLenum>(bufferMode));
    detail::error_check("TransformFeedbackVaryingsEXT"sv, check_errors);
}

} // namespace gl::ext::transform_feedback
#endif // GL_EXT_transform_feedback
namespace gl::ext::transform_feedback {
constexpr auto name = "GL_EXT_transform_feedback";
}
