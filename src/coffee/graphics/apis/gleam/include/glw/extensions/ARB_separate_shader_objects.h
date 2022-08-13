#pragma once

#ifdef GL_ARB_separate_shader_objects
#include "../enums/GetPName.h"
#include "../enums/PipelineParameterName.h"
#include "../enums/ProgramParameterPName.h"
#include "../enums/UseProgramStageMask.h"
namespace gl::arb::separate_shader_objects {
using gl::group::get_prop;
using gl::group::pipeline_parameter_name;
using gl::group::program_parameter_prop;
using gl::group::use_program_stage_mask;
namespace values {
} // namespace values
STATICINLINE void active_shader_program(u32 pipeline, u32 program)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ActiveShaderProgram)
        glIsProgramPipeline(pipeline);
        glIsProgram(program);
    }
    glActiveShaderProgram(pipeline, program);
    detail::error_check("ActiveShaderProgram"sv);
}

STATICINLINE void bind_program_pipeline(u32 pipeline)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindProgramPipeline)
        glIsProgramPipeline(pipeline);
    }
    glBindProgramPipeline(pipeline);
    detail::error_check("BindProgramPipeline"sv);
}

STATICINLINE GLuint create_shader_programv(
    group::shader_type type, std::vector<std::string_view> strings)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateShaderProgramv)
    }
    auto [strings_lens, strings_cstr, strings_store] =
        detail::transform_strings(strings);
    auto out = glCreateShaderProgramv(
        static_cast<GLenum>(type), strings_cstr.size(), strings_cstr.data());
    detail::error_check("CreateShaderProgramv"sv);
    return out;
}

template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void delete_program_pipelines(span_const_u32 const& pipelines)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteProgramPipelines)
    }
    glDeleteProgramPipelines(
        pipelines.size(),
        pipelines.size() ? reinterpret_cast<const GLuint*>(pipelines.data())
                         : nullptr);
    detail::error_check("DeleteProgramPipelines"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void gen_program_pipelines(span_u32 pipelines)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenProgramPipelines)
    }
    glGenProgramPipelines(
        pipelines.size(),
        pipelines.size() ? reinterpret_cast<GLuint*>(pipelines.data())
                         : nullptr);
    detail::error_check("GenProgramPipelines"sv);
}

template<class span_GLchar>
requires(semantic::concepts::Span<span_GLchar>&& std::is_same_v<
         std::decay_t<typename span_GLchar::value_type>,
         std::decay_t<GLchar>>) STATICINLINE
    void get_program_pipeline_info_log(
        u32 pipeline, i32& length, span_GLchar infoLog)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramPipelineInfoLog)
        glIsProgramPipeline(pipeline);
    }
    glGetProgramPipelineInfoLog(
        pipeline, infoLog.size(), &length, infoLog.data());
    detail::error_check("GetProgramPipelineInfoLog"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_program_pipelineiv(
        u32 pipeline, group::pipeline_parameter_name pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramPipelineiv)
        glIsProgramPipeline(pipeline);
    }
    glGetProgramPipelineiv(
        pipeline,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetProgramPipelineiv"sv);
}

STATICINLINE GLboolean is_program_pipeline(u32 pipeline)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsProgramPipeline)
        glIsProgramPipeline(pipeline);
    }
    auto out = glIsProgramPipeline(pipeline);
    detail::error_check("IsProgramPipeline"sv);
    return out;
}

STATICINLINE void program_parameter(
    u32 program, group::program_parameter_prop pname, i32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramParameteri)
        glIsProgram(program);
    }
    glProgramParameteri(program, static_cast<GLenum>(pname), value);
    detail::error_check("ProgramParameteri"sv);
}

STATICINLINE void program_uniform(u32 program, i32 location, f64 v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1d)
        glIsProgram(program);
    }
    glProgramUniform1d(program, location, v0);
    detail::error_check("ProgramUniform1d"sv);
}

template<class span_const_f64>
requires(semantic::concepts::Span<span_const_f64>&& std::is_same_v<
         std::decay_t<typename span_const_f64::value_type>,
         std::decay_t<f64>>) STATICINLINE
    void program_uniform(u32 program, i32 location, span_const_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1dv)
        glIsProgram(program);
    }
    glProgramUniform1dv(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniform1dv"sv);
}

STATICINLINE void program_uniform(u32 program, i32 location, f32 v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1f)
        glIsProgram(program);
    }
    glProgramUniform1f(program, location, v0);
    detail::error_check("ProgramUniform1f"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void program_uniform(u32 program, i32 location, span_const_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1fv)
        glIsProgram(program);
    }
    glProgramUniform1fv(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniform1fv"sv);
}

STATICINLINE void program_uniform(u32 program, i32 location, i32 v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1i)
        glIsProgram(program);
    }
    glProgramUniform1i(program, location, v0);
    detail::error_check("ProgramUniform1i"sv);
}

template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void program_uniform(u32 program, i32 location, span_const_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1iv)
        glIsProgram(program);
    }
    glProgramUniform1iv(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("ProgramUniform1iv"sv);
}

STATICINLINE void program_uniform(u32 program, i32 location, u32 v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1ui)
        glIsProgram(program);
    }
    glProgramUniform1ui(program, location, v0);
    detail::error_check("ProgramUniform1ui"sv);
}

template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void program_uniform(u32 program, i32 location, span_const_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1uiv)
        glIsProgram(program);
    }
    glProgramUniform1uiv(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("ProgramUniform1uiv"sv);
}

template<class vec_2_f64>
requires(semantic::concepts::Vector<vec_2_f64, f64, 2>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_2_f64 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2d)
        glIsProgram(program);
    }
    glProgramUniform2d(program, location, v0.x(), v0.y());
    detail::error_check("ProgramUniform2d"sv);
}

template<class span_const_vec_2_f64>
requires(semantic::concepts::Span<span_const_vec_2_f64>&& semantic::concepts::
             Vector<typename span_const_vec_2_f64::value_type, f64, 2>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_2_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2dv)
        glIsProgram(program);
    }
    glProgramUniform2dv(
        program,
        location,
        count,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniform2dv"sv);
}

template<class vec_2_f32>
requires(semantic::concepts::Vector<vec_2_f32, f32, 2>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_2_f32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2f)
        glIsProgram(program);
    }
    glProgramUniform2f(program, location, v0.x(), v0.y());
    detail::error_check("ProgramUniform2f"sv);
}

template<class span_const_vec_2_f32>
requires(semantic::concepts::Span<span_const_vec_2_f32>&& semantic::concepts::
             Vector<typename span_const_vec_2_f32::value_type, f32, 2>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_2_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2fv)
        glIsProgram(program);
    }
    glProgramUniform2fv(
        program,
        location,
        count,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniform2fv"sv);
}

template<class vec_2_i32>
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_2_i32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2i)
        glIsProgram(program);
    }
    glProgramUniform2i(program, location, v0.x(), v0.y());
    detail::error_check("ProgramUniform2i"sv);
}

template<class span_const_vec_2_i32>
requires(semantic::concepts::Span<span_const_vec_2_i32>&& semantic::concepts::
             Vector<typename span_const_vec_2_i32::value_type, i32, 2>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_2_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2iv)
        glIsProgram(program);
    }
    glProgramUniform2iv(
        program, location, count, reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("ProgramUniform2iv"sv);
}

template<class vec_2_u32>
requires(semantic::concepts::Vector<vec_2_u32, u32, 2>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_2_u32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2ui)
        glIsProgram(program);
    }
    glProgramUniform2ui(program, location, v0.x(), v0.y());
    detail::error_check("ProgramUniform2ui"sv);
}

template<class span_const_vec_2_u32>
requires(semantic::concepts::Span<span_const_vec_2_u32>&& semantic::concepts::
             Vector<typename span_const_vec_2_u32::value_type, u32, 2>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_2_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2uiv)
        glIsProgram(program);
    }
    glProgramUniform2uiv(
        program,
        location,
        count,
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("ProgramUniform2uiv"sv);
}

template<class vec_3_f64>
requires(semantic::concepts::Vector<vec_3_f64, f64, 3>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_3_f64 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3d)
        glIsProgram(program);
    }
    glProgramUniform3d(program, location, v0.x(), v0.y(), v0.z());
    detail::error_check("ProgramUniform3d"sv);
}

template<class span_const_vec_3_f64>
requires(semantic::concepts::Span<span_const_vec_3_f64>&& semantic::concepts::
             Vector<typename span_const_vec_3_f64::value_type, f64, 3>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_3_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3dv)
        glIsProgram(program);
    }
    glProgramUniform3dv(
        program,
        location,
        count,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniform3dv"sv);
}

template<class vec_3_f32>
requires(semantic::concepts::Vector<vec_3_f32, f32, 3>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_3_f32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3f)
        glIsProgram(program);
    }
    glProgramUniform3f(program, location, v0.x(), v0.y(), v0.z());
    detail::error_check("ProgramUniform3f"sv);
}

template<class span_const_vec_3_f32>
requires(semantic::concepts::Span<span_const_vec_3_f32>&& semantic::concepts::
             Vector<typename span_const_vec_3_f32::value_type, f32, 3>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_3_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3fv)
        glIsProgram(program);
    }
    glProgramUniform3fv(
        program,
        location,
        count,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniform3fv"sv);
}

template<class vec_3_i32>
requires(semantic::concepts::Vector<vec_3_i32, i32, 3>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_3_i32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3i)
        glIsProgram(program);
    }
    glProgramUniform3i(program, location, v0.x(), v0.y(), v0.z());
    detail::error_check("ProgramUniform3i"sv);
}

template<class span_const_vec_3_i32>
requires(semantic::concepts::Span<span_const_vec_3_i32>&& semantic::concepts::
             Vector<typename span_const_vec_3_i32::value_type, i32, 3>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_3_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3iv)
        glIsProgram(program);
    }
    glProgramUniform3iv(
        program, location, count, reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("ProgramUniform3iv"sv);
}

template<class vec_3_u32>
requires(semantic::concepts::Vector<vec_3_u32, u32, 3>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_3_u32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3ui)
        glIsProgram(program);
    }
    glProgramUniform3ui(program, location, v0.x(), v0.y(), v0.z());
    detail::error_check("ProgramUniform3ui"sv);
}

template<class span_const_vec_3_u32>
requires(semantic::concepts::Span<span_const_vec_3_u32>&& semantic::concepts::
             Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_3_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3uiv)
        glIsProgram(program);
    }
    glProgramUniform3uiv(
        program,
        location,
        count,
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("ProgramUniform3uiv"sv);
}

template<class vec_4_f64>
requires(semantic::concepts::Vector<vec_4_f64, f64, 4>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_4_f64 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4d)
        glIsProgram(program);
    }
    glProgramUniform4d(program, location, v0.x(), v0.y(), v0.z(), v0.w());
    detail::error_check("ProgramUniform4d"sv);
}

template<class span_const_vec_4_f64>
requires(semantic::concepts::Span<span_const_vec_4_f64>&& semantic::concepts::
             Vector<typename span_const_vec_4_f64::value_type, f64, 4>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_4_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4dv)
        glIsProgram(program);
    }
    glProgramUniform4dv(
        program,
        location,
        count,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniform4dv"sv);
}

template<class vec_4_f32>
requires(semantic::concepts::Vector<vec_4_f32, f32, 4>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_4_f32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4f)
        glIsProgram(program);
    }
    glProgramUniform4f(program, location, v0.x(), v0.y(), v0.z(), v0.w());
    detail::error_check("ProgramUniform4f"sv);
}

template<class span_const_vec_4_f32>
requires(semantic::concepts::Span<span_const_vec_4_f32>&& semantic::concepts::
             Vector<typename span_const_vec_4_f32::value_type, f32, 4>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_4_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4fv)
        glIsProgram(program);
    }
    glProgramUniform4fv(
        program,
        location,
        count,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniform4fv"sv);
}

template<class vec_4_i32>
requires(semantic::concepts::Vector<vec_4_i32, i32, 4>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_4_i32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4i)
        glIsProgram(program);
    }
    glProgramUniform4i(program, location, v0.x(), v0.y(), v0.z(), v0.w());
    detail::error_check("ProgramUniform4i"sv);
}

template<class span_const_vec_4_i32>
requires(semantic::concepts::Span<span_const_vec_4_i32>&& semantic::concepts::
             Vector<typename span_const_vec_4_i32::value_type, i32, 4>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_4_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4iv)
        glIsProgram(program);
    }
    glProgramUniform4iv(
        program, location, count, reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("ProgramUniform4iv"sv);
}

template<class vec_4_u32>
requires(semantic::concepts::Vector<vec_4_u32, u32, 4>) STATICINLINE
    void program_uniform(u32 program, i32 location, vec_4_u32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4ui)
        glIsProgram(program);
    }
    glProgramUniform4ui(program, location, v0.x(), v0.y(), v0.z(), v0.w());
    detail::error_check("ProgramUniform4ui"sv);
}

template<class span_const_vec_4_u32>
requires(semantic::concepts::Span<span_const_vec_4_u32>&& semantic::concepts::
             Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_4_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4uiv)
        glIsProgram(program);
    }
    glProgramUniform4uiv(
        program,
        location,
        count,
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("ProgramUniform4uiv"sv);
}

template<class span_const_mat_2x2_f64>
requires(semantic::concepts::Span<span_const_mat_2x2_f64>&& semantic::concepts::
             Matrix<typename span_const_mat_2x2_f64::value_type, f64, 2, 2>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_2x2_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix2dv)
        glIsProgram(program);
    }
    glProgramUniformMatrix2dv(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix2dv"sv);
}

template<class span_const_mat_2x2_f32>
requires(semantic::concepts::Span<span_const_mat_2x2_f32>&& semantic::concepts::
             Matrix<typename span_const_mat_2x2_f32::value_type, f32, 2, 2>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_2x2_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix2fv)
        glIsProgram(program);
    }
    glProgramUniformMatrix2fv(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix2fv"sv);
}

template<class span_const_mat_2x3_f64>
requires(semantic::concepts::Span<span_const_mat_2x3_f64>&& semantic::concepts::
             Matrix<typename span_const_mat_2x3_f64::value_type, f64, 2, 3>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_2x3_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix2x3dv)
        glIsProgram(program);
    }
    glProgramUniformMatrix2x3dv(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix2x3dv"sv);
}

template<class span_const_mat_2x3_f32>
requires(semantic::concepts::Span<span_const_mat_2x3_f32>&& semantic::concepts::
             Matrix<typename span_const_mat_2x3_f32::value_type, f32, 2, 3>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_2x3_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix2x3fv)
        glIsProgram(program);
    }
    glProgramUniformMatrix2x3fv(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix2x3fv"sv);
}

template<class span_const_mat_2x4_f64>
requires(semantic::concepts::Span<span_const_mat_2x4_f64>&& semantic::concepts::
             Matrix<typename span_const_mat_2x4_f64::value_type, f64, 2, 4>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_2x4_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix2x4dv)
        glIsProgram(program);
    }
    glProgramUniformMatrix2x4dv(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix2x4dv"sv);
}

template<class span_const_mat_2x4_f32>
requires(semantic::concepts::Span<span_const_mat_2x4_f32>&& semantic::concepts::
             Matrix<typename span_const_mat_2x4_f32::value_type, f32, 2, 4>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_2x4_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix2x4fv)
        glIsProgram(program);
    }
    glProgramUniformMatrix2x4fv(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix2x4fv"sv);
}

template<class span_const_mat_3x3_f64>
requires(semantic::concepts::Span<span_const_mat_3x3_f64>&& semantic::concepts::
             Matrix<typename span_const_mat_3x3_f64::value_type, f64, 3, 3>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_3x3_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix3dv)
        glIsProgram(program);
    }
    glProgramUniformMatrix3dv(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix3dv"sv);
}

template<class span_const_mat_3x3_f32>
requires(semantic::concepts::Span<span_const_mat_3x3_f32>&& semantic::concepts::
             Matrix<typename span_const_mat_3x3_f32::value_type, f32, 3, 3>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_3x3_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix3fv)
        glIsProgram(program);
    }
    glProgramUniformMatrix3fv(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix3fv"sv);
}

template<class span_const_mat_3x2_f64>
requires(semantic::concepts::Span<span_const_mat_3x2_f64>&& semantic::concepts::
             Matrix<typename span_const_mat_3x2_f64::value_type, f64, 3, 2>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_3x2_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix3x2dv)
        glIsProgram(program);
    }
    glProgramUniformMatrix3x2dv(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix3x2dv"sv);
}

template<class span_const_mat_3x2_f32>
requires(semantic::concepts::Span<span_const_mat_3x2_f32>&& semantic::concepts::
             Matrix<typename span_const_mat_3x2_f32::value_type, f32, 3, 2>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_3x2_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix3x2fv)
        glIsProgram(program);
    }
    glProgramUniformMatrix3x2fv(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix3x2fv"sv);
}

template<class span_const_mat_3x4_f64>
requires(semantic::concepts::Span<span_const_mat_3x4_f64>&& semantic::concepts::
             Matrix<typename span_const_mat_3x4_f64::value_type, f64, 3, 4>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_3x4_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix3x4dv)
        glIsProgram(program);
    }
    glProgramUniformMatrix3x4dv(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix3x4dv"sv);
}

template<class span_const_mat_3x4_f32>
requires(semantic::concepts::Span<span_const_mat_3x4_f32>&& semantic::concepts::
             Matrix<typename span_const_mat_3x4_f32::value_type, f32, 3, 4>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_3x4_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix3x4fv)
        glIsProgram(program);
    }
    glProgramUniformMatrix3x4fv(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix3x4fv"sv);
}

template<class span_const_mat_4x4_f64>
requires(semantic::concepts::Span<span_const_mat_4x4_f64>&& semantic::concepts::
             Matrix<typename span_const_mat_4x4_f64::value_type, f64, 4, 4>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_4x4_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix4dv)
        glIsProgram(program);
    }
    glProgramUniformMatrix4dv(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix4dv"sv);
}

template<class span_const_mat_4x4_f32>
requires(semantic::concepts::Span<span_const_mat_4x4_f32>&& semantic::concepts::
             Matrix<typename span_const_mat_4x4_f32::value_type, f32, 4, 4>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_4x4_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix4fv)
        glIsProgram(program);
    }
    glProgramUniformMatrix4fv(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix4fv"sv);
}

template<class span_const_mat_4x2_f64>
requires(semantic::concepts::Span<span_const_mat_4x2_f64>&& semantic::concepts::
             Matrix<typename span_const_mat_4x2_f64::value_type, f64, 4, 2>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_4x2_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix4x2dv)
        glIsProgram(program);
    }
    glProgramUniformMatrix4x2dv(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix4x2dv"sv);
}

template<class span_const_mat_4x2_f32>
requires(semantic::concepts::Span<span_const_mat_4x2_f32>&& semantic::concepts::
             Matrix<typename span_const_mat_4x2_f32::value_type, f32, 4, 2>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_4x2_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix4x2fv)
        glIsProgram(program);
    }
    glProgramUniformMatrix4x2fv(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix4x2fv"sv);
}

template<class span_const_mat_4x3_f64>
requires(semantic::concepts::Span<span_const_mat_4x3_f64>&& semantic::concepts::
             Matrix<typename span_const_mat_4x3_f64::value_type, f64, 4, 3>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_4x3_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix4x3dv)
        glIsProgram(program);
    }
    glProgramUniformMatrix4x3dv(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix4x3dv"sv);
}

template<class span_const_mat_4x3_f32>
requires(semantic::concepts::Span<span_const_mat_4x3_f32>&& semantic::concepts::
             Matrix<typename span_const_mat_4x3_f32::value_type, f32, 4, 3>)
    STATICINLINE void program_uniform(
        u32                           program,
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_4x3_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix4x3fv)
        glIsProgram(program);
    }
    glProgramUniformMatrix4x3fv(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix4x3fv"sv);
}

STATICINLINE void use_program_stages(
    u32 pipeline, group::use_program_stage_mask stages, u32 program)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UseProgramStages)
        glIsProgramPipeline(pipeline);
        glIsProgram(program);
    }
    glUseProgramStages(pipeline, static_cast<GLenum>(stages), program);
    detail::error_check("UseProgramStages"sv);
}

STATICINLINE void validate_program_pipeline(u32 pipeline)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ValidateProgramPipeline)
        glIsProgramPipeline(pipeline);
    }
    glValidateProgramPipeline(pipeline);
    detail::error_check("ValidateProgramPipeline"sv);
}

} // namespace gl::arb::separate_shader_objects
#endif // GL_ARB_separate_shader_objects
namespace gl::arb::separate_shader_objects {
constexpr auto name = "GL_ARB_separate_shader_objects";
}