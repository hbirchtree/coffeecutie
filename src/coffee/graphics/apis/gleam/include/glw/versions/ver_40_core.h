#ifdef GL_VERSION_4_0
/* Introduced in GL core 4.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>) STATICINLINE
    void blend_equation_separatei(
        u32                            buf,
        group::blend_equation_mode_ext modeRGB,
        group::blend_equation_mode_ext modeAlpha)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendEquationSeparatei)
    }
    glBlendEquationSeparatei(
        buf, static_cast<GLenum>(modeRGB), static_cast<GLenum>(modeAlpha));
    detail::error_check("BlendEquationSeparatei"sv);
}

/* Introduced in GL core 4.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>) STATICINLINE
    void blend_equationi(u32 buf, group::blend_equation_mode_ext mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendEquationi)
    }
    glBlendEquationi(buf, static_cast<GLenum>(mode));
    detail::error_check("BlendEquationi"sv);
}

/* Introduced in GL core 4.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>) STATICINLINE
    void blend_func_separatei(
        u32                    buf,
        group::blending_factor srcRGB,
        group::blending_factor dstRGB,
        group::blending_factor srcAlpha,
        group::blending_factor dstAlpha)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendFuncSeparatei)
    }
    glBlendFuncSeparatei(
        buf,
        static_cast<GLenum>(srcRGB),
        static_cast<GLenum>(dstRGB),
        static_cast<GLenum>(srcAlpha),
        static_cast<GLenum>(dstAlpha));
    detail::error_check("BlendFuncSeparatei"sv);
}

/* Introduced in GL core 4.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>) STATICINLINE void blend_funci(
    u32 buf, group::blending_factor src, group::blending_factor dst)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendFunci)
    }
    glBlendFunci(buf, static_cast<GLenum>(src), static_cast<GLenum>(dst));
    detail::error_check("BlendFunci"sv);
}

/* Introduced in GL core 4.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>) STATICINLINE
    void min_sample_shading(f32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MinSampleShading)
    }
    glMinSampleShading(value);
    detail::error_check("MinSampleShading"sv);
}

/* Introduced in GL core 4.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>) STATICINLINE
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

/* Introduced in GL core 4.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>) STATICINLINE
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

/* Introduced in GL core 4.0 */
template<class span_f64>
requires(MinimumVersion<Current, Version<4, 0>>&&
             semantic::concepts::Span<span_f64>&& std::is_same_v<
                 std::decay_t<typename span_f64::value_type>,
                 std::decay_t<f64>>) STATICINLINE
    void get_uniformdv(u32 program, i32 location, span_f64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformdv)
        glIsProgram(program);
    }
    glGetUniformdv(
        program,
        location,
        params.size() ? reinterpret_cast<GLdouble*>(params.data()) : nullptr);
    detail::error_check("GetUniformdv"sv);
}

/* Introduced in GL core 4.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>) STATICINLINE
    void uniform(i32 location, f64 x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1d)
    }
    glUniform1d(location, x);
    detail::error_check("Uniform1d"sv);
}

/* Introduced in GL core 4.0 */
template<class span_const_f64>
requires(MinimumVersion<Current, Version<4, 0>>&&
             semantic::concepts::Span<span_const_f64>&& std::is_same_v<
                 std::decay_t<typename span_const_f64::value_type>,
                 std::decay_t<f64>>) STATICINLINE
    void uniform(i32 location, i32 count, span_const_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1dv)
    }
    glUniform1dv(
        location, count, reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("Uniform1dv"sv);
}

/* Introduced in GL core 4.0 */
template<class vec_2_f64>
requires(MinimumVersion<Current, Version<4, 0>>&&
             semantic::concepts::Vector<vec_2_f64, f64, 2>) STATICINLINE
    void uniform(i32 location, vec_2_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2d)
    }
    glUniform2d(location, x.x(), x.y());
    detail::error_check("Uniform2d"sv);
}

/* Introduced in GL core 4.0 */
template<class span_const_vec_2_f64>
requires(
    MinimumVersion<Current, Version<4, 0>>&&
        semantic::concepts::Span<span_const_vec_2_f64>&& semantic::concepts::
            Vector<typename span_const_vec_2_f64::value_type, f64, 2>)
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_2_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2dv)
    }
    glUniform2dv(
        location, count, reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("Uniform2dv"sv);
}

/* Introduced in GL core 4.0 */
template<class vec_3_f64>
requires(MinimumVersion<Current, Version<4, 0>>&&
             semantic::concepts::Vector<vec_3_f64, f64, 3>) STATICINLINE
    void uniform(i32 location, vec_3_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3d)
    }
    glUniform3d(location, x.x(), x.y(), x.z());
    detail::error_check("Uniform3d"sv);
}

/* Introduced in GL core 4.0 */
template<class span_const_vec_3_f64>
requires(
    MinimumVersion<Current, Version<4, 0>>&&
        semantic::concepts::Span<span_const_vec_3_f64>&& semantic::concepts::
            Vector<typename span_const_vec_3_f64::value_type, f64, 3>)
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_3_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3dv)
    }
    glUniform3dv(
        location, count, reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("Uniform3dv"sv);
}

/* Introduced in GL core 4.0 */
template<class vec_4_f64>
requires(MinimumVersion<Current, Version<4, 0>>&&
             semantic::concepts::Vector<vec_4_f64, f64, 4>) STATICINLINE
    void uniform(i32 location, vec_4_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4d)
    }
    glUniform4d(location, x.x(), x.y(), x.z(), x.w());
    detail::error_check("Uniform4d"sv);
}

/* Introduced in GL core 4.0 */
template<class span_const_vec_4_f64>
requires(
    MinimumVersion<Current, Version<4, 0>>&&
        semantic::concepts::Span<span_const_vec_4_f64>&& semantic::concepts::
            Vector<typename span_const_vec_4_f64::value_type, f64, 4>)
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_4_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4dv)
    }
    glUniform4dv(
        location, count, reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("Uniform4dv"sv);
}

/* Introduced in GL core 4.0 */
template<class span_const_mat_2x2_f64>
requires(
    MinimumVersion<Current, Version<4, 0>>&&
        semantic::concepts::Span<span_const_mat_2x2_f64>&& semantic::concepts::
            Matrix<typename span_const_mat_2x2_f64::value_type, f64, 2, 2>)
    STATICINLINE void uniform(
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_2x2_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix2dv)
    }
    glUniformMatrix2dv(
        location,
        count,
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix2dv"sv);
}

/* Introduced in GL core 4.0 */
template<class span_const_mat_2x3_f64>
requires(
    MinimumVersion<Current, Version<4, 0>>&&
        semantic::concepts::Span<span_const_mat_2x3_f64>&& semantic::concepts::
            Matrix<typename span_const_mat_2x3_f64::value_type, f64, 2, 3>)
    STATICINLINE void uniform(
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_2x3_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix2x3dv)
    }
    glUniformMatrix2x3dv(
        location,
        count,
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix2x3dv"sv);
}

/* Introduced in GL core 4.0 */
template<class span_const_mat_2x4_f64>
requires(
    MinimumVersion<Current, Version<4, 0>>&&
        semantic::concepts::Span<span_const_mat_2x4_f64>&& semantic::concepts::
            Matrix<typename span_const_mat_2x4_f64::value_type, f64, 2, 4>)
    STATICINLINE void uniform(
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_2x4_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix2x4dv)
    }
    glUniformMatrix2x4dv(
        location,
        count,
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix2x4dv"sv);
}

/* Introduced in GL core 4.0 */
template<class span_const_mat_3x3_f64>
requires(
    MinimumVersion<Current, Version<4, 0>>&&
        semantic::concepts::Span<span_const_mat_3x3_f64>&& semantic::concepts::
            Matrix<typename span_const_mat_3x3_f64::value_type, f64, 3, 3>)
    STATICINLINE void uniform(
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_3x3_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix3dv)
    }
    glUniformMatrix3dv(
        location,
        count,
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix3dv"sv);
}

/* Introduced in GL core 4.0 */
template<class span_const_mat_3x2_f64>
requires(
    MinimumVersion<Current, Version<4, 0>>&&
        semantic::concepts::Span<span_const_mat_3x2_f64>&& semantic::concepts::
            Matrix<typename span_const_mat_3x2_f64::value_type, f64, 3, 2>)
    STATICINLINE void uniform(
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_3x2_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix3x2dv)
    }
    glUniformMatrix3x2dv(
        location,
        count,
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix3x2dv"sv);
}

/* Introduced in GL core 4.0 */
template<class span_const_mat_3x4_f64>
requires(
    MinimumVersion<Current, Version<4, 0>>&&
        semantic::concepts::Span<span_const_mat_3x4_f64>&& semantic::concepts::
            Matrix<typename span_const_mat_3x4_f64::value_type, f64, 3, 4>)
    STATICINLINE void uniform(
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_3x4_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix3x4dv)
    }
    glUniformMatrix3x4dv(
        location,
        count,
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix3x4dv"sv);
}

/* Introduced in GL core 4.0 */
template<class span_const_mat_4x4_f64>
requires(
    MinimumVersion<Current, Version<4, 0>>&&
        semantic::concepts::Span<span_const_mat_4x4_f64>&& semantic::concepts::
            Matrix<typename span_const_mat_4x4_f64::value_type, f64, 4, 4>)
    STATICINLINE void uniform(
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_4x4_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix4dv)
    }
    glUniformMatrix4dv(
        location,
        count,
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix4dv"sv);
}

/* Introduced in GL core 4.0 */
template<class span_const_mat_4x2_f64>
requires(
    MinimumVersion<Current, Version<4, 0>>&&
        semantic::concepts::Span<span_const_mat_4x2_f64>&& semantic::concepts::
            Matrix<typename span_const_mat_4x2_f64::value_type, f64, 4, 2>)
    STATICINLINE void uniform(
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_4x2_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix4x2dv)
    }
    glUniformMatrix4x2dv(
        location,
        count,
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix4x2dv"sv);
}

/* Introduced in GL core 4.0 */
template<class span_const_mat_4x3_f64>
requires(
    MinimumVersion<Current, Version<4, 0>>&&
        semantic::concepts::Span<span_const_mat_4x3_f64>&& semantic::concepts::
            Matrix<typename span_const_mat_4x3_f64::value_type, f64, 4, 3>)
    STATICINLINE void uniform(
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_4x3_f64 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix4x3dv)
    }
    glUniformMatrix4x3dv(
        location,
        count,
        transpose,
        reinterpret_cast<const GLdouble*>(value.data()));
    detail::error_check("UniformMatrix4x3dv"sv);
}

/* Introduced in GL core 4.0 */
template<class span_GLchar>
requires(MinimumVersion<Current, Version<4, 0>>&&
             semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>) STATICINLINE
    void get_active_subroutine_name(
        u32                program,
        group::shader_type shadertype,
        u32                index,
        i32&               length,
        span_GLchar        name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveSubroutineName)
        glIsProgram(program);
    }
    glGetActiveSubroutineName(
        program,
        static_cast<GLenum>(shadertype),
        index,
        name.size(),
        &length,
        name.data());
    detail::error_check("GetActiveSubroutineName"sv);
}

/* Introduced in GL core 4.0 */
template<class span_GLchar>
requires(MinimumVersion<Current, Version<4, 0>>&&
             semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>) STATICINLINE
    void get_active_subroutine_uniform_name(
        u32                program,
        group::shader_type shadertype,
        u32                index,
        i32&               length,
        span_GLchar        name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveSubroutineUniformName)
        glIsProgram(program);
    }
    glGetActiveSubroutineUniformName(
        program,
        static_cast<GLenum>(shadertype),
        index,
        name.size(),
        &length,
        name.data());
    detail::error_check("GetActiveSubroutineUniformName"sv);
}

/* Introduced in GL core 4.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<4, 0>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_active_subroutine_uniformiv(
        u32                              program,
        group::shader_type               shadertype,
        u32                              index,
        group::subroutine_parameter_name pname,
        span_i32                         values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveSubroutineUniformiv)
        glIsProgram(program);
    }
    glGetActiveSubroutineUniformiv(
        program,
        static_cast<GLenum>(shadertype),
        index,
        static_cast<GLenum>(pname),
        values.size() ? reinterpret_cast<GLint*>(values.data()) : nullptr);
    detail::error_check("GetActiveSubroutineUniformiv"sv);
}

/* Introduced in GL core 4.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>) STATICINLINE
    void get_program_stageiv(
        u32                       program,
        group::shader_type        shadertype,
        group::program_stage_prop pname,
        i32&                      values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramStageiv)
        glIsProgram(program);
    }
    glGetProgramStageiv(
        program,
        static_cast<GLenum>(shadertype),
        static_cast<GLenum>(pname),
        &values);
    detail::error_check("GetProgramStageiv"sv);
}

/* Introduced in GL core 4.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>) STATICINLINE GLuint
    get_subroutine_index(
        u32                     program,
        group::shader_type      shadertype,
        std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSubroutineIndex)
        glIsProgram(program);
    }
    auto out = glGetSubroutineIndex(
        program, static_cast<GLenum>(shadertype), name.data());
    detail::error_check("GetSubroutineIndex"sv);
    return out;
}

/* Introduced in GL core 4.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>) STATICINLINE GLint
    get_subroutine_uniform_location(
        u32                     program,
        group::shader_type      shadertype,
        std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSubroutineUniformLocation)
        glIsProgram(program);
    }
    auto out = glGetSubroutineUniformLocation(
        program, static_cast<GLenum>(shadertype), name.data());
    detail::error_check("GetSubroutineUniformLocation"sv);
    return out;
}

/* Introduced in GL core 4.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>) STATICINLINE
    void get_uniform_subroutineuiv(
        group::shader_type shadertype, i32 location, u32& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformSubroutineuiv)
    }
    glGetUniformSubroutineuiv(
        static_cast<GLenum>(shadertype), location, &params);
    detail::error_check("GetUniformSubroutineuiv"sv);
}

/* Introduced in GL core 4.0 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<4, 0>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void uniform_subroutines(
        group::shader_type shadertype, span_const_u32 const& indices)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformSubroutinesuiv)
    }
    glUniformSubroutinesuiv(
        static_cast<GLenum>(shadertype),
        indices.size(),
        indices.size() ? reinterpret_cast<const GLuint*>(indices.data())
                       : nullptr);
    detail::error_check("UniformSubroutinesuiv"sv);
}

/* Introduced in GL core 4.0 */
template<class span_const_f32>
requires(MinimumVersion<Current, Version<4, 0>>&&
             semantic::concepts::Span<span_const_f32>&& std::is_same_v<
                 std::decay_t<typename span_const_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void patch_parameter(
        group::patch_parameter_name pname, span_const_f32 const& values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PatchParameterfv)
    }
    glPatchParameterfv(
        static_cast<GLenum>(pname),
        values.size() ? reinterpret_cast<const GLfloat*>(values.data())
                      : nullptr);
    detail::error_check("PatchParameterfv"sv);
}

/* Introduced in GL core 4.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>) STATICINLINE
    void patch_parameter(group::patch_parameter_name pname, i32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PatchParameteri)
    }
    glPatchParameteri(static_cast<GLenum>(pname), value);
    detail::error_check("PatchParameteri"sv);
}

/* Introduced in GL core 4.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>) STATICINLINE
    void bind_transform_feedback(
        group::bind_transform_feedback_target target, u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindTransformFeedback)
    }
    glBindTransformFeedback(static_cast<GLenum>(target), id);
    detail::error_check("BindTransformFeedback"sv);
}

/* Introduced in GL core 4.0 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<4, 0>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void delete_transform_feedbacks(span_const_u32 const& ids)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteTransformFeedbacks)
    }
    glDeleteTransformFeedbacks(
        ids.size(),
        ids.size() ? reinterpret_cast<const GLuint*>(ids.data()) : nullptr);
    detail::error_check("DeleteTransformFeedbacks"sv);
}

/* Introduced in GL core 4.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>) STATICINLINE
    void draw_transform_feedback(group::primitive_type mode, u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawTransformFeedback)
    }
    glDrawTransformFeedback(static_cast<GLenum>(mode), id);
    detail::error_check("DrawTransformFeedback"sv);
}

/* Introduced in GL core 4.0 */
template<class span_u32>
requires(MinimumVersion<Current, Version<4, 0>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void gen_transform_feedbacks(span_u32 ids)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenTransformFeedbacks)
    }
    glGenTransformFeedbacks(
        ids.size(),
        ids.size() ? reinterpret_cast<GLuint*>(ids.data()) : nullptr);
    detail::error_check("GenTransformFeedbacks"sv);
}

/* Introduced in GL core 4.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>) STATICINLINE GLboolean
    is_transform_feedback(u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsTransformFeedback)
    }
    auto out = glIsTransformFeedback(id);
    detail::error_check("IsTransformFeedback"sv);
    return out;
}

/* Introduced in GL core 4.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>) STATICINLINE
    void pause_transform_feedback()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PauseTransformFeedback)
    }
    glPauseTransformFeedback();
    detail::error_check("PauseTransformFeedback"sv);
}

/* Introduced in GL core 4.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>) STATICINLINE
    void resume_transform_feedback()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ResumeTransformFeedback)
    }
    glResumeTransformFeedback();
    detail::error_check("ResumeTransformFeedback"sv);
}

/* Introduced in GL core 4.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>) STATICINLINE
    void begin_query_indexed(group::query_target target, u32 index, u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BeginQueryIndexed)
    }
    glBeginQueryIndexed(static_cast<GLenum>(target), index, id);
    detail::error_check("BeginQueryIndexed"sv);
}

/* Introduced in GL core 4.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>) STATICINLINE
    void draw_transform_feedback_stream(
        group::primitive_type mode, u32 id, u32 stream)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawTransformFeedbackStream)
    }
    glDrawTransformFeedbackStream(static_cast<GLenum>(mode), id, stream);
    detail::error_check("DrawTransformFeedbackStream"sv);
}

/* Introduced in GL core 4.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<4, 0>>) STATICINLINE
    void end_query_indexed(group::query_target target, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EndQueryIndexed)
    }
    glEndQueryIndexed(static_cast<GLenum>(target), index);
    detail::error_check("EndQueryIndexed"sv);
}

/* Introduced in GL core 4.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<4, 0>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_query_indexediv(
        group::query_target         target,
        u32                         index,
        group::query_parameter_name pname,
        span_i32                    params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryIndexediv)
    }
    glGetQueryIndexediv(
        static_cast<GLenum>(target),
        index,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetQueryIndexediv"sv);
}

#endif // GL_VERSION_4_0
