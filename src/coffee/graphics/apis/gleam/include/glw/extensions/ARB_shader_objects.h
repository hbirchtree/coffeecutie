#pragma once

#ifdef GL_ARB_shader_objects
#include "../enums/AttributeType.h"
#include "../enums/ContainerType.h"
namespace gl::arb::shader_objects {
using gl::group::attribute_type;
using gl::group::container_type;
namespace values {
constexpr libc_types::u32 shader_object                    = 0x8B48;
constexpr libc_types::u32 object_type                      = 0x8B4E;
constexpr libc_types::u32 object_subtype                   = 0x8B4F;
constexpr libc_types::u32 object_delete_status             = 0x8B80;
constexpr libc_types::u32 object_compile_status            = 0x8B81;
constexpr libc_types::u32 object_link_status               = 0x8B82;
constexpr libc_types::u32 object_validate_status           = 0x8B83;
constexpr libc_types::u32 object_info_log_length           = 0x8B84;
constexpr libc_types::u32 object_attached_objects          = 0x8B85;
constexpr libc_types::u32 object_active_uniforms           = 0x8B86;
constexpr libc_types::u32 object_active_uniform_max_length = 0x8B87;
constexpr libc_types::u32 object_shader_source_length      = 0x8B88;
} // namespace values
/*!
 * \brief Part of GL_ARB_shader_objects
 * \param containerObj GLhandleARB
 * \param obj GLhandleARB
 * \return void
 */
STATICINLINE void attach_object(GLhandleARB containerObj, GLhandleARB obj)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(AttachObjectARB)
    }
    glAttachObjectARB(containerObj, obj);
    detail::error_check("AttachObjectARB"sv);
}

/*!
 * \brief Part of GL_ARB_shader_objects
 * \param shaderObj GLhandleARB
 * \return void
 */
STATICINLINE void compile_shader(GLhandleARB shaderObj)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompileShaderARB)
    }
    glCompileShaderARB(shaderObj);
    detail::error_check("CompileShaderARB"sv);
}

/*!
 * \brief Part of GL_ARB_shader_objects

 * \return handleARB
 */
STATICINLINE GLhandleARB create_program_object()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateProgramObjectARB)
    }
    auto out = glCreateProgramObjectARB();
    detail::error_check("CreateProgramObjectARB"sv);
    return out;
}

/*!
 * \brief Part of GL_ARB_shader_objects
 * \param shaderType GLenum
 * \return handleARB
 */
STATICINLINE GLhandleARB create_shader_object(group::shader_type shaderType)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateShaderObjectARB)
    }
    auto out = glCreateShaderObjectARB(static_cast<GLenum>(shaderType));
    detail::error_check("CreateShaderObjectARB"sv);
    return out;
}

/*!
 * \brief Part of GL_ARB_shader_objects
 * \param obj GLhandleARB
 * \return void
 */
STATICINLINE void delete_object(GLhandleARB obj)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteObjectARB)
    }
    glDeleteObjectARB(obj);
    detail::error_check("DeleteObjectARB"sv);
}

/*!
 * \brief Part of GL_ARB_shader_objects
 * \param containerObj GLhandleARB
 * \param attachedObj GLhandleARB
 * \return void
 */
STATICINLINE void detach_object(
    GLhandleARB containerObj, GLhandleARB attachedObj)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DetachObjectARB)
    }
    glDetachObjectARB(containerObj, attachedObj);
    detail::error_check("DetachObjectARB"sv);
}

template<class span_GLcharARB>
requires(semantic::concepts::Span<span_GLcharARB>&& std::is_same_v<
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
        span_GLcharARB name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveUniformARB)
    }
    glGetActiveUniformARB(
        programObj,
        index,
        name.size(),
        &length,
        &size,
        &type,
        name.size() ? reinterpret_cast<GLcharARB*>(name.data()) : nullptr);
    detail::error_check("GetActiveUniformARB"sv);
}

template<class span_GLhandleARB>
requires(semantic::concepts::Span<span_GLhandleARB>&& std::is_same_v<
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
        GLhandleARB containerObj, i32& count, span_GLhandleARB obj)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetAttachedObjectsARB)
    }
    glGetAttachedObjectsARB(
        containerObj,
        obj.size(),
        &count,
        obj.size() ? reinterpret_cast<GLhandleARB*>(obj.data()) : nullptr);
    detail::error_check("GetAttachedObjectsARB"sv);
}

/*!
 * \brief Part of GL_ARB_shader_objects
 * \param pname GLenum
 * \return handleARB
 */
STATICINLINE GLhandleARB get_handle(group::container_type pname)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetHandleARB)
    }
    auto out = glGetHandleARB(static_cast<GLenum>(pname));
    detail::error_check("GetHandleARB"sv);
    return out;
}

template<class span_GLcharARB>
requires(semantic::concepts::Span<span_GLcharARB>&& std::is_same_v<
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
    STATICINLINE
    void get_info_log(GLhandleARB obj, i32& length, span_GLcharARB infoLog)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetInfoLogARB)
    }
    glGetInfoLogARB(
        obj,
        infoLog.size(),
        &length,
        infoLog.size() ? reinterpret_cast<GLcharARB*>(infoLog.data())
                       : nullptr);
    detail::error_check("GetInfoLogARB"sv);
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_ARB_shader_objects
     * \param obj GLhandleARB
     * \param pname GLenum
     * \param params GLfloat *
     * \return void
     */
    STATICINLINE
    void get_object_parameter(GLhandleARB obj, GLenum pname, span_f32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetObjectParameterfvARB)
    }
    glGetObjectParameterfvARB(
        obj,
        pname,
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetObjectParameterfvARB"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_ARB_shader_objects
     * \param obj GLhandleARB
     * \param pname GLenum
     * \param params GLint *
     * \return void
     */
    STATICINLINE
    void get_object_parameter(GLhandleARB obj, GLenum pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetObjectParameterivARB)
    }
    glGetObjectParameterivARB(
        obj,
        pname,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetObjectParameterivARB"sv);
}

template<class span_GLcharARB>
requires(semantic::concepts::Span<span_GLcharARB>&& std::is_same_v<
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
    STATICINLINE
    void get_shader_source(GLhandleARB obj, i32& length, span_GLcharARB source)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetShaderSourceARB)
    }
    glGetShaderSourceARB(
        obj,
        source.size(),
        &length,
        source.size() ? reinterpret_cast<GLcharARB*>(source.data()) : nullptr);
    detail::error_check("GetShaderSourceARB"sv);
}

template<class span_const_GLcharARB>
requires(semantic::concepts::Span<span_const_GLcharARB>&& std::is_same_v<
         std::decay_t<typename span_const_GLcharARB::value_type>,
         std::decay_t<GLcharARB>>)
    /*!
     * \brief Part of GL_ARB_shader_objects
     * \param programObj GLhandleARB
     * \param name const GLcharARB *
     * \return GLint
     */
    STATICINLINE GLint get_uniform_location(
        GLhandleARB programObj, span_const_GLcharARB const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformLocationARB)
    }
    auto out = glGetUniformLocationARB(
        programObj,
        name.size() ? reinterpret_cast<const GLcharARB*>(name.data())
                    : nullptr);
    detail::error_check("GetUniformLocationARB"sv);
    return out;
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_ARB_shader_objects
     * \param programObj GLhandleARB
     * \param location GLint
     * \param params GLfloat *
     * \return void
     */
    STATICINLINE
    void get_uniformfv(GLhandleARB programObj, i32 location, span_f32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformfvARB)
    }
    glGetUniformfvARB(
        programObj,
        location,
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetUniformfvARB"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_ARB_shader_objects
     * \param programObj GLhandleARB
     * \param location GLint
     * \param params GLint *
     * \return void
     */
    STATICINLINE
    void get_uniformiv(GLhandleARB programObj, i32 location, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformivARB)
    }
    glGetUniformivARB(
        programObj,
        location,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetUniformivARB"sv);
}

/*!
 * \brief Part of GL_ARB_shader_objects
 * \param programObj GLhandleARB
 * \return void
 */
STATICINLINE void link_program(GLhandleARB programObj)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LinkProgramARB)
    }
    glLinkProgramARB(programObj);
    detail::error_check("LinkProgramARB"sv);
}

template<class span_const_GLcharARB, class span_const_i32>
requires(semantic::concepts::Span<span_const_GLcharARB>&& std::is_same_v<
         std::decay_t<typename span_const_GLcharARB::value_type>,
         std::decay_t<GLcharARB>>&& semantic::concepts::Span<span_const_i32>&&
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
        span_const_i32 const&       length)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ShaderSourceARB)
    }
    glShaderSourceARB(
        shaderObj,
        string.size(),
        string.size() ? reinterpret_cast<const GLcharARB**>(string.data())
                      : nullptr,
        length.size() ? reinterpret_cast<const GLint*>(length.data())
                      : nullptr);
    detail::error_check("ShaderSourceARB"sv);
}

/*!
 * \brief Part of GL_ARB_shader_objects
 * \param location GLint
 * \param v0 GLfloat
 * \return void
 */
STATICINLINE void uniform(i32 location, f32 v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1fARB)
    }
    glUniform1fARB(location, v0);
    detail::error_check("Uniform1fARB"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_ARB_shader_objects
     * \param location GLint
     * \param count GLsizei
     * \param value const GLfloat *
     * \return void
     */
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1fvARB)
    }
    glUniform1fvARB(
        location, count, reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("Uniform1fvARB"sv);
}

/*!
 * \brief Part of GL_ARB_shader_objects
 * \param location GLint
 * \param v0 GLint
 * \return void
 */
STATICINLINE void uniform(i32 location, i32 v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1iARB)
    }
    glUniform1iARB(location, v0);
    detail::error_check("Uniform1iARB"sv);
}

template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_ARB_shader_objects
     * \param location GLint
     * \param count GLsizei
     * \param value const GLint *
     * \return void
     */
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1ivARB)
    }
    glUniform1ivARB(
        location, count, reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("Uniform1ivARB"sv);
}

template<class vec_2_f32>
requires(semantic::concepts::Vector<vec_2_f32, f32, 2>)
    /*!
     * \brief Part of GL_ARB_shader_objects
     * \param location GLint
     * \param v0 GLfloat
     * \param v1 GLfloat
     * \return void
     */
    STATICINLINE void uniform(i32 location, vec_2_f32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2fARB)
    }
    glUniform2fARB(location, v0.x(), v0.y());
    detail::error_check("Uniform2fARB"sv);
}

template<class span_const_vec_2_f32>
requires(semantic::concepts::Span<span_const_vec_2_f32>&& semantic::concepts::
             Vector<typename span_const_vec_2_f32::value_type, f32, 2>)
    /*!
     * \brief Part of GL_ARB_shader_objects
     * \param location GLint
     * \param count GLsizei
     * \param value const GLfloat *
     * \return void
     */
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_2_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2fvARB)
    }
    glUniform2fvARB(
        location, count, reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("Uniform2fvARB"sv);
}

template<class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>)
    /*!
     * \brief Part of GL_ARB_shader_objects
     * \param location GLint
     * \param v0 GLint
     * \param v1 GLint
     * \return void
     */
    STATICINLINE void uniform(i32 location, vec_2_i32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2iARB)
    }
    glUniform2iARB(location, v0.x(), v0.y());
    detail::error_check("Uniform2iARB"sv);
}

template<class span_const_vec_2_i32>
requires(semantic::concepts::Span<span_const_vec_2_i32>&& semantic::concepts::
             Vector<typename span_const_vec_2_i32::value_type, i32, 2>)
    /*!
     * \brief Part of GL_ARB_shader_objects
     * \param location GLint
     * \param count GLsizei
     * \param value const GLint *
     * \return void
     */
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_2_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2ivARB)
    }
    glUniform2ivARB(
        location, count, reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("Uniform2ivARB"sv);
}

template<class vec_3_f32>
requires(semantic::concepts::Vector<vec_3_f32, f32, 3>)
    /*!
     * \brief Part of GL_ARB_shader_objects
     * \param location GLint
     * \param v0 GLfloat
     * \param v1 GLfloat
     * \param v2 GLfloat
     * \return void
     */
    STATICINLINE void uniform(i32 location, vec_3_f32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3fARB)
    }
    glUniform3fARB(location, v0.x(), v0.y(), v0.z());
    detail::error_check("Uniform3fARB"sv);
}

template<class span_const_vec_3_f32>
requires(semantic::concepts::Span<span_const_vec_3_f32>&& semantic::concepts::
             Vector<typename span_const_vec_3_f32::value_type, f32, 3>)
    /*!
     * \brief Part of GL_ARB_shader_objects
     * \param location GLint
     * \param count GLsizei
     * \param value const GLfloat *
     * \return void
     */
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_3_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3fvARB)
    }
    glUniform3fvARB(
        location, count, reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("Uniform3fvARB"sv);
}

template<class vec_3_i32>
requires(semantic::concepts::Vector<vec_3_i32, i32, 3>)
    /*!
     * \brief Part of GL_ARB_shader_objects
     * \param location GLint
     * \param v0 GLint
     * \param v1 GLint
     * \param v2 GLint
     * \return void
     */
    STATICINLINE void uniform(i32 location, vec_3_i32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3iARB)
    }
    glUniform3iARB(location, v0.x(), v0.y(), v0.z());
    detail::error_check("Uniform3iARB"sv);
}

template<class span_const_vec_3_i32>
requires(semantic::concepts::Span<span_const_vec_3_i32>&& semantic::concepts::
             Vector<typename span_const_vec_3_i32::value_type, i32, 3>)
    /*!
     * \brief Part of GL_ARB_shader_objects
     * \param location GLint
     * \param count GLsizei
     * \param value const GLint *
     * \return void
     */
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_3_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3ivARB)
    }
    glUniform3ivARB(
        location, count, reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("Uniform3ivARB"sv);
}

template<class vec_4_f32>
requires(semantic::concepts::Vector<vec_4_f32, f32, 4>)
    /*!
     * \brief Part of GL_ARB_shader_objects
     * \param location GLint
     * \param v0 GLfloat
     * \param v1 GLfloat
     * \param v2 GLfloat
     * \param v3 GLfloat
     * \return void
     */
    STATICINLINE void uniform(i32 location, vec_4_f32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4fARB)
    }
    glUniform4fARB(location, v0.x(), v0.y(), v0.z(), v0.w());
    detail::error_check("Uniform4fARB"sv);
}

template<class span_const_vec_4_f32>
requires(semantic::concepts::Span<span_const_vec_4_f32>&& semantic::concepts::
             Vector<typename span_const_vec_4_f32::value_type, f32, 4>)
    /*!
     * \brief Part of GL_ARB_shader_objects
     * \param location GLint
     * \param count GLsizei
     * \param value const GLfloat *
     * \return void
     */
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_4_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4fvARB)
    }
    glUniform4fvARB(
        location, count, reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("Uniform4fvARB"sv);
}

template<class vec_4_i32>
requires(semantic::concepts::Vector<vec_4_i32, i32, 4>)
    /*!
     * \brief Part of GL_ARB_shader_objects
     * \param location GLint
     * \param v0 GLint
     * \param v1 GLint
     * \param v2 GLint
     * \param v3 GLint
     * \return void
     */
    STATICINLINE void uniform(i32 location, vec_4_i32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4iARB)
    }
    glUniform4iARB(location, v0.x(), v0.y(), v0.z(), v0.w());
    detail::error_check("Uniform4iARB"sv);
}

template<class span_const_vec_4_i32>
requires(semantic::concepts::Span<span_const_vec_4_i32>&& semantic::concepts::
             Vector<typename span_const_vec_4_i32::value_type, i32, 4>)
    /*!
     * \brief Part of GL_ARB_shader_objects
     * \param location GLint
     * \param count GLsizei
     * \param value const GLint *
     * \return void
     */
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_4_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4ivARB)
    }
    glUniform4ivARB(
        location, count, reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("Uniform4ivARB"sv);
}

template<class span_const_mat_2x2_f32>
requires(semantic::concepts::Span<span_const_mat_2x2_f32>&& semantic::concepts::
             Matrix<typename span_const_mat_2x2_f32::value_type, f32, 2, 2>)
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
        i32                           count,
        bool                          transpose,
        span_const_mat_2x2_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix2fvARB)
    }
    glUniformMatrix2fvARB(
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix2fvARB"sv);
}

template<class span_const_mat_3x3_f32>
requires(semantic::concepts::Span<span_const_mat_3x3_f32>&& semantic::concepts::
             Matrix<typename span_const_mat_3x3_f32::value_type, f32, 3, 3>)
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
        i32                           count,
        bool                          transpose,
        span_const_mat_3x3_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix3fvARB)
    }
    glUniformMatrix3fvARB(
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix3fvARB"sv);
}

template<class span_const_mat_4x4_f32>
requires(semantic::concepts::Span<span_const_mat_4x4_f32>&& semantic::concepts::
             Matrix<typename span_const_mat_4x4_f32::value_type, f32, 4, 4>)
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
        i32                           count,
        bool                          transpose,
        span_const_mat_4x4_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix4fvARB)
    }
    glUniformMatrix4fvARB(
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix4fvARB"sv);
}

/*!
 * \brief Part of GL_ARB_shader_objects
 * \param programObj GLhandleARB
 * \return void
 */
STATICINLINE void use_program_object(GLhandleARB programObj)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UseProgramObjectARB)
    }
    glUseProgramObjectARB(programObj);
    detail::error_check("UseProgramObjectARB"sv);
}

/*!
 * \brief Part of GL_ARB_shader_objects
 * \param programObj GLhandleARB
 * \return void
 */
STATICINLINE void validate_program(GLhandleARB programObj)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ValidateProgramARB)
    }
    glValidateProgramARB(programObj);
    detail::error_check("ValidateProgramARB"sv);
}

} // namespace gl::arb::shader_objects
#endif // GL_ARB_shader_objects
namespace gl::arb::shader_objects {
constexpr auto name = "GL_ARB_shader_objects";
}
