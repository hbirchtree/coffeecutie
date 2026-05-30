#pragma once

#ifdef GL_ARB_shader_objects
#include "../enums/AttributeType.h"
#include "../enums/ContainerType.h"

namespace gl::arb::shader_objects {
using gl::group::attribute_type;
using gl::group::container_type;

namespace values {
constexpr u32 shader_object                    = 0x8B48;
constexpr u32 object_type                      = 0x8B4E;
constexpr u32 object_subtype                   = 0x8B4F;
constexpr u32 object_delete_status             = 0x8B80;
constexpr u32 object_compile_status            = 0x8B81;
constexpr u32 object_link_status               = 0x8B82;
constexpr u32 object_validate_status           = 0x8B83;
constexpr u32 object_info_log_length           = 0x8B84;
constexpr u32 object_attached_objects          = 0x8B85;
constexpr u32 object_active_uniforms           = 0x8B86;
constexpr u32 object_active_uniform_max_length = 0x8B87;
constexpr u32 object_shader_source_length      = 0x8B88;
} // namespace values

/*!
 * \brief Part of GL_ARB_shader_objects
 * \param containerObj GLhandleARB
 * \param obj GLhandleARB
 * \return void
 */
STATICINLINE void attach_object(
    GLhandleARB containerObj,
    GLhandleARB obj,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(AttachObjectARB)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glAttachObjectARB, containerObj, obj);
#endif
    glAttachObjectARB(containerObj, obj);
    detail::error_check("AttachObjectARB"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_shader_objects
 * \param shaderObj GLhandleARB
 * \return void
 */
STATICINLINE void compile_shader(
    GLhandleARB shaderObj, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompileShaderARB)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glCompileShaderARB, shaderObj);
#endif
    glCompileShaderARB(shaderObj);
    detail::error_check("CompileShaderARB"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_shader_objects

 * \return GLhandleARB
 */
STATICINLINE GLhandleARB
create_program_object(error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateProgramObjectARB)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glCreateProgramObjectARB);
#endif
    auto out = glCreateProgramObjectARB();
    detail::error_check("CreateProgramObjectARB"sv, check_errors);
    return out;
}

/*!
 * \brief Part of GL_ARB_shader_objects
 * \param shaderType GLenum
 * \return GLhandleARB
 */
STATICINLINE GLhandleARB create_shader_object(
    group::shader_type shaderType, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateShaderObjectARB)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glCreateShaderObjectARB, shaderType);
#endif
    auto out = glCreateShaderObjectARB(static_cast<GLenum>(shaderType));
    detail::error_check("CreateShaderObjectARB"sv, check_errors);
    return out;
}

/*!
 * \brief Part of GL_ARB_shader_objects
 * \param obj GLhandleARB
 * \return void
 */
STATICINLINE void delete_object(
    GLhandleARB obj, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteObjectARB)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glDeleteObjectARB, obj);
#endif
    glDeleteObjectARB(obj);
    detail::error_check("DeleteObjectARB"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_shader_objects
 * \param containerObj GLhandleARB
 * \param attachedObj GLhandleARB
 * \return void
 */
STATICINLINE void detach_object(
    GLhandleARB containerObj,
    GLhandleARB attachedObj,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DetachObjectARB)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glDetachObjectARB, containerObj, attachedObj);
#endif
    glDetachObjectARB(containerObj, attachedObj);
    detail::error_check("DetachObjectARB"sv, check_errors);
}

template<class span_GLcharARB>
requires(
    concepts::span<span_GLcharARB> &&
    std::is_same_v<
        std::decay_t<typename span_GLcharARB::value_type>,
        std::decay_t<GLcharARB>>)
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param programObj GLhandleARB
 * \param index GLuint
 * \param maxLength GLsizei
 * \param length GLsizei *
 * \param size GLint *
 * \param type GLenum *
 * \param name GLcharARB *
 * \return void
 */
STATICINLINE void get_active_uniform(
    GLhandleARB    programObj,
    u32            index,
    i32&           length,
    i32&           size,
    GLenum&        type,
    span_GLcharARB name,
    error_check    check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveUniformARB)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetActiveUniformARB,
        gsl::span<char>(
            reinterpret_cast<char*>(name.data()), name.size_bytes()),
        programObj,
        index,
        length,
        size,
        type);
#endif
    glGetActiveUniformARB(
        programObj,
        index,
        name.size(),
        &length,
        &size,
        &type,
        name.size() ? reinterpret_cast<GLcharARB*>(name.data()) : nullptr);
    detail::error_check("GetActiveUniformARB"sv, check_errors);
}

template<class span_GLhandleARB>
requires(
    concepts::span<span_GLhandleARB> &&
    std::is_same_v<
        std::decay_t<typename span_GLhandleARB::value_type>,
        std::decay_t<GLhandleARB>>)
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param containerObj GLhandleARB
 * \param maxCount GLsizei
 * \param count GLsizei *
 * \param obj GLhandleARB *
 * \return void
 */
STATICINLINE void get_attached_objects(
    GLhandleARB      containerObj,
    i32&             count,
    span_GLhandleARB obj,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetAttachedObjectsARB)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetAttachedObjectsARB,
        gsl::span<char>(reinterpret_cast<char*>(obj.data()), obj.size_bytes()),
        containerObj,
        count);
#endif
    glGetAttachedObjectsARB(
        containerObj,
        obj.size(),
        &count,
        obj.size() ? reinterpret_cast<GLhandleARB*>(obj.data()) : nullptr);
    detail::error_check("GetAttachedObjectsARB"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_shader_objects
 * \param pname GLenum
 * \return GLhandleARB
 */
STATICINLINE GLhandleARB get_handle(
    group::container_type pname, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetHandleARB)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glGetHandleARB, pname);
#endif
    auto out = glGetHandleARB(static_cast<GLenum>(pname));
    detail::error_check("GetHandleARB"sv, check_errors);
    return out;
}

template<class span_GLcharARB>
requires(
    concepts::span<span_GLcharARB> &&
    std::is_same_v<
        std::decay_t<typename span_GLcharARB::value_type>,
        std::decay_t<GLcharARB>>)
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param obj GLhandleARB
 * \param maxLength GLsizei
 * \param length GLsizei *
 * \param infoLog GLcharARB *
 * \return void
 */
STATICINLINE void get_info_log(
    GLhandleARB    obj,
    i32&           length,
    span_GLcharARB infoLog,
    error_check    check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetInfoLogARB)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetInfoLogARB,
        gsl::span<char>(
            reinterpret_cast<char*>(infoLog.data()), infoLog.size_bytes()),
        obj,
        length);
#endif
    glGetInfoLogARB(
        obj,
        infoLog.size(),
        &length,
        infoLog.size() ? reinterpret_cast<GLcharARB*>(infoLog.data())
                       : nullptr);
    detail::error_check("GetInfoLogARB"sv, check_errors);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param obj GLhandleARB
 * \param pname GLenum
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_object_parameter(
    GLhandleARB obj,
    GLenum      pname,
    span_f32    params,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetObjectParameterfvARB)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetObjectParameterfvARB,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        obj,
        pname);
#endif
    glGetObjectParameterfvARB(
        obj,
        pname,
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetObjectParameterfvARB"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param obj GLhandleARB
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_object_parameter(
    GLhandleARB obj,
    GLenum      pname,
    span_i32    params,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetObjectParameterivARB)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetObjectParameterivARB,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        obj,
        pname);
#endif
    glGetObjectParameterivARB(
        obj,
        pname,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetObjectParameterivARB"sv, check_errors);
}

template<class span_GLcharARB>
requires(
    concepts::span<span_GLcharARB> &&
    std::is_same_v<
        std::decay_t<typename span_GLcharARB::value_type>,
        std::decay_t<GLcharARB>>)
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param obj GLhandleARB
 * \param maxLength GLsizei
 * \param length GLsizei *
 * \param source GLcharARB *
 * \return void
 */
STATICINLINE void get_shader_source(
    GLhandleARB    obj,
    i32&           length,
    span_GLcharARB source,
    error_check    check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetShaderSourceARB)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetShaderSourceARB,
        gsl::span<char>(
            reinterpret_cast<char*>(source.data()), source.size_bytes()),
        obj,
        length);
#endif
    glGetShaderSourceARB(
        obj,
        source.size(),
        &length,
        source.size() ? reinterpret_cast<GLcharARB*>(source.data()) : nullptr);
    detail::error_check("GetShaderSourceARB"sv, check_errors);
}

template<class span_const_GLcharARB>
requires(
    concepts::span<span_const_GLcharARB> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLcharARB::value_type>,
        std::decay_t<GLcharARB>>)
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param programObj GLhandleARB
 * \param name const GLcharARB *
 * \return GLint
 */
STATICINLINE GLint get_uniform_location(
    GLhandleARB                 programObj,
    span_const_GLcharARB const& name,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformLocationARB)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetUniformLocationARB,
        gsl::span<const char>(
            reinterpret_cast<const char*>(name.data()), name.size_bytes()),
        programObj);
#endif
    auto out = glGetUniformLocationARB(
        programObj,
        name.size() ? reinterpret_cast<const GLcharARB*>(name.data())
                    : nullptr);
    detail::error_check("GetUniformLocationARB"sv, check_errors);
    return out;
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param programObj GLhandleARB
 * \param location GLint
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_uniformfv(
    GLhandleARB programObj,
    i32         location,
    span_f32    params,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformfvARB)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetUniformfvARB,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        programObj,
        location);
#endif
    glGetUniformfvARB(
        programObj,
        location,
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetUniformfvARB"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param programObj GLhandleARB
 * \param location GLint
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_uniformiv(
    GLhandleARB programObj,
    i32         location,
    span_i32    params,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformivARB)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetUniformivARB,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        programObj,
        location);
#endif
    glGetUniformivARB(
        programObj,
        location,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetUniformivARB"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_shader_objects
 * \param programObj GLhandleARB
 * \return void
 */
STATICINLINE void link_program(
    GLhandleARB programObj, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LinkProgramARB)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glLinkProgramARB, programObj);
#endif
    glLinkProgramARB(programObj);
    detail::error_check("LinkProgramARB"sv, check_errors);
}

template<class span_const_GLcharARB, class span_const_i32>
requires(
    concepts::span<span_const_GLcharARB> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLcharARB::value_type>,
        std::decay_t<GLcharARB>> &&
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param shaderObj GLhandleARB
 * \param count GLsizei
 * \param string const GLcharARB **
 * \param length const GLint *
 * \return void
 */
STATICINLINE void shader_source(
    GLhandleARB                 shaderObj,
    span_const_GLcharARB const& string,
    span_const_i32 const&       length,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ShaderSourceARB)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glShaderSourceARB,
        shaderObj,
        gsl::span<const char>(
            reinterpret_cast<const char*>(string.data()), string.size_bytes()),
        gsl::span<const char>(
            reinterpret_cast<const char*>(length.data()), length.size_bytes()));
#endif
    glShaderSourceARB(
        shaderObj,
        string.size(),
        string.size() ? reinterpret_cast<const GLcharARB**>(string.data())
                      : nullptr,
        length.size() ? reinterpret_cast<const GLint*>(length.data())
                      : nullptr);
    detail::error_check("ShaderSourceARB"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_shader_objects
 * \param location GLint
 * \param v0 GLfloat
 * \return void
 */
STATICINLINE void uniform(
    i32 location, f32 v0, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1fARB)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glUniform1fARB, location, v0);
#endif
    glUniform1fARB(location, v0);
    detail::error_check("Uniform1fARB"sv, check_errors);
}

template<class span_const_f32>
requires(
    concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param location GLint
 * \param count GLsizei
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void uniform(
    i32                   location,
    span_const_f32 const& value,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1fvARB)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniform1fvARB,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location);
#endif
    glUniform1fvARB(
        location, value.size(), reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("Uniform1fvARB"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_shader_objects
 * \param location GLint
 * \param v0 GLint
 * \return void
 */
STATICINLINE void uniform(
    i32 location, i32 v0, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1iARB)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glUniform1iARB, location, v0);
#endif
    glUniform1iARB(location, v0);
    detail::error_check("Uniform1iARB"sv, check_errors);
}

template<class span_const_i32>
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param location GLint
 * \param count GLsizei
 * \param value const GLint *
 * \return void
 */
STATICINLINE void uniform(
    i32                   location,
    span_const_i32 const& value,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1ivARB)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniform1ivARB,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location);
#endif
    glUniform1ivARB(
        location, value.size(), reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("Uniform1ivARB"sv, check_errors);
}

template<class vec_2_f32>
requires(concepts::vector<vec_2_f32, f32, 2>)
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param location GLint
 * \param v0 GLfloat
 * \param v1 GLfloat
 * \return void
 */
STATICINLINE void uniform(
    i32              location,
    vec_2_f32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2fARB)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glUniform2fARB, location, v0);
#endif
    glUniform2fARB(location, v0[0], v0[1]);
    detail::error_check("Uniform2fARB"sv, check_errors);
}

template<class span_const_vec_2_f32>
requires(
    concepts::span<span_const_vec_2_f32> &&
    concepts::vector<typename span_const_vec_2_f32::value_type, f32, 2>)
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param location GLint
 * \param count GLsizei
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void uniform(
    i32                         location,
    span_const_vec_2_f32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2fvARB)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniform2fvARB,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location);
#endif
    glUniform2fvARB(
        location, value.size(), reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("Uniform2fvARB"sv, check_errors);
}

template<class vec_2_i32>
requires(concepts::vector<vec_2_i32, i32, 2>)
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param location GLint
 * \param v0 GLint
 * \param v1 GLint
 * \return void
 */
STATICINLINE void uniform(
    i32              location,
    vec_2_i32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2iARB)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glUniform2iARB, location, v0);
#endif
    glUniform2iARB(location, v0[0], v0[1]);
    detail::error_check("Uniform2iARB"sv, check_errors);
}

template<class span_const_vec_2_i32>
requires(
    concepts::span<span_const_vec_2_i32> &&
    concepts::vector<typename span_const_vec_2_i32::value_type, i32, 2>)
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param location GLint
 * \param count GLsizei
 * \param value const GLint *
 * \return void
 */
STATICINLINE void uniform(
    i32                         location,
    span_const_vec_2_i32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2ivARB)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniform2ivARB,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location);
#endif
    glUniform2ivARB(
        location, value.size(), reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("Uniform2ivARB"sv, check_errors);
}

template<class vec_3_f32>
requires(concepts::vector<vec_3_f32, f32, 3>)
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param location GLint
 * \param v0 GLfloat
 * \param v1 GLfloat
 * \param v2 GLfloat
 * \return void
 */
STATICINLINE void uniform(
    i32              location,
    vec_3_f32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3fARB)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glUniform3fARB, location, v0);
#endif
    glUniform3fARB(location, v0[0], v0[1], v0[2]);
    detail::error_check("Uniform3fARB"sv, check_errors);
}

template<class span_const_vec_3_f32>
requires(
    concepts::span<span_const_vec_3_f32> &&
    concepts::vector<typename span_const_vec_3_f32::value_type, f32, 3>)
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param location GLint
 * \param count GLsizei
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void uniform(
    i32                         location,
    span_const_vec_3_f32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3fvARB)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniform3fvARB,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location);
#endif
    glUniform3fvARB(
        location, value.size(), reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("Uniform3fvARB"sv, check_errors);
}

template<class vec_3_i32>
requires(concepts::vector<vec_3_i32, i32, 3>)
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param location GLint
 * \param v0 GLint
 * \param v1 GLint
 * \param v2 GLint
 * \return void
 */
STATICINLINE void uniform(
    i32              location,
    vec_3_i32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3iARB)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glUniform3iARB, location, v0);
#endif
    glUniform3iARB(location, v0[0], v0[1], v0[2]);
    detail::error_check("Uniform3iARB"sv, check_errors);
}

template<class span_const_vec_3_i32>
requires(
    concepts::span<span_const_vec_3_i32> &&
    concepts::vector<typename span_const_vec_3_i32::value_type, i32, 3>)
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param location GLint
 * \param count GLsizei
 * \param value const GLint *
 * \return void
 */
STATICINLINE void uniform(
    i32                         location,
    span_const_vec_3_i32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3ivARB)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniform3ivARB,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location);
#endif
    glUniform3ivARB(
        location, value.size(), reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("Uniform3ivARB"sv, check_errors);
}

template<class vec_4_f32>
requires(concepts::vector<vec_4_f32, f32, 4>)
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param location GLint
 * \param v0 GLfloat
 * \param v1 GLfloat
 * \param v2 GLfloat
 * \param v3 GLfloat
 * \return void
 */
STATICINLINE void uniform(
    i32              location,
    vec_4_f32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4fARB)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glUniform4fARB, location, v0);
#endif
    glUniform4fARB(location, v0[0], v0[1], v0[2], v0[3]);
    detail::error_check("Uniform4fARB"sv, check_errors);
}

template<class span_const_vec_4_f32>
requires(
    concepts::span<span_const_vec_4_f32> &&
    concepts::vector<typename span_const_vec_4_f32::value_type, f32, 4>)
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param location GLint
 * \param count GLsizei
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void uniform(
    i32                         location,
    span_const_vec_4_f32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4fvARB)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniform4fvARB,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location);
#endif
    glUniform4fvARB(
        location, value.size(), reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("Uniform4fvARB"sv, check_errors);
}

template<class vec_4_i32>
requires(concepts::vector<vec_4_i32, i32, 4>)
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param location GLint
 * \param v0 GLint
 * \param v1 GLint
 * \param v2 GLint
 * \param v3 GLint
 * \return void
 */
STATICINLINE void uniform(
    i32              location,
    vec_4_i32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4iARB)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glUniform4iARB, location, v0);
#endif
    glUniform4iARB(location, v0[0], v0[1], v0[2], v0[3]);
    detail::error_check("Uniform4iARB"sv, check_errors);
}

template<class span_const_vec_4_i32>
requires(
    concepts::span<span_const_vec_4_i32> &&
    concepts::vector<typename span_const_vec_4_i32::value_type, i32, 4>)
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param location GLint
 * \param count GLsizei
 * \param value const GLint *
 * \return void
 */
STATICINLINE void uniform(
    i32                         location,
    span_const_vec_4_i32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4ivARB)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniform4ivARB,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location);
#endif
    glUniform4ivARB(
        location, value.size(), reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("Uniform4ivARB"sv, check_errors);
}

template<class span_const_mat_2x2_f32>
requires(
    concepts::span<span_const_mat_2x2_f32> &&
    concepts::matrix<typename span_const_mat_2x2_f32::value_type, f32, 2, 2>)
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    bool                          transpose,
    span_const_mat_2x2_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix2fvARB)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix2fvARB,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix2fvARB(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix2fvARB"sv, check_errors);
}

template<class span_const_mat_3x3_f32>
requires(
    concepts::span<span_const_mat_3x3_f32> &&
    concepts::matrix<typename span_const_mat_3x3_f32::value_type, f32, 3, 3>)
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    bool                          transpose,
    span_const_mat_3x3_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix3fvARB)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix3fvARB,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix3fvARB(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix3fvARB"sv, check_errors);
}

template<class span_const_mat_4x4_f32>
requires(
    concepts::span<span_const_mat_4x4_f32> &&
    concepts::matrix<typename span_const_mat_4x4_f32::value_type, f32, 4, 4>)
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    bool                          transpose,
    span_const_mat_4x4_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix4fvARB)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniformMatrix4fvARB,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        transpose);
#endif
    glUniformMatrix4fvARB(
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix4fvARB"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_shader_objects
 * \param programObj GLhandleARB
 * \return void
 */
STATICINLINE void use_program_object(
    GLhandleARB programObj, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UseProgramObjectARB)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glUseProgramObjectARB, programObj);
#endif
    glUseProgramObjectARB(programObj);
    detail::error_check("UseProgramObjectARB"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_shader_objects
 * \param programObj GLhandleARB
 * \return void
 */
STATICINLINE void validate_program(
    GLhandleARB programObj, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ValidateProgramARB)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glValidateProgramARB, programObj);
#endif
    glValidateProgramARB(programObj);
    detail::error_check("ValidateProgramARB"sv, check_errors);
}

} // namespace gl::arb::shader_objects
#endif // GL_ARB_shader_objects
namespace gl::arb::shader_objects {
constexpr auto name = "GL_ARB_shader_objects";
}
