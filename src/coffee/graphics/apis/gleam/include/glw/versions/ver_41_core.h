#ifdef GL_VERSION_4_1
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>)
/*!
 * \brief Wraps around glClearDepthf. Introduced in GL core 4.1
 * \param d GLfloat
 * \return void
 */
STATICINLINE void clear_depthf(
    f32 d, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ClearDepthf)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glClearDepthf, d);
#endif
    glClearDepthf(d);
    detail::error_check("ClearDepthf"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>)
/*!
 * \brief Wraps around glDepthRangef. Introduced in GL core 4.1
 * \param n GLfloat
 * \param f GLfloat
 * \return void
 */
STATICINLINE void depth_rangef(
    f32 n, f32 f, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(DepthRangef)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glDepthRangef, n, f);
#endif
    glDepthRangef(n, f);
    detail::error_check("DepthRangef"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<4, 1>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetShaderPrecisionFormat. Introduced in GL core 4.1
 * \param shadertype GLenum
 * \param precisiontype GLenum
 * \param range GLint *
 * \param precision GLint *
 * \return void
 */
STATICINLINE void get_shader_precision_format(
    group::shader_type    shadertype,
    group::precision_type precisiontype,
    span_i32              range,
    i32&                  precision,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetShaderPrecisionFormat)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetShaderPrecisionFormat,
        gsl::span<char>(
            reinterpret_cast<char*>(range.data()), range.size_bytes()),
        shadertype,
        precisiontype,
        precision);
#endif
    glGetShaderPrecisionFormat(
        static_cast<GLenum>(shadertype),
        static_cast<GLenum>(precisiontype),
        range.size() ? reinterpret_cast<GLint*>(range.data()) : nullptr,
        &precision);
    detail::error_check("GetShaderPrecisionFormat"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>)
/*!
 * \brief Wraps around glReleaseShaderCompiler. Introduced in GL core 4.1

 * \return void
 */
STATICINLINE void release_shader_compiler(
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ReleaseShaderCompiler)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glReleaseShaderCompiler);
#endif
    glReleaseShaderCompiler();
    detail::error_check("ReleaseShaderCompiler"sv, check_errors);
}

template<class span_const_u32, class span_const_void>
requires(
    MinimumVersion<Current, Version<4, 1>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>> &&
    concepts::span<span_const_void>)
/*!
 * \brief Wraps around glShaderBinary. Introduced in GL core 4.1
 * \param count GLsizei
 * \param shaders const GLuint *
 * \param binaryFormat GLenum
 * \param binary const void *
 * \param length GLsizei
 * \return void
 */
STATICINLINE void shader_binary(
    span_const_u32 const&       shaders,
    group::shader_binary_format binaryFormat,
    span_const_void const&      binary,
    i32                         length,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ShaderBinary)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glShaderBinary,
        gsl::span<const char>(
            reinterpret_cast<const char*>(shaders.data()),
            shaders.size_bytes()),
        binaryFormat,
        gsl::span<const char>(
            reinterpret_cast<const char*>(binary.data()), binary.size_bytes()),
        length);
#endif
    glShaderBinary(
        shaders.size(),
        shaders.size() ? reinterpret_cast<const GLuint*>(shaders.data())
                       : nullptr,
        static_cast<GLenum>(binaryFormat),
        binary.size() ? reinterpret_cast<const void*>(binary.data()) : nullptr,
        length);
    detail::error_check("ShaderBinary"sv, check_errors);
}

template<class span_void>
requires(MinimumVersion<Current, Version<4, 1>> && concepts::span<span_void>)
/*!
 * \brief Wraps around glGetProgramBinary. Introduced in GL core 4.1
 * \param program GLuint
 * \param bufSize GLsizei
 * \param length GLsizei *
 * \param binaryFormat GLenum *
 * \param binary void *
 * \return void
 */
STATICINLINE void get_program_binary(
    u32         program,
    i32&        length,
    GLenum&     binaryFormat,
    span_void   binary,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetProgramBinary)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetProgramBinary,
        gsl::span<char>(
            reinterpret_cast<char*>(binary.data()), binary.size_bytes()),
        program,
        length,
        binaryFormat);
#endif
    glGetProgramBinary(
        program,
        binary.size() * sizeof(typename std::decay_t<span_void>::value_type),
        &length,
        &binaryFormat,
        binary.size() ? reinterpret_cast<void*>(binary.data()) : nullptr);
    detail::error_check("GetProgramBinary"sv, check_errors);
}

template<class span_const_void>
requires(
    MinimumVersion<Current, Version<4, 1>> && concepts::span<span_const_void>)
/*!
 * \brief Wraps around glProgramBinary. Introduced in GL core 4.1
 * \param program GLuint
 * \param binaryFormat GLenum
 * \param binary const void *
 * \param length GLsizei
 * \return void
 */
STATICINLINE void program_binary(
    u32                    program,
    GLenum                 binaryFormat,
    span_const_void const& binary,
    i32                    length,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramBinary)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramBinary,
        gsl::span<const char>(
            reinterpret_cast<const char*>(binary.data()), binary.size_bytes()),
        program,
        binaryFormat,
        length);
#endif
    glProgramBinary(
        program,
        binaryFormat,
        binary.size() ? reinterpret_cast<const void*>(binary.data()) : nullptr,
        length);
    detail::error_check("ProgramBinary"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>)
/*!
 * \brief Wraps around glProgramParameteri. Introduced in GL core 4.1
 * \param program GLuint
 * \param pname GLenum
 * \param value GLint
 * \return void
 */
STATICINLINE void program_parameter(
    u32                           program,
    group::program_parameter_prop pname,
    i32                           value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramParameteri)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glProgramParameteri, program, pname, value);
#endif
    glProgramParameteri(program, static_cast<GLenum>(pname), value);
    detail::error_check("ProgramParameteri"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>)
/*!
 * \brief Wraps around glActiveShaderProgram. Introduced in GL core 4.1
 * \param pipeline GLuint
 * \param program GLuint
 * \return void
 */
STATICINLINE void active_shader_program(
    u32 pipeline, u32 program, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ActiveShaderProgram)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgramPipeline)
            glIsProgramPipeline(pipeline);
#endif
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glActiveShaderProgram, pipeline, program);
#endif
    glActiveShaderProgram(pipeline, program);
    detail::error_check("ActiveShaderProgram"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>)
/*!
 * \brief Wraps around glBindProgramPipeline. Introduced in GL core 4.1
 * \param pipeline GLuint
 * \return void
 */
STATICINLINE void bind_program_pipeline(
    u32 pipeline, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(BindProgramPipeline)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgramPipeline)
            glIsProgramPipeline(pipeline);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBindProgramPipeline, pipeline);
#endif
    glBindProgramPipeline(pipeline);
    detail::error_check("BindProgramPipeline"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>)
/*!
 * \brief Wraps around glCreateShaderProgramv. Introduced in GL core 4.1
 * \param type GLenum
 * \param count GLsizei
 * \param strings const GLchar *const*
 * \return GLuint
 */
STATICINLINE GLuint create_shader_programv(
    group::shader_type            type,
    std::vector<std::string_view> strings,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(CreateShaderProgramv)
    }
    auto [strings_lens, strings_cstr, strings_store] =
        detail::transform_strings(strings);
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glCreateShaderProgramv, type, strings);
#endif
    auto out = glCreateShaderProgramv(
        static_cast<GLenum>(type), strings_cstr.size(), strings_cstr.data());
    detail::error_check("CreateShaderProgramv"sv, check_errors);
    return out;
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<4, 1>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glDeleteProgramPipelines. Introduced in GL core 4.1
 * \param n GLsizei
 * \param pipelines const GLuint *
 * \return void
 */
STATICINLINE void delete_program_pipelines(
    span_const_u32 const& pipelines, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(DeleteProgramPipelines)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glDeleteProgramPipelines,
        gsl::span<const char>(
            reinterpret_cast<const char*>(pipelines.data()),
            pipelines.size_bytes()));
#endif
    glDeleteProgramPipelines(
        pipelines.size(),
        pipelines.size() ? reinterpret_cast<const GLuint*>(pipelines.data())
                         : nullptr);
    detail::error_check("DeleteProgramPipelines"sv, check_errors);
}

template<class span_u32>
requires(
    MinimumVersion<Current, Version<4, 1>> && concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glGenProgramPipelines. Introduced in GL core 4.1
 * \param n GLsizei
 * \param pipelines GLuint *
 * \return void
 */
STATICINLINE void gen_program_pipelines(
    span_u32 pipelines, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GenProgramPipelines)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGenProgramPipelines,
        gsl::span<char>(
            reinterpret_cast<char*>(pipelines.data()), pipelines.size_bytes()));
#endif
    glGenProgramPipelines(
        pipelines.size(),
        pipelines.size() ? reinterpret_cast<GLuint*>(pipelines.data())
                         : nullptr);
    detail::error_check("GenProgramPipelines"sv, check_errors);
}

template<class span_GLchar>
requires(
    MinimumVersion<Current, Version<4, 1>> && concepts::span<span_GLchar> &&
    std::is_same_v<
        std::decay_t<typename span_GLchar::value_type>,
        std::decay_t<GLchar>>)
/*!
 * \brief Wraps around glGetProgramPipelineInfoLog. Introduced in GL core 4.1
 * \param pipeline GLuint
 * \param bufSize GLsizei
 * \param length GLsizei *
 * \param infoLog GLchar *
 * \return void
 */
STATICINLINE void get_program_pipeline_info_log(
    u32         pipeline,
    i32&        length,
    span_GLchar infoLog,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetProgramPipelineInfoLog)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgramPipeline)
            glIsProgramPipeline(pipeline);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetProgramPipelineInfoLog,
        gsl::span<char>(
            reinterpret_cast<char*>(infoLog.data()), infoLog.size_bytes()),
        pipeline,
        length);
#endif
    glGetProgramPipelineInfoLog(
        pipeline, infoLog.size(), &length, infoLog.data());
    detail::error_check("GetProgramPipelineInfoLog"sv, check_errors);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<4, 1>> && concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetProgramPipelineiv. Introduced in GL core 4.1
 * \param pipeline GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_program_pipelineiv(
    u32                            pipeline,
    group::pipeline_parameter_name pname,
    span_i32                       params,
    error_check                    check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetProgramPipelineiv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgramPipeline)
            glIsProgramPipeline(pipeline);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetProgramPipelineiv,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        pipeline,
        pname);
#endif
    glGetProgramPipelineiv(
        pipeline,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetProgramPipelineiv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>)
/*!
 * \brief Wraps around glIsProgramPipeline. Introduced in GL core 4.1
 * \param pipeline GLuint
 * \return GLboolean
 */
STATICINLINE GLboolean
is_program_pipeline(u32 pipeline, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(IsProgramPipeline)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgramPipeline)
            glIsProgramPipeline(pipeline);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glIsProgramPipeline, pipeline);
#endif
    auto out = glIsProgramPipeline(pipeline);
    detail::error_check("IsProgramPipeline"sv, check_errors);
    return out;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>)
/*!
 * \brief Wraps around glProgramUniform1d. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param v0 GLdouble
 * \return void
 */
STATICINLINE void program_uniform(
    u32         program,
    i32         location,
    f64         v0,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform1d)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glProgramUniform1d, program, location, v0);
#endif
    glProgramUniform1d(program, location, v0);
    detail::error_check("ProgramUniform1d"sv, check_errors);
}

template<class span_const_f64>
requires(
    MinimumVersion<Current, Version<4, 1>> && concepts::span<span_const_f64> &&
    std::is_same_v<
        std::decay_t<typename span_const_f64::value_type>,
        std::decay_t<f64>>)
/*!
 * \brief Wraps around glProgramUniform1dv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                   program,
    i32                   location,
    span_const_f64 const& value,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform1dv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniform1dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location);
#endif
    glProgramUniform1dv(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniform1dv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>)
/*!
 * \brief Wraps around glProgramUniform1f. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param v0 GLfloat
 * \return void
 */
STATICINLINE void program_uniform(
    u32         program,
    i32         location,
    f32         v0,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform1f)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glProgramUniform1f, program, location, v0);
#endif
    glProgramUniform1f(program, location, v0);
    detail::error_check("ProgramUniform1f"sv, check_errors);
}

template<class span_const_f32>
requires(
    MinimumVersion<Current, Version<4, 1>> && concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Wraps around glProgramUniform1fv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                   program,
    i32                   location,
    span_const_f32 const& value,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform1fv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniform1fv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location);
#endif
    glProgramUniform1fv(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniform1fv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>)
/*!
 * \brief Wraps around glProgramUniform1i. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param v0 GLint
 * \return void
 */
STATICINLINE void program_uniform(
    u32         program,
    i32         location,
    i32         v0,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform1i)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glProgramUniform1i, program, location, v0);
#endif
    glProgramUniform1i(program, location, v0);
    detail::error_check("ProgramUniform1i"sv, check_errors);
}

template<class span_const_i32>
requires(
    MinimumVersion<Current, Version<4, 1>> && concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glProgramUniform1iv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLint *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                   program,
    i32                   location,
    span_const_i32 const& value,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform1iv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniform1iv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location);
#endif
    glProgramUniform1iv(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("ProgramUniform1iv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>)
/*!
 * \brief Wraps around glProgramUniform1ui. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param v0 GLuint
 * \return void
 */
STATICINLINE void program_uniform(
    u32         program,
    i32         location,
    u32         v0,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform1ui)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glProgramUniform1ui, program, location, v0);
#endif
    glProgramUniform1ui(program, location, v0);
    detail::error_check("ProgramUniform1ui"sv, check_errors);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<4, 1>> && concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glProgramUniform1uiv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                   program,
    i32                   location,
    span_const_u32 const& value,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform1uiv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniform1uiv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location);
#endif
    glProgramUniform1uiv(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("ProgramUniform1uiv"sv, check_errors);
}

template<class vec_2_f64>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::vector<vec_2_f64, f64, 2>)
/*!
 * \brief Wraps around glProgramUniform2d. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param v0 GLdouble
 * \param v1 GLdouble
 * \return void
 */
STATICINLINE void program_uniform(
    u32              program,
    i32              location,
    vec_2_f64 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform2d)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glProgramUniform2d, program, location, v0);
#endif
    glProgramUniform2d(program, location, v0[0], v0[1]);
    detail::error_check("ProgramUniform2d"sv, check_errors);
}

template<class span_const_vec_2_f64>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_vec_2_f64> &&
    concepts::vector<typename span_const_vec_2_f64::value_type, f64, 2>)
/*!
 * \brief Wraps around glProgramUniform2dv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                         program,
    i32                         location,
    span_const_vec_2_f64 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform2dv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniform2dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location);
#endif
    glProgramUniform2dv(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniform2dv"sv, check_errors);
}

template<class vec_2_f32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::vector<vec_2_f32, f32, 2>)
/*!
 * \brief Wraps around glProgramUniform2f. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param v0 GLfloat
 * \param v1 GLfloat
 * \return void
 */
STATICINLINE void program_uniform(
    u32              program,
    i32              location,
    vec_2_f32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform2f)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glProgramUniform2f, program, location, v0);
#endif
    glProgramUniform2f(program, location, v0[0], v0[1]);
    detail::error_check("ProgramUniform2f"sv, check_errors);
}

template<class span_const_vec_2_f32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_vec_2_f32> &&
    concepts::vector<typename span_const_vec_2_f32::value_type, f32, 2>)
/*!
 * \brief Wraps around glProgramUniform2fv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                         program,
    i32                         location,
    span_const_vec_2_f32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform2fv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniform2fv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location);
#endif
    glProgramUniform2fv(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniform2fv"sv, check_errors);
}

template<class vec_2_i32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::vector<vec_2_i32, i32, 2>)
/*!
 * \brief Wraps around glProgramUniform2i. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param v0 GLint
 * \param v1 GLint
 * \return void
 */
STATICINLINE void program_uniform(
    u32              program,
    i32              location,
    vec_2_i32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform2i)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glProgramUniform2i, program, location, v0);
#endif
    glProgramUniform2i(program, location, v0[0], v0[1]);
    detail::error_check("ProgramUniform2i"sv, check_errors);
}

template<class span_const_vec_2_i32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_vec_2_i32> &&
    concepts::vector<typename span_const_vec_2_i32::value_type, i32, 2>)
/*!
 * \brief Wraps around glProgramUniform2iv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLint *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                         program,
    i32                         location,
    span_const_vec_2_i32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform2iv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniform2iv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location);
#endif
    glProgramUniform2iv(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("ProgramUniform2iv"sv, check_errors);
}

template<class vec_2_u32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::vector<vec_2_u32, u32, 2>)
/*!
 * \brief Wraps around glProgramUniform2ui. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param v0 GLuint
 * \param v1 GLuint
 * \return void
 */
STATICINLINE void program_uniform(
    u32              program,
    i32              location,
    vec_2_u32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform2ui)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glProgramUniform2ui, program, location, v0);
#endif
    glProgramUniform2ui(program, location, v0[0], v0[1]);
    detail::error_check("ProgramUniform2ui"sv, check_errors);
}

template<class span_const_vec_2_u32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_vec_2_u32> &&
    concepts::vector<typename span_const_vec_2_u32::value_type, u32, 2>)
/*!
 * \brief Wraps around glProgramUniform2uiv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                         program,
    i32                         location,
    span_const_vec_2_u32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform2uiv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniform2uiv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location);
#endif
    glProgramUniform2uiv(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("ProgramUniform2uiv"sv, check_errors);
}

template<class vec_3_f64>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::vector<vec_3_f64, f64, 3>)
/*!
 * \brief Wraps around glProgramUniform3d. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param v0 GLdouble
 * \param v1 GLdouble
 * \param v2 GLdouble
 * \return void
 */
STATICINLINE void program_uniform(
    u32              program,
    i32              location,
    vec_3_f64 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform3d)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glProgramUniform3d, program, location, v0);
#endif
    glProgramUniform3d(program, location, v0[0], v0[1], v0[2]);
    detail::error_check("ProgramUniform3d"sv, check_errors);
}

template<class span_const_vec_3_f64>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_vec_3_f64> &&
    concepts::vector<typename span_const_vec_3_f64::value_type, f64, 3>)
/*!
 * \brief Wraps around glProgramUniform3dv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                         program,
    i32                         location,
    span_const_vec_3_f64 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform3dv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniform3dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location);
#endif
    glProgramUniform3dv(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniform3dv"sv, check_errors);
}

template<class vec_3_f32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::vector<vec_3_f32, f32, 3>)
/*!
 * \brief Wraps around glProgramUniform3f. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param v0 GLfloat
 * \param v1 GLfloat
 * \param v2 GLfloat
 * \return void
 */
STATICINLINE void program_uniform(
    u32              program,
    i32              location,
    vec_3_f32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform3f)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glProgramUniform3f, program, location, v0);
#endif
    glProgramUniform3f(program, location, v0[0], v0[1], v0[2]);
    detail::error_check("ProgramUniform3f"sv, check_errors);
}

template<class span_const_vec_3_f32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_vec_3_f32> &&
    concepts::vector<typename span_const_vec_3_f32::value_type, f32, 3>)
/*!
 * \brief Wraps around glProgramUniform3fv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                         program,
    i32                         location,
    span_const_vec_3_f32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform3fv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniform3fv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location);
#endif
    glProgramUniform3fv(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniform3fv"sv, check_errors);
}

template<class vec_3_i32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::vector<vec_3_i32, i32, 3>)
/*!
 * \brief Wraps around glProgramUniform3i. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param v0 GLint
 * \param v1 GLint
 * \param v2 GLint
 * \return void
 */
STATICINLINE void program_uniform(
    u32              program,
    i32              location,
    vec_3_i32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform3i)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glProgramUniform3i, program, location, v0);
#endif
    glProgramUniform3i(program, location, v0[0], v0[1], v0[2]);
    detail::error_check("ProgramUniform3i"sv, check_errors);
}

template<class span_const_vec_3_i32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_vec_3_i32> &&
    concepts::vector<typename span_const_vec_3_i32::value_type, i32, 3>)
/*!
 * \brief Wraps around glProgramUniform3iv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLint *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                         program,
    i32                         location,
    span_const_vec_3_i32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform3iv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniform3iv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location);
#endif
    glProgramUniform3iv(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("ProgramUniform3iv"sv, check_errors);
}

template<class vec_3_u32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::vector<vec_3_u32, u32, 3>)
/*!
 * \brief Wraps around glProgramUniform3ui. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param v0 GLuint
 * \param v1 GLuint
 * \param v2 GLuint
 * \return void
 */
STATICINLINE void program_uniform(
    u32              program,
    i32              location,
    vec_3_u32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform3ui)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glProgramUniform3ui, program, location, v0);
#endif
    glProgramUniform3ui(program, location, v0[0], v0[1], v0[2]);
    detail::error_check("ProgramUniform3ui"sv, check_errors);
}

template<class span_const_vec_3_u32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_vec_3_u32> &&
    concepts::vector<typename span_const_vec_3_u32::value_type, u32, 3>)
/*!
 * \brief Wraps around glProgramUniform3uiv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                         program,
    i32                         location,
    span_const_vec_3_u32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform3uiv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniform3uiv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location);
#endif
    glProgramUniform3uiv(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("ProgramUniform3uiv"sv, check_errors);
}

template<class vec_4_f64>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::vector<vec_4_f64, f64, 4>)
/*!
 * \brief Wraps around glProgramUniform4d. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param v0 GLdouble
 * \param v1 GLdouble
 * \param v2 GLdouble
 * \param v3 GLdouble
 * \return void
 */
STATICINLINE void program_uniform(
    u32              program,
    i32              location,
    vec_4_f64 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform4d)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glProgramUniform4d, program, location, v0);
#endif
    glProgramUniform4d(program, location, v0[0], v0[1], v0[2], v0[3]);
    detail::error_check("ProgramUniform4d"sv, check_errors);
}

template<class span_const_vec_4_f64>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_vec_4_f64> &&
    concepts::vector<typename span_const_vec_4_f64::value_type, f64, 4>)
/*!
 * \brief Wraps around glProgramUniform4dv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                         program,
    i32                         location,
    span_const_vec_4_f64 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform4dv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniform4dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location);
#endif
    glProgramUniform4dv(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniform4dv"sv, check_errors);
}

template<class vec_4_f32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::vector<vec_4_f32, f32, 4>)
/*!
 * \brief Wraps around glProgramUniform4f. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param v0 GLfloat
 * \param v1 GLfloat
 * \param v2 GLfloat
 * \param v3 GLfloat
 * \return void
 */
STATICINLINE void program_uniform(
    u32              program,
    i32              location,
    vec_4_f32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform4f)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glProgramUniform4f, program, location, v0);
#endif
    glProgramUniform4f(program, location, v0[0], v0[1], v0[2], v0[3]);
    detail::error_check("ProgramUniform4f"sv, check_errors);
}

template<class span_const_vec_4_f32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_vec_4_f32> &&
    concepts::vector<typename span_const_vec_4_f32::value_type, f32, 4>)
/*!
 * \brief Wraps around glProgramUniform4fv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                         program,
    i32                         location,
    span_const_vec_4_f32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform4fv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniform4fv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location);
#endif
    glProgramUniform4fv(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniform4fv"sv, check_errors);
}

template<class vec_4_i32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::vector<vec_4_i32, i32, 4>)
/*!
 * \brief Wraps around glProgramUniform4i. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param v0 GLint
 * \param v1 GLint
 * \param v2 GLint
 * \param v3 GLint
 * \return void
 */
STATICINLINE void program_uniform(
    u32              program,
    i32              location,
    vec_4_i32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform4i)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glProgramUniform4i, program, location, v0);
#endif
    glProgramUniform4i(program, location, v0[0], v0[1], v0[2], v0[3]);
    detail::error_check("ProgramUniform4i"sv, check_errors);
}

template<class span_const_vec_4_i32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_vec_4_i32> &&
    concepts::vector<typename span_const_vec_4_i32::value_type, i32, 4>)
/*!
 * \brief Wraps around glProgramUniform4iv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLint *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                         program,
    i32                         location,
    span_const_vec_4_i32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform4iv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniform4iv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location);
#endif
    glProgramUniform4iv(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("ProgramUniform4iv"sv, check_errors);
}

template<class vec_4_u32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::vector<vec_4_u32, u32, 4>)
/*!
 * \brief Wraps around glProgramUniform4ui. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param v0 GLuint
 * \param v1 GLuint
 * \param v2 GLuint
 * \param v3 GLuint
 * \return void
 */
STATICINLINE void program_uniform(
    u32              program,
    i32              location,
    vec_4_u32 const& v0,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform4ui)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glProgramUniform4ui, program, location, v0);
#endif
    glProgramUniform4ui(program, location, v0[0], v0[1], v0[2], v0[3]);
    detail::error_check("ProgramUniform4ui"sv, check_errors);
}

template<class span_const_vec_4_u32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_vec_4_u32> &&
    concepts::vector<typename span_const_vec_4_u32::value_type, u32, 4>)
/*!
 * \brief Wraps around glProgramUniform4uiv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                         program,
    i32                         location,
    span_const_vec_4_u32 const& value,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniform4uiv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniform4uiv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location);
#endif
    glProgramUniform4uiv(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("ProgramUniform4uiv"sv, check_errors);
}

template<class span_const_mat_2x2_f64>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_mat_2x2_f64> &&
    concepts::matrix<typename span_const_mat_2x2_f64::value_type, f64, 2, 2>)
/*!
 * \brief Wraps around glProgramUniformMatrix2dv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_2x2_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix2dv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniformMatrix2dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location,
        transpose);
#endif
    glProgramUniformMatrix2dv(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix2dv"sv, check_errors);
}

template<class span_const_mat_2x2_f32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_mat_2x2_f32> &&
    concepts::matrix<typename span_const_mat_2x2_f32::value_type, f32, 2, 2>)
/*!
 * \brief Wraps around glProgramUniformMatrix2fv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_2x2_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix2fv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniformMatrix2fv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location,
        transpose);
#endif
    glProgramUniformMatrix2fv(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix2fv"sv, check_errors);
}

template<class span_const_mat_2x3_f64>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_mat_2x3_f64> &&
    concepts::matrix<typename span_const_mat_2x3_f64::value_type, f64, 2, 3>)
/*!
 * \brief Wraps around glProgramUniformMatrix2x3dv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_2x3_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix2x3dv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniformMatrix2x3dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location,
        transpose);
#endif
    glProgramUniformMatrix2x3dv(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix2x3dv"sv, check_errors);
}

template<class span_const_mat_2x3_f32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_mat_2x3_f32> &&
    concepts::matrix<typename span_const_mat_2x3_f32::value_type, f32, 2, 3>)
/*!
 * \brief Wraps around glProgramUniformMatrix2x3fv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_2x3_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix2x3fv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniformMatrix2x3fv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location,
        transpose);
#endif
    glProgramUniformMatrix2x3fv(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix2x3fv"sv, check_errors);
}

template<class span_const_mat_2x4_f64>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_mat_2x4_f64> &&
    concepts::matrix<typename span_const_mat_2x4_f64::value_type, f64, 2, 4>)
/*!
 * \brief Wraps around glProgramUniformMatrix2x4dv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_2x4_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix2x4dv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniformMatrix2x4dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location,
        transpose);
#endif
    glProgramUniformMatrix2x4dv(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix2x4dv"sv, check_errors);
}

template<class span_const_mat_2x4_f32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_mat_2x4_f32> &&
    concepts::matrix<typename span_const_mat_2x4_f32::value_type, f32, 2, 4>)
/*!
 * \brief Wraps around glProgramUniformMatrix2x4fv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_2x4_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix2x4fv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniformMatrix2x4fv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location,
        transpose);
#endif
    glProgramUniformMatrix2x4fv(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix2x4fv"sv, check_errors);
}

template<class span_const_mat_3x3_f64>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_mat_3x3_f64> &&
    concepts::matrix<typename span_const_mat_3x3_f64::value_type, f64, 3, 3>)
/*!
 * \brief Wraps around glProgramUniformMatrix3dv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_3x3_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix3dv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniformMatrix3dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location,
        transpose);
#endif
    glProgramUniformMatrix3dv(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix3dv"sv, check_errors);
}

template<class span_const_mat_3x3_f32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_mat_3x3_f32> &&
    concepts::matrix<typename span_const_mat_3x3_f32::value_type, f32, 3, 3>)
/*!
 * \brief Wraps around glProgramUniformMatrix3fv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_3x3_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix3fv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniformMatrix3fv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location,
        transpose);
#endif
    glProgramUniformMatrix3fv(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix3fv"sv, check_errors);
}

template<class span_const_mat_3x2_f64>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_mat_3x2_f64> &&
    concepts::matrix<typename span_const_mat_3x2_f64::value_type, f64, 3, 2>)
/*!
 * \brief Wraps around glProgramUniformMatrix3x2dv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_3x2_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix3x2dv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniformMatrix3x2dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location,
        transpose);
#endif
    glProgramUniformMatrix3x2dv(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix3x2dv"sv, check_errors);
}

template<class span_const_mat_3x2_f32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_mat_3x2_f32> &&
    concepts::matrix<typename span_const_mat_3x2_f32::value_type, f32, 3, 2>)
/*!
 * \brief Wraps around glProgramUniformMatrix3x2fv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_3x2_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix3x2fv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniformMatrix3x2fv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location,
        transpose);
#endif
    glProgramUniformMatrix3x2fv(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix3x2fv"sv, check_errors);
}

template<class span_const_mat_3x4_f64>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_mat_3x4_f64> &&
    concepts::matrix<typename span_const_mat_3x4_f64::value_type, f64, 3, 4>)
/*!
 * \brief Wraps around glProgramUniformMatrix3x4dv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_3x4_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix3x4dv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniformMatrix3x4dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location,
        transpose);
#endif
    glProgramUniformMatrix3x4dv(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix3x4dv"sv, check_errors);
}

template<class span_const_mat_3x4_f32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_mat_3x4_f32> &&
    concepts::matrix<typename span_const_mat_3x4_f32::value_type, f32, 3, 4>)
/*!
 * \brief Wraps around glProgramUniformMatrix3x4fv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_3x4_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix3x4fv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniformMatrix3x4fv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location,
        transpose);
#endif
    glProgramUniformMatrix3x4fv(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix3x4fv"sv, check_errors);
}

template<class span_const_mat_4x4_f64>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_mat_4x4_f64> &&
    concepts::matrix<typename span_const_mat_4x4_f64::value_type, f64, 4, 4>)
/*!
 * \brief Wraps around glProgramUniformMatrix4dv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_4x4_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix4dv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniformMatrix4dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location,
        transpose);
#endif
    glProgramUniformMatrix4dv(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix4dv"sv, check_errors);
}

template<class span_const_mat_4x4_f32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_mat_4x4_f32> &&
    concepts::matrix<typename span_const_mat_4x4_f32::value_type, f32, 4, 4>)
/*!
 * \brief Wraps around glProgramUniformMatrix4fv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_4x4_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix4fv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniformMatrix4fv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location,
        transpose);
#endif
    glProgramUniformMatrix4fv(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix4fv"sv, check_errors);
}

template<class span_const_mat_4x2_f64>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_mat_4x2_f64> &&
    concepts::matrix<typename span_const_mat_4x2_f64::value_type, f64, 4, 2>)
/*!
 * \brief Wraps around glProgramUniformMatrix4x2dv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_4x2_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix4x2dv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniformMatrix4x2dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location,
        transpose);
#endif
    glProgramUniformMatrix4x2dv(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix4x2dv"sv, check_errors);
}

template<class span_const_mat_4x2_f32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_mat_4x2_f32> &&
    concepts::matrix<typename span_const_mat_4x2_f32::value_type, f32, 4, 2>)
/*!
 * \brief Wraps around glProgramUniformMatrix4x2fv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_4x2_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix4x2fv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniformMatrix4x2fv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location,
        transpose);
#endif
    glProgramUniformMatrix4x2fv(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix4x2fv"sv, check_errors);
}

template<class span_const_mat_4x3_f64>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_mat_4x3_f64> &&
    concepts::matrix<typename span_const_mat_4x3_f64::value_type, f64, 4, 3>)
/*!
 * \brief Wraps around glProgramUniformMatrix4x3dv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLdouble *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_4x3_f64 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix4x3dv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniformMatrix4x3dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location,
        transpose);
#endif
    glProgramUniformMatrix4x3dv(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniformMatrix4x3dv"sv, check_errors);
}

template<class span_const_mat_4x3_f32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_mat_4x3_f32> &&
    concepts::matrix<typename span_const_mat_4x3_f32::value_type, f32, 4, 3>)
/*!
 * \brief Wraps around glProgramUniformMatrix4x3fv. Introduced in GL core 4.1
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param transpose GLboolean
 * \param value const GLfloat *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    bool                          transpose,
    span_const_mat_4x3_f32 const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix4x3fv)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniformMatrix4x3fv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location,
        transpose);
#endif
    glProgramUniformMatrix4x3fv(
        program,
        location,
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix4x3fv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>)
/*!
 * \brief Wraps around glUseProgramStages. Introduced in GL core 4.1
 * \param pipeline GLuint
 * \param stages GLbitfield
 * \param program GLuint
 * \return void
 */
STATICINLINE void use_program_stages(
    u32                           pipeline,
    group::use_program_stage_mask stages,
    u32                           program,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(UseProgramStages)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgramPipeline)
            glIsProgramPipeline(pipeline);
#endif
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glUseProgramStages, pipeline, stages, program);
#endif
    glUseProgramStages(pipeline, static_cast<GLenum>(stages), program);
    detail::error_check("UseProgramStages"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>)
/*!
 * \brief Wraps around glValidateProgramPipeline. Introduced in GL core 4.1
 * \param pipeline GLuint
 * \return void
 */
STATICINLINE void validate_program_pipeline(
    u32 pipeline, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ValidateProgramPipeline)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgramPipeline)
            glIsProgramPipeline(pipeline);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glValidateProgramPipeline, pipeline);
#endif
    glValidateProgramPipeline(pipeline);
    detail::error_check("ValidateProgramPipeline"sv, check_errors);
}

template<class span_f64>
requires(
    MinimumVersion<Current, Version<4, 1>> && concepts::span<span_f64> &&
    std::is_same_v<
        std::decay_t<typename span_f64::value_type>,
        std::decay_t<f64>>)
/*!
 * \brief Wraps around glGetVertexAttribLdv. Introduced in GL core 4.1
 * \param index GLuint
 * \param pname GLenum
 * \param params GLdouble *
 * \return void
 */
STATICINLINE void get_vertex_attrib_ldv(
    u32                       index,
    group::vertex_attrib_enum pname,
    span_f64                  params,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetVertexAttribLdv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetVertexAttribLdv,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        index,
        pname);
#endif
    glGetVertexAttribLdv(
        index,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLdouble*>(params.data()) : nullptr);
    detail::error_check("GetVertexAttribLdv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>)
/*!
 * \brief Wraps around glVertexAttribL1d. Introduced in GL core 4.1
 * \param index GLuint
 * \param x GLdouble
 * \return void
 */
STATICINLINE void vertex_attrib_l1d(
    u32 index, f64 x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(VertexAttribL1d)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertexAttribL1d, index, x);
#endif
    glVertexAttribL1d(index, x);
    detail::error_check("VertexAttribL1d"sv, check_errors);
}

template<class span_const_f64>
requires(
    MinimumVersion<Current, Version<4, 1>> && concepts::span<span_const_f64> &&
    std::is_same_v<
        std::decay_t<typename span_const_f64::value_type>,
        std::decay_t<f64>>)
/*!
 * \brief Wraps around glVertexAttribL1dv. Introduced in GL core 4.1
 * \param index GLuint
 * \param v const GLdouble *
 * \return void
 */
STATICINLINE void vertex_attrib_l1dv(
    u32                   index,
    span_const_f64 const& v,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(VertexAttribL1dv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glVertexAttribL1dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        index);
#endif
    glVertexAttribL1dv(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttribL1dv"sv, check_errors);
}

template<class vec_2_f64>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::vector<vec_2_f64, f64, 2>)
/*!
 * \brief Wraps around glVertexAttribL2d. Introduced in GL core 4.1
 * \param index GLuint
 * \param x GLdouble
 * \param y GLdouble
 * \return void
 */
STATICINLINE void vertex_attrib_l2d(
    u32 index, vec_2_f64 const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(VertexAttribL2d)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertexAttribL2d, index, x);
#endif
    glVertexAttribL2d(index, x[0], x[1]);
    detail::error_check("VertexAttribL2d"sv, check_errors);
}

template<class span_const_vec_2_f64>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_vec_2_f64> &&
    concepts::vector<typename span_const_vec_2_f64::value_type, f64, 2>)
/*!
 * \brief Wraps around glVertexAttribL2dv. Introduced in GL core 4.1
 * \param index GLuint
 * \param v const GLdouble *
 * \return void
 */
STATICINLINE void vertex_attrib_l2dv(
    u32                         index,
    span_const_vec_2_f64 const& v,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(VertexAttribL2dv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glVertexAttribL2dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        index);
#endif
    glVertexAttribL2dv(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttribL2dv"sv, check_errors);
}

template<class vec_3_f64>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::vector<vec_3_f64, f64, 3>)
/*!
 * \brief Wraps around glVertexAttribL3d. Introduced in GL core 4.1
 * \param index GLuint
 * \param x GLdouble
 * \param y GLdouble
 * \param z GLdouble
 * \return void
 */
STATICINLINE void vertex_attrib_l3d(
    u32 index, vec_3_f64 const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(VertexAttribL3d)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertexAttribL3d, index, x);
#endif
    glVertexAttribL3d(index, x[0], x[1], x[2]);
    detail::error_check("VertexAttribL3d"sv, check_errors);
}

template<class span_const_vec_3_f64>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_vec_3_f64> &&
    concepts::vector<typename span_const_vec_3_f64::value_type, f64, 3>)
/*!
 * \brief Wraps around glVertexAttribL3dv. Introduced in GL core 4.1
 * \param index GLuint
 * \param v const GLdouble *
 * \return void
 */
STATICINLINE void vertex_attrib_l3dv(
    u32                         index,
    span_const_vec_3_f64 const& v,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(VertexAttribL3dv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glVertexAttribL3dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        index);
#endif
    glVertexAttribL3dv(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttribL3dv"sv, check_errors);
}

template<class vec_4_f64>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::vector<vec_4_f64, f64, 4>)
/*!
 * \brief Wraps around glVertexAttribL4d. Introduced in GL core 4.1
 * \param index GLuint
 * \param x GLdouble
 * \param y GLdouble
 * \param z GLdouble
 * \param w GLdouble
 * \return void
 */
STATICINLINE void vertex_attrib_l4d(
    u32 index, vec_4_f64 const& x, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(VertexAttribL4d)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glVertexAttribL4d, index, x);
#endif
    glVertexAttribL4d(index, x[0], x[1], x[2], x[3]);
    detail::error_check("VertexAttribL4d"sv, check_errors);
}

template<class span_const_vec_4_f64>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::span<span_const_vec_4_f64> &&
    concepts::vector<typename span_const_vec_4_f64::value_type, f64, 4>)
/*!
 * \brief Wraps around glVertexAttribL4dv. Introduced in GL core 4.1
 * \param index GLuint
 * \param v const GLdouble *
 * \return void
 */
STATICINLINE void vertex_attrib_l4dv(
    u32                         index,
    span_const_vec_4_f64 const& v,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(VertexAttribL4dv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glVertexAttribL4dv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        index);
#endif
    glVertexAttribL4dv(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttribL4dv"sv, check_errors);
}

template<class span_const_void>
requires(
    MinimumVersion<Current, Version<4, 1>> && concepts::span<span_const_void>)
/*!
 * \brief Wraps around glVertexAttribLPointer. Introduced in GL core 4.1
 * \param index GLuint
 * \param size GLint
 * \param type GLenum
 * \param stride GLsizei
 * \param pointer const void *
 * \return void
 */
STATICINLINE void vertex_attrib_l_pointer(
    u32                       index,
    i32                       size,
    group::vertex_attrib_long type,
    i32                       stride,
    span_const_void const&    pointer,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(VertexAttribLPointer)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glVertexAttribLPointer,
        gsl::span<const char>(
            reinterpret_cast<const char*>(pointer.data()),
            pointer.size_bytes()),
        index,
        size,
        type,
        stride);
#endif
    glVertexAttribLPointer(
        index,
        size,
        static_cast<GLenum>(type),
        stride,
        pointer.size() ? reinterpret_cast<const void*>(pointer.data())
                       : nullptr);
    detail::error_check("VertexAttribLPointer"sv, check_errors);
}

template<class span_const_f64>
requires(
    MinimumVersion<Current, Version<4, 1>> && concepts::span<span_const_f64> &&
    std::is_same_v<
        std::decay_t<typename span_const_f64::value_type>,
        std::decay_t<f64>>)
/*!
 * \brief Wraps around glDepthRangeArrayv. Introduced in GL core 4.1
 * \param first GLuint
 * \param count GLsizei
 * \param v const GLdouble *
 * \return void
 */
STATICINLINE void depth_range_arrayv(
    u32                   first,
    i32                   count,
    span_const_f64 const& v,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(DepthRangeArrayv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glDepthRangeArrayv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        first,
        count);
#endif
    glDepthRangeArrayv(
        first,
        count,
        v.size() ? reinterpret_cast<const GLdouble*>(v.data()) : nullptr);
    detail::error_check("DepthRangeArrayv"sv, check_errors);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>)
/*!
 * \brief Wraps around glDepthRangeIndexed. Introduced in GL core 4.1
 * \param index GLuint
 * \param n GLdouble
 * \param f GLdouble
 * \return void
 */
STATICINLINE void depth_range_indexed(
    u32 index, f64 n, f64 f, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(DepthRangeIndexed)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glDepthRangeIndexed, index, n, f);
#endif
    glDepthRangeIndexed(index, n, f);
    detail::error_check("DepthRangeIndexed"sv, check_errors);
}

template<class span_f64>
requires(
    MinimumVersion<Current, Version<4, 1>> && concepts::span<span_f64> &&
    std::is_same_v<
        std::decay_t<typename span_f64::value_type>,
        std::decay_t<f64>>)
/*!
 * \brief Wraps around glGetDoublei_v. Introduced in GL core 4.1
 * \param target GLenum
 * \param index GLuint
 * \param data GLdouble *
 * \return void
 */
STATICINLINE void get_doublei_v(
    group::get_prop target,
    u32             index,
    span_f64        data,
    error_check     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetDoublei_v)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetDoublei_v,
        gsl::span<char>(
            reinterpret_cast<char*>(data.data()), data.size_bytes()),
        target,
        index);
#endif
    glGetDoublei_v(
        static_cast<GLenum>(target),
        index,
        data.size() ? reinterpret_cast<GLdouble*>(data.data()) : nullptr);
    detail::error_check("GetDoublei_v"sv, check_errors);
}

template<class span_f32>
requires(
    MinimumVersion<Current, Version<4, 1>> && concepts::span<span_f32> &&
    std::is_same_v<
        std::decay_t<typename span_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Wraps around glGetFloati_v. Introduced in GL core 4.1
 * \param target GLenum
 * \param index GLuint
 * \param data GLfloat *
 * \return void
 */
STATICINLINE void get_floati_v(
    group::get_prop target,
    u32             index,
    span_f32        data,
    error_check     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetFloati_v)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetFloati_v,
        gsl::span<char>(
            reinterpret_cast<char*>(data.data()), data.size_bytes()),
        target,
        index);
#endif
    glGetFloati_v(
        static_cast<GLenum>(target),
        index,
        data.size() ? reinterpret_cast<GLfloat*>(data.data()) : nullptr);
    detail::error_check("GetFloati_v"sv, check_errors);
}

template<class span_const_i32>
requires(
    MinimumVersion<Current, Version<4, 1>> && concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glScissorArrayv. Introduced in GL core 4.1
 * \param first GLuint
 * \param count GLsizei
 * \param v const GLint *
 * \return void
 */
STATICINLINE void scissor_arrayv(
    u32                   first,
    i32                   count,
    span_const_i32 const& v,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ScissorArrayv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glScissorArrayv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        first,
        count);
#endif
    glScissorArrayv(
        first,
        count,
        v.size() ? reinterpret_cast<const GLint*>(v.data()) : nullptr);
    detail::error_check("ScissorArrayv"sv, check_errors);
}

template<class size_2_i32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Wraps around glScissorIndexed. Introduced in GL core 4.1
 * \param index GLuint
 * \param left GLint
 * \param bottom GLint
 * \param width GLsizei
 * \param height GLsizei
 * \return void
 */
STATICINLINE void scissor_indexed(
    u32               index,
    i32               left,
    i32               bottom,
    size_2_i32 const& width,
    error_check       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ScissorIndexed)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glScissorIndexed, index, left, bottom, width);
#endif
    glScissorIndexed(index, left, bottom, width[0], width[1]);
    detail::error_check("ScissorIndexed"sv, check_errors);
}

template<class span_const_i32>
requires(
    MinimumVersion<Current, Version<4, 1>> && concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glScissorIndexedv. Introduced in GL core 4.1
 * \param index GLuint
 * \param v const GLint *
 * \return void
 */
STATICINLINE void scissor_indexedv(
    u32                   index,
    span_const_i32 const& v,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ScissorIndexedv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glScissorIndexedv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        index);
#endif
    glScissorIndexedv(
        index, v.size() ? reinterpret_cast<const GLint*>(v.data()) : nullptr);
    detail::error_check("ScissorIndexedv"sv, check_errors);
}

template<class span_const_f32>
requires(
    MinimumVersion<Current, Version<4, 1>> && concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Wraps around glViewportArrayv. Introduced in GL core 4.1
 * \param first GLuint
 * \param count GLsizei
 * \param v const GLfloat *
 * \return void
 */
STATICINLINE void viewport_arrayv(
    u32                   first,
    i32                   count,
    span_const_f32 const& v,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ViewportArrayv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glViewportArrayv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        first,
        count);
#endif
    glViewportArrayv(
        first,
        count,
        v.size() ? reinterpret_cast<const GLfloat*>(v.data()) : nullptr);
    detail::error_check("ViewportArrayv"sv, check_errors);
}

template<class size_2_f32, class vec_2_f32>
requires(
    MinimumVersion<Current, Version<4, 1>> &&
    concepts::vector<vec_2_f32, f32, 2> && concepts::size_2d<size_2_f32, f32>)
/*!
 * \brief Wraps around glViewportIndexedf. Introduced in GL core 4.1
 * \param index GLuint
 * \param x GLfloat
 * \param y GLfloat
 * \param w GLfloat
 * \param h GLfloat
 * \return void
 */
STATICINLINE void viewport_indexedf(
    u32               index,
    vec_2_f32 const&  x,
    size_2_f32 const& w,
    error_check       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ViewportIndexedf)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glViewportIndexedf, index, x, w);
#endif
    glViewportIndexedf(index, x[0], x[1], w[0], w[1]);
    detail::error_check("ViewportIndexedf"sv, check_errors);
}

template<class span_const_f32>
requires(
    MinimumVersion<Current, Version<4, 1>> && concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Wraps around glViewportIndexedfv. Introduced in GL core 4.1
 * \param index GLuint
 * \param v const GLfloat *
 * \return void
 */
STATICINLINE void viewport_indexedfv(
    u32                   index,
    span_const_f32 const& v,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ViewportIndexedfv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glViewportIndexedfv,
        gsl::span<const char>(
            reinterpret_cast<const char*>(v.data()), v.size_bytes()),
        index);
#endif
    glViewportIndexedfv(
        index, v.size() ? reinterpret_cast<const GLfloat*>(v.data()) : nullptr);
    detail::error_check("ViewportIndexedfv"sv, check_errors);
}

#endif // GL_VERSION_4_1
