#pragma once

#ifdef GL_OES_get_program_binary
namespace gl::oes::get_program_binary {
namespace values {
constexpr u32 program_binary_length      = 0x8741;
constexpr u32 num_program_binary_formats = 0x87FE;
constexpr u32 program_binary_formats     = 0x87FF;
} // namespace values

template<class span_void>
requires(concepts::span<span_void>)
/*!
 * \brief Part of GL_OES_get_program_binary
 * \param program GLuint
 * \param bufSize GLsizei
 * \param length GLsizei *
 * \param binaryFormat GLenum *
 * \param binary void *
 * \return void
 */
STATICINLINE void get_program_binary(
    u32         program,
    i32&        length,
    GLenum&     binaryFormat,
    span_void   binary,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramBinaryOES)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetProgramBinaryOES(
        program,
        binary.size() * sizeof(typename std::decay_t<span_void>::value_type),
        &length,
        &binaryFormat,
        binary.size() ? reinterpret_cast<void*>(binary.data()) : nullptr);
    detail::error_check("GetProgramBinaryOES"sv, check_errors);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_OES_get_program_binary
 * \param program GLuint
 * \param binaryFormat GLenum
 * \param binary const void *
 * \param length GLint
 * \return void
 */
STATICINLINE void program_binary(
    u32                    program,
    GLenum                 binaryFormat,
    span_const_void const& binary,
    i32                    length,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramBinaryOES)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramBinaryOES(
        program,
        binaryFormat,
        binary.size() ? reinterpret_cast<const void*>(binary.data()) : nullptr,
        length);
    detail::error_check("ProgramBinaryOES"sv, check_errors);
}

} // namespace gl::oes::get_program_binary
#endif // GL_OES_get_program_binary
namespace gl::oes::get_program_binary {
constexpr auto name = "GL_OES_get_program_binary";
}
