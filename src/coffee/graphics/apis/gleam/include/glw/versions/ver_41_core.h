#ifdef GL_VERSION_4_1
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>)
    /*!
     * \brief Wraps around glClearDepthf. Introduced in GL core 4.1
     * \param d GLfloat
     * \return void
     */
    STATICINLINE void clear_depthf(f32 d)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearDepthf)
    }
    glClearDepthf(d);
    detail::error_check("ClearDepthf"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>)
    /*!
     * \brief Wraps around glDepthRangef. Introduced in GL core 4.1
     * \param n GLfloat
     * \param f GLfloat
     * \return void
     */
    STATICINLINE void depth_rangef(f32 n, f32 f)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DepthRangef)
    }
    glDepthRangef(n, f);
    detail::error_check("DepthRangef"sv);
}

template<class span_i32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
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

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>)
    /*!
     * \brief Wraps around glReleaseShaderCompiler. Introduced in GL core 4.1

     * \return void
     */
    STATICINLINE void release_shader_compiler()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ReleaseShaderCompiler)
    }
    glReleaseShaderCompiler();
    detail::error_check("ReleaseShaderCompiler"sv);
}

template<class span_const_u32, class span_const_void>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>&& semantic::concepts::Span<span_const_void>)
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

template<class span_void>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_void>)
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
        u32 program, i32& length, GLenum& binaryFormat, span_void binary)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramBinary)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetProgramBinary(
        program,
        binary.size() * sizeof(typename std::decay_t<span_void>::value_type),
        &length,
        &binaryFormat,
        binary.size() ? reinterpret_cast<void*>(binary.data()) : nullptr);
    detail::error_check("GetProgramBinary"sv);
}

template<class span_const_void>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_void>)
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
        i32                    length)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramBinary)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramBinary(
        program,
        binaryFormat,
        binary.size() ? reinterpret_cast<const void*>(binary.data()) : nullptr,
        length);
    detail::error_check("ProgramBinary"sv);
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
        u32 program, group::program_parameter_prop pname, i32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramParameteri)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramParameteri(program, static_cast<GLenum>(pname), value);
    detail::error_check("ProgramParameteri"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>)
    /*!
     * \brief Wraps around glActiveShaderProgram. Introduced in GL core 4.1
     * \param pipeline GLuint
     * \param program GLuint
     * \return void
     */
    STATICINLINE void active_shader_program(u32 pipeline, u32 program)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ActiveShaderProgram)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgramPipeline)
            glIsProgramPipeline(pipeline);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glActiveShaderProgram(pipeline, program);
    detail::error_check("ActiveShaderProgram"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>)
    /*!
     * \brief Wraps around glBindProgramPipeline. Introduced in GL core 4.1
     * \param pipeline GLuint
     * \return void
     */
    STATICINLINE void bind_program_pipeline(u32 pipeline)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindProgramPipeline)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgramPipeline)
            glIsProgramPipeline(pipeline);
#endif
    }
    glBindProgramPipeline(pipeline);
    detail::error_check("BindProgramPipeline"sv);
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
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>)
    /*!
     * \brief Wraps around glDeleteProgramPipelines. Introduced in GL core 4.1
     * \param n GLsizei
     * \param pipelines const GLuint *
     * \return void
     */
    STATICINLINE void delete_program_pipelines(span_const_u32 const& pipelines)
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
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>)
    /*!
     * \brief Wraps around glGenProgramPipelines. Introduced in GL core 4.1
     * \param n GLsizei
     * \param pipelines GLuint *
     * \return void
     */
    STATICINLINE void gen_program_pipelines(span_u32 pipelines)
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
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>)
    /*!
     * \brief Wraps around glGetProgramPipelineInfoLog. Introduced in GL
     * core 4.1 \param pipeline GLuint \param bufSize GLsizei \param length
     * GLsizei * \param infoLog GLchar * \return void
     */
    STATICINLINE void get_program_pipeline_info_log(
        u32 pipeline, i32& length, span_GLchar infoLog)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramPipelineInfoLog)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgramPipeline)
            glIsProgramPipeline(pipeline);
#endif
    }
    glGetProgramPipelineInfoLog(
        pipeline, infoLog.size(), &length, infoLog.data());
    detail::error_check("GetProgramPipelineInfoLog"sv);
}

template<class span_i32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
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
        u32 pipeline, group::pipeline_parameter_name pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramPipelineiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgramPipeline)
            glIsProgramPipeline(pipeline);
#endif
    }
    glGetProgramPipelineiv(
        pipeline,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetProgramPipelineiv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>)
    /*!
     * \brief Wraps around glIsProgramPipeline. Introduced in GL core 4.1
     * \param pipeline GLuint
     * \return Boolean
     */
    STATICINLINE GLboolean is_program_pipeline(u32 pipeline)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsProgramPipeline)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgramPipeline)
            glIsProgramPipeline(pipeline);
#endif
    }
    auto out = glIsProgramPipeline(pipeline);
    detail::error_check("IsProgramPipeline"sv);
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
    STATICINLINE void program_uniform(u32 program, i32 location, f64 v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1d)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform1d(program, location, v0);
    detail::error_check("ProgramUniform1d"sv);
}

template<class span_const_f64>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_f64>&& std::is_same_v<
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
    STATICINLINE
    void program_uniform(u32 program, i32 location, span_const_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1dv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform1dv(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniform1dv"sv);
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
    STATICINLINE void program_uniform(u32 program, i32 location, f32 v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1f)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform1f(program, location, v0);
    detail::error_check("ProgramUniform1f"sv);
}

template<class span_const_f32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_f32>&& std::is_same_v<
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
    STATICINLINE
    void program_uniform(u32 program, i32 location, span_const_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1fv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform1fv(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniform1fv"sv);
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
    STATICINLINE void program_uniform(u32 program, i32 location, i32 v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1i)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform1i(program, location, v0);
    detail::error_check("ProgramUniform1i"sv);
}

template<class span_const_i32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
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
    STATICINLINE
    void program_uniform(u32 program, i32 location, span_const_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1iv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform1iv(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("ProgramUniform1iv"sv);
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
    STATICINLINE void program_uniform(u32 program, i32 location, u32 v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1ui)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform1ui(program, location, v0);
    detail::error_check("ProgramUniform1ui"sv);
}

template<class span_const_u32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
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
    STATICINLINE
    void program_uniform(u32 program, i32 location, span_const_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform1uiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform1uiv(
        program,
        location,
        value.size(),
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("ProgramUniform1uiv"sv);
}

template<class vec_2_f64>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_2_f64, f64, 2>)
    /*!
     * \brief Wraps around glProgramUniform2d. Introduced in GL core 4.1
     * \param program GLuint
     * \param location GLint
     * \param v0 GLdouble
     * \param v1 GLdouble
     * \return void
     */
    STATICINLINE
    void program_uniform(u32 program, i32 location, vec_2_f64 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2d)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform2d(program, location, v0.x(), v0.y());
    detail::error_check("ProgramUniform2d"sv);
}

template<class span_const_vec_2_f64>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_2_f64>&& semantic::concepts::
            Vector<typename span_const_vec_2_f64::value_type, f64, 2>)
    /*!
     * \brief Wraps around glProgramUniform2dv. Introduced in GL core 4.1
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLdouble *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_2_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2dv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform2dv(
        program,
        location,
        count,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniform2dv"sv);
}

template<class vec_2_f32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_2_f32, f32, 2>)
    /*!
     * \brief Wraps around glProgramUniform2f. Introduced in GL core 4.1
     * \param program GLuint
     * \param location GLint
     * \param v0 GLfloat
     * \param v1 GLfloat
     * \return void
     */
    STATICINLINE
    void program_uniform(u32 program, i32 location, vec_2_f32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2f)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform2f(program, location, v0.x(), v0.y());
    detail::error_check("ProgramUniform2f"sv);
}

template<class span_const_vec_2_f32>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_2_f32>&& semantic::concepts::
            Vector<typename span_const_vec_2_f32::value_type, f32, 2>)
    /*!
     * \brief Wraps around glProgramUniform2fv. Introduced in GL core 4.1
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLfloat *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_2_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2fv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform2fv(
        program,
        location,
        count,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniform2fv"sv);
}

template<class vec_2_i32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_2_i32, i32, 2>)
    /*!
     * \brief Wraps around glProgramUniform2i. Introduced in GL core 4.1
     * \param program GLuint
     * \param location GLint
     * \param v0 GLint
     * \param v1 GLint
     * \return void
     */
    STATICINLINE
    void program_uniform(u32 program, i32 location, vec_2_i32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2i)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform2i(program, location, v0.x(), v0.y());
    detail::error_check("ProgramUniform2i"sv);
}

template<class span_const_vec_2_i32>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_2_i32>&& semantic::concepts::
            Vector<typename span_const_vec_2_i32::value_type, i32, 2>)
    /*!
     * \brief Wraps around glProgramUniform2iv. Introduced in GL core 4.1
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLint *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_2_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2iv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform2iv(
        program, location, count, reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("ProgramUniform2iv"sv);
}

template<class vec_2_u32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_2_u32, u32, 2>)
    /*!
     * \brief Wraps around glProgramUniform2ui. Introduced in GL core 4.1
     * \param program GLuint
     * \param location GLint
     * \param v0 GLuint
     * \param v1 GLuint
     * \return void
     */
    STATICINLINE
    void program_uniform(u32 program, i32 location, vec_2_u32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2ui)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform2ui(program, location, v0.x(), v0.y());
    detail::error_check("ProgramUniform2ui"sv);
}

template<class span_const_vec_2_u32>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_2_u32>&& semantic::concepts::
            Vector<typename span_const_vec_2_u32::value_type, u32, 2>)
    /*!
     * \brief Wraps around glProgramUniform2uiv. Introduced in GL core 4.1
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLuint *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_2_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform2uiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform2uiv(
        program,
        location,
        count,
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("ProgramUniform2uiv"sv);
}

template<class vec_3_f64>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_3_f64, f64, 3>)
    /*!
     * \brief Wraps around glProgramUniform3d. Introduced in GL core 4.1
     * \param program GLuint
     * \param location GLint
     * \param v0 GLdouble
     * \param v1 GLdouble
     * \param v2 GLdouble
     * \return void
     */
    STATICINLINE
    void program_uniform(u32 program, i32 location, vec_3_f64 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3d)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform3d(program, location, v0.x(), v0.y(), v0.z());
    detail::error_check("ProgramUniform3d"sv);
}

template<class span_const_vec_3_f64>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_3_f64>&& semantic::concepts::
            Vector<typename span_const_vec_3_f64::value_type, f64, 3>)
    /*!
     * \brief Wraps around glProgramUniform3dv. Introduced in GL core 4.1
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLdouble *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_3_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3dv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform3dv(
        program,
        location,
        count,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniform3dv"sv);
}

template<class vec_3_f32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_3_f32, f32, 3>)
    /*!
     * \brief Wraps around glProgramUniform3f. Introduced in GL core 4.1
     * \param program GLuint
     * \param location GLint
     * \param v0 GLfloat
     * \param v1 GLfloat
     * \param v2 GLfloat
     * \return void
     */
    STATICINLINE
    void program_uniform(u32 program, i32 location, vec_3_f32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3f)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform3f(program, location, v0.x(), v0.y(), v0.z());
    detail::error_check("ProgramUniform3f"sv);
}

template<class span_const_vec_3_f32>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_3_f32>&& semantic::concepts::
            Vector<typename span_const_vec_3_f32::value_type, f32, 3>)
    /*!
     * \brief Wraps around glProgramUniform3fv. Introduced in GL core 4.1
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLfloat *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_3_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3fv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform3fv(
        program,
        location,
        count,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniform3fv"sv);
}

template<class vec_3_i32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_3_i32, i32, 3>)
    /*!
     * \brief Wraps around glProgramUniform3i. Introduced in GL core 4.1
     * \param program GLuint
     * \param location GLint
     * \param v0 GLint
     * \param v1 GLint
     * \param v2 GLint
     * \return void
     */
    STATICINLINE
    void program_uniform(u32 program, i32 location, vec_3_i32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3i)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform3i(program, location, v0.x(), v0.y(), v0.z());
    detail::error_check("ProgramUniform3i"sv);
}

template<class span_const_vec_3_i32>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_3_i32>&& semantic::concepts::
            Vector<typename span_const_vec_3_i32::value_type, i32, 3>)
    /*!
     * \brief Wraps around glProgramUniform3iv. Introduced in GL core 4.1
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLint *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_3_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3iv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform3iv(
        program, location, count, reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("ProgramUniform3iv"sv);
}

template<class vec_3_u32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_3_u32, u32, 3>)
    /*!
     * \brief Wraps around glProgramUniform3ui. Introduced in GL core 4.1
     * \param program GLuint
     * \param location GLint
     * \param v0 GLuint
     * \param v1 GLuint
     * \param v2 GLuint
     * \return void
     */
    STATICINLINE
    void program_uniform(u32 program, i32 location, vec_3_u32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3ui)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform3ui(program, location, v0.x(), v0.y(), v0.z());
    detail::error_check("ProgramUniform3ui"sv);
}

template<class span_const_vec_3_u32>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_3_u32>&& semantic::concepts::
            Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
    /*!
     * \brief Wraps around glProgramUniform3uiv. Introduced in GL core 4.1
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLuint *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_3_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform3uiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform3uiv(
        program,
        location,
        count,
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("ProgramUniform3uiv"sv);
}

template<class vec_4_f64>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_4_f64, f64, 4>)
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
    STATICINLINE
    void program_uniform(u32 program, i32 location, vec_4_f64 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4d)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform4d(program, location, v0.x(), v0.y(), v0.z(), v0.w());
    detail::error_check("ProgramUniform4d"sv);
}

template<class span_const_vec_4_f64>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_4_f64>&& semantic::concepts::
            Vector<typename span_const_vec_4_f64::value_type, f64, 4>)
    /*!
     * \brief Wraps around glProgramUniform4dv. Introduced in GL core 4.1
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLdouble *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_4_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4dv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform4dv(
        program,
        location,
        count,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("ProgramUniform4dv"sv);
}

template<class vec_4_f32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_4_f32, f32, 4>)
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
    STATICINLINE
    void program_uniform(u32 program, i32 location, vec_4_f32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4f)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform4f(program, location, v0.x(), v0.y(), v0.z(), v0.w());
    detail::error_check("ProgramUniform4f"sv);
}

template<class span_const_vec_4_f32>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_4_f32>&& semantic::concepts::
            Vector<typename span_const_vec_4_f32::value_type, f32, 4>)
    /*!
     * \brief Wraps around glProgramUniform4fv. Introduced in GL core 4.1
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLfloat *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_4_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4fv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform4fv(
        program,
        location,
        count,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniform4fv"sv);
}

template<class vec_4_i32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_4_i32, i32, 4>)
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
    STATICINLINE
    void program_uniform(u32 program, i32 location, vec_4_i32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4i)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform4i(program, location, v0.x(), v0.y(), v0.z(), v0.w());
    detail::error_check("ProgramUniform4i"sv);
}

template<class span_const_vec_4_i32>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_4_i32>&& semantic::concepts::
            Vector<typename span_const_vec_4_i32::value_type, i32, 4>)
    /*!
     * \brief Wraps around glProgramUniform4iv. Introduced in GL core 4.1
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLint *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_4_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4iv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform4iv(
        program, location, count, reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("ProgramUniform4iv"sv);
}

template<class vec_4_u32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_4_u32, u32, 4>)
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
    STATICINLINE
    void program_uniform(u32 program, i32 location, vec_4_u32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4ui)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform4ui(program, location, v0.x(), v0.y(), v0.z(), v0.w());
    detail::error_check("ProgramUniform4ui"sv);
}

template<class span_const_vec_4_u32>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_4_u32>&& semantic::concepts::
            Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
    /*!
     * \brief Wraps around glProgramUniform4uiv. Introduced in GL core 4.1
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLuint *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, i32 count, span_const_vec_4_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniform4uiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniform4uiv(
        program,
        location,
        count,
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("ProgramUniform4uiv"sv);
}

template<class span_const_mat_2x2_f64>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_2x2_f64>&& semantic::concepts::
            Matrix<typename span_const_mat_2x2_f64::value_type, f64, 2, 2>)
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
        i32                           count,
        bool                          transpose,
        span_const_mat_2x2_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix2dv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
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
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_2x2_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_2x2_f32::value_type, f32, 2, 2>)
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
        i32                           count,
        bool                          transpose,
        span_const_mat_2x2_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix2fv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
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
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_2x3_f64>&& semantic::concepts::
            Matrix<typename span_const_mat_2x3_f64::value_type, f64, 2, 3>)
    /*!
     * \brief Wraps around glProgramUniformMatrix2x3dv. Introduced in GL
     * core 4.1 \param program GLuint \param location GLint \param count GLsizei
     * \param transpose GLboolean
     * \param value const GLdouble *
     * \return void
     */
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
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
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_2x3_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_2x3_f32::value_type, f32, 2, 3>)
    /*!
     * \brief Wraps around glProgramUniformMatrix2x3fv. Introduced in GL
     * core 4.1 \param program GLuint \param location GLint \param count GLsizei
     * \param transpose GLboolean
     * \param value const GLfloat *
     * \return void
     */
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
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
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_2x4_f64>&& semantic::concepts::
            Matrix<typename span_const_mat_2x4_f64::value_type, f64, 2, 4>)
    /*!
     * \brief Wraps around glProgramUniformMatrix2x4dv. Introduced in GL
     * core 4.1 \param program GLuint \param location GLint \param count GLsizei
     * \param transpose GLboolean
     * \param value const GLdouble *
     * \return void
     */
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
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
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_2x4_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_2x4_f32::value_type, f32, 2, 4>)
    /*!
     * \brief Wraps around glProgramUniformMatrix2x4fv. Introduced in GL
     * core 4.1 \param program GLuint \param location GLint \param count GLsizei
     * \param transpose GLboolean
     * \param value const GLfloat *
     * \return void
     */
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
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
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_3x3_f64>&& semantic::concepts::
            Matrix<typename span_const_mat_3x3_f64::value_type, f64, 3, 3>)
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
        i32                           count,
        bool                          transpose,
        span_const_mat_3x3_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix3dv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
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
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_3x3_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_3x3_f32::value_type, f32, 3, 3>)
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
        i32                           count,
        bool                          transpose,
        span_const_mat_3x3_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix3fv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
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
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_3x2_f64>&& semantic::concepts::
            Matrix<typename span_const_mat_3x2_f64::value_type, f64, 3, 2>)
    /*!
     * \brief Wraps around glProgramUniformMatrix3x2dv. Introduced in GL
     * core 4.1 \param program GLuint \param location GLint \param count GLsizei
     * \param transpose GLboolean
     * \param value const GLdouble *
     * \return void
     */
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
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
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_3x2_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_3x2_f32::value_type, f32, 3, 2>)
    /*!
     * \brief Wraps around glProgramUniformMatrix3x2fv. Introduced in GL
     * core 4.1 \param program GLuint \param location GLint \param count GLsizei
     * \param transpose GLboolean
     * \param value const GLfloat *
     * \return void
     */
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
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
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_3x4_f64>&& semantic::concepts::
            Matrix<typename span_const_mat_3x4_f64::value_type, f64, 3, 4>)
    /*!
     * \brief Wraps around glProgramUniformMatrix3x4dv. Introduced in GL
     * core 4.1 \param program GLuint \param location GLint \param count GLsizei
     * \param transpose GLboolean
     * \param value const GLdouble *
     * \return void
     */
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
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
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_3x4_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_3x4_f32::value_type, f32, 3, 4>)
    /*!
     * \brief Wraps around glProgramUniformMatrix3x4fv. Introduced in GL
     * core 4.1 \param program GLuint \param location GLint \param count GLsizei
     * \param transpose GLboolean
     * \param value const GLfloat *
     * \return void
     */
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
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
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_4x4_f64>&& semantic::concepts::
            Matrix<typename span_const_mat_4x4_f64::value_type, f64, 4, 4>)
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
        i32                           count,
        bool                          transpose,
        span_const_mat_4x4_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix4dv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
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
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_4x4_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_4x4_f32::value_type, f32, 4, 4>)
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
        i32                           count,
        bool                          transpose,
        span_const_mat_4x4_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformMatrix4fv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
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
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_4x2_f64>&& semantic::concepts::
            Matrix<typename span_const_mat_4x2_f64::value_type, f64, 4, 2>)
    /*!
     * \brief Wraps around glProgramUniformMatrix4x2dv. Introduced in GL
     * core 4.1 \param program GLuint \param location GLint \param count GLsizei
     * \param transpose GLboolean
     * \param value const GLdouble *
     * \return void
     */
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
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
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_4x2_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_4x2_f32::value_type, f32, 4, 2>)
    /*!
     * \brief Wraps around glProgramUniformMatrix4x2fv. Introduced in GL
     * core 4.1 \param program GLuint \param location GLint \param count GLsizei
     * \param transpose GLboolean
     * \param value const GLfloat *
     * \return void
     */
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
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
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_4x3_f64>&& semantic::concepts::
            Matrix<typename span_const_mat_4x3_f64::value_type, f64, 4, 3>)
    /*!
     * \brief Wraps around glProgramUniformMatrix4x3dv. Introduced in GL
     * core 4.1 \param program GLuint \param location GLint \param count GLsizei
     * \param transpose GLboolean
     * \param value const GLdouble *
     * \return void
     */
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
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
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_mat_4x3_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_4x3_f32::value_type, f32, 4, 3>)
    /*!
     * \brief Wraps around glProgramUniformMatrix4x3fv. Introduced in GL
     * core 4.1 \param program GLuint \param location GLint \param count GLsizei
     * \param transpose GLboolean
     * \param value const GLfloat *
     * \return void
     */
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniformMatrix4x3fv(
        program,
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix4x3fv"sv);
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
        u32 pipeline, group::use_program_stage_mask stages, u32 program)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UseProgramStages)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgramPipeline)
            glIsProgramPipeline(pipeline);
#endif
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glUseProgramStages(pipeline, static_cast<GLenum>(stages), program);
    detail::error_check("UseProgramStages"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>)
    /*!
     * \brief Wraps around glValidateProgramPipeline. Introduced in GL core 4.1
     * \param pipeline GLuint
     * \return void
     */
    STATICINLINE void validate_program_pipeline(u32 pipeline)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ValidateProgramPipeline)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgramPipeline)
            glIsProgramPipeline(pipeline);
#endif
    }
    glValidateProgramPipeline(pipeline);
    detail::error_check("ValidateProgramPipeline"sv);
}

template<class span_f64>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_f64>&& std::is_same_v<
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

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>)
    /*!
     * \brief Wraps around glVertexAttribL1d. Introduced in GL core 4.1
     * \param index GLuint
     * \param x GLdouble
     * \return void
     */
    STATICINLINE void vertex_attrib_l1d(u32 index, f64 x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL1d)
    }
    glVertexAttribL1d(index, x);
    detail::error_check("VertexAttribL1d"sv);
}

template<class span_const_f64>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_f64>&& std::is_same_v<
                 std::decay_t<typename span_const_f64::value_type>,
                 std::decay_t<f64>>)
    /*!
     * \brief Wraps around glVertexAttribL1dv. Introduced in GL core 4.1
     * \param index GLuint
     * \param v const GLdouble *
     * \return void
     */
    STATICINLINE void vertex_attrib_l1dv(u32 index, span_const_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL1dv)
    }
    glVertexAttribL1dv(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttribL1dv"sv);
}

template<class vec_2_f64>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_2_f64, f64, 2>)
    /*!
     * \brief Wraps around glVertexAttribL2d. Introduced in GL core 4.1
     * \param index GLuint
     * \param x GLdouble
     * \param y GLdouble
     * \return void
     */
    STATICINLINE void vertex_attrib_l2d(u32 index, vec_2_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL2d)
    }
    glVertexAttribL2d(index, x.x(), x.y());
    detail::error_check("VertexAttribL2d"sv);
}

template<class span_const_vec_2_f64>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_2_f64>&& semantic::concepts::
            Vector<typename span_const_vec_2_f64::value_type, f64, 2>)
    /*!
     * \brief Wraps around glVertexAttribL2dv. Introduced in GL core 4.1
     * \param index GLuint
     * \param v const GLdouble *
     * \return void
     */
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

template<class vec_3_f64>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_3_f64, f64, 3>)
    /*!
     * \brief Wraps around glVertexAttribL3d. Introduced in GL core 4.1
     * \param index GLuint
     * \param x GLdouble
     * \param y GLdouble
     * \param z GLdouble
     * \return void
     */
    STATICINLINE void vertex_attrib_l3d(u32 index, vec_3_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL3d)
    }
    glVertexAttribL3d(index, x.x(), x.y(), x.z());
    detail::error_check("VertexAttribL3d"sv);
}

template<class span_const_vec_3_f64>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_3_f64>&& semantic::concepts::
            Vector<typename span_const_vec_3_f64::value_type, f64, 3>)
    /*!
     * \brief Wraps around glVertexAttribL3dv. Introduced in GL core 4.1
     * \param index GLuint
     * \param v const GLdouble *
     * \return void
     */
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

template<class vec_4_f64>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Vector<vec_4_f64, f64, 4>)
    /*!
     * \brief Wraps around glVertexAttribL4d. Introduced in GL core 4.1
     * \param index GLuint
     * \param x GLdouble
     * \param y GLdouble
     * \param z GLdouble
     * \param w GLdouble
     * \return void
     */
    STATICINLINE void vertex_attrib_l4d(u32 index, vec_4_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribL4d)
    }
    glVertexAttribL4d(index, x.x(), x.y(), x.z(), x.w());
    detail::error_check("VertexAttribL4d"sv);
}

template<class span_const_vec_4_f64>
requires(
    MinimumVersion<Current, Version<4, 1>>&&
        semantic::concepts::Span<span_const_vec_4_f64>&& semantic::concepts::
            Vector<typename span_const_vec_4_f64::value_type, f64, 4>)
    /*!
     * \brief Wraps around glVertexAttribL4dv. Introduced in GL core 4.1
     * \param index GLuint
     * \param v const GLdouble *
     * \return void
     */
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

template<class span_const_void>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_void>)
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

template<class span_const_f64>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_f64>&& std::is_same_v<
                 std::decay_t<typename span_const_f64::value_type>,
                 std::decay_t<f64>>)
    /*!
     * \brief Wraps around glDepthRangeArrayv. Introduced in GL core 4.1
     * \param first GLuint
     * \param count GLsizei
     * \param v const GLdouble *
     * \return void
     */
    STATICINLINE
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

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 1>>)
    /*!
     * \brief Wraps around glDepthRangeIndexed. Introduced in GL core 4.1
     * \param index GLuint
     * \param n GLdouble
     * \param f GLdouble
     * \return void
     */
    STATICINLINE void depth_range_indexed(u32 index, f64 n, f64 f)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DepthRangeIndexed)
    }
    glDepthRangeIndexed(index, n, f);
    detail::error_check("DepthRangeIndexed"sv);
}

template<class span_f64>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_f64>&& std::is_same_v<
                 std::decay_t<typename span_f64::value_type>,
                 std::decay_t<f64>>)
    /*!
     * \brief Wraps around glGetDoublei_v. Introduced in GL core 4.1
     * \param target GLenum
     * \param index GLuint
     * \param data GLdouble *
     * \return void
     */
    STATICINLINE
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

template<class span_f32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_f32>&& std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>)
    /*!
     * \brief Wraps around glGetFloati_v. Introduced in GL core 4.1
     * \param target GLenum
     * \param index GLuint
     * \param data GLfloat *
     * \return void
     */
    STATICINLINE
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

template<class span_const_i32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Wraps around glScissorArrayv. Introduced in GL core 4.1
     * \param first GLuint
     * \param count GLsizei
     * \param v const GLint *
     * \return void
     */
    STATICINLINE
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

template<class size_2_i32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Size2D<size_2_i32, i32>)
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

template<class span_const_i32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Wraps around glScissorIndexedv. Introduced in GL core 4.1
     * \param index GLuint
     * \param v const GLint *
     * \return void
     */
    STATICINLINE void scissor_indexedv(u32 index, span_const_i32 const& v)
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

template<class span_const_f32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_f32>&& std::is_same_v<
                 std::decay_t<typename span_const_f32::value_type>,
                 std::decay_t<f32>>)
    /*!
     * \brief Wraps around glViewportArrayv. Introduced in GL core 4.1
     * \param first GLuint
     * \param count GLsizei
     * \param v const GLfloat *
     * \return void
     */
    STATICINLINE
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

template<class size_2_f32, class vec_2_f32>
requires(MinimumVersion<Current, Version<4, 1>>&&
                 semantic::concepts::Vector<vec_2_f32, f32, 2>&&
                 semantic::concepts::Size2D<size_2_f32, f32>)
    /*!
     * \brief Wraps around glViewportIndexedf. Introduced in GL core 4.1
     * \param index GLuint
     * \param x GLfloat
     * \param y GLfloat
     * \param w GLfloat
     * \param h GLfloat
     * \return void
     */
    STATICINLINE
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

template<class span_const_f32>
requires(MinimumVersion<Current, Version<4, 1>>&&
             semantic::concepts::Span<span_const_f32>&& std::is_same_v<
                 std::decay_t<typename span_const_f32::value_type>,
                 std::decay_t<f32>>)
    /*!
     * \brief Wraps around glViewportIndexedfv. Introduced in GL core 4.1
     * \param index GLuint
     * \param v const GLfloat *
     * \return void
     */
    STATICINLINE void viewport_indexedfv(u32 index, span_const_f32 const& v)
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
