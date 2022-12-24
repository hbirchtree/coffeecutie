#pragma once

#ifdef GL_ARB_program_interface_query
#include "../enums/BufferStorageTarget.h"
#include "../enums/BufferTargetARB.h"
#include "../enums/CopyBufferSubDataTarget.h"
#include "../enums/ProgramInterface.h"
#include "../enums/ProgramInterfacePName.h"
#include "../enums/ProgramResourceProperty.h"
#include "../enums/SubroutineParameterName.h"
namespace gl::arb::program_interface_query {
using gl::group::buffer_storage_target;
using gl::group::buffer_target_arb;
using gl::group::copy_buffer_sub_data_target;
using gl::group::program_interface;
using gl::group::program_interface_prop;
using gl::group::program_resource_property;
using gl::group::subroutine_parameter_name;
namespace values {
} // namespace values
template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_ARB_program_interface_query
     * \param program GLuint
     * \param programInterface GLenum
     * \param pname GLenum
     * \param params GLint *
     * \return void
     */
    STATICINLINE void get_program_interfaceiv(
        u32                           program,
        group::program_interface      programInterface,
        group::program_interface_prop pname,
        span_i32                      params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramInterfaceiv)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetProgramInterfaceiv(
        program,
        static_cast<GLenum>(programInterface),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetProgramInterfaceiv"sv);
}

/*!
 * \brief Part of GL_ARB_program_interface_query
 * \param program GLuint
 * \param programInterface GLenum
 * \param name const GLchar *
 * \return GLuint
 */
STATICINLINE GLuint get_program_resource_index(
    u32                      program,
    group::program_interface programInterface,
    std::string_view const&  name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramResourceIndex)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    auto out = glGetProgramResourceIndex(
        program, static_cast<GLenum>(programInterface), name.data());
    detail::error_check("GetProgramResourceIndex"sv);
    return out;
}

/*!
 * \brief Part of GL_ARB_program_interface_query
 * \param program GLuint
 * \param programInterface GLenum
 * \param name const GLchar *
 * \return GLint
 */
STATICINLINE GLint get_program_resource_location(
    u32                      program,
    group::program_interface programInterface,
    std::string_view const&  name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramResourceLocation)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    auto out = glGetProgramResourceLocation(
        program, static_cast<GLenum>(programInterface), name.data());
    detail::error_check("GetProgramResourceLocation"sv);
    return out;
}

/*!
 * \brief Part of GL_ARB_program_interface_query
 * \param program GLuint
 * \param programInterface GLenum
 * \param name const GLchar *
 * \return GLint
 */
STATICINLINE GLint get_program_resource_location_index(
    u32                      program,
    group::program_interface programInterface,
    std::string_view const&  name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramResourceLocationIndex)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    auto out = glGetProgramResourceLocationIndex(
        program, static_cast<GLenum>(programInterface), name.data());
    detail::error_check("GetProgramResourceLocationIndex"sv);
    return out;
}

template<class span_GLchar>
requires(semantic::concepts::Span<span_GLchar>&& std::is_same_v<
         std::decay_t<typename span_GLchar::value_type>,
         std::decay_t<GLchar>>)
    /*!
     * \brief Part of GL_ARB_program_interface_query
     * \param program GLuint
     * \param programInterface GLenum
     * \param index GLuint
     * \param bufSize GLsizei
     * \param length GLsizei *
     * \param name GLchar *
     * \return void
     */
    STATICINLINE void get_program_resource_name(
        u32                      program,
        group::program_interface programInterface,
        u32                      index,
        i32&                     length,
        span_GLchar              name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramResourceName)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetProgramResourceName(
        program,
        static_cast<GLenum>(programInterface),
        index,
        name.size(),
        &length,
        name.data());
    detail::error_check("GetProgramResourceName"sv);
}

template<class span_const_program_resource_property, class span_i32>
requires(semantic::concepts::Span<span_const_program_resource_property>&&
             std::is_same_v<
                 std::decay_t<
                     typename span_const_program_resource_property::value_type>,
                 std::decay_t<group::program_resource_property>>&&
                 semantic::concepts::Span<span_i32>&& std::is_same_v<
                     std::decay_t<typename span_i32::value_type>,
                     std::decay_t<i32>>)
    /*!
     * \brief Part of GL_ARB_program_interface_query
     * \param program GLuint
     * \param programInterface GLenum
     * \param index GLuint
     * \param propCount GLsizei
     * \param props const GLenum *
     * \param count GLsizei
     * \param length GLsizei *
     * \param params GLint *
     * \return void
     */
    STATICINLINE void get_program_resourceiv(
        u32                                         program,
        group::program_interface                    programInterface,
        u32                                         index,
        span_const_program_resource_property const& props,
        i32&                                        length,
        span_i32                                    params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramResourceiv)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetProgramResourceiv(
        program,
        static_cast<GLenum>(programInterface),
        index,
        props.size(),
        props.size() ? reinterpret_cast<const GLenum*>(props.data()) : nullptr,
        params.size(),
        &length,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetProgramResourceiv"sv);
}

} // namespace gl::arb::program_interface_query
#endif // GL_ARB_program_interface_query
namespace gl::arb::program_interface_query {
constexpr auto name = "GL_ARB_program_interface_query";
}
