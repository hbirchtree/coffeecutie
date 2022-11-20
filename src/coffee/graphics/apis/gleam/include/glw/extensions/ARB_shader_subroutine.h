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
constexpr libc_types::u32 max_subroutines                  = 0x8DE7;
constexpr libc_types::u32 max_subroutine_uniform_locations = 0x8DE8;
} // namespace values
template<class span_GLchar>
requires(semantic::concepts::Span<span_GLchar>&& std::is_same_v<
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
        span_GLchar        name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveSubroutineName)
        glIsProgram(program);
    }
    glGetActiveSubroutineName(
        program,
        static_cast<GLenum>(shadertype),
        index,
        name.size(),
        &length,
        name.data());
    detail::error_check("GetActiveSubroutineName"sv);
}

template<class span_GLchar>
requires(semantic::concepts::Span<span_GLchar>&& std::is_same_v<
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
        span_GLchar        name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveSubroutineUniformName)
        glIsProgram(program);
    }
    glGetActiveSubroutineUniformName(
        program,
        static_cast<GLenum>(shadertype),
        index,
        name.size(),
        &length,
        name.data());
    detail::error_check("GetActiveSubroutineUniformName"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
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
        span_i32                         values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveSubroutineUniformiv)
        glIsProgram(program);
    }
    glGetActiveSubroutineUniformiv(
        program,
        static_cast<GLenum>(shadertype),
        index,
        static_cast<GLenum>(pname),
        values.size() ? reinterpret_cast<GLint*>(values.data()) : nullptr);
    detail::error_check("GetActiveSubroutineUniformiv"sv);
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
    i32&                      values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramStageiv)
        glIsProgram(program);
    }
    glGetProgramStageiv(
        program,
        static_cast<GLenum>(shadertype),
        static_cast<GLenum>(pname),
        &values);
    detail::error_check("GetProgramStageiv"sv);
}

/*!
 * \brief Part of GL_ARB_shader_subroutine
 * \param program GLuint
 * \param shadertype GLenum
 * \param name const GLchar *
 * \return GLuint
 */
STATICINLINE GLuint get_subroutine_index(
    u32 program, group::shader_type shadertype, std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSubroutineIndex)
        glIsProgram(program);
    }
    auto out = glGetSubroutineIndex(
        program, static_cast<GLenum>(shadertype), name.data());
    detail::error_check("GetSubroutineIndex"sv);
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
    u32 program, group::shader_type shadertype, std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSubroutineUniformLocation)
        glIsProgram(program);
    }
    auto out = glGetSubroutineUniformLocation(
        program, static_cast<GLenum>(shadertype), name.data());
    detail::error_check("GetSubroutineUniformLocation"sv);
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
    group::shader_type shadertype, i32 location, u32& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformSubroutineuiv)
    }
    glGetUniformSubroutineuiv(
        static_cast<GLenum>(shadertype), location, &params);
    detail::error_check("GetUniformSubroutineuiv"sv);
}

template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
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
        group::shader_type shadertype, span_const_u32 const& indices)
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
    detail::error_check("UniformSubroutinesuiv"sv);
}

} // namespace gl::arb::shader_subroutine
#endif // GL_ARB_shader_subroutine
namespace gl::arb::shader_subroutine {
constexpr auto name = "GL_ARB_shader_subroutine";
}
