#pragma once

#ifdef GL_ARB_get_program_binary
#include "../enums/GetPName.h"
#include "../enums/HintTarget.h"
#include "../enums/ProgramParameterPName.h"
#include "../enums/ProgramPropertyARB.h"

namespace gl::arb::get_program_binary {
using gl::group::get_prop;
using gl::group::hint_target;
using gl::group::program_parameter_prop;
using gl::group::program_property_arb;

namespace values {
} // namespace values

template<class span_void>
requires(concepts::span<span_void>)
/*!
 * \brief Part of GL_ARB_get_program_binary
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
        GLW_FPTR_CHECK(GetProgramBinary)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetProgramBinary,
        gsl::span<char>(
            reinterpret_cast<char*>(binary.data()), binary.size_bytes()),
        program,
        length,
        binaryFormat);
#endif
    glGetProgramBinary(
        program,
        binary.size() * sizeof(typename std::decay_t<span_void>::value_type),
        &length,
        &binaryFormat,
        binary.size() ? reinterpret_cast<void*>(binary.data()) : nullptr);
    detail::error_check("GetProgramBinary"sv, check_errors);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_ARB_get_program_binary
 * \param program GLuint
 * \param binaryFormat GLenum
 * \param binary const void *
 * \param length GLsizei
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
        GLW_FPTR_CHECK(ProgramBinary)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glProgramBinary,
        gsl::span<const char>(
            reinterpret_cast<const char*>(binary.data()), binary.size_bytes()),
        program,
        binaryFormat,
        length);
#endif
    glProgramBinary(
        program,
        binaryFormat,
        binary.size() ? reinterpret_cast<const void*>(binary.data()) : nullptr,
        length);
    detail::error_check("ProgramBinary"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_get_program_binary
 * \param program GLuint
 * \param pname GLenum
 * \param value GLint
 * \return void
 */
STATICINLINE void program_parameter(
    u32                           program,
    group::program_parameter_prop pname,
    i32                           value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramParameteri)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glProgramParameteri, program, pname, value);
#endif
    glProgramParameteri(program, static_cast<GLenum>(pname), value);
    detail::error_check("ProgramParameteri"sv, check_errors);
}

} // namespace gl::arb::get_program_binary
#endif // GL_ARB_get_program_binary
namespace gl::arb::get_program_binary {
constexpr auto name = "GL_ARB_get_program_binary";
}
