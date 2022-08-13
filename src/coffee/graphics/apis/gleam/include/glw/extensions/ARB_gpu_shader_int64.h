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
requires(semantic::concepts::Span<span_i64>&& std::is_same_v<
         std::decay_t<typename span_i64::value_type>,
         std::decay_t<i64>>) STATICINLINE
    void get_uniformi64v(u32 program, i32 location, span_i64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformi64vARB)
        glIsProgram(program);
    }
    glGetUniformi64vARB(
        program,
        location,
        params.size() ? reinterpret_cast<GLint64*>(params.data()) : nullptr);
    detail::error_check("GetUniformi64vARB"sv);
}

template<class span_u64>
requires(semantic::concepts::Span<span_u64>&& std::is_same_v<
         std::decay_t<typename span_u64::value_type>,
         std::decay_t<u64>>) STATICINLINE
    void get_uniformui64v(u32 program, i32 location, span_u64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformui64vARB)
        glIsProgram(program);
    }
    glGetUniformui64vARB(
        program,
        location,
        params.size() ? reinterpret_cast<GLuint64*>(params.data()) : nullptr);
    detail::error_check("GetUniformui64vARB"sv);
}

template<class span_i64>
requires(semantic::concepts::Span<span_i64>&& std::is_same_v<
         std::decay_t<typename span_i64::value_type>,
         std::decay_t<i64>>) STATICINLINE
    void getn_uniformi64v(
        u32 program, i32 location, i32 bufSize, span_i64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformi64vARB)
        glIsProgram(program);
    }
    glGetnUniformi64vARB(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLint64*>(params.data()) : nullptr);
    detail::error_check("GetnUniformi64vARB"sv);
}

template<class span_u64>
requires(semantic::concepts::Span<span_u64>&& std::is_same_v<
         std::decay_t<typename span_u64::value_type>,
         std::decay_t<u64>>) STATICINLINE
    void getn_uniformui64v(
        u32 program, i32 location, i32 bufSize, span_u64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformui64vARB)
        glIsProgram(program);
    }
    glGetnUniformui64vARB(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLuint64*>(params.data()) : nullptr);
    detail::error_check("GetnUniformui64vARB"sv);
}

STATICINLINE void program_uniform(u32 program, i32 location, i64 x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1i64ARB)
        glIsProgram(program);
    }
    glProgramUniform1i64ARB(program, location, x);
    detail::error_check("ProgramUniform1i64ARB"sv);
}

template<class span_const_i64>
requires(semantic::concepts::Span<span_const_i64>&& std::is_same_v<
         std::decay_t<typename span_const_i64::value_type>,
         std::decay_t<i64>>) STATICINLINE
    void program_uniform(u32 program, i32 location, span_const_i64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1i64vARB)
        glIsProgram(program);
    }
    glProgramUniform1i64vARB(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLint64*>(value.data()));
    detail::error_check("ProgramUniform1i64vARB"sv);
}

STATICINLINE void program_uniform(u32 program, i32 location, u64 x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1ui64ARB)
        glIsProgram(program);
    }
    glProgramUniform1ui64ARB(program, location, x);
    detail::error_check("ProgramUniform1ui64ARB"sv);
}

template<class span_const_u64>
requires(semantic::concepts::Span<span_const_u64>&& std::is_same_v<
         std::decay_t<typename span_const_u64::value_type>,
         std::decay_t<u64>>) STATICINLINE
    void program_uniform(u32 program, i32 location, span_const_u64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1ui64vARB)
        glIsProgram(program);
    }
    glProgramUniform1ui64vARB(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLuint64*>(value.data()));
    detail::error_check("ProgramUniform1ui64vARB"sv);
}

template<class vec_2_i64>
requires(semantic::concepts::Vector<vec_2_i64, i64, 2>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_2_i64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2i64ARB)
        glIsProgram(program);
    }
    glProgramUniform2i64ARB(program, location, x.x(), x.y());
    detail::error_check("ProgramUniform2i64ARB"sv);
}

template<class span_const_vec_2_i64>
requires(semantic::concepts::Span<span_const_vec_2_i64>&& semantic::concepts::
             Vector<typename span_const_vec_2_i64::value_type, i64, 2>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_2_i64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2i64vARB)
        glIsProgram(program);
    }
    glProgramUniform2i64vARB(
        program,
        location,
        count,
        reinterpret_cast<const GLint64*>(value.data()));
    detail::error_check("ProgramUniform2i64vARB"sv);
}

template<class vec_2_u64>
requires(semantic::concepts::Vector<vec_2_u64, u64, 2>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_2_u64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2ui64ARB)
        glIsProgram(program);
    }
    glProgramUniform2ui64ARB(program, location, x.x(), x.y());
    detail::error_check("ProgramUniform2ui64ARB"sv);
}

template<class span_const_vec_2_u64>
requires(semantic::concepts::Span<span_const_vec_2_u64>&& semantic::concepts::
             Vector<typename span_const_vec_2_u64::value_type, u64, 2>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_2_u64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2ui64vARB)
        glIsProgram(program);
    }
    glProgramUniform2ui64vARB(
        program,
        location,
        count,
        reinterpret_cast<const GLuint64*>(value.data()));
    detail::error_check("ProgramUniform2ui64vARB"sv);
}

template<class vec_3_i64>
requires(semantic::concepts::Vector<vec_3_i64, i64, 3>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_3_i64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3i64ARB)
        glIsProgram(program);
    }
    glProgramUniform3i64ARB(program, location, x.x(), x.y(), x.z());
    detail::error_check("ProgramUniform3i64ARB"sv);
}

template<class span_const_vec_3_i64>
requires(semantic::concepts::Span<span_const_vec_3_i64>&& semantic::concepts::
             Vector<typename span_const_vec_3_i64::value_type, i64, 3>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_3_i64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3i64vARB)
        glIsProgram(program);
    }
    glProgramUniform3i64vARB(
        program,
        location,
        count,
        reinterpret_cast<const GLint64*>(value.data()));
    detail::error_check("ProgramUniform3i64vARB"sv);
}

template<class vec_3_u64>
requires(semantic::concepts::Vector<vec_3_u64, u64, 3>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_3_u64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3ui64ARB)
        glIsProgram(program);
    }
    glProgramUniform3ui64ARB(program, location, x.x(), x.y(), x.z());
    detail::error_check("ProgramUniform3ui64ARB"sv);
}

template<class span_const_vec_3_u64>
requires(semantic::concepts::Span<span_const_vec_3_u64>&& semantic::concepts::
             Vector<typename span_const_vec_3_u64::value_type, u64, 3>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_3_u64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3ui64vARB)
        glIsProgram(program);
    }
    glProgramUniform3ui64vARB(
        program,
        location,
        count,
        reinterpret_cast<const GLuint64*>(value.data()));
    detail::error_check("ProgramUniform3ui64vARB"sv);
}

template<class vec_4_i64>
requires(semantic::concepts::Vector<vec_4_i64, i64, 4>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_4_i64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4i64ARB)
        glIsProgram(program);
    }
    glProgramUniform4i64ARB(program, location, x.x(), x.y(), x.z(), x.w());
    detail::error_check("ProgramUniform4i64ARB"sv);
}

template<class span_const_vec_4_i64>
requires(semantic::concepts::Span<span_const_vec_4_i64>&& semantic::concepts::
             Vector<typename span_const_vec_4_i64::value_type, i64, 4>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_4_i64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4i64vARB)
        glIsProgram(program);
    }
    glProgramUniform4i64vARB(
        program,
        location,
        count,
        reinterpret_cast<const GLint64*>(value.data()));
    detail::error_check("ProgramUniform4i64vARB"sv);
}

template<class vec_4_u64>
requires(semantic::concepts::Vector<vec_4_u64, u64, 4>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_4_u64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4ui64ARB)
        glIsProgram(program);
    }
    glProgramUniform4ui64ARB(program, location, x.x(), x.y(), x.z(), x.w());
    detail::error_check("ProgramUniform4ui64ARB"sv);
}

template<class span_const_vec_4_u64>
requires(semantic::concepts::Span<span_const_vec_4_u64>&& semantic::concepts::
             Vector<typename span_const_vec_4_u64::value_type, u64, 4>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_4_u64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4ui64vARB)
        glIsProgram(program);
    }
    glProgramUniform4ui64vARB(
        program,
        location,
        count,
        reinterpret_cast<const GLuint64*>(value.data()));
    detail::error_check("ProgramUniform4ui64vARB"sv);
}

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
requires(semantic::concepts::Span<span_const_i64>&& std::is_same_v<
         std::decay_t<typename span_const_i64::value_type>,
         std::decay_t<i64>>) STATICINLINE
    void uniform(i32 location, i32 count, span_const_i64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1i64vARB)
    }
    glUniform1i64vARB(
        location, count, reinterpret_cast<const GLint64*>(value.data()));
    detail::error_check("Uniform1i64vARB"sv);
}

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
requires(semantic::concepts::Span<span_const_u64>&& std::is_same_v<
         std::decay_t<typename span_const_u64::value_type>,
         std::decay_t<u64>>) STATICINLINE
    void uniform(i32 location, i32 count, span_const_u64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1ui64vARB)
    }
    glUniform1ui64vARB(
        location, count, reinterpret_cast<const GLuint64*>(value.data()));
    detail::error_check("Uniform1ui64vARB"sv);
}

template<class vec_2_i64>
requires(semantic::concepts::Vector<vec_2_i64, i64, 2>) STATICINLINE
    void uniform(i32 location, vec_2_i64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2i64ARB)
    }
    glUniform2i64ARB(location, x.x(), x.y());
    detail::error_check("Uniform2i64ARB"sv);
}

template<class span_const_vec_2_i64>
requires(semantic::concepts::Span<span_const_vec_2_i64>&& semantic::concepts::
             Vector<typename span_const_vec_2_i64::value_type, i64, 2>)
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_2_i64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2i64vARB)
    }
    glUniform2i64vARB(
        location, count, reinterpret_cast<const GLint64*>(value.data()));
    detail::error_check("Uniform2i64vARB"sv);
}

template<class vec_2_u64>
requires(semantic::concepts::Vector<vec_2_u64, u64, 2>) STATICINLINE
    void uniform(i32 location, vec_2_u64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2ui64ARB)
    }
    glUniform2ui64ARB(location, x.x(), x.y());
    detail::error_check("Uniform2ui64ARB"sv);
}

template<class span_const_vec_2_u64>
requires(semantic::concepts::Span<span_const_vec_2_u64>&& semantic::concepts::
             Vector<typename span_const_vec_2_u64::value_type, u64, 2>)
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_2_u64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2ui64vARB)
    }
    glUniform2ui64vARB(
        location, count, reinterpret_cast<const GLuint64*>(value.data()));
    detail::error_check("Uniform2ui64vARB"sv);
}

template<class vec_3_i64>
requires(semantic::concepts::Vector<vec_3_i64, i64, 3>) STATICINLINE
    void uniform(i32 location, vec_3_i64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3i64ARB)
    }
    glUniform3i64ARB(location, x.x(), x.y(), x.z());
    detail::error_check("Uniform3i64ARB"sv);
}

template<class span_const_vec_3_i64>
requires(semantic::concepts::Span<span_const_vec_3_i64>&& semantic::concepts::
             Vector<typename span_const_vec_3_i64::value_type, i64, 3>)
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_3_i64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3i64vARB)
    }
    glUniform3i64vARB(
        location, count, reinterpret_cast<const GLint64*>(value.data()));
    detail::error_check("Uniform3i64vARB"sv);
}

template<class vec_3_u64>
requires(semantic::concepts::Vector<vec_3_u64, u64, 3>) STATICINLINE
    void uniform(i32 location, vec_3_u64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3ui64ARB)
    }
    glUniform3ui64ARB(location, x.x(), x.y(), x.z());
    detail::error_check("Uniform3ui64ARB"sv);
}

template<class span_const_vec_3_u64>
requires(semantic::concepts::Span<span_const_vec_3_u64>&& semantic::concepts::
             Vector<typename span_const_vec_3_u64::value_type, u64, 3>)
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_3_u64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3ui64vARB)
    }
    glUniform3ui64vARB(
        location, count, reinterpret_cast<const GLuint64*>(value.data()));
    detail::error_check("Uniform3ui64vARB"sv);
}

template<class vec_4_i64>
requires(semantic::concepts::Vector<vec_4_i64, i64, 4>) STATICINLINE
    void uniform(i32 location, vec_4_i64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4i64ARB)
    }
    glUniform4i64ARB(location, x.x(), x.y(), x.z(), x.w());
    detail::error_check("Uniform4i64ARB"sv);
}

template<class span_const_vec_4_i64>
requires(semantic::concepts::Span<span_const_vec_4_i64>&& semantic::concepts::
             Vector<typename span_const_vec_4_i64::value_type, i64, 4>)
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_4_i64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4i64vARB)
    }
    glUniform4i64vARB(
        location, count, reinterpret_cast<const GLint64*>(value.data()));
    detail::error_check("Uniform4i64vARB"sv);
}

template<class vec_4_u64>
requires(semantic::concepts::Vector<vec_4_u64, u64, 4>) STATICINLINE
    void uniform(i32 location, vec_4_u64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4ui64ARB)
    }
    glUniform4ui64ARB(location, x.x(), x.y(), x.z(), x.w());
    detail::error_check("Uniform4ui64ARB"sv);
}

template<class span_const_vec_4_u64>
requires(semantic::concepts::Span<span_const_vec_4_u64>&& semantic::concepts::
             Vector<typename span_const_vec_4_u64::value_type, u64, 4>)
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_4_u64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4ui64vARB)
    }
    glUniform4ui64vARB(
        location, count, reinterpret_cast<const GLuint64*>(value.data()));
    detail::error_check("Uniform4ui64vARB"sv);
}

} // namespace gl::arb::gpu_shader_int64
#endif // GL_ARB_gpu_shader_int64
namespace gl::arb::gpu_shader_int64 {
constexpr auto name = "GL_ARB_gpu_shader_int64";
}