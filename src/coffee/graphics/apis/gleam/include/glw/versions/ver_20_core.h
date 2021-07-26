#ifdef GL_VERSION_2_0
/* Introduced in GL core 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void attach_shader(u32 program, u32 shader)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(AttachShader)
        glIsProgram(program);
        glIsShader(shader);
    }
    glAttachShader(program, shader);
    detail::error_check("AttachShader"sv);
}

/* Introduced in GL core 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void bind_attrib_location(
        u32 program, u32 index, std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindAttribLocation)
        glIsProgram(program);
    }
    glBindAttribLocation(program, index, name.data());
    detail::error_check("BindAttribLocation"sv);
}

/* Introduced in GL core 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void blend_equation_separate(
        group::blend_equation_mode_ext modeRGB,
        group::blend_equation_mode_ext modeAlpha)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlendEquationSeparate)
    }
    glBlendEquationSeparate(
        static_cast<GLenum>(modeRGB), static_cast<GLenum>(modeAlpha));
    detail::error_check("BlendEquationSeparate"sv);
}

/* Introduced in GL core 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void compile_shader(u32 shader)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompileShader)
        glIsShader(shader);
    }
    glCompileShader(shader);
    detail::error_check("CompileShader"sv);
}

/* Introduced in GL core 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE GLuint
    create_program()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateProgram)
    }
    auto out = glCreateProgram();
    detail::error_check("CreateProgram"sv);
    return out;
}

/* Introduced in GL core 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE GLuint
    create_shader(group::shader_type type)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateShader)
    }
    auto out = glCreateShader(static_cast<GLenum>(type));
    detail::error_check("CreateShader"sv);
    return out;
}

/* Introduced in GL core 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void delete_program(u32 program)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteProgram)
        glIsProgram(program);
    }
    glDeleteProgram(program);
    detail::error_check("DeleteProgram"sv);
}

/* Introduced in GL core 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void delete_shader(u32 shader)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteShader)
        glIsShader(shader);
    }
    glDeleteShader(shader);
    detail::error_check("DeleteShader"sv);
}

/* Introduced in GL core 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void detach_shader(u32 program, u32 shader)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DetachShader)
        glIsProgram(program);
        glIsShader(shader);
    }
    glDetachShader(program, shader);
    detail::error_check("DetachShader"sv);
}

/* Introduced in GL core 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void disable_vertex_attrib_array(u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DisableVertexAttribArray)
    }
    glDisableVertexAttribArray(index);
    detail::error_check("DisableVertexAttribArray"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_draw_buffer_mode>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_draw_buffer_mode>&& std::is_same_v<
            std::decay_t<typename span_const_draw_buffer_mode::value_type>,
            std::decay_t<group::draw_buffer_mode>>) STATICINLINE
    void draw_buffers(span_const_draw_buffer_mode const& bufs)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DrawBuffers)
    }
    glDrawBuffers(
        bufs.size(),
        bufs.size() ? reinterpret_cast<const GLenum*>(bufs.data()) : nullptr);
    detail::error_check("DrawBuffers"sv);
}

/* Introduced in GL core 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void enable_vertex_attrib_array(u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EnableVertexAttribArray)
    }
    glEnableVertexAttribArray(index);
    detail::error_check("EnableVertexAttribArray"sv);
}

/* Introduced in GL core 2.0 */
template<class span_GLchar>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>) STATICINLINE
    void get_active_attrib(
        u32         program,
        u32         index,
        i32&        length,
        i32&        size,
        GLenum&     type,
        span_GLchar name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveAttrib)
        glIsProgram(program);
    }
    glGetActiveAttrib(
        program, index, name.size(), &length, &size, &type, name.data());
    detail::error_check("GetActiveAttrib"sv);
}

/* Introduced in GL core 2.0 */
template<class span_GLchar>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>) STATICINLINE
    void get_active_uniform(
        u32         program,
        u32         index,
        i32&        length,
        i32&        size,
        GLenum&     type,
        span_GLchar name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetActiveUniform)
        glIsProgram(program);
    }
    glGetActiveUniform(
        program, index, name.size(), &length, &size, &type, name.data());
    detail::error_check("GetActiveUniform"sv);
}

/* Introduced in GL core 2.0 */
template<class span_u32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void get_attached_shaders(u32 program, i32& count, span_u32 shaders)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetAttachedShaders)
        glIsProgram(program);
    }
    glGetAttachedShaders(
        program,
        shaders.size(),
        &count,
        shaders.size() ? reinterpret_cast<GLuint*>(shaders.data()) : nullptr);
    detail::error_check("GetAttachedShaders"sv);
}

/* Introduced in GL core 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE GLint
    get_attrib_location(u32 program, std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetAttribLocation)
        glIsProgram(program);
    }
    auto out = glGetAttribLocation(program, name.data());
    detail::error_check("GetAttribLocation"sv);
    return out;
}

/* Introduced in GL core 2.0 */
template<class span_GLchar>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>) STATICINLINE
    void get_program_info_log(u32 program, i32& length, span_GLchar infoLog)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramInfoLog)
        glIsProgram(program);
    }
    glGetProgramInfoLog(program, infoLog.size(), &length, infoLog.data());
    detail::error_check("GetProgramInfoLog"sv);
}

/* Introduced in GL core 2.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_programiv(
        u32 program, group::program_property_arb pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramiv)
        glIsProgram(program);
    }
    glGetProgramiv(
        program,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetProgramiv"sv);
}

/* Introduced in GL core 2.0 */
template<class span_GLchar>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>) STATICINLINE
    void get_shader_info_log(u32 shader, i32& length, span_GLchar infoLog)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetShaderInfoLog)
        glIsShader(shader);
    }
    glGetShaderInfoLog(shader, infoLog.size(), &length, infoLog.data());
    detail::error_check("GetShaderInfoLog"sv);
}

/* Introduced in GL core 2.0 */
template<class span_GLchar>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>) STATICINLINE
    void get_shader_source(u32 shader, i32& length, span_GLchar source)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetShaderSource)
        glIsShader(shader);
    }
    glGetShaderSource(shader, source.size(), &length, source.data());
    detail::error_check("GetShaderSource"sv);
}

/* Introduced in GL core 2.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_shaderiv(
        u32 shader, group::shader_parameter_name pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetShaderiv)
        glIsShader(shader);
    }
    glGetShaderiv(
        shader,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetShaderiv"sv);
}

/* Introduced in GL core 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE GLint
    get_uniform_location(u32 program, std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformLocation)
        glIsProgram(program);
    }
    auto out = glGetUniformLocation(program, name.data());
    detail::error_check("GetUniformLocation"sv);
    return out;
}

/* Introduced in GL core 2.0 */
template<class span_f32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_f32>&& std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void get_uniformfv(u32 program, i32 location, span_f32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformfv)
        glIsProgram(program);
    }
    glGetUniformfv(
        program,
        location,
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetUniformfv"sv);
}

/* Introduced in GL core 2.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_uniformiv(u32 program, i32 location, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformiv)
        glIsProgram(program);
    }
    glGetUniformiv(
        program,
        location,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetUniformiv"sv);
}

/* Introduced in GL core 2.0 */
template<class span_void>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_void>) STATICINLINE
    void get_vertex_attrib_pointerv(
        u32                                       index,
        group::vertex_attrib_pointer_property_arb pname,
        span_void                                 pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribPointerv)
    }
    glGetVertexAttribPointerv(
        index,
        static_cast<GLenum>(pname),
        pointer.size() ? reinterpret_cast<void**>(pointer.data()) : nullptr);
    detail::error_check("GetVertexAttribPointerv"sv);
}

/* Introduced in GL core 2.0 */
template<class span_f64>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_f64>&& std::is_same_v<
                 std::decay_t<typename span_f64::value_type>,
                 std::decay_t<f64>>) STATICINLINE
    void get_vertex_attribdv(
        u32 index, group::vertex_attrib_property_arb pname, span_f64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribdv)
    }
    glGetVertexAttribdv(
        index,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLdouble*>(params.data()) : nullptr);
    detail::error_check("GetVertexAttribdv"sv);
}

/* Introduced in GL core 2.0 */
template<class span_f32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_f32>&& std::is_same_v<
                 std::decay_t<typename span_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void get_vertex_attribfv(
        u32 index, group::vertex_attrib_property_arb pname, span_f32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribfv)
    }
    glGetVertexAttribfv(
        index,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetVertexAttribfv"sv);
}

/* Introduced in GL core 2.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_vertex_attribiv(
        u32 index, group::vertex_attrib_property_arb pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribiv)
    }
    glGetVertexAttribiv(
        index,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetVertexAttribiv"sv);
}

/* Introduced in GL core 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE GLboolean
    is_program(u32 program)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsProgram)
        glIsProgram(program);
    }
    auto out = glIsProgram(program);
    detail::error_check("IsProgram"sv);
    return out;
}

/* Introduced in GL core 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE GLboolean
    is_shader(u32 shader)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsShader)
        glIsShader(shader);
    }
    auto out = glIsShader(shader);
    detail::error_check("IsShader"sv);
    return out;
}

/* Introduced in GL core 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void link_program(u32 program)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(LinkProgram)
        glIsProgram(program);
    }
    glLinkProgram(program);
    detail::error_check("LinkProgram"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void shader_source(
        u32                           shader,
        std::vector<std::string_view> string,
        span_const_i32 const&         length)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ShaderSource)
        glIsShader(shader);
    }
    auto [string_lens, string_cstr, string_store] =
        detail::transform_strings(string);
    glShaderSource(
        shader,
        string_cstr.size(),
        string_cstr.data(),
        length.size() ? reinterpret_cast<const GLint*>(length.data())
                      : nullptr);
    detail::error_check("ShaderSource"sv);
}

/* Introduced in GL core 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void stencil_func_separate(
        group::stencil_face_direction face,
        group::stencil_function       func,
        i32                           ref,
        u32                           mask)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(StencilFuncSeparate)
    }
    glStencilFuncSeparate(
        static_cast<GLenum>(face), static_cast<GLenum>(func), ref, mask);
    detail::error_check("StencilFuncSeparate"sv);
}

/* Introduced in GL core 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void stencil_mask_separate(group::stencil_face_direction face, u32 mask)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(StencilMaskSeparate)
    }
    glStencilMaskSeparate(static_cast<GLenum>(face), mask);
    detail::error_check("StencilMaskSeparate"sv);
}

/* Introduced in GL core 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void stencil_op_separate(
        group::stencil_face_direction face,
        group::stencil_op             sfail,
        group::stencil_op             dpfail,
        group::stencil_op             dppass)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(StencilOpSeparate)
    }
    glStencilOpSeparate(
        static_cast<GLenum>(face),
        static_cast<GLenum>(sfail),
        static_cast<GLenum>(dpfail),
        static_cast<GLenum>(dppass));
    detail::error_check("StencilOpSeparate"sv);
}

/* Introduced in GL core 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void uniform(i32 location, f32 v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1f)
    }
    glUniform1f(location, v0);
    detail::error_check("Uniform1f"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_f32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_f32>&& std::is_same_v<
                 std::decay_t<typename span_const_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void uniform(i32 location, i32 count, span_const_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1fv)
    }
    glUniform1fv(
        location, count, reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("Uniform1fv"sv);
}

/* Introduced in GL core 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void uniform(i32 location, i32 v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1i)
    }
    glUniform1i(location, v0);
    detail::error_check("Uniform1i"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void uniform(i32 location, i32 count, span_const_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1iv)
    }
    glUniform1iv(location, count, reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("Uniform1iv"sv);
}

/* Introduced in GL core 2.0 */
template<class vec_2_f32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_2_f32, f32, 2>) STATICINLINE
    void uniform(i32 location, vec_2_f32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2f)
    }
    glUniform2f(location, v0.x(), v0.y());
    detail::error_check("Uniform2f"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_vec_2_f32>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_vec_2_f32>&& semantic::concepts::
            Vector<typename span_const_vec_2_f32::value_type, f32, 2>)
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_2_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2fv)
    }
    glUniform2fv(
        location, count, reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("Uniform2fv"sv);
}

/* Introduced in GL core 2.0 */
template<class vec_2_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_2_i32, i32, 2>) STATICINLINE
    void uniform(i32 location, vec_2_i32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2i)
    }
    glUniform2i(location, v0.x(), v0.y());
    detail::error_check("Uniform2i"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_vec_2_i32>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_vec_2_i32>&& semantic::concepts::
            Vector<typename span_const_vec_2_i32::value_type, i32, 2>)
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_2_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2iv)
    }
    glUniform2iv(location, count, reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("Uniform2iv"sv);
}

/* Introduced in GL core 2.0 */
template<class vec_3_f32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_3_f32, f32, 3>) STATICINLINE
    void uniform(i32 location, vec_3_f32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3f)
    }
    glUniform3f(location, v0.x(), v0.y(), v0.z());
    detail::error_check("Uniform3f"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_vec_3_f32>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_vec_3_f32>&& semantic::concepts::
            Vector<typename span_const_vec_3_f32::value_type, f32, 3>)
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_3_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3fv)
    }
    glUniform3fv(
        location, count, reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("Uniform3fv"sv);
}

/* Introduced in GL core 2.0 */
template<class vec_3_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_3_i32, i32, 3>) STATICINLINE
    void uniform(i32 location, vec_3_i32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3i)
    }
    glUniform3i(location, v0.x(), v0.y(), v0.z());
    detail::error_check("Uniform3i"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_vec_3_i32>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_vec_3_i32>&& semantic::concepts::
            Vector<typename span_const_vec_3_i32::value_type, i32, 3>)
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_3_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3iv)
    }
    glUniform3iv(location, count, reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("Uniform3iv"sv);
}

/* Introduced in GL core 2.0 */
template<class vec_4_f32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_4_f32, f32, 4>) STATICINLINE
    void uniform(i32 location, vec_4_f32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4f)
    }
    glUniform4f(location, v0.x(), v0.y(), v0.z(), v0.w());
    detail::error_check("Uniform4f"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_vec_4_f32>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_vec_4_f32>&& semantic::concepts::
            Vector<typename span_const_vec_4_f32::value_type, f32, 4>)
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_4_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4fv)
    }
    glUniform4fv(
        location, count, reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("Uniform4fv"sv);
}

/* Introduced in GL core 2.0 */
template<class vec_4_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_4_i32, i32, 4>) STATICINLINE
    void uniform(i32 location, vec_4_i32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4i)
    }
    glUniform4i(location, v0.x(), v0.y(), v0.z(), v0.w());
    detail::error_check("Uniform4i"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_vec_4_i32>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_vec_4_i32>&& semantic::concepts::
            Vector<typename span_const_vec_4_i32::value_type, i32, 4>)
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_4_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4iv)
    }
    glUniform4iv(location, count, reinterpret_cast<const GLint*>(value.data()));
    detail::error_check("Uniform4iv"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_mat_2x2_f32>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_mat_2x2_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_2x2_f32::value_type, f32, 2, 2>)
    STATICINLINE void uniform(
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_2x2_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix2fv)
    }
    glUniformMatrix2fv(
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix2fv"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_mat_3x3_f32>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_mat_3x3_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_3x3_f32::value_type, f32, 3, 3>)
    STATICINLINE void uniform(
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_3x3_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix3fv)
    }
    glUniformMatrix3fv(
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix3fv"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_mat_4x4_f32>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_mat_4x4_f32>&& semantic::concepts::
            Matrix<typename span_const_mat_4x4_f32::value_type, f32, 4, 4>)
    STATICINLINE void uniform(
        i32                           location,
        i32                           count,
        bool                          transpose,
        span_const_mat_4x4_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UniformMatrix4fv)
    }
    glUniformMatrix4fv(
        location,
        count,
        transpose,
        reinterpret_cast<const GLfloat*>(value.data()));
    detail::error_check("UniformMatrix4fv"sv);
}

/* Introduced in GL core 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void use_program(u32 program)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(UseProgram)
        glIsProgram(program);
    }
    glUseProgram(program);
    detail::error_check("UseProgram"sv);
}

/* Introduced in GL core 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void validate_program(u32 program)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ValidateProgram)
        glIsProgram(program);
    }
    glValidateProgram(program);
    detail::error_check("ValidateProgram"sv);
}

/* Introduced in GL core 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void vertex_attrib1d(u32 index, f64 x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib1d)
    }
    glVertexAttrib1d(index, x);
    detail::error_check("VertexAttrib1d"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_f64>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_f64>&& std::is_same_v<
                 std::decay_t<typename span_const_f64::value_type>,
                 std::decay_t<f64>>) STATICINLINE
    void vertex_attrib1dv(u32 index, span_const_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib1dv)
    }
    glVertexAttrib1dv(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttrib1dv"sv);
}

/* Introduced in GL core 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void vertex_attrib1f(u32 index, f32 x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib1f)
    }
    glVertexAttrib1f(index, x);
    detail::error_check("VertexAttrib1f"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_f32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_f32>&& std::is_same_v<
                 std::decay_t<typename span_const_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void vertex_attrib1fv(u32 index, span_const_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib1fv)
    }
    glVertexAttrib1fv(index, reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("VertexAttrib1fv"sv);
}

/* Introduced in GL core 2.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<2, 0>>) STATICINLINE
    void vertex_attrib1s(u32 index, i16 x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib1s)
    }
    glVertexAttrib1s(index, x);
    detail::error_check("VertexAttrib1s"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_i16>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_i16>&& std::is_same_v<
                 std::decay_t<typename span_const_i16::value_type>,
                 std::decay_t<i16>>) STATICINLINE
    void vertex_attrib1sv(u32 index, span_const_i16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib1sv)
    }
    glVertexAttrib1sv(index, reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("VertexAttrib1sv"sv);
}

/* Introduced in GL core 2.0 */
template<class vec_2_f64>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_2_f64, f64, 2>) STATICINLINE
    void vertex_attrib2d(u32 index, vec_2_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib2d)
    }
    glVertexAttrib2d(index, x.x(), x.y());
    detail::error_check("VertexAttrib2d"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_vec_2_f64>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_vec_2_f64>&& semantic::concepts::
            Vector<typename span_const_vec_2_f64::value_type, f64, 2>)
    STATICINLINE void vertex_attrib2dv(u32 index, span_const_vec_2_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib2dv)
    }
    glVertexAttrib2dv(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttrib2dv"sv);
}

/* Introduced in GL core 2.0 */
template<class vec_2_f32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_2_f32, f32, 2>) STATICINLINE
    void vertex_attrib2f(u32 index, vec_2_f32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib2f)
    }
    glVertexAttrib2f(index, x.x(), x.y());
    detail::error_check("VertexAttrib2f"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_vec_2_f32>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_vec_2_f32>&& semantic::concepts::
            Vector<typename span_const_vec_2_f32::value_type, f32, 2>)
    STATICINLINE void vertex_attrib2fv(u32 index, span_const_vec_2_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib2fv)
    }
    glVertexAttrib2fv(index, reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("VertexAttrib2fv"sv);
}

/* Introduced in GL core 2.0 */
template<class vec_2_i16>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_2_i16, i16, 2>) STATICINLINE
    void vertex_attrib2s(u32 index, vec_2_i16 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib2s)
    }
    glVertexAttrib2s(index, x.x(), x.y());
    detail::error_check("VertexAttrib2s"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_vec_2_i16>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_vec_2_i16>&& semantic::concepts::
            Vector<typename span_const_vec_2_i16::value_type, i16, 2>)
    STATICINLINE void vertex_attrib2sv(u32 index, span_const_vec_2_i16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib2sv)
    }
    glVertexAttrib2sv(index, reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("VertexAttrib2sv"sv);
}

/* Introduced in GL core 2.0 */
template<class vec_3_f64>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_3_f64, f64, 3>) STATICINLINE
    void vertex_attrib3d(u32 index, vec_3_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib3d)
    }
    glVertexAttrib3d(index, x.x(), x.y(), x.z());
    detail::error_check("VertexAttrib3d"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_vec_3_f64>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_vec_3_f64>&& semantic::concepts::
            Vector<typename span_const_vec_3_f64::value_type, f64, 3>)
    STATICINLINE void vertex_attrib3dv(u32 index, span_const_vec_3_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib3dv)
    }
    glVertexAttrib3dv(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttrib3dv"sv);
}

/* Introduced in GL core 2.0 */
template<class vec_3_f32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_3_f32, f32, 3>) STATICINLINE
    void vertex_attrib3f(u32 index, vec_3_f32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib3f)
    }
    glVertexAttrib3f(index, x.x(), x.y(), x.z());
    detail::error_check("VertexAttrib3f"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_vec_3_f32>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_vec_3_f32>&& semantic::concepts::
            Vector<typename span_const_vec_3_f32::value_type, f32, 3>)
    STATICINLINE void vertex_attrib3fv(u32 index, span_const_vec_3_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib3fv)
    }
    glVertexAttrib3fv(index, reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("VertexAttrib3fv"sv);
}

/* Introduced in GL core 2.0 */
template<class vec_3_i16>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_3_i16, i16, 3>) STATICINLINE
    void vertex_attrib3s(u32 index, vec_3_i16 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib3s)
    }
    glVertexAttrib3s(index, x.x(), x.y(), x.z());
    detail::error_check("VertexAttrib3s"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_vec_3_i16>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_vec_3_i16>&& semantic::concepts::
            Vector<typename span_const_vec_3_i16::value_type, i16, 3>)
    STATICINLINE void vertex_attrib3sv(u32 index, span_const_vec_3_i16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib3sv)
    }
    glVertexAttrib3sv(index, reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("VertexAttrib3sv"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_i8>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_i8>&& std::is_same_v<
                 std::decay_t<typename span_const_i8::value_type>,
                 std::decay_t<i8>>) STATICINLINE
    void vertex_attrib4_nbv(u32 index, span_const_i8 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4Nbv)
    }
    glVertexAttrib4Nbv(
        index, v.size() ? reinterpret_cast<const GLbyte*>(v.data()) : nullptr);
    detail::error_check("VertexAttrib4Nbv"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_i32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void vertex_attrib4_niv(u32 index, span_const_i32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4Niv)
    }
    glVertexAttrib4Niv(
        index, v.size() ? reinterpret_cast<const GLint*>(v.data()) : nullptr);
    detail::error_check("VertexAttrib4Niv"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_i16>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_i16>&& std::is_same_v<
                 std::decay_t<typename span_const_i16::value_type>,
                 std::decay_t<i16>>) STATICINLINE
    void vertex_attrib4_nsv(u32 index, span_const_i16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4Nsv)
    }
    glVertexAttrib4Nsv(
        index, v.size() ? reinterpret_cast<const GLshort*>(v.data()) : nullptr);
    detail::error_check("VertexAttrib4Nsv"sv);
}

/* Introduced in GL core 2.0 */
template<class vec_4_u8>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_4_u8, u8, 4>) STATICINLINE
    void vertex_attrib4_nub(u32 index, vec_4_u8 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4Nub)
    }
    glVertexAttrib4Nub(index, x.x(), x.y(), x.z(), x.w());
    detail::error_check("VertexAttrib4Nub"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_u8>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_u8>&& std::is_same_v<
                 std::decay_t<typename span_const_u8::value_type>,
                 std::decay_t<u8>>) STATICINLINE
    void vertex_attrib4_nubv(u32 index, span_const_u8 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4Nubv)
    }
    glVertexAttrib4Nubv(
        index, v.size() ? reinterpret_cast<const GLubyte*>(v.data()) : nullptr);
    detail::error_check("VertexAttrib4Nubv"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void vertex_attrib4_nuiv(u32 index, span_const_u32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4Nuiv)
    }
    glVertexAttrib4Nuiv(
        index, v.size() ? reinterpret_cast<const GLuint*>(v.data()) : nullptr);
    detail::error_check("VertexAttrib4Nuiv"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_u16>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_u16>&& std::is_same_v<
                 std::decay_t<typename span_const_u16::value_type>,
                 std::decay_t<u16>>) STATICINLINE
    void vertex_attrib4_nusv(u32 index, span_const_u16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4Nusv)
    }
    glVertexAttrib4Nusv(
        index,
        v.size() ? reinterpret_cast<const GLushort*>(v.data()) : nullptr);
    detail::error_check("VertexAttrib4Nusv"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_i8>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_i8>&& std::is_same_v<
                 std::decay_t<typename span_const_i8::value_type>,
                 std::decay_t<i8>>) STATICINLINE
    void vertex_attrib4bv(u32 index, span_const_i8 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4bv)
    }
    glVertexAttrib4bv(
        index, v.size() ? reinterpret_cast<const GLbyte*>(v.data()) : nullptr);
    detail::error_check("VertexAttrib4bv"sv);
}

/* Introduced in GL core 2.0 */
template<class vec_4_f64>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_4_f64, f64, 4>) STATICINLINE
    void vertex_attrib4d(u32 index, vec_4_f64 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4d)
    }
    glVertexAttrib4d(index, x.x(), x.y(), x.z(), x.w());
    detail::error_check("VertexAttrib4d"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_vec_4_f64>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_vec_4_f64>&& semantic::concepts::
            Vector<typename span_const_vec_4_f64::value_type, f64, 4>)
    STATICINLINE void vertex_attrib4dv(u32 index, span_const_vec_4_f64 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4dv)
    }
    glVertexAttrib4dv(index, reinterpret_cast<const GLdouble*>(v.data()));
    detail::error_check("VertexAttrib4dv"sv);
}

/* Introduced in GL core 2.0 */
template<class vec_4_f32>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_4_f32, f32, 4>) STATICINLINE
    void vertex_attrib4f(u32 index, vec_4_f32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4f)
    }
    glVertexAttrib4f(index, x.x(), x.y(), x.z(), x.w());
    detail::error_check("VertexAttrib4f"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_vec_4_f32>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_vec_4_f32>&& semantic::concepts::
            Vector<typename span_const_vec_4_f32::value_type, f32, 4>)
    STATICINLINE void vertex_attrib4fv(u32 index, span_const_vec_4_f32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4fv)
    }
    glVertexAttrib4fv(index, reinterpret_cast<const GLfloat*>(v.data()));
    detail::error_check("VertexAttrib4fv"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_vec_4_i32>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_vec_4_i32>&& semantic::concepts::
            Vector<typename span_const_vec_4_i32::value_type, i32, 4>)
    STATICINLINE void vertex_attrib4iv(u32 index, span_const_vec_4_i32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4iv)
    }
    glVertexAttrib4iv(index, reinterpret_cast<const GLint*>(v.data()));
    detail::error_check("VertexAttrib4iv"sv);
}

/* Introduced in GL core 2.0 */
template<class vec_4_i16>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Vector<vec_4_i16, i16, 4>) STATICINLINE
    void vertex_attrib4s(u32 index, vec_4_i16 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4s)
    }
    glVertexAttrib4s(index, x.x(), x.y(), x.z(), x.w());
    detail::error_check("VertexAttrib4s"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_vec_4_i16>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_vec_4_i16>&& semantic::concepts::
            Vector<typename span_const_vec_4_i16::value_type, i16, 4>)
    STATICINLINE void vertex_attrib4sv(u32 index, span_const_vec_4_i16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4sv)
    }
    glVertexAttrib4sv(index, reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("VertexAttrib4sv"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_u8>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_u8>&& std::is_same_v<
                 std::decay_t<typename span_const_u8::value_type>,
                 std::decay_t<u8>>) STATICINLINE
    void vertex_attrib4ubv(u32 index, span_const_u8 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4ubv)
    }
    glVertexAttrib4ubv(
        index, v.size() ? reinterpret_cast<const GLubyte*>(v.data()) : nullptr);
    detail::error_check("VertexAttrib4ubv"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_vec_4_u32>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_vec_4_u32>&& semantic::concepts::
            Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
    STATICINLINE
    void vertex_attrib4uiv(u32 index, span_const_vec_4_u32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4uiv)
    }
    glVertexAttrib4uiv(index, reinterpret_cast<const GLuint*>(v.data()));
    detail::error_check("VertexAttrib4uiv"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_vec_4_u16>
requires(
    MinimumVersion<Current, Version<2, 0>>&&
        semantic::concepts::Span<span_const_vec_4_u16>&& semantic::concepts::
            Vector<typename span_const_vec_4_u16::value_type, u16, 4>)
    STATICINLINE
    void vertex_attrib4usv(u32 index, span_const_vec_4_u16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttrib4usv)
    }
    glVertexAttrib4usv(index, reinterpret_cast<const GLushort*>(v.data()));
    detail::error_check("VertexAttrib4usv"sv);
}

/* Introduced in GL core 2.0 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<2, 0>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void vertex_attrib_pointer(
        u32                               index,
        i32                               size,
        group::vertex_attrib_pointer_type type,
        bool                              normalized,
        i32                               stride,
        span_const_void const&            pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribPointer)
    }
    glVertexAttribPointer(
        index,
        size,
        static_cast<GLenum>(type),
        normalized,
        stride,
        pointer.size() ? reinterpret_cast<const void*>(pointer.data())
                       : nullptr);
    detail::error_check("VertexAttribPointer"sv);
}

#endif // GL_VERSION_2_0
