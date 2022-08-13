#ifdef GL_VERSION_4_1
/* Introduced in GL core 4.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>) STATICINLINE
    void clear_depthf(f32 d)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearDepthf)
    }
    glClearDepthf(d);
    detail::error_check("ClearDepthf"sv);
}

/* Introduced in GL core 4.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>) STATICINLINE
    void depth_rangef(f32 n, f32 f)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DepthRangef)
    }
    glDepthRangef(n, f);
    detail::error_check("DepthRangef"sv);
}

/* Introduced in GL core 4.1 */
template<class span_i32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_shader_precision_format(
        group::shader_type    shadertype,
        group::precision_type precisiontype,
        span_i32              range,
        i32&                  precision)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetShaderPrecisionFormat)
    }
    glGetShaderPrecisionFormat(
        static_cast<GLenum>(shadertype),
        static_cast<GLenum>(precisiontype),
        range.size() ? reinterpret_cast<GLint*>(range.data()) : nullptr,
        &precision);
    detail::error_check("GetShaderPrecisionFormat"sv);
}

/* Introduced in GL core 4.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>) STATICINLINE
    void release_shader_compiler()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ReleaseShaderCompiler)
    }
    glReleaseShaderCompiler();
    detail::error_check("ReleaseShaderCompiler"sv);
}

/* Introduced in GL core 4.1 */
template<class span_const_u32, class span_const_void>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>&& semantic::concepts::Span<span_const_void>)
    STATICINLINE void shader_binary(
        span_const_u32 const&       shaders,
        group::shader_binary_format binaryFormat,
        span_const_void const&      binary,
        i32                         length)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ShaderBinary)
    }
    glShaderBinary(
        shaders.size(),
        shaders.size() ? reinterpret_cast<const GLuint*>(shaders.data())
                       : nullptr,
        static_cast<GLenum>(binaryFormat),
        binary.size() ? reinterpret_cast<const void*>(binary.data()) : nullptr,
        length);
    detail::error_check("ShaderBinary"sv);
}

/* Introduced in GL core 4.1 */
template<class span_void>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_void>) STATICINLINE
    void get_program_binary(
        u32 program, i32& length, GLenum& binaryFormat, span_void binary)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramBinary)
        glIsProgram(program);
    }
    glGetProgramBinary(
        program,
        binary.size() * sizeof(typename std::decay_t<span_void>::value_type),
        &length,
        &binaryFormat,
        binary.size() ? reinterpret_cast<void*>(binary.data()) : nullptr);
    detail::error_check("GetProgramBinary"sv);
}

/* Introduced in GL core 4.1 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void program_binary(
        u32                    program,
        GLenum                 binaryFormat,
        span_const_void const& binary,
        i32                    length)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramBinary)
        glIsProgram(program);
    }
    glProgramBinary(
        program,
        binaryFormat,
        binary.size() ? reinterpret_cast<const void*>(binary.data()) : nullptr,
        length);
    detail::error_check("ProgramBinary"sv);
}

/* Introduced in GL core 4.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>) STATICINLINE
    void program_parameter(
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

/* Introduced in GL core 4.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>) STATICINLINE
    void active_shader_program(u32 pipeline, u32 program)
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

/* Introduced in GL core 4.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>) STATICINLINE
    void bind_program_pipeline(u32 pipeline)
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

/* Introduced in GL core 4.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>) STATICINLINE GLuint
    create_shader_programv(
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

/* Introduced in GL core 4.1 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
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

/* Introduced in GL core 4.1 */
template<class span_u32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
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

/* Introduced in GL core 4.1 */
template<class span_GLchar>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_GLchar>&& std::is_same_v<
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

/* Introduced in GL core 4.1 */
template<class span_i32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
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

/* Introduced in GL core 4.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>) STATICINLINE GLboolean
    is_program_pipeline(u32 pipeline)
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

/* Introduced in GL core 4.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>) STATICINLINE
    void program_uniform(u32 program, i32 location, f64 v0)
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

/* Introduced in GL core 4.1 */
template<class span_const_f64>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_f64>&& std::is_same_v<
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

/* Introduced in GL core 4.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>) STATICINLINE
    void program_uniform(u32 program, i32 location, f32 v0)
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

/* Introduced in GL core 4.1 */
template<class span_const_f32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_f32>&& std::is_same_v<
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

/* Introduced in GL core 4.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>) STATICINLINE
    void program_uniform(u32 program, i32 location, i32 v0)
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

/* Introduced in GL core 4.1 */
template<class span_const_i32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
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

/* Introduced in GL core 4.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>) STATICINLINE
    void program_uniform(u32 program, i32 location, u32 v0)
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

/* Introduced in GL core 4.1 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
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

/* Introduced in GL core 4.1 */
template<class vec_2_f64>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_2_f64, f64, 2>) STATICINLINE
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

/* Introduced in GL core 4.1 */
template<class span_const_vec_2_f64>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_2_f64>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class vec_2_f32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_2_f32, f32, 2>) STATICINLINE
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

/* Introduced in GL core 4.1 */
template<class span_const_vec_2_f32>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_2_f32>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class vec_2_i32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_2_i32, i32, 2>) STATICINLINE
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

/* Introduced in GL core 4.1 */
template<class span_const_vec_2_i32>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_2_i32>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class vec_2_u32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_2_u32, u32, 2>) STATICINLINE
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

/* Introduced in GL core 4.1 */
template<class span_const_vec_2_u32>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_2_u32>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class vec_3_f64>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_3_f64, f64, 3>) STATICINLINE
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

/* Introduced in GL core 4.1 */
template<class span_const_vec_3_f64>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_3_f64>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class vec_3_f32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_3_f32, f32, 3>) STATICINLINE
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

/* Introduced in GL core 4.1 */
template<class span_const_vec_3_f32>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_3_f32>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class vec_3_i32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_3_i32, i32, 3>) STATICINLINE
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

/* Introduced in GL core 4.1 */
template<class span_const_vec_3_i32>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_3_i32>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class vec_3_u32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_3_u32, u32, 3>) STATICINLINE
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

/* Introduced in GL core 4.1 */
template<class span_const_vec_3_u32>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_3_u32>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class vec_4_f64>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_4_f64, f64, 4>) STATICINLINE
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

/* Introduced in GL core 4.1 */
template<class span_const_vec_4_f64>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_4_f64>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class vec_4_f32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_4_f32, f32, 4>) STATICINLINE
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

/* Introduced in GL core 4.1 */
template<class span_const_vec_4_f32>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_4_f32>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class vec_4_i32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_4_i32, i32, 4>) STATICINLINE
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

/* Introduced in GL core 4.1 */
template<class span_const_vec_4_i32>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_4_i32>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class vec_4_u32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_4_u32, u32, 4>) STATICINLINE
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

/* Introduced in GL core 4.1 */
template<class span_const_vec_4_u32>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_4_u32>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class span_const_mat_2x2_f64>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_2x2_f64>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class span_const_mat_2x2_f32>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_2x2_f32>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class span_const_mat_2x3_f64>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_2x3_f64>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class span_const_mat_2x3_f32>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_2x3_f32>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class span_const_mat_2x4_f64>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_2x4_f64>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class span_const_mat_2x4_f32>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_2x4_f32>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class span_const_mat_3x3_f64>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_3x3_f64>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class span_const_mat_3x3_f32>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_3x3_f32>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class span_const_mat_3x2_f64>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_3x2_f64>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class span_const_mat_3x2_f32>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_3x2_f32>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class span_const_mat_3x4_f64>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_3x4_f64>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class span_const_mat_3x4_f32>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_3x4_f32>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class span_const_mat_4x4_f64>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_4x4_f64>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class span_const_mat_4x4_f32>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_4x4_f32>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class span_const_mat_4x2_f64>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_4x2_f64>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class span_const_mat_4x2_f32>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_4x2_f32>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class span_const_mat_4x3_f64>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_4x3_f64>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<class span_const_mat_4x3_f32>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_4x3_f32>&& semantic::concepts::
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

/* Introduced in GL core 4.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>) STATICINLINE
    void use_program_stages(
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

/* Introduced in GL core 4.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>) STATICINLINE
    void validate_program_pipeline(u32 pipeline)
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

/* Introduced in GL core 4.1 */
template<class span_f64>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_f64>&& std::is_same_v<
                 std::decay_t<typename span_f64::value_type>,
                 std::decay_t<f64>>) STATICINLINE
    void get_vertex_attrib_ldv(
        u32 index, group::vertex_attrib_enum pname, span_f64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribLdv)
    }
    glGetVertexAttribLdv(
        index,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLdouble*>(params.data()) : nullptr);
    detail::error_check("GetVertexAttribLdv"sv);
}

/* Introduced in GL core 4.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>) STATICINLINE
    void vertex_attrib_l1d(u32 index, f64 x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL1d)
    }
    glVertexAttribL1d(index, x);
    detail::error_check("VertexAttribL1d"sv);
}

/* Introduced in GL core 4.1 */
template<class span_const_f64>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_f64>&& std::is_same_v<
                 std::decay_t<typename span_const_f64::value_type>,
                 std::decay_t<f64>>) STATICINLINE
    void vertex_attrib_l1dv(u32 index, span_const_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL1dv)
    }
    glVertexAttribL1dv(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttribL1dv"sv);
}

/* Introduced in GL core 4.1 */
template<class vec_2_f64>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_2_f64, f64, 2>) STATICINLINE
    void vertex_attrib_l2d(u32 index, vec_2_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL2d)
    }
    glVertexAttribL2d(index, x.x(), x.y());
    detail::error_check("VertexAttribL2d"sv);
}

/* Introduced in GL core 4.1 */
template<class span_const_vec_2_f64>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_2_f64>&& semantic::concepts::
            Vector<typename span_const_vec_2_f64::value_type, f64, 2>)
    STATICINLINE
    void vertex_attrib_l2dv(u32 index, span_const_vec_2_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL2dv)
    }
    glVertexAttribL2dv(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttribL2dv"sv);
}

/* Introduced in GL core 4.1 */
template<class vec_3_f64>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_3_f64, f64, 3>) STATICINLINE
    void vertex_attrib_l3d(u32 index, vec_3_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL3d)
    }
    glVertexAttribL3d(index, x.x(), x.y(), x.z());
    detail::error_check("VertexAttribL3d"sv);
}

/* Introduced in GL core 4.1 */
template<class span_const_vec_3_f64>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_3_f64>&& semantic::concepts::
            Vector<typename span_const_vec_3_f64::value_type, f64, 3>)
    STATICINLINE
    void vertex_attrib_l3dv(u32 index, span_const_vec_3_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL3dv)
    }
    glVertexAttribL3dv(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttribL3dv"sv);
}

/* Introduced in GL core 4.1 */
template<class vec_4_f64>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_4_f64, f64, 4>) STATICINLINE
    void vertex_attrib_l4d(u32 index, vec_4_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL4d)
    }
    glVertexAttribL4d(index, x.x(), x.y(), x.z(), x.w());
    detail::error_check("VertexAttribL4d"sv);
}

/* Introduced in GL core 4.1 */
template<class span_const_vec_4_f64>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_4_f64>&& semantic::concepts::
            Vector<typename span_const_vec_4_f64::value_type, f64, 4>)
    STATICINLINE
    void vertex_attrib_l4dv(u32 index, span_const_vec_4_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL4dv)
    }
    glVertexAttribL4dv(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttribL4dv"sv);
}

/* Introduced in GL core 4.1 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void vertex_attrib_l_pointer(
        u32                       index,
        group::vertex_attrib_long type,
        i32                       stride,
        span_const_void const&    pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribLPointer)
    }
    glVertexAttribLPointer(
        index,
        pointer.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        static_cast<GLenum>(type),
        stride,
        pointer.size() ? reinterpret_cast<const void*>(pointer.data())
                       : nullptr);
    detail::error_check("VertexAttribLPointer"sv);
}

/* Introduced in GL core 4.1 */
template<class span_const_f64>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_f64>&& std::is_same_v<
                 std::decay_t<typename span_const_f64::value_type>,
                 std::decay_t<f64>>) STATICINLINE
    void depth_range_arrayv(u32 first, i32 count, span_const_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DepthRangeArrayv)
    }
    glDepthRangeArrayv(
        first,
        count,
        v.size() ? reinterpret_cast<const GLdouble*>(v.data()) : nullptr);
    detail::error_check("DepthRangeArrayv"sv);
}

/* Introduced in GL core 4.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>) STATICINLINE
    void depth_range_indexed(u32 index, f64 n, f64 f)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DepthRangeIndexed)
    }
    glDepthRangeIndexed(index, n, f);
    detail::error_check("DepthRangeIndexed"sv);
}

/* Introduced in GL core 4.1 */
template<class span_f64>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_f64>&& std::is_same_v<
                 std::decay_t<typename span_f64::value_type>,
                 std::decay_t<f64>>) STATICINLINE
    void get_doublei_v(group::get_prop target, u32 index, span_f64 data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetDoublei_v)
    }
    glGetDoublei_v(
        static_cast<GLenum>(target),
        index,
        data.size() ? reinterpret_cast<GLdouble*>(data.data()) : nullptr);
    detail::error_check("GetDoublei_v"sv);
}

/* Introduced in GL core 4.1 */
template<class span_f32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_f32>&& std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void get_floati_v(group::get_prop target, u32 index, span_f32 data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFloati_v)
    }
    glGetFloati_v(
        static_cast<GLenum>(target),
        index,
        data.size() ? reinterpret_cast<GLfloat*>(data.data()) : nullptr);
    detail::error_check("GetFloati_v"sv);
}

/* Introduced in GL core 4.1 */
template<class span_const_i32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void scissor_arrayv(u32 first, i32 count, span_const_i32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ScissorArrayv)
    }
    glScissorArrayv(
        first,
        count,
        v.size() ? reinterpret_cast<const GLint*>(v.data()) : nullptr);
    detail::error_check("ScissorArrayv"sv);
}

/* Introduced in GL core 4.1 */
template<class size_2_i32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void scissor_indexed(
        u32 index, i32 left, i32 bottom, size_2_i32 const& width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ScissorIndexed)
    }
    glScissorIndexed(index, left, bottom, width[0], width[1]);
    detail::error_check("ScissorIndexed"sv);
}

/* Introduced in GL core 4.1 */
template<class span_const_i32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void scissor_indexedv(u32 index, span_const_i32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ScissorIndexedv)
    }
    glScissorIndexedv(
        index, v.size() ? reinterpret_cast<const GLint*>(v.data()) : nullptr);
    detail::error_check("ScissorIndexedv"sv);
}

/* Introduced in GL core 4.1 */
template<class span_const_f32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_f32>&& std::is_same_v<
                 std::decay_t<typename span_const_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void viewport_arrayv(u32 first, i32 count, span_const_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ViewportArrayv)
    }
    glViewportArrayv(
        first,
        count,
        v.size() ? reinterpret_cast<const GLfloat*>(v.data()) : nullptr);
    detail::error_check("ViewportArrayv"sv);
}

/* Introduced in GL core 4.1 */
template<class size_2_f32, class vec_2_f32>
requires(MinimumVersion<Current, Version<4, 1>>&&
                 semantic::concepts::Vector<vec_2_f32, f32, 2>&&
                 semantic::concepts::Size2D<size_2_f32, f32>) STATICINLINE
    void viewport_indexedf(u32 index, vec_2_f32 const& x, size_2_f32 const& w)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ViewportIndexedf)
    }
    glViewportIndexedf(index, x.x(), x.y(), w[0], w[1]);
    detail::error_check("ViewportIndexedf"sv);
}

/* Introduced in GL core 4.1 */
template<class span_const_f32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_f32>&& std::is_same_v<
                 std::decay_t<typename span_const_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void viewport_indexedfv(u32 index, span_const_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ViewportIndexedfv)
    }
    glViewportIndexedfv(
        index, v.size() ? reinterpret_cast<const GLfloat*>(v.data()) : nullptr);
    detail::error_check("ViewportIndexedfv"sv);
}

#endif // GL_VERSION_4_1