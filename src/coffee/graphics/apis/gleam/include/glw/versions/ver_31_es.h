#ifdef GL_ES_VERSION_3_1
/* Introduced in GL es 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE
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

/* Introduced in GL es 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE
    void dispatch_compute_indirect(GLintptr indirect)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DispatchComputeIndirect)
    }
    glDispatchComputeIndirect(indirect);
    detail::error_check("DispatchComputeIndirect"sv);
}

/* Introduced in GL es 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE
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

/* Introduced in GL es 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE
    void draw_elements_indirect(
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

/* Introduced in GL es 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE
    void framebuffer_parameter(
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

/* Introduced in GL es 3.1 */
template<class span_i32>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_framebuffer_parameter(
        group::framebuffer_target                    target,
        group::framebuffer_attachment_parameter_name pname,
        span_i32                                     params)
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

/* Introduced in GL es 3.1 */
template<class span_i32>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_program_interfaceiv(
        u32                           program,
        group::program_interface      programInterface,
        group::program_interface_prop pname,
        span_i32                      params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramInterfaceiv)
        glIsProgram(program);
    }
    glGetProgramInterfaceiv(
        program,
        static_cast<GLenum>(programInterface),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetProgramInterfaceiv"sv);
}

/* Introduced in GL es 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE GLuint
    get_program_resource_index(
        u32                      program,
        group::program_interface programInterface,
        std::string_view const&  name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramResourceIndex)
        glIsProgram(program);
    }
    auto out = glGetProgramResourceIndex(
        program, static_cast<GLenum>(programInterface), name.data());
    detail::error_check("GetProgramResourceIndex"sv);
    return out;
}

/* Introduced in GL es 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE GLint
    get_program_resource_location(
        u32                      program,
        group::program_interface programInterface,
        std::string_view const&  name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramResourceLocation)
        glIsProgram(program);
    }
    auto out = glGetProgramResourceLocation(
        program, static_cast<GLenum>(programInterface), name.data());
    detail::error_check("GetProgramResourceLocation"sv);
    return out;
}

/* Introduced in GL es 3.1 */
template<class span_GLchar>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>) STATICINLINE
    void get_program_resource_name(
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
        glIsProgram(program);
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

/* Introduced in GL es 3.1 */
template<class span_const_program_resource_property, class span_i32>
requires(MinimumVersion<Current, Version<3, 1>>&&         semantic::concepts::
             Span<span_const_program_resource_property>&& std::is_same_v<
                 std::decay_t<
                     typename span_const_program_resource_property::value_type>,
                 std::decay_t<group::program_resource_property>>&&
                 semantic::concepts::Span<span_i32>&& std::is_same_v<
                     std::decay_t<typename span_i32::value_type>,
                     std::decay_t<i32>>) STATICINLINE
    void get_program_resourceiv(
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
        glIsProgram(program);
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

/* Introduced in GL es 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE
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

/* Introduced in GL es 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE
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

/* Introduced in GL es 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE GLuint
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

/* Introduced in GL es 3.1 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class span_u32>
requires(MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class span_GLchar>
requires(MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class span_i32>
requires(MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE GLboolean
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

/* Introduced in GL es 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE
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

/* Introduced in GL es 3.1 */
template<class span_const_f32>
requires(MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE
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

/* Introduced in GL es 3.1 */
template<class span_const_i32>
requires(MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE
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

/* Introduced in GL es 3.1 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class vec_2_f32>
requires(MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class span_const_vec_2_f32>
requires(
    MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class vec_2_i32>
requires(MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class span_const_vec_2_i32>
requires(
    MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class vec_2_u32>
requires(MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class span_const_vec_2_u32>
requires(
    MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class vec_3_f32>
requires(MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class span_const_vec_3_f32>
requires(
    MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class vec_3_i32>
requires(MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class span_const_vec_3_i32>
requires(
    MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class vec_3_u32>
requires(MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class span_const_vec_3_u32>
requires(
    MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class vec_4_f32>
requires(MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class span_const_vec_4_f32>
requires(
    MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class vec_4_i32>
requires(MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class span_const_vec_4_i32>
requires(
    MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class vec_4_u32>
requires(MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class span_const_vec_4_u32>
requires(
    MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class span_const_mat_2x2_f32>
requires(
    MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class span_const_mat_2x3_f32>
requires(
    MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class span_const_mat_2x4_f32>
requires(
    MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class span_const_mat_3x3_f32>
requires(
    MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class span_const_mat_3x2_f32>
requires(
    MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class span_const_mat_3x4_f32>
requires(
    MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class span_const_mat_4x4_f32>
requires(
    MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class span_const_mat_4x2_f32>
requires(
    MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<class span_const_mat_4x3_f32>
requires(
    MinimumVersion<Current, Version<3, 1>>&&
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

/* Introduced in GL es 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE
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

/* Introduced in GL es 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE
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

/* Introduced in GL es 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE
    void bind_image_texture(
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
        glIsTexture(texture);
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

/* Introduced in GL es 3.1 */
template<class span_bool>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Span<span_bool>&& std::is_same_v<
                 std::decay_t<typename span_bool::value_type>,
                 std::decay_t<bool>>) STATICINLINE
    void get_booleani_v(
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

/* Introduced in GL es 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE
    void memory_barrier(group::memory_barrier_mask barriers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MemoryBarrier)
    }
    glMemoryBarrier(static_cast<GLenum>(barriers));
    detail::error_check("MemoryBarrier"sv);
}

/* Introduced in GL es 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE
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

/* Introduced in GL es 3.1 */
template<class span_f32>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Span<span_f32>&& std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void get_multisamplefv(
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

/* Introduced in GL es 3.1 */
template<class span_f32>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Span<span_f32>&& std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void get_tex_level_parameter(
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

/* Introduced in GL es 3.1 */
template<class span_i32>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_tex_level_parameter(
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

/* Introduced in GL es 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE
    void sample_maski(u32 maskNumber, GLbitfield mask)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SampleMaski)
    }
    glSampleMaski(maskNumber, mask);
    detail::error_check("SampleMaski"sv);
}

/* Introduced in GL es 3.1 */
template<class size_2_i32>
requires(MinimumVersion<Current, Version<3, 1>>&&
             semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void tex_storage_2d_multisample(
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

/* Introduced in GL es 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE
    void bind_vertex_buffer(
        u32 bindingindex, u32 buffer, GLintptr offset, i32 stride)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindVertexBuffer)
        glIsBuffer(buffer);
    }
    glBindVertexBuffer(bindingindex, buffer, offset, stride);
    detail::error_check("BindVertexBuffer"sv);
}

/* Introduced in GL es 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE
    void vertex_attrib_binding(u32 attribindex, u32 bindingindex)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribBinding)
    }
    glVertexAttribBinding(attribindex, bindingindex);
    detail::error_check("VertexAttribBinding"sv);
}

/* Introduced in GL es 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE
    void vertex_attrib_format(
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

/* Introduced in GL es 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE
    void vertex_attrib_i_format(
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

/* Introduced in GL es 3.1 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 1>>) STATICINLINE
    void vertex_binding_divisor(u32 bindingindex, u32 divisor)
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
