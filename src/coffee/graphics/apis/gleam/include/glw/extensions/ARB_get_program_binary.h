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
requires(semantic::concepts::Span<span_void>)
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
    u32 program, i32& length, GLenum& binaryFormat, span_void binary)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramBinary)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetProgramBinary(
        program,
        binary.size() * sizeof(typename std::decay_t<span_void>::value_type),
        &length,
        &binaryFormat,
        binary.size() ? reinterpret_cast<void*>(binary.data()) : nullptr);
    detail::error_check("GetProgramBinary"sv);
}

template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>)
/*!
 * \brief Part of GL_ARB_get_program_binary
 * \param program GLuint
 * \param binaryFormat GLenum
 * \param binary const void *
 * \param length GLsizei
 * \return void
 */
STATICINLINE void program_binary(
    u32 program, GLenum binaryFormat, span_const_void const& binary, i32 length)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramBinary)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramBinary(
        program,
        binaryFormat,
        binary.size() ? reinterpret_cast<const void*>(binary.data()) : nullptr,
        length);
    detail::error_check("ProgramBinary"sv);
}

/*!
 * \brief Part of GL_ARB_get_program_binary
 * \param program GLuint
 * \param pname GLenum
 * \param value GLint
 * \return void
 */
STATICINLINE void program_parameter(
    u32 program, group::program_parameter_prop pname, i32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramParameteri)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramParameteri(program, static_cast<GLenum>(pname), value);
    detail::error_check("ProgramParameteri"sv);
}

} // namespace gl::arb::get_program_binary
#endif // GL_ARB_get_program_binary
namespace gl::arb::get_program_binary {
constexpr auto name = "GL_ARB_get_program_binary";
}
