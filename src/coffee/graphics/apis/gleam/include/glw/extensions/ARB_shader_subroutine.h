#pragma once

#ifdef GL_ARB_shader_subroutine
#include "../enums/ProgramResourceProperty.h"
#include "../enums/ProgramStagePName.h"
#include "../enums/SubroutineParameterName.h"
#include "../enums/UniformPName.h"

namespace gl::arb::shader_subroutine {
using gl::group::program_resource_property;
using gl::group::program_stage_prop;
using gl::group::subroutine_parameter_name;
using gl::group::uniform_prop;

namespace values {
constexpr u32 max_subroutines                  = 0x8DE7;
constexpr u32 max_subroutine_uniform_locations = 0x8DE8;
} // namespace values

template<class span_GLchar>
requires(
    concepts::span<span_GLchar> &&
    std::is_same_v<
        std::decay_t<typename span_GLchar::value_type>,
        std::decay_t<GLchar>>)
/*!
 * \brief Part of GL_ARB_shader_subroutine
 * \param program GLuint
 * \param shadertype GLenum
 * \param index GLuint
 * \param bufSize GLsizei
 * \param length GLsizei *
 * \param name GLchar *
 * \return void
 */
STATICINLINE void get_active_subroutine_name(
    u32                program,
    group::shader_type shadertype,
    u32                index,
    i32&               length,
    span_GLchar        name,
    error_check        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveSubroutineName)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetActiveSubroutineName(
        program,
        static_cast<GLenum>(shadertype),
        index,
        name.size(),
        &length,
        name.data());
    detail::error_check("GetActiveSubroutineName"sv, check_errors);
}

template<class span_GLchar>
requires(
    concepts::span<span_GLchar> &&
    std::is_same_v<
        std::decay_t<typename span_GLchar::value_type>,
        std::decay_t<GLchar>>)
/*!
 * \brief Part of GL_ARB_shader_subroutine
 * \param program GLuint
 * \param shadertype GLenum
 * \param index GLuint
 * \param bufSize GLsizei
 * \param length GLsizei *
 * \param name GLchar *
 * \return void
 */
STATICINLINE void get_active_subroutine_uniform_name(
    u32                program,
    group::shader_type shadertype,
    u32                index,
    i32&               length,
    span_GLchar        name,
    error_check        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveSubroutineUniformName)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetActiveSubroutineUniformName(
        program,
        static_cast<GLenum>(shadertype),
        index,
        name.size(),
        &length,
        name.data());
    detail::error_check("GetActiveSubroutineUniformName"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_ARB_shader_subroutine
 * \param program GLuint
 * \param shadertype GLenum
 * \param index GLuint
 * \param pname GLenum
 * \param values GLint *
 * \return void
 */
STATICINLINE void get_active_subroutine_uniformiv(
    u32                              program,
    group::shader_type               shadertype,
    u32                              index,
    group::subroutine_parameter_name pname,
    span_i32                         values,
    error_check                      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveSubroutineUniformiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetActiveSubroutineUniformiv(
        program,
        static_cast<GLenum>(shadertype),
        index,
        static_cast<GLenum>(pname),
        values.size() ? reinterpret_cast<GLint*>(values.data()) : nullptr);
    detail::error_check("GetActiveSubroutineUniformiv"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_shader_subroutine
 * \param program GLuint
 * \param shadertype GLenum
 * \param pname GLenum
 * \param values GLint *
 * \return void
 */
STATICINLINE void get_program_stageiv(
    u32                       program,
    group::shader_type        shadertype,
    group::program_stage_prop pname,
    i32&                      values,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramStageiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetProgramStageiv(
        program,
        static_cast<GLenum>(shadertype),
        static_cast<GLenum>(pname),
        &values);
    detail::error_check("GetProgramStageiv"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_shader_subroutine
 * \param program GLuint
 * \param shadertype GLenum
 * \param name const GLchar *
 * \return GLuint
 */
STATICINLINE GLuint get_subroutine_index(
    u32                     program,
    group::shader_type      shadertype,
    std::string_view const& name,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSubroutineIndex)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    auto out = glGetSubroutineIndex(
        program, static_cast<GLenum>(shadertype), name.data());
    detail::error_check("GetSubroutineIndex"sv, check_errors);
    return out;
}

/*!
 * \brief Part of GL_ARB_shader_subroutine
 * \param program GLuint
 * \param shadertype GLenum
 * \param name const GLchar *
 * \return GLint
 */
STATICINLINE GLint get_subroutine_uniform_location(
    u32                     program,
    group::shader_type      shadertype,
    std::string_view const& name,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSubroutineUniformLocation)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    auto out = glGetSubroutineUniformLocation(
        program, static_cast<GLenum>(shadertype), name.data());
    detail::error_check("GetSubroutineUniformLocation"sv, check_errors);
    return out;
}

/*!
 * \brief Part of GL_ARB_shader_subroutine
 * \param shadertype GLenum
 * \param location GLint
 * \param params GLuint *
 * \return void
 */
STATICINLINE void get_uniform_subroutineuiv(
    group::shader_type shadertype,
    i32                location,
    u32&               params,
    error_check        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformSubroutineuiv)
    }
    glGetUniformSubroutineuiv(
        static_cast<GLenum>(shadertype), location, &params);
    detail::error_check("GetUniformSubroutineuiv"sv, check_errors);
}

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_ARB_shader_subroutine
 * \param shadertype GLenum
 * \param count GLsizei
 * \param indices const GLuint *
 * \return void
 */
STATICINLINE void uniform_subroutines(
    group::shader_type    shadertype,
    span_const_u32 const& indices,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformSubroutinesuiv)
    }
    glUniformSubroutinesuiv(
        static_cast<GLenum>(shadertype),
        indices.size(),
        indices.size() ? reinterpret_cast<const GLuint*>(indices.data())
                       : nullptr);
    detail::error_check("UniformSubroutinesuiv"sv, check_errors);
}

} // namespace gl::arb::shader_subroutine
#endif // GL_ARB_shader_subroutine
namespace gl::arb::shader_subroutine {
constexpr auto name = "GL_ARB_shader_subroutine";
}
