#pragma once

#ifdef GL_ARB_gpu_shader_int64
#include "../enums/AttributeType.h"
#include "../enums/VertexAttribPointerType.h"

namespace gl::arb::gpu_shader_int64 {
using gl::group::attribute_type;
using gl::group::vertex_attrib_pointer_type;

namespace values {
} // namespace values

template<class span_i64>
requires(
    concepts::span<span_i64> && std::is_same_v<
                                    std::decay_t<typename span_i64::value_type>,
                                    std::decay_t<i64>>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param program GLuint
 * \param location GLint
 * \param params GLint64 *
 * \return void
 */
STATICINLINE void get_uniformi64v(u32 program, i32 location, span_i64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformi64vARB)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetUniformi64vARB(
        program,
        location,
        params.size() ? reinterpret_cast<GLint64*>(params.data()) : nullptr);
    detail::error_check("GetUniformi64vARB"sv);
}

template<class span_u64>
requires(
    concepts::span<span_u64> && std::is_same_v<
                                    std::decay_t<typename span_u64::value_type>,
                                    std::decay_t<u64>>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param program GLuint
 * \param location GLint
 * \param params GLuint64 *
 * \return void
 */
STATICINLINE void get_uniformui64v(u32 program, i32 location, span_u64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformui64vARB)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetUniformui64vARB(
        program,
        location,
        params.size() ? reinterpret_cast<GLuint64*>(params.data()) : nullptr);
    detail::error_check("GetUniformui64vARB"sv);
}

template<class span_i64>
requires(
    concepts::span<span_i64> && std::is_same_v<
                                    std::decay_t<typename span_i64::value_type>,
                                    std::decay_t<i64>>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param program GLuint
 * \param location GLint
 * \param bufSize GLsizei
 * \param params GLint64 *
 * \return void
 */
STATICINLINE void getn_uniformi64v(
    u32 program, i32 location, i32 bufSize, span_i64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformi64vARB)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetnUniformi64vARB(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLint64*>(params.data()) : nullptr);
    detail::error_check("GetnUniformi64vARB"sv);
}

template<class span_u64>
requires(
    concepts::span<span_u64> && std::is_same_v<
                                    std::decay_t<typename span_u64::value_type>,
                                    std::decay_t<u64>>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param program GLuint
 * \param location GLint
 * \param bufSize GLsizei
 * \param params GLuint64 *
 * \return void
 */
STATICINLINE void getn_uniformui64v(
    u32 program, i32 location, i32 bufSize, span_u64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformui64vARB)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetnUniformui64vARB(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLuint64*>(params.data()) : nullptr);
    detail::error_check("GetnUniformui64vARB"sv);
}

/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param program GLuint
 * \param location GLint
 * \param x GLint64
 * \return void
 */
STATICINLINE void program_uniform(u32 program, i32 location, i64 x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1i64ARB)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform1i64ARB(program, location, x);
    detail::error_check("ProgramUniform1i64ARB"sv);
}

template<class span_const_i64>
requires(
    concepts::span<span_const_i64> &&
    std::is_same_v<
        std::decay_t<typename span_const_i64::value_type>,
        std::decay_t<i64>>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLint64 *
 * \return void
 */
STATICINLINE void program_uniform(
    u32 program, i32 location, span_const_i64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1i64vARB)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform1i64vARB(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLint64*>(value.data()));
    detail::error_check("ProgramUniform1i64vARB"sv);
}

/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param program GLuint
 * \param location GLint
 * \param x GLuint64
 * \return void
 */
STATICINLINE void program_uniform(u32 program, i32 location, u64 x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1ui64ARB)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform1ui64ARB(program, location, x);
    detail::error_check("ProgramUniform1ui64ARB"sv);
}

template<class span_const_u64>
requires(
    concepts::span<span_const_u64> &&
    std::is_same_v<
        std::decay_t<typename span_const_u64::value_type>,
        std::decay_t<u64>>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLuint64 *
 * \return void
 */
STATICINLINE void program_uniform(
    u32 program, i32 location, span_const_u64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1ui64vARB)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform1ui64vARB(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLuint64*>(value.data()));
    detail::error_check("ProgramUniform1ui64vARB"sv);
}

template<class vec_2_i64>
requires(concepts::vector<vec_2_i64, i64, 2>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param program GLuint
 * \param location GLint
 * \param x GLint64
 * \param y GLint64
 * \return void
 */
STATICINLINE void program_uniform(u32 program, i32 location, vec_2_i64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2i64ARB)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform2i64ARB(program, location, x[0], x[1]);
    detail::error_check("ProgramUniform2i64ARB"sv);
}

template<class span_const_vec_2_i64>
requires(
    concepts::span<span_const_vec_2_i64> &&
    concepts::vector<typename span_const_vec_2_i64::value_type, i64, 2>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLint64 *
 * \return void
 */
STATICINLINE void program_uniform(
    u32 program, i32 location, span_const_vec_2_i64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2i64vARB)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform2i64vARB(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLint64*>(value.data()));
    detail::error_check("ProgramUniform2i64vARB"sv);
}

template<class vec_2_u64>
requires(concepts::vector<vec_2_u64, u64, 2>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param program GLuint
 * \param location GLint
 * \param x GLuint64
 * \param y GLuint64
 * \return void
 */
STATICINLINE void program_uniform(u32 program, i32 location, vec_2_u64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2ui64ARB)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform2ui64ARB(program, location, x[0], x[1]);
    detail::error_check("ProgramUniform2ui64ARB"sv);
}

template<class span_const_vec_2_u64>
requires(
    concepts::span<span_const_vec_2_u64> &&
    concepts::vector<typename span_const_vec_2_u64::value_type, u64, 2>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLuint64 *
 * \return void
 */
STATICINLINE void program_uniform(
    u32 program, i32 location, span_const_vec_2_u64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2ui64vARB)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform2ui64vARB(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLuint64*>(value.data()));
    detail::error_check("ProgramUniform2ui64vARB"sv);
}

template<class vec_3_i64>
requires(concepts::vector<vec_3_i64, i64, 3>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param program GLuint
 * \param location GLint
 * \param x GLint64
 * \param y GLint64
 * \param z GLint64
 * \return void
 */
STATICINLINE void program_uniform(u32 program, i32 location, vec_3_i64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3i64ARB)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform3i64ARB(program, location, x[0], x[1], x[2]);
    detail::error_check("ProgramUniform3i64ARB"sv);
}

template<class span_const_vec_3_i64>
requires(
    concepts::span<span_const_vec_3_i64> &&
    concepts::vector<typename span_const_vec_3_i64::value_type, i64, 3>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLint64 *
 * \return void
 */
STATICINLINE void program_uniform(
    u32 program, i32 location, span_const_vec_3_i64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3i64vARB)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform3i64vARB(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLint64*>(value.data()));
    detail::error_check("ProgramUniform3i64vARB"sv);
}

template<class vec_3_u64>
requires(concepts::vector<vec_3_u64, u64, 3>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param program GLuint
 * \param location GLint
 * \param x GLuint64
 * \param y GLuint64
 * \param z GLuint64
 * \return void
 */
STATICINLINE void program_uniform(u32 program, i32 location, vec_3_u64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3ui64ARB)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform3ui64ARB(program, location, x[0], x[1], x[2]);
    detail::error_check("ProgramUniform3ui64ARB"sv);
}

template<class span_const_vec_3_u64>
requires(
    concepts::span<span_const_vec_3_u64> &&
    concepts::vector<typename span_const_vec_3_u64::value_type, u64, 3>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLuint64 *
 * \return void
 */
STATICINLINE void program_uniform(
    u32 program, i32 location, span_const_vec_3_u64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3ui64vARB)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform3ui64vARB(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLuint64*>(value.data()));
    detail::error_check("ProgramUniform3ui64vARB"sv);
}

template<class vec_4_i64>
requires(concepts::vector<vec_4_i64, i64, 4>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param program GLuint
 * \param location GLint
 * \param x GLint64
 * \param y GLint64
 * \param z GLint64
 * \param w GLint64
 * \return void
 */
STATICINLINE void program_uniform(u32 program, i32 location, vec_4_i64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4i64ARB)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform4i64ARB(program, location, x[0], x[1], x[2], x[3]);
    detail::error_check("ProgramUniform4i64ARB"sv);
}

template<class span_const_vec_4_i64>
requires(
    concepts::span<span_const_vec_4_i64> &&
    concepts::vector<typename span_const_vec_4_i64::value_type, i64, 4>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLint64 *
 * \return void
 */
STATICINLINE void program_uniform(
    u32 program, i32 location, span_const_vec_4_i64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4i64vARB)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform4i64vARB(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLint64*>(value.data()));
    detail::error_check("ProgramUniform4i64vARB"sv);
}

template<class vec_4_u64>
requires(concepts::vector<vec_4_u64, u64, 4>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param program GLuint
 * \param location GLint
 * \param x GLuint64
 * \param y GLuint64
 * \param z GLuint64
 * \param w GLuint64
 * \return void
 */
STATICINLINE void program_uniform(u32 program, i32 location, vec_4_u64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4ui64ARB)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform4ui64ARB(program, location, x[0], x[1], x[2], x[3]);
    detail::error_check("ProgramUniform4ui64ARB"sv);
}

template<class span_const_vec_4_u64>
requires(
    concepts::span<span_const_vec_4_u64> &&
    concepts::vector<typename span_const_vec_4_u64::value_type, u64, 4>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLuint64 *
 * \return void
 */
STATICINLINE void program_uniform(
    u32 program, i32 location, span_const_vec_4_u64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4ui64vARB)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform4ui64vARB(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLuint64*>(value.data()));
    detail::error_check("ProgramUniform4ui64vARB"sv);
}

/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param location GLint
 * \param x GLint64
 * \return void
 */
STATICINLINE void uniform(i32 location, i64 x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1i64ARB)
    }
    glUniform1i64ARB(location, x);
    detail::error_check("Uniform1i64ARB"sv);
}

template<class span_const_i64>
requires(
    concepts::span<span_const_i64> &&
    std::is_same_v<
        std::decay_t<typename span_const_i64::value_type>,
        std::decay_t<i64>>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param location GLint
 * \param count GLsizei
 * \param value const GLint64 *
 * \return void
 */
STATICINLINE void uniform(i32 location, span_const_i64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1i64vARB)
    }
    glUniform1i64vARB(
        location, value.size(), reinterpret_cast<const GLint64*>(value.data()));
    detail::error_check("Uniform1i64vARB"sv);
}

/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param location GLint
 * \param x GLuint64
 * \return void
 */
STATICINLINE void uniform(i32 location, u64 x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1ui64ARB)
    }
    glUniform1ui64ARB(location, x);
    detail::error_check("Uniform1ui64ARB"sv);
}

template<class span_const_u64>
requires(
    concepts::span<span_const_u64> &&
    std::is_same_v<
        std::decay_t<typename span_const_u64::value_type>,
        std::decay_t<u64>>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param location GLint
 * \param count GLsizei
 * \param value const GLuint64 *
 * \return void
 */
STATICINLINE void uniform(i32 location, span_const_u64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1ui64vARB)
    }
    glUniform1ui64vARB(
        location,
        value.size(),
        reinterpret_cast<const GLuint64*>(value.data()));
    detail::error_check("Uniform1ui64vARB"sv);
}

template<class vec_2_i64>
requires(concepts::vector<vec_2_i64, i64, 2>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param location GLint
 * \param x GLint64
 * \param y GLint64
 * \return void
 */
STATICINLINE void uniform(i32 location, vec_2_i64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2i64ARB)
    }
    glUniform2i64ARB(location, x[0], x[1]);
    detail::error_check("Uniform2i64ARB"sv);
}

template<class span_const_vec_2_i64>
requires(
    concepts::span<span_const_vec_2_i64> &&
    concepts::vector<typename span_const_vec_2_i64::value_type, i64, 2>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param location GLint
 * \param count GLsizei
 * \param value const GLint64 *
 * \return void
 */
STATICINLINE void uniform(i32 location, span_const_vec_2_i64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2i64vARB)
    }
    glUniform2i64vARB(
        location, value.size(), reinterpret_cast<const GLint64*>(value.data()));
    detail::error_check("Uniform2i64vARB"sv);
}

template<class vec_2_u64>
requires(concepts::vector<vec_2_u64, u64, 2>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param location GLint
 * \param x GLuint64
 * \param y GLuint64
 * \return void
 */
STATICINLINE void uniform(i32 location, vec_2_u64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2ui64ARB)
    }
    glUniform2ui64ARB(location, x[0], x[1]);
    detail::error_check("Uniform2ui64ARB"sv);
}

template<class span_const_vec_2_u64>
requires(
    concepts::span<span_const_vec_2_u64> &&
    concepts::vector<typename span_const_vec_2_u64::value_type, u64, 2>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param location GLint
 * \param count GLsizei
 * \param value const GLuint64 *
 * \return void
 */
STATICINLINE void uniform(i32 location, span_const_vec_2_u64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2ui64vARB)
    }
    glUniform2ui64vARB(
        location,
        value.size(),
        reinterpret_cast<const GLuint64*>(value.data()));
    detail::error_check("Uniform2ui64vARB"sv);
}

template<class vec_3_i64>
requires(concepts::vector<vec_3_i64, i64, 3>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param location GLint
 * \param x GLint64
 * \param y GLint64
 * \param z GLint64
 * \return void
 */
STATICINLINE void uniform(i32 location, vec_3_i64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3i64ARB)
    }
    glUniform3i64ARB(location, x[0], x[1], x[2]);
    detail::error_check("Uniform3i64ARB"sv);
}

template<class span_const_vec_3_i64>
requires(
    concepts::span<span_const_vec_3_i64> &&
    concepts::vector<typename span_const_vec_3_i64::value_type, i64, 3>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param location GLint
 * \param count GLsizei
 * \param value const GLint64 *
 * \return void
 */
STATICINLINE void uniform(i32 location, span_const_vec_3_i64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3i64vARB)
    }
    glUniform3i64vARB(
        location, value.size(), reinterpret_cast<const GLint64*>(value.data()));
    detail::error_check("Uniform3i64vARB"sv);
}

template<class vec_3_u64>
requires(concepts::vector<vec_3_u64, u64, 3>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param location GLint
 * \param x GLuint64
 * \param y GLuint64
 * \param z GLuint64
 * \return void
 */
STATICINLINE void uniform(i32 location, vec_3_u64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3ui64ARB)
    }
    glUniform3ui64ARB(location, x[0], x[1], x[2]);
    detail::error_check("Uniform3ui64ARB"sv);
}

template<class span_const_vec_3_u64>
requires(
    concepts::span<span_const_vec_3_u64> &&
    concepts::vector<typename span_const_vec_3_u64::value_type, u64, 3>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param location GLint
 * \param count GLsizei
 * \param value const GLuint64 *
 * \return void
 */
STATICINLINE void uniform(i32 location, span_const_vec_3_u64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3ui64vARB)
    }
    glUniform3ui64vARB(
        location,
        value.size(),
        reinterpret_cast<const GLuint64*>(value.data()));
    detail::error_check("Uniform3ui64vARB"sv);
}

template<class vec_4_i64>
requires(concepts::vector<vec_4_i64, i64, 4>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param location GLint
 * \param x GLint64
 * \param y GLint64
 * \param z GLint64
 * \param w GLint64
 * \return void
 */
STATICINLINE void uniform(i32 location, vec_4_i64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4i64ARB)
    }
    glUniform4i64ARB(location, x[0], x[1], x[2], x[3]);
    detail::error_check("Uniform4i64ARB"sv);
}

template<class span_const_vec_4_i64>
requires(
    concepts::span<span_const_vec_4_i64> &&
    concepts::vector<typename span_const_vec_4_i64::value_type, i64, 4>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param location GLint
 * \param count GLsizei
 * \param value const GLint64 *
 * \return void
 */
STATICINLINE void uniform(i32 location, span_const_vec_4_i64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4i64vARB)
    }
    glUniform4i64vARB(
        location, value.size(), reinterpret_cast<const GLint64*>(value.data()));
    detail::error_check("Uniform4i64vARB"sv);
}

template<class vec_4_u64>
requires(concepts::vector<vec_4_u64, u64, 4>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param location GLint
 * \param x GLuint64
 * \param y GLuint64
 * \param z GLuint64
 * \param w GLuint64
 * \return void
 */
STATICINLINE void uniform(i32 location, vec_4_u64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4ui64ARB)
    }
    glUniform4ui64ARB(location, x[0], x[1], x[2], x[3]);
    detail::error_check("Uniform4ui64ARB"sv);
}

template<class span_const_vec_4_u64>
requires(
    concepts::span<span_const_vec_4_u64> &&
    concepts::vector<typename span_const_vec_4_u64::value_type, u64, 4>)
/*!
 * \brief Part of GL_ARB_gpu_shader_int64
 * \param location GLint
 * \param count GLsizei
 * \param value const GLuint64 *
 * \return void
 */
STATICINLINE void uniform(i32 location, span_const_vec_4_u64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4ui64vARB)
    }
    glUniform4ui64vARB(
        location,
        value.size(),
        reinterpret_cast<const GLuint64*>(value.data()));
    detail::error_check("Uniform4ui64vARB"sv);
}

} // namespace gl::arb::gpu_shader_int64
#endif // GL_ARB_gpu_shader_int64
namespace gl::arb::gpu_shader_int64 {
constexpr auto name = "GL_ARB_gpu_shader_int64";
}
