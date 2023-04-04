#pragma once

#ifdef GL_AMD_gpu_shader_int64
#include "../enums/AttributeType.h"
#include "../enums/VertexAttribPointerType.h"
namespace gl::amd::gpu_shader_int64 {
using gl::group::attribute_type;
using gl::group::vertex_attrib_pointer_type;
namespace values {
constexpr libc_types::u32 int8_nv                = 0x8FE0;
constexpr libc_types::u32 int8_vec2_nv           = 0x8FE1;
constexpr libc_types::u32 int8_vec3_nv           = 0x8FE2;
constexpr libc_types::u32 int8_vec4_nv           = 0x8FE3;
constexpr libc_types::u32 int16_nv               = 0x8FE4;
constexpr libc_types::u32 int16_vec2_nv          = 0x8FE5;
constexpr libc_types::u32 int16_vec3_nv          = 0x8FE6;
constexpr libc_types::u32 int16_vec4_nv          = 0x8FE7;
constexpr libc_types::u32 int64_vec2_nv          = 0x8FE9;
constexpr libc_types::u32 int64_vec3_nv          = 0x8FEA;
constexpr libc_types::u32 int64_vec4_nv          = 0x8FEB;
constexpr libc_types::u32 unsigned_int8_nv       = 0x8FEC;
constexpr libc_types::u32 unsigned_int8_vec2_nv  = 0x8FED;
constexpr libc_types::u32 unsigned_int8_vec3_nv  = 0x8FEE;
constexpr libc_types::u32 unsigned_int8_vec4_nv  = 0x8FEF;
constexpr libc_types::u32 unsigned_int16_nv      = 0x8FF0;
constexpr libc_types::u32 unsigned_int16_vec2_nv = 0x8FF1;
constexpr libc_types::u32 unsigned_int16_vec3_nv = 0x8FF2;
constexpr libc_types::u32 unsigned_int16_vec4_nv = 0x8FF3;
constexpr libc_types::u32 unsigned_int64_vec2_nv = 0x8FF5;
constexpr libc_types::u32 unsigned_int64_vec3_nv = 0x8FF6;
constexpr libc_types::u32 unsigned_int64_vec4_nv = 0x8FF7;
constexpr libc_types::u32 float16_nv             = 0x8FF8;
constexpr libc_types::u32 float16_vec2_nv        = 0x8FF9;
constexpr libc_types::u32 float16_vec3_nv        = 0x8FFA;
constexpr libc_types::u32 float16_vec4_nv        = 0x8FFB;
} // namespace values
template<class span_GLint64EXT>
requires(semantic::concepts::Span<span_GLint64EXT>&& std::is_same_v<
         std::decay_t<typename span_GLint64EXT::value_type>,
         std::decay_t<GLint64EXT>>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param program GLuint
     * \param location GLint
     * \param params GLint64EXT *
     * \return void
     */
    STATICINLINE
    void get_uniformi64v_nv(u32 program, i32 location, span_GLint64EXT params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformi64vNV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetUniformi64vNV(
        program,
        location,
        params.size() ? reinterpret_cast<GLint64EXT*>(params.data()) : nullptr);
    detail::error_check("GetUniformi64vNV"sv);
}

template<class span_GLuint64EXT>
requires(semantic::concepts::Span<span_GLuint64EXT>&& std::is_same_v<
         std::decay_t<typename span_GLuint64EXT::value_type>,
         std::decay_t<GLuint64EXT>>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param program GLuint
     * \param location GLint
     * \param params GLuint64EXT *
     * \return void
     */
    STATICINLINE
    void get_uniformui64v_nv(u32 program, i32 location, span_GLuint64EXT params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformui64vNV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetUniformui64vNV(
        program,
        location,
        params.size() ? reinterpret_cast<GLuint64EXT*>(params.data())
                      : nullptr);
    detail::error_check("GetUniformui64vNV"sv);
}

/*!
 * \brief Part of GL_AMD_gpu_shader_int64
 * \param location GLint
 * \param x GLint64EXT
 * \return void
 */
STATICINLINE void uniform(i32 location, GLint64EXT x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1i64NV)
    }
    glUniform1i64NV(location, x);
    detail::error_check("Uniform1i64NV"sv);
}

template<class span_const_GLint64EXT>
requires(semantic::concepts::Span<span_const_GLint64EXT>&& std::is_same_v<
         std::decay_t<typename span_const_GLint64EXT::value_type>,
         std::decay_t<GLint64EXT>>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param location GLint
     * \param count GLsizei
     * \param value const GLint64EXT *
     * \return void
     */
    STATICINLINE void uniform(i32 location, span_const_GLint64EXT const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1i64vNV)
    }
    glUniform1i64vNV(
        location,
        value.size(),
        reinterpret_cast<const GLint64EXT*>(value.data()));
    detail::error_check("Uniform1i64vNV"sv);
}

/*!
 * \brief Part of GL_AMD_gpu_shader_int64
 * \param location GLint
 * \param x GLuint64EXT
 * \return void
 */
STATICINLINE void uniform(i32 location, GLuint64EXT x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1ui64NV)
    }
    glUniform1ui64NV(location, x);
    detail::error_check("Uniform1ui64NV"sv);
}

template<class span_const_GLuint64EXT>
requires(semantic::concepts::Span<span_const_GLuint64EXT>&& std::is_same_v<
         std::decay_t<typename span_const_GLuint64EXT::value_type>,
         std::decay_t<GLuint64EXT>>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param location GLint
     * \param count GLsizei
     * \param value const GLuint64EXT *
     * \return void
     */
    STATICINLINE void uniform(i32 location, span_const_GLuint64EXT const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1ui64vNV)
    }
    glUniform1ui64vNV(
        location,
        value.size(),
        reinterpret_cast<const GLuint64EXT*>(value.data()));
    detail::error_check("Uniform1ui64vNV"sv);
}

template<class vec_2_GLint64EXT>
requires(semantic::concepts::Vector<vec_2_GLint64EXT, GLint64EXT, 2>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param location GLint
     * \param x GLint64EXT
     * \param y GLint64EXT
     * \return void
     */
    STATICINLINE void uniform(i32 location, vec_2_GLint64EXT const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2i64NV)
    }
    glUniform2i64NV(location, x.x(), x.y());
    detail::error_check("Uniform2i64NV"sv);
}

template<class span_const_vec_2_GLint64EXT>
requires(
    semantic::concepts::Span<span_const_vec_2_GLint64EXT>&& semantic::concepts::
        Vector<typename span_const_vec_2_GLint64EXT::value_type, GLint64EXT, 2>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param location GLint
     * \param count GLsizei
     * \param value const GLint64EXT *
     * \return void
     */
    STATICINLINE
    void uniform(i32 location, span_const_vec_2_GLint64EXT const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2i64vNV)
    }
    glUniform2i64vNV(
        location,
        value.size(),
        reinterpret_cast<const GLint64EXT*>(value.data()));
    detail::error_check("Uniform2i64vNV"sv);
}

template<class vec_2_GLuint64EXT>
requires(semantic::concepts::Vector<vec_2_GLuint64EXT, GLuint64EXT, 2>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param location GLint
     * \param x GLuint64EXT
     * \param y GLuint64EXT
     * \return void
     */
    STATICINLINE void uniform(i32 location, vec_2_GLuint64EXT const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2ui64NV)
    }
    glUniform2ui64NV(location, x.x(), x.y());
    detail::error_check("Uniform2ui64NV"sv);
}

template<class span_const_vec_2_GLuint64EXT>
requires(semantic::concepts::Span<span_const_vec_2_GLuint64EXT>&&
             semantic::concepts::Vector<
                 typename span_const_vec_2_GLuint64EXT::value_type,
                 GLuint64EXT,
                 2>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param location GLint
     * \param count GLsizei
     * \param value const GLuint64EXT *
     * \return void
     */
    STATICINLINE
    void uniform(i32 location, span_const_vec_2_GLuint64EXT const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2ui64vNV)
    }
    glUniform2ui64vNV(
        location,
        value.size(),
        reinterpret_cast<const GLuint64EXT*>(value.data()));
    detail::error_check("Uniform2ui64vNV"sv);
}

template<class vec_3_GLint64EXT>
requires(semantic::concepts::Vector<vec_3_GLint64EXT, GLint64EXT, 3>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param location GLint
     * \param x GLint64EXT
     * \param y GLint64EXT
     * \param z GLint64EXT
     * \return void
     */
    STATICINLINE void uniform(i32 location, vec_3_GLint64EXT const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3i64NV)
    }
    glUniform3i64NV(location, x.x(), x.y(), x.z());
    detail::error_check("Uniform3i64NV"sv);
}

template<class span_const_vec_3_GLint64EXT>
requires(
    semantic::concepts::Span<span_const_vec_3_GLint64EXT>&& semantic::concepts::
        Vector<typename span_const_vec_3_GLint64EXT::value_type, GLint64EXT, 3>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param location GLint
     * \param count GLsizei
     * \param value const GLint64EXT *
     * \return void
     */
    STATICINLINE
    void uniform(i32 location, span_const_vec_3_GLint64EXT const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3i64vNV)
    }
    glUniform3i64vNV(
        location,
        value.size(),
        reinterpret_cast<const GLint64EXT*>(value.data()));
    detail::error_check("Uniform3i64vNV"sv);
}

template<class vec_3_GLuint64EXT>
requires(semantic::concepts::Vector<vec_3_GLuint64EXT, GLuint64EXT, 3>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param location GLint
     * \param x GLuint64EXT
     * \param y GLuint64EXT
     * \param z GLuint64EXT
     * \return void
     */
    STATICINLINE void uniform(i32 location, vec_3_GLuint64EXT const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3ui64NV)
    }
    glUniform3ui64NV(location, x.x(), x.y(), x.z());
    detail::error_check("Uniform3ui64NV"sv);
}

template<class span_const_vec_3_GLuint64EXT>
requires(semantic::concepts::Span<span_const_vec_3_GLuint64EXT>&&
             semantic::concepts::Vector<
                 typename span_const_vec_3_GLuint64EXT::value_type,
                 GLuint64EXT,
                 3>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param location GLint
     * \param count GLsizei
     * \param value const GLuint64EXT *
     * \return void
     */
    STATICINLINE
    void uniform(i32 location, span_const_vec_3_GLuint64EXT const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3ui64vNV)
    }
    glUniform3ui64vNV(
        location,
        value.size(),
        reinterpret_cast<const GLuint64EXT*>(value.data()));
    detail::error_check("Uniform3ui64vNV"sv);
}

template<class vec_4_GLint64EXT>
requires(semantic::concepts::Vector<vec_4_GLint64EXT, GLint64EXT, 4>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param location GLint
     * \param x GLint64EXT
     * \param y GLint64EXT
     * \param z GLint64EXT
     * \param w GLint64EXT
     * \return void
     */
    STATICINLINE void uniform(i32 location, vec_4_GLint64EXT const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4i64NV)
    }
    glUniform4i64NV(location, x.x(), x.y(), x.z(), x.w());
    detail::error_check("Uniform4i64NV"sv);
}

template<class span_const_vec_4_GLint64EXT>
requires(
    semantic::concepts::Span<span_const_vec_4_GLint64EXT>&& semantic::concepts::
        Vector<typename span_const_vec_4_GLint64EXT::value_type, GLint64EXT, 4>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param location GLint
     * \param count GLsizei
     * \param value const GLint64EXT *
     * \return void
     */
    STATICINLINE
    void uniform(i32 location, span_const_vec_4_GLint64EXT const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4i64vNV)
    }
    glUniform4i64vNV(
        location,
        value.size(),
        reinterpret_cast<const GLint64EXT*>(value.data()));
    detail::error_check("Uniform4i64vNV"sv);
}

template<class vec_4_GLuint64EXT>
requires(semantic::concepts::Vector<vec_4_GLuint64EXT, GLuint64EXT, 4>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param location GLint
     * \param x GLuint64EXT
     * \param y GLuint64EXT
     * \param z GLuint64EXT
     * \param w GLuint64EXT
     * \return void
     */
    STATICINLINE void uniform(i32 location, vec_4_GLuint64EXT const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4ui64NV)
    }
    glUniform4ui64NV(location, x.x(), x.y(), x.z(), x.w());
    detail::error_check("Uniform4ui64NV"sv);
}

template<class span_const_vec_4_GLuint64EXT>
requires(semantic::concepts::Span<span_const_vec_4_GLuint64EXT>&&
             semantic::concepts::Vector<
                 typename span_const_vec_4_GLuint64EXT::value_type,
                 GLuint64EXT,
                 4>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param location GLint
     * \param count GLsizei
     * \param value const GLuint64EXT *
     * \return void
     */
    STATICINLINE
    void uniform(i32 location, span_const_vec_4_GLuint64EXT const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4ui64vNV)
    }
    glUniform4ui64vNV(
        location,
        value.size(),
        reinterpret_cast<const GLuint64EXT*>(value.data()));
    detail::error_check("Uniform4ui64vNV"sv);
}

/*!
 * \brief Part of GL_AMD_gpu_shader_int64
 * \param program GLuint
 * \param location GLint
 * \param x GLint64EXT
 * \return void
 */
STATICINLINE void program_uniform(u32 program, i32 location, GLint64EXT x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1i64NV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform1i64NV(program, location, x);
    detail::error_check("ProgramUniform1i64NV"sv);
}

template<class span_const_GLint64EXT>
requires(semantic::concepts::Span<span_const_GLint64EXT>&& std::is_same_v<
         std::decay_t<typename span_const_GLint64EXT::value_type>,
         std::decay_t<GLint64EXT>>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLint64EXT *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, span_const_GLint64EXT const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1i64vNV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform1i64vNV(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLint64EXT*>(value.data()));
    detail::error_check("ProgramUniform1i64vNV"sv);
}

/*!
 * \brief Part of GL_AMD_gpu_shader_int64
 * \param program GLuint
 * \param location GLint
 * \param x GLuint64EXT
 * \return void
 */
STATICINLINE void program_uniform(u32 program, i32 location, GLuint64EXT x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1ui64NV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform1ui64NV(program, location, x);
    detail::error_check("ProgramUniform1ui64NV"sv);
}

template<class span_const_GLuint64EXT>
requires(semantic::concepts::Span<span_const_GLuint64EXT>&& std::is_same_v<
         std::decay_t<typename span_const_GLuint64EXT::value_type>,
         std::decay_t<GLuint64EXT>>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLuint64EXT *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, span_const_GLuint64EXT const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1ui64vNV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform1ui64vNV(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLuint64EXT*>(value.data()));
    detail::error_check("ProgramUniform1ui64vNV"sv);
}

template<class vec_2_GLint64EXT>
requires(semantic::concepts::Vector<vec_2_GLint64EXT, GLint64EXT, 2>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param program GLuint
     * \param location GLint
     * \param x GLint64EXT
     * \param y GLint64EXT
     * \return void
     */
    STATICINLINE
    void program_uniform(u32 program, i32 location, vec_2_GLint64EXT const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2i64NV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform2i64NV(program, location, x.x(), x.y());
    detail::error_check("ProgramUniform2i64NV"sv);
}

template<class span_const_vec_2_GLint64EXT>
requires(
    semantic::concepts::Span<span_const_vec_2_GLint64EXT>&& semantic::concepts::
        Vector<typename span_const_vec_2_GLint64EXT::value_type, GLint64EXT, 2>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLint64EXT *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, span_const_vec_2_GLint64EXT const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2i64vNV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform2i64vNV(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLint64EXT*>(value.data()));
    detail::error_check("ProgramUniform2i64vNV"sv);
}

template<class vec_2_GLuint64EXT>
requires(semantic::concepts::Vector<vec_2_GLuint64EXT, GLuint64EXT, 2>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param program GLuint
     * \param location GLint
     * \param x GLuint64EXT
     * \param y GLuint64EXT
     * \return void
     */
    STATICINLINE
    void program_uniform(u32 program, i32 location, vec_2_GLuint64EXT const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2ui64NV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform2ui64NV(program, location, x.x(), x.y());
    detail::error_check("ProgramUniform2ui64NV"sv);
}

template<class span_const_vec_2_GLuint64EXT>
requires(semantic::concepts::Span<span_const_vec_2_GLuint64EXT>&&
             semantic::concepts::Vector<
                 typename span_const_vec_2_GLuint64EXT::value_type,
                 GLuint64EXT,
                 2>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLuint64EXT *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, span_const_vec_2_GLuint64EXT const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2ui64vNV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform2ui64vNV(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLuint64EXT*>(value.data()));
    detail::error_check("ProgramUniform2ui64vNV"sv);
}

template<class vec_3_GLint64EXT>
requires(semantic::concepts::Vector<vec_3_GLint64EXT, GLint64EXT, 3>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param program GLuint
     * \param location GLint
     * \param x GLint64EXT
     * \param y GLint64EXT
     * \param z GLint64EXT
     * \return void
     */
    STATICINLINE
    void program_uniform(u32 program, i32 location, vec_3_GLint64EXT const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3i64NV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform3i64NV(program, location, x.x(), x.y(), x.z());
    detail::error_check("ProgramUniform3i64NV"sv);
}

template<class span_const_vec_3_GLint64EXT>
requires(
    semantic::concepts::Span<span_const_vec_3_GLint64EXT>&& semantic::concepts::
        Vector<typename span_const_vec_3_GLint64EXT::value_type, GLint64EXT, 3>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLint64EXT *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, span_const_vec_3_GLint64EXT const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3i64vNV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform3i64vNV(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLint64EXT*>(value.data()));
    detail::error_check("ProgramUniform3i64vNV"sv);
}

template<class vec_3_GLuint64EXT>
requires(semantic::concepts::Vector<vec_3_GLuint64EXT, GLuint64EXT, 3>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param program GLuint
     * \param location GLint
     * \param x GLuint64EXT
     * \param y GLuint64EXT
     * \param z GLuint64EXT
     * \return void
     */
    STATICINLINE
    void program_uniform(u32 program, i32 location, vec_3_GLuint64EXT const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3ui64NV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform3ui64NV(program, location, x.x(), x.y(), x.z());
    detail::error_check("ProgramUniform3ui64NV"sv);
}

template<class span_const_vec_3_GLuint64EXT>
requires(semantic::concepts::Span<span_const_vec_3_GLuint64EXT>&&
             semantic::concepts::Vector<
                 typename span_const_vec_3_GLuint64EXT::value_type,
                 GLuint64EXT,
                 3>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLuint64EXT *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, span_const_vec_3_GLuint64EXT const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3ui64vNV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform3ui64vNV(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLuint64EXT*>(value.data()));
    detail::error_check("ProgramUniform3ui64vNV"sv);
}

template<class vec_4_GLint64EXT>
requires(semantic::concepts::Vector<vec_4_GLint64EXT, GLint64EXT, 4>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param program GLuint
     * \param location GLint
     * \param x GLint64EXT
     * \param y GLint64EXT
     * \param z GLint64EXT
     * \param w GLint64EXT
     * \return void
     */
    STATICINLINE
    void program_uniform(u32 program, i32 location, vec_4_GLint64EXT const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4i64NV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform4i64NV(program, location, x.x(), x.y(), x.z(), x.w());
    detail::error_check("ProgramUniform4i64NV"sv);
}

template<class span_const_vec_4_GLint64EXT>
requires(
    semantic::concepts::Span<span_const_vec_4_GLint64EXT>&& semantic::concepts::
        Vector<typename span_const_vec_4_GLint64EXT::value_type, GLint64EXT, 4>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLint64EXT *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, span_const_vec_4_GLint64EXT const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4i64vNV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform4i64vNV(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLint64EXT*>(value.data()));
    detail::error_check("ProgramUniform4i64vNV"sv);
}

template<class vec_4_GLuint64EXT>
requires(semantic::concepts::Vector<vec_4_GLuint64EXT, GLuint64EXT, 4>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param program GLuint
     * \param location GLint
     * \param x GLuint64EXT
     * \param y GLuint64EXT
     * \param z GLuint64EXT
     * \param w GLuint64EXT
     * \return void
     */
    STATICINLINE
    void program_uniform(u32 program, i32 location, vec_4_GLuint64EXT const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4ui64NV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform4ui64NV(program, location, x.x(), x.y(), x.z(), x.w());
    detail::error_check("ProgramUniform4ui64NV"sv);
}

template<class span_const_vec_4_GLuint64EXT>
requires(semantic::concepts::Span<span_const_vec_4_GLuint64EXT>&&
             semantic::concepts::Vector<
                 typename span_const_vec_4_GLuint64EXT::value_type,
                 GLuint64EXT,
                 4>)
    /*!
     * \brief Part of GL_AMD_gpu_shader_int64
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLuint64EXT *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, span_const_vec_4_GLuint64EXT const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4ui64vNV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform4ui64vNV(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLuint64EXT*>(value.data()));
    detail::error_check("ProgramUniform4ui64vNV"sv);
}

} // namespace gl::amd::gpu_shader_int64
#endif // GL_AMD_gpu_shader_int64
namespace gl::amd::gpu_shader_int64 {
constexpr auto name = "GL_AMD_gpu_shader_int64";
}
