#ifdef GL_ES_VERSION_3_1
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glDispatchCompute. Introduced in GL es 3.1
     * \param num_groups_x GLuint
     * \param num_groups_y GLuint
     * \param num_groups_z GLuint
     * \return void
     */
    STATICINLINE
    void dispatch_compute(u32 num_groups_x, u32 num_groups_y, u32 num_groups_z)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DispatchCompute)
    }
    glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);
    detail::error_check("DispatchCompute"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glDispatchComputeIndirect. Introduced in GL es 3.1
     * \param indirect GLintptr
     * \return void
     */
    STATICINLINE void dispatch_compute_indirect(GLintptr indirect)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DispatchComputeIndirect)
    }
    glDispatchComputeIndirect(indirect);
    detail::error_check("DispatchComputeIndirect"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glDrawArraysIndirect. Introduced in GL es 3.1
     * \param mode GLenum
     * \param indirect const void *
     * \return void
     */
    STATICINLINE
    void draw_arrays_indirect(group::primitive_type mode, ptroff indirect)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawArraysIndirect)
    }
    glDrawArraysIndirect(
        static_cast<GLenum>(mode), reinterpret_cast<const void*>(indirect));
    detail::error_check("DrawArraysIndirect"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glDrawElementsIndirect. Introduced in GL es 3.1
     * \param mode GLenum
     * \param type GLenum
     * \param indirect const void *
     * \return void
     */
    STATICINLINE void draw_elements_indirect(
        group::primitive_type     mode,
        group::draw_elements_type type,
        ptroff                    indirect)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawElementsIndirect)
    }
    glDrawElementsIndirect(
        static_cast<GLenum>(mode),
        static_cast<GLenum>(type),
        reinterpret_cast<const void*>(indirect));
    detail::error_check("DrawElementsIndirect"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glFramebufferParameteri. Introduced in GL es 3.1
     * \param target GLenum
     * \param pname GLenum
     * \param param GLint
     * \return void
     */
    STATICINLINE void framebuffer_parameter(
        group::framebuffer_target         target,
        group::framebuffer_parameter_name pname,
        i32                               param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferParameteri)
    }
    glFramebufferParameteri(
        static_cast<GLenum>(target), static_cast<GLenum>(pname), param);
    detail::error_check("FramebufferParameteri"sv);
}

template<class span_i32>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Wraps around glGetFramebufferParameteriv. Introduced in GL es 3.1
     * \param target GLenum
     * \param pname GLenum
     * \param params GLint *
     * \return void
     */
    STATICINLINE void get_framebuffer_parameter(
        group::framebuffer_target        target,
        group::get_framebuffer_parameter pname,
        span_i32                         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFramebufferParameteriv)
    }
    glGetFramebufferParameteriv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetFramebufferParameteriv"sv);
}

template<class span_i32>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Wraps around glGetProgramInterfaceiv. Introduced in GL es 3.1
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
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

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glGetProgramResourceIndex. Introduced in GL es 3.1
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    auto out = glGetProgramResourceIndex(
        program, static_cast<GLenum>(programInterface), name.data());
    detail::error_check("GetProgramResourceIndex"sv);
    return out;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glGetProgramResourceLocation. Introduced in GL es 3.1
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    auto out = glGetProgramResourceLocation(
        program, static_cast<GLenum>(programInterface), name.data());
    detail::error_check("GetProgramResourceLocation"sv);
    return out;
}

template<class span_GLchar>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>)
    /*!
     * \brief Wraps around glGetProgramResourceName. Introduced in GL es 3.1
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
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
requires(MinimumVersion<Current, Version<3, 1>>&&         semantic::concepts::
             Span<span_const_program_resource_property>&& std::is_same_v<
                 std::decay_t<
                     typename span_const_program_resource_property::value_type>,
                 std::decay_t<group::program_resource_property>>&&
                 semantic::concepts::Span<span_i32>&& std::is_same_v<
                     std::decay_t<typename span_i32::value_type>,
                     std::decay_t<i32>>)
    /*!
     * \brief Wraps around glGetProgramResourceiv. Introduced in GL es 3.1
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
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
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

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glActiveShaderProgram. Introduced in GL es 3.1
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
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glBindProgramPipeline. Introduced in GL es 3.1
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
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glCreateShaderProgramv. Introduced in GL es 3.1
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
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>)
    /*!
     * \brief Wraps around glDeleteProgramPipelines. Introduced in GL es 3.1
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
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>)
    /*!
     * \brief Wraps around glGenProgramPipelines. Introduced in GL es 3.1
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
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>)
    /*!
     * \brief Wraps around glGetProgramPipelineInfoLog. Introduced in GL es 3.1
     * \param pipeline GLuint
     * \param bufSize GLsizei
     * \param length GLsizei *
     * \param infoLog GLchar *
     * \return void
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
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Wraps around glGetProgramPipelineiv. Introduced in GL es 3.1
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
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glIsProgramPipeline. Introduced in GL es 3.1
     * \param pipeline GLuint
     * \return Boolean
     */
    STATICINLINE bool is_program_pipeline(u32 pipeline)
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
    return out == GL_TRUE ? true : false;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glProgramUniform1f. Introduced in GL es 3.1
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
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Span<span_const_f32>&& std::is_same_v<
                 std::decay_t<typename span_const_f32::value_type>,
                 std::decay_t<f32>>)
    /*!
     * \brief Wraps around glProgramUniform1fv. Introduced in GL es 3.1
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
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glProgramUniform1i. Introduced in GL es 3.1
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
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Wraps around glProgramUniform1iv. Introduced in GL es 3.1
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
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glProgramUniform1ui. Introduced in GL es 3.1
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
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>)
    /*!
     * \brief Wraps around glProgramUniform1uiv. Introduced in GL es 3.1
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

template<class vec_2_f32>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Vector<vec_2_f32, f32, 2>)
    /*!
     * \brief Wraps around glProgramUniform2f. Introduced in GL es 3.1
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
    MinimumVersion<Current, Version<3, 1>>&&
        semantic::concepts::Span<span_const_vec_2_f32>&& semantic::concepts::
            Vector<typename span_const_vec_2_f32::value_type, f32, 2>)
    /*!
     * \brief Wraps around glProgramUniform2fv. Introduced in GL es 3.1
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLfloat *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, span_const_vec_2_f32 const& value)
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
        value.size(),
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniform2fv"sv);
}

template<class vec_2_i32>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Vector<vec_2_i32, i32, 2>)
    /*!
     * \brief Wraps around glProgramUniform2i. Introduced in GL es 3.1
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
    MinimumVersion<Current, Version<3, 1>>&&
        semantic::concepts::Span<span_const_vec_2_i32>&& semantic::concepts::
            Vector<typename span_const_vec_2_i32::value_type, i32, 2>)
    /*!
     * \brief Wraps around glProgramUniform2iv. Introduced in GL es 3.1
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLint *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, span_const_vec_2_i32 const& value)
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
        program,
        location,
        value.size(),
        reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("ProgramUniform2iv"sv);
}

template<class vec_2_u32>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Vector<vec_2_u32, u32, 2>)
    /*!
     * \brief Wraps around glProgramUniform2ui. Introduced in GL es 3.1
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
    MinimumVersion<Current, Version<3, 1>>&&
        semantic::concepts::Span<span_const_vec_2_u32>&& semantic::concepts::
            Vector<typename span_const_vec_2_u32::value_type, u32, 2>)
    /*!
     * \brief Wraps around glProgramUniform2uiv. Introduced in GL es 3.1
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLuint *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, span_const_vec_2_u32 const& value)
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
        value.size(),
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("ProgramUniform2uiv"sv);
}

template<class vec_3_f32>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Vector<vec_3_f32, f32, 3>)
    /*!
     * \brief Wraps around glProgramUniform3f. Introduced in GL es 3.1
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
    MinimumVersion<Current, Version<3, 1>>&&
        semantic::concepts::Span<span_const_vec_3_f32>&& semantic::concepts::
            Vector<typename span_const_vec_3_f32::value_type, f32, 3>)
    /*!
     * \brief Wraps around glProgramUniform3fv. Introduced in GL es 3.1
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLfloat *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, span_const_vec_3_f32 const& value)
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
        value.size(),
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniform3fv"sv);
}

template<class vec_3_i32>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Vector<vec_3_i32, i32, 3>)
    /*!
     * \brief Wraps around glProgramUniform3i. Introduced in GL es 3.1
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
    MinimumVersion<Current, Version<3, 1>>&&
        semantic::concepts::Span<span_const_vec_3_i32>&& semantic::concepts::
            Vector<typename span_const_vec_3_i32::value_type, i32, 3>)
    /*!
     * \brief Wraps around glProgramUniform3iv. Introduced in GL es 3.1
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLint *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, span_const_vec_3_i32 const& value)
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
        program,
        location,
        value.size(),
        reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("ProgramUniform3iv"sv);
}

template<class vec_3_u32>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Vector<vec_3_u32, u32, 3>)
    /*!
     * \brief Wraps around glProgramUniform3ui. Introduced in GL es 3.1
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
    MinimumVersion<Current, Version<3, 1>>&&
        semantic::concepts::Span<span_const_vec_3_u32>&& semantic::concepts::
            Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
    /*!
     * \brief Wraps around glProgramUniform3uiv. Introduced in GL es 3.1
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLuint *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, span_const_vec_3_u32 const& value)
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
        value.size(),
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("ProgramUniform3uiv"sv);
}

template<class vec_4_f32>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Vector<vec_4_f32, f32, 4>)
    /*!
     * \brief Wraps around glProgramUniform4f. Introduced in GL es 3.1
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
    MinimumVersion<Current, Version<3, 1>>&&
        semantic::concepts::Span<span_const_vec_4_f32>&& semantic::concepts::
            Vector<typename span_const_vec_4_f32::value_type, f32, 4>)
    /*!
     * \brief Wraps around glProgramUniform4fv. Introduced in GL es 3.1
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLfloat *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, span_const_vec_4_f32 const& value)
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
        value.size(),
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniform4fv"sv);
}

template<class vec_4_i32>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Vector<vec_4_i32, i32, 4>)
    /*!
     * \brief Wraps around glProgramUniform4i. Introduced in GL es 3.1
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
    MinimumVersion<Current, Version<3, 1>>&&
        semantic::concepts::Span<span_const_vec_4_i32>&& semantic::concepts::
            Vector<typename span_const_vec_4_i32::value_type, i32, 4>)
    /*!
     * \brief Wraps around glProgramUniform4iv. Introduced in GL es 3.1
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLint *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, span_const_vec_4_i32 const& value)
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
        program,
        location,
        value.size(),
        reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("ProgramUniform4iv"sv);
}

template<class vec_4_u32>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Vector<vec_4_u32, u32, 4>)
    /*!
     * \brief Wraps around glProgramUniform4ui. Introduced in GL es 3.1
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
    MinimumVersion<Current, Version<3, 1>>&&
        semantic::concepts::Span<span_const_vec_4_u32>&& semantic::concepts::
            Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
    /*!
     * \brief Wraps around glProgramUniform4uiv. Introduced in GL es 3.1
     * \param program GLuint
     * \param location GLint
     * \param count GLsizei
     * \param value const GLuint *
     * \return void
     */
    STATICINLINE void program_uniform(
        u32 program, i32 location, span_const_vec_4_u32 const& value)
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
        value.size(),
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("ProgramUniform4uiv"sv);
}

template<class span_const_mat_2x2_f32>
requires(
    MinimumVersion<Current, Version<3, 1>>&&
        semantic::concepts::Span<span_const_mat_2x2_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_2x2_f32::value_type, f32, 2, 2>)
    /*!
     * \brief Wraps around glProgramUniformMatrix2fv. Introduced in GL es 3.1
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
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix2fv"sv);
}

template<class span_const_mat_2x3_f32>
requires(
    MinimumVersion<Current, Version<3, 1>>&&
        semantic::concepts::Span<span_const_mat_2x3_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_2x3_f32::value_type, f32, 2, 3>)
    /*!
     * \brief Wraps around glProgramUniformMatrix2x3fv. Introduced in GL es 3.1
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
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix2x3fv"sv);
}

template<class span_const_mat_2x4_f32>
requires(
    MinimumVersion<Current, Version<3, 1>>&&
        semantic::concepts::Span<span_const_mat_2x4_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_2x4_f32::value_type, f32, 2, 4>)
    /*!
     * \brief Wraps around glProgramUniformMatrix2x4fv. Introduced in GL es 3.1
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
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix2x4fv"sv);
}

template<class span_const_mat_3x3_f32>
requires(
    MinimumVersion<Current, Version<3, 1>>&&
        semantic::concepts::Span<span_const_mat_3x3_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_3x3_f32::value_type, f32, 3, 3>)
    /*!
     * \brief Wraps around glProgramUniformMatrix3fv. Introduced in GL es 3.1
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
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix3fv"sv);
}

template<class span_const_mat_3x2_f32>
requires(
    MinimumVersion<Current, Version<3, 1>>&&
        semantic::concepts::Span<span_const_mat_3x2_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_3x2_f32::value_type, f32, 3, 2>)
    /*!
     * \brief Wraps around glProgramUniformMatrix3x2fv. Introduced in GL es 3.1
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
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix3x2fv"sv);
}

template<class span_const_mat_3x4_f32>
requires(
    MinimumVersion<Current, Version<3, 1>>&&
        semantic::concepts::Span<span_const_mat_3x4_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_3x4_f32::value_type, f32, 3, 4>)
    /*!
     * \brief Wraps around glProgramUniformMatrix3x4fv. Introduced in GL es 3.1
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
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix3x4fv"sv);
}

template<class span_const_mat_4x4_f32>
requires(
    MinimumVersion<Current, Version<3, 1>>&&
        semantic::concepts::Span<span_const_mat_4x4_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_4x4_f32::value_type, f32, 4, 4>)
    /*!
     * \brief Wraps around glProgramUniformMatrix4fv. Introduced in GL es 3.1
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
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix4fv"sv);
}

template<class span_const_mat_4x2_f32>
requires(
    MinimumVersion<Current, Version<3, 1>>&&
        semantic::concepts::Span<span_const_mat_4x2_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_4x2_f32::value_type, f32, 4, 2>)
    /*!
     * \brief Wraps around glProgramUniformMatrix4x2fv. Introduced in GL es 3.1
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
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix4x2fv"sv);
}

template<class span_const_mat_4x3_f32>
requires(
    MinimumVersion<Current, Version<3, 1>>&&
        semantic::concepts::Span<span_const_mat_4x3_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_4x3_f32::value_type, f32, 4, 3>)
    /*!
     * \brief Wraps around glProgramUniformMatrix4x3fv. Introduced in GL es 3.1
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
        value.size(),
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("ProgramUniformMatrix4x3fv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glUseProgramStages. Introduced in GL es 3.1
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
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glValidateProgramPipeline. Introduced in GL es 3.1
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

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glBindImageTexture. Introduced in GL es 3.1
     * \param unit GLuint
     * \param texture GLuint
     * \param level GLint
     * \param layered GLboolean
     * \param layer GLint
     * \param access GLenum
     * \param format GLenum
     * \return void
     */
    STATICINLINE void bind_image_texture(
        u32                      unit,
        u32                      texture,
        i32                      level,
        bool                     layered,
        i32                      layer,
        group::buffer_access_arb access,
        group::internal_format   format)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindImageTexture)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glBindImageTexture(
        unit,
        texture,
        level,
        layered,
        layer,
        static_cast<GLenum>(access),
        static_cast<GLenum>(format));
    detail::error_check("BindImageTexture"sv);
}

template<class span_bool>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Span<span_bool>&& std::is_same_v<
                 std::decay_t<typename span_bool::value_type>,
                 std::decay_t<bool>>)
    /*!
     * \brief Wraps around glGetBooleani_v. Introduced in GL es 3.1
     * \param target GLenum
     * \param index GLuint
     * \param data GLboolean *
     * \return void
     */
    STATICINLINE void get_booleani_v(
        group::buffer_target_arb target, u32 index, span_bool data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetBooleani_v)
    }
    glGetBooleani_v(
        static_cast<GLenum>(target),
        index,
        data.size() ? reinterpret_cast<GLboolean*>(data.data()) : nullptr);
    detail::error_check("GetBooleani_v"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glMemoryBarrier. Introduced in GL es 3.1
     * \param barriers GLbitfield
     * \return void
     */
    STATICINLINE void memory_barrier(group::memory_barrier_mask barriers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MemoryBarrier)
    }
    glMemoryBarrier(static_cast<GLenum>(barriers));
    detail::error_check("MemoryBarrier"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glMemoryBarrierByRegion. Introduced in GL es 3.1
     * \param barriers GLbitfield
     * \return void
     */
    STATICINLINE
    void memory_barrier_by_region(group::memory_barrier_mask barriers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MemoryBarrierByRegion)
    }
    glMemoryBarrierByRegion(static_cast<GLenum>(barriers));
    detail::error_check("MemoryBarrierByRegion"sv);
}

template<class span_f32>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Span<span_f32>&& std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>)
    /*!
     * \brief Wraps around glGetMultisamplefv. Introduced in GL es 3.1
     * \param pname GLenum
     * \param index GLuint
     * \param val GLfloat *
     * \return void
     */
    STATICINLINE void get_multisamplefv(
        group::get_multisample_prop_nv pname, u32 index, span_f32 val)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetMultisamplefv)
    }
    glGetMultisamplefv(
        static_cast<GLenum>(pname),
        index,
        val.size() ? reinterpret_cast<GLfloat*>(val.data()) : nullptr);
    detail::error_check("GetMultisamplefv"sv);
}

template<class span_f32>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Span<span_f32>&& std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>)
    /*!
     * \brief Wraps around glGetTexLevelParameterfv. Introduced in GL es 3.1
     * \param target GLenum
     * \param level GLint
     * \param pname GLenum
     * \param params GLfloat *
     * \return void
     */
    STATICINLINE void get_tex_level_parameter(
        group::texture_target        target,
        i32                          level,
        group::get_texture_parameter pname,
        span_f32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTexLevelParameterfv)
    }
    glGetTexLevelParameterfv(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetTexLevelParameterfv"sv);
}

template<class span_i32>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>)
    /*!
     * \brief Wraps around glGetTexLevelParameteriv. Introduced in GL es 3.1
     * \param target GLenum
     * \param level GLint
     * \param pname GLenum
     * \param params GLint *
     * \return void
     */
    STATICINLINE void get_tex_level_parameter(
        group::texture_target        target,
        i32                          level,
        group::get_texture_parameter pname,
        span_i32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTexLevelParameteriv)
    }
    glGetTexLevelParameteriv(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetTexLevelParameteriv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glSampleMaski. Introduced in GL es 3.1
     * \param maskNumber GLuint
     * \param mask GLbitfield
     * \return void
     */
    STATICINLINE void sample_maski(u32 maskNumber, GLbitfield mask)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SampleMaski)
    }
    glSampleMaski(maskNumber, mask);
    detail::error_check("SampleMaski"sv);
}

template<class size_2_i32>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Size2D<size_2_i32, i32>)
    /*!
     * \brief Wraps around glTexStorage2DMultisample. Introduced in GL es 3.1
     * \param target GLenum
     * \param samples GLsizei
     * \param internalformat GLenum
     * \param width GLsizei
     * \param height GLsizei
     * \param fixedsamplelocations GLboolean
     * \return void
     */
    STATICINLINE void tex_storage_2d_multisample(
        group::texture_target        target,
        i32                          samples,
        group::sized_internal_format internalformat,
        size_2_i32 const&            width,
        bool                         fixedsamplelocations)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexStorage2DMultisample)
    }
    glTexStorage2DMultisample(
        static_cast<GLenum>(target),
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        fixedsamplelocations);
    detail::error_check("TexStorage2DMultisample"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glBindVertexBuffer. Introduced in GL es 3.1
     * \param bindingindex GLuint
     * \param buffer GLuint
     * \param offset GLintptr
     * \param stride GLsizei
     * \return void
     */
    STATICINLINE void bind_vertex_buffer(
        u32 bindingindex, u32 buffer, GLintptr offset, i32 stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindVertexBuffer)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glBindVertexBuffer(bindingindex, buffer, offset, stride);
    detail::error_check("BindVertexBuffer"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glVertexAttribBinding. Introduced in GL es 3.1
     * \param attribindex GLuint
     * \param bindingindex GLuint
     * \return void
     */
    STATICINLINE void vertex_attrib_binding(u32 attribindex, u32 bindingindex)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribBinding)
    }
    glVertexAttribBinding(attribindex, bindingindex);
    detail::error_check("VertexAttribBinding"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glVertexAttribFormat. Introduced in GL es 3.1
     * \param attribindex GLuint
     * \param size GLint
     * \param type GLenum
     * \param normalized GLboolean
     * \param relativeoffset GLuint
     * \return void
     */
    STATICINLINE void vertex_attrib_format(
        u32                       attribindex,
        i32                       size,
        group::vertex_attrib_type type,
        bool                      normalized,
        u32                       relativeoffset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribFormat)
    }
    glVertexAttribFormat(
        attribindex,
        size,
        static_cast<GLenum>(type),
        normalized,
        relativeoffset);
    detail::error_check("VertexAttribFormat"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glVertexAttribIFormat. Introduced in GL es 3.1
     * \param attribindex GLuint
     * \param size GLint
     * \param type GLenum
     * \param relativeoffset GLuint
     * \return void
     */
    STATICINLINE void vertex_attrib_i_format(
        u32                      attribindex,
        i32                      size,
        group::vertex_attrib_int type,
        u32                      relativeoffset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribIFormat)
    }
    glVertexAttribIFormat(
        attribindex, size, static_cast<GLenum>(type), relativeoffset);
    detail::error_check("VertexAttribIFormat"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>)
    /*!
     * \brief Wraps around glVertexBindingDivisor. Introduced in GL es 3.1
     * \param bindingindex GLuint
     * \param divisor GLuint
     * \return void
     */
    STATICINLINE void vertex_binding_divisor(u32 bindingindex, u32 divisor)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexBindingDivisor)
    }
    glVertexBindingDivisor(bindingindex, divisor);
    detail::error_check("VertexBindingDivisor"sv);
}

#endif // GL_ES_VERSION_3_1
