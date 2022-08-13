#ifdef GL_VERSION_3_0
/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void begin_conditional_render(u32 id, group::conditional_render_mode mode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BeginConditionalRender)
    }
    glBeginConditionalRender(id, static_cast<GLenum>(mode));
    detail::error_check("BeginConditionalRender"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void begin_transform_feedback(group::primitive_type primitiveMode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BeginTransformFeedback)
    }
    glBeginTransformFeedback(static_cast<GLenum>(primitiveMode));
    detail::error_check("BeginTransformFeedback"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void bind_buffer_base(
        group::buffer_target_arb target, u32 index, u32 buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindBufferBase)
        glIsBuffer(buffer);
    }
    glBindBufferBase(static_cast<GLenum>(target), index, buffer);
    detail::error_check("BindBufferBase"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void bind_buffer_range(
        group::buffer_target_arb target,
        u32                      index,
        u32                      buffer,
        GLintptr                 offset,
        GLsizeiptr               size)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindBufferRange)
        glIsBuffer(buffer);
    }
    glBindBufferRange(static_cast<GLenum>(target), index, buffer, offset, size);
    detail::error_check("BindBufferRange"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void bind_frag_data_location(
        u32 program, u32 color, std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindFragDataLocation)
        glIsProgram(program);
    }
    glBindFragDataLocation(program, color, name.data());
    detail::error_check("BindFragDataLocation"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE void clamp_color(
    group::clamp_color_target_arb target, group::clamp_color_mode_arb clamp)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClampColor)
    }
    glClampColor(static_cast<GLenum>(target), static_cast<GLenum>(clamp));
    detail::error_check("ClampColor"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void clear_bufferfi(
        group::buffer buffer, i32 drawbuffer, f32 depth, i32 stencil)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearBufferfi)
    }
    glClearBufferfi(static_cast<GLenum>(buffer), drawbuffer, depth, stencil);
    detail::error_check("ClearBufferfi"sv);
}

/* Introduced in GL core 3.0 */
template<class span_const_f32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_f32>&& std::is_same_v<
                 std::decay_t<typename span_const_f32::value_type>,
                 std::decay_t<f32>>) STATICINLINE
    void clear_bufferfv(
        group::buffer buffer, i32 drawbuffer, span_const_f32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearBufferfv)
    }
    glClearBufferfv(
        static_cast<GLenum>(buffer),
        drawbuffer,
        value.size() ? reinterpret_cast<const GLfloat*>(value.data())
                     : nullptr);
    detail::error_check("ClearBufferfv"sv);
}

/* Introduced in GL core 3.0 */
template<class span_const_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void clear_bufferiv(
        group::buffer buffer, i32 drawbuffer, span_const_i32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearBufferiv)
    }
    glClearBufferiv(
        static_cast<GLenum>(buffer),
        drawbuffer,
        value.size() ? reinterpret_cast<const GLint*>(value.data()) : nullptr);
    detail::error_check("ClearBufferiv"sv);
}

/* Introduced in GL core 3.0 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void clear_bufferuiv(
        group::buffer buffer, i32 drawbuffer, span_const_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClearBufferuiv)
    }
    glClearBufferuiv(
        static_cast<GLenum>(buffer),
        drawbuffer,
        value.size() ? reinterpret_cast<const GLuint*>(value.data()) : nullptr);
    detail::error_check("ClearBufferuiv"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void color_maski(u32 index, bool r, bool g, bool b, bool a)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ColorMaski)
    }
    glColorMaski(index, r, g, b, a);
    detail::error_check("ColorMaski"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void disablei(group::enable_cap target, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Disablei)
    }
    glDisablei(static_cast<GLenum>(target), index);
    detail::error_check("Disablei"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void enablei(group::enable_cap target, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Enablei)
    }
    glEnablei(static_cast<GLenum>(target), index);
    detail::error_check("Enablei"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void end_conditional_render()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EndConditionalRender)
    }
    glEndConditionalRender();
    detail::error_check("EndConditionalRender"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void end_transform_feedback()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EndTransformFeedback)
    }
    glEndTransformFeedback();
    detail::error_check("EndTransformFeedback"sv);
}

/* Introduced in GL core 3.0 */
template<class span_bool>
requires(MinimumVersion<Current, Version<3, 0>>&&
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

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE GLint
    get_frag_data_location(u32 program, std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFragDataLocation)
        glIsProgram(program);
    }
    auto out = glGetFragDataLocation(program, name.data());
    detail::error_check("GetFragDataLocation"sv);
    return out;
}

/* Introduced in GL core 3.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_integeri_v(group::get_prop target, u32 index, span_i32 data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetIntegeri_v)
    }
    glGetIntegeri_v(
        static_cast<GLenum>(target),
        index,
        data.size() ? reinterpret_cast<GLint*>(data.data()) : nullptr);
    detail::error_check("GetIntegeri_v"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE stl_types::String
    get_stringi(group::string_name name, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetStringi)
    }
    auto out = glGetStringi(static_cast<GLenum>(name), index);
    detail::error_check("GetStringi"sv);
    return reinterpret_cast<const char*>(out);
}

/* Introduced in GL core 3.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_tex_parameter_iiv(
        group::texture_target        target,
        group::get_texture_parameter pname,
        span_i32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTexParameterIiv)
    }
    glGetTexParameterIiv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetTexParameterIiv"sv);
}

/* Introduced in GL core 3.0 */
template<class span_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void get_tex_parameter_iuiv(
        group::texture_target        target,
        group::get_texture_parameter pname,
        span_u32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTexParameterIuiv)
    }
    glGetTexParameterIuiv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetTexParameterIuiv"sv);
}

/* Introduced in GL core 3.0 */
template<class span_GLchar>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>) STATICINLINE
    void get_transform_feedback_varying(
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
        GLW_FPTR_CHECK(GetTransformFeedbackVarying)
        glIsProgram(program);
    }
    glGetTransformFeedbackVarying(
        program, index, name.size(), &length, &size, &type, name.data());
    detail::error_check("GetTransformFeedbackVarying"sv);
}

/* Introduced in GL core 3.0 */
template<class span_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void get_uniformuiv(u32 program, i32 location, span_u32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformuiv)
        glIsProgram(program);
    }
    glGetUniformuiv(
        program,
        location,
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetUniformuiv"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void get_vertex_attrib_iiv(
        u32 index, group::vertex_attrib_enum pname, i32& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribIiv)
    }
    glGetVertexAttribIiv(index, static_cast<GLenum>(pname), &params);
    detail::error_check("GetVertexAttribIiv"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void get_vertex_attrib_iuiv(
        u32 index, group::vertex_attrib_enum pname, u32& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetVertexAttribIuiv)
    }
    glGetVertexAttribIuiv(index, static_cast<GLenum>(pname), &params);
    detail::error_check("GetVertexAttribIuiv"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE GLboolean
    is_enabledi(group::enable_cap target, u32 index)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsEnabledi)
    }
    auto out = glIsEnabledi(static_cast<GLenum>(target), index);
    detail::error_check("IsEnabledi"sv);
    return out;
}

/* Introduced in GL core 3.0 */
template<class span_const_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void tex_parameter_iiv(
        group::texture_target         target,
        group::texture_parameter_name pname,
        span_const_i32 const&         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexParameterIiv)
    }
    glTexParameterIiv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("TexParameterIiv"sv);
}

/* Introduced in GL core 3.0 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void tex_parameter_iuiv(
        group::texture_target         target,
        group::texture_parameter_name pname,
        span_const_u32 const&         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexParameterIuiv)
    }
    glTexParameterIuiv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLuint*>(params.data())
                      : nullptr);
    detail::error_check("TexParameterIuiv"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void transform_feedback_varyings(
        u32                                   program,
        std::vector<std::string_view>         varyings,
        group::transform_feedback_buffer_mode bufferMode)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TransformFeedbackVaryings)
        glIsProgram(program);
    }
    auto [varyings_lens, varyings_cstr, varyings_store] =
        detail::transform_strings(varyings);
    glTransformFeedbackVaryings(
        program,
        varyings_cstr.size(),
        varyings_cstr.data(),
        static_cast<GLenum>(bufferMode));
    detail::error_check("TransformFeedbackVaryings"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void uniform(i32 location, u32 v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1ui)
    }
    glUniform1ui(location, v0);
    detail::error_check("Uniform1ui"sv);
}

/* Introduced in GL core 3.0 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void uniform(i32 location, i32 count, span_const_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform1uiv)
    }
    glUniform1uiv(
        location, count, reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("Uniform1uiv"sv);
}

/* Introduced in GL core 3.0 */
template<class vec_2_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Vector<vec_2_u32, u32, 2>) STATICINLINE
    void uniform(i32 location, vec_2_u32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2ui)
    }
    glUniform2ui(location, v0.x(), v0.y());
    detail::error_check("Uniform2ui"sv);
}

/* Introduced in GL core 3.0 */
template<class span_const_vec_2_u32>
requires(
    MinimumVersion<Current, Version<3, 0>>&&
        semantic::concepts::Span<span_const_vec_2_u32>&& semantic::concepts::
            Vector<typename span_const_vec_2_u32::value_type, u32, 2>)
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_2_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform2uiv)
    }
    glUniform2uiv(
        location, count, reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("Uniform2uiv"sv);
}

/* Introduced in GL core 3.0 */
template<class vec_3_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Vector<vec_3_u32, u32, 3>) STATICINLINE
    void uniform(i32 location, vec_3_u32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3ui)
    }
    glUniform3ui(location, v0.x(), v0.y(), v0.z());
    detail::error_check("Uniform3ui"sv);
}

/* Introduced in GL core 3.0 */
template<class span_const_vec_3_u32>
requires(
    MinimumVersion<Current, Version<3, 0>>&&
        semantic::concepts::Span<span_const_vec_3_u32>&& semantic::concepts::
            Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_3_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform3uiv)
    }
    glUniform3uiv(
        location, count, reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("Uniform3uiv"sv);
}

/* Introduced in GL core 3.0 */
template<class vec_4_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Vector<vec_4_u32, u32, 4>) STATICINLINE
    void uniform(i32 location, vec_4_u32 const& v0)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4ui)
    }
    glUniform4ui(location, v0.x(), v0.y(), v0.z(), v0.w());
    detail::error_check("Uniform4ui"sv);
}

/* Introduced in GL core 3.0 */
template<class span_const_vec_4_u32>
requires(
    MinimumVersion<Current, Version<3, 0>>&&
        semantic::concepts::Span<span_const_vec_4_u32>&& semantic::concepts::
            Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
    STATICINLINE
    void uniform(i32 location, i32 count, span_const_vec_4_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniform4uiv)
    }
    glUniform4uiv(
        location, count, reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("Uniform4uiv"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void vertex_attrib_i1i(u32 index, i32 x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI1i)
    }
    glVertexAttribI1i(index, x);
    detail::error_check("VertexAttribI1i"sv);
}

/* Introduced in GL core 3.0 */
template<class span_const_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_i32>&& std::is_same_v<
                 std::decay_t<typename span_const_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void vertex_attrib_i1iv(u32 index, span_const_i32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI1iv)
    }
    glVertexAttribI1iv(index, reinterpret_cast<const GLint*>(v.data()));
    detail::error_check("VertexAttribI1iv"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void vertex_attrib_i1ui(u32 index, u32 x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI1ui)
    }
    glVertexAttribI1ui(index, x);
    detail::error_check("VertexAttribI1ui"sv);
}

/* Introduced in GL core 3.0 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void vertex_attrib_i1uiv(u32 index, span_const_u32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI1uiv)
    }
    glVertexAttribI1uiv(index, reinterpret_cast<const GLuint*>(v.data()));
    detail::error_check("VertexAttribI1uiv"sv);
}

/* Introduced in GL core 3.0 */
template<class vec_2_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Vector<vec_2_i32, i32, 2>) STATICINLINE
    void vertex_attrib_i2i(u32 index, vec_2_i32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI2i)
    }
    glVertexAttribI2i(index, x.x(), x.y());
    detail::error_check("VertexAttribI2i"sv);
}

/* Introduced in GL core 3.0 */
template<class span_const_vec_2_i32>
requires(
    MinimumVersion<Current, Version<3, 0>>&&
        semantic::concepts::Span<span_const_vec_2_i32>&& semantic::concepts::
            Vector<typename span_const_vec_2_i32::value_type, i32, 2>)
    STATICINLINE
    void vertex_attrib_i2iv(u32 index, span_const_vec_2_i32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI2iv)
    }
    glVertexAttribI2iv(index, reinterpret_cast<const GLint*>(v.data()));
    detail::error_check("VertexAttribI2iv"sv);
}

/* Introduced in GL core 3.0 */
template<class vec_2_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Vector<vec_2_u32, u32, 2>) STATICINLINE
    void vertex_attrib_i2ui(u32 index, vec_2_u32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI2ui)
    }
    glVertexAttribI2ui(index, x.x(), x.y());
    detail::error_check("VertexAttribI2ui"sv);
}

/* Introduced in GL core 3.0 */
template<class span_const_vec_2_u32>
requires(
    MinimumVersion<Current, Version<3, 0>>&&
        semantic::concepts::Span<span_const_vec_2_u32>&& semantic::concepts::
            Vector<typename span_const_vec_2_u32::value_type, u32, 2>)
    STATICINLINE
    void vertex_attrib_i2uiv(u32 index, span_const_vec_2_u32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI2uiv)
    }
    glVertexAttribI2uiv(index, reinterpret_cast<const GLuint*>(v.data()));
    detail::error_check("VertexAttribI2uiv"sv);
}

/* Introduced in GL core 3.0 */
template<class vec_3_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Vector<vec_3_i32, i32, 3>) STATICINLINE
    void vertex_attrib_i3i(u32 index, vec_3_i32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI3i)
    }
    glVertexAttribI3i(index, x.x(), x.y(), x.z());
    detail::error_check("VertexAttribI3i"sv);
}

/* Introduced in GL core 3.0 */
template<class span_const_vec_3_i32>
requires(
    MinimumVersion<Current, Version<3, 0>>&&
        semantic::concepts::Span<span_const_vec_3_i32>&& semantic::concepts::
            Vector<typename span_const_vec_3_i32::value_type, i32, 3>)
    STATICINLINE
    void vertex_attrib_i3iv(u32 index, span_const_vec_3_i32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI3iv)
    }
    glVertexAttribI3iv(index, reinterpret_cast<const GLint*>(v.data()));
    detail::error_check("VertexAttribI3iv"sv);
}

/* Introduced in GL core 3.0 */
template<class vec_3_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Vector<vec_3_u32, u32, 3>) STATICINLINE
    void vertex_attrib_i3ui(u32 index, vec_3_u32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI3ui)
    }
    glVertexAttribI3ui(index, x.x(), x.y(), x.z());
    detail::error_check("VertexAttribI3ui"sv);
}

/* Introduced in GL core 3.0 */
template<class span_const_vec_3_u32>
requires(
    MinimumVersion<Current, Version<3, 0>>&&
        semantic::concepts::Span<span_const_vec_3_u32>&& semantic::concepts::
            Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
    STATICINLINE
    void vertex_attrib_i3uiv(u32 index, span_const_vec_3_u32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI3uiv)
    }
    glVertexAttribI3uiv(index, reinterpret_cast<const GLuint*>(v.data()));
    detail::error_check("VertexAttribI3uiv"sv);
}

/* Introduced in GL core 3.0 */
template<class span_const_i8>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_i8>&& std::is_same_v<
                 std::decay_t<typename span_const_i8::value_type>,
                 std::decay_t<i8>>) STATICINLINE
    void vertex_attrib_i4bv(u32 index, span_const_i8 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4bv)
    }
    glVertexAttribI4bv(
        index, v.size() ? reinterpret_cast<const GLbyte*>(v.data()) : nullptr);
    detail::error_check("VertexAttribI4bv"sv);
}

/* Introduced in GL core 3.0 */
template<class vec_4_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Vector<vec_4_i32, i32, 4>) STATICINLINE
    void vertex_attrib_i4i(u32 index, vec_4_i32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4i)
    }
    glVertexAttribI4i(index, x.x(), x.y(), x.z(), x.w());
    detail::error_check("VertexAttribI4i"sv);
}

/* Introduced in GL core 3.0 */
template<class span_const_vec_4_i32>
requires(
    MinimumVersion<Current, Version<3, 0>>&&
        semantic::concepts::Span<span_const_vec_4_i32>&& semantic::concepts::
            Vector<typename span_const_vec_4_i32::value_type, i32, 4>)
    STATICINLINE
    void vertex_attrib_i4iv(u32 index, span_const_vec_4_i32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4iv)
    }
    glVertexAttribI4iv(index, reinterpret_cast<const GLint*>(v.data()));
    detail::error_check("VertexAttribI4iv"sv);
}

/* Introduced in GL core 3.0 */
template<class span_const_vec_4_i16>
requires(
    MinimumVersion<Current, Version<3, 0>>&&
        semantic::concepts::Span<span_const_vec_4_i16>&& semantic::concepts::
            Vector<typename span_const_vec_4_i16::value_type, i16, 4>)
    STATICINLINE
    void vertex_attrib_i4sv(u32 index, span_const_vec_4_i16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4sv)
    }
    glVertexAttribI4sv(index, reinterpret_cast<const GLshort*>(v.data()));
    detail::error_check("VertexAttribI4sv"sv);
}

/* Introduced in GL core 3.0 */
template<class span_const_u8>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_u8>&& std::is_same_v<
                 std::decay_t<typename span_const_u8::value_type>,
                 std::decay_t<u8>>) STATICINLINE
    void vertex_attrib_i4ubv(u32 index, span_const_u8 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4ubv)
    }
    glVertexAttribI4ubv(
        index, v.size() ? reinterpret_cast<const GLubyte*>(v.data()) : nullptr);
    detail::error_check("VertexAttribI4ubv"sv);
}

/* Introduced in GL core 3.0 */
template<class vec_4_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Vector<vec_4_u32, u32, 4>) STATICINLINE
    void vertex_attrib_i4ui(u32 index, vec_4_u32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4ui)
    }
    glVertexAttribI4ui(index, x.x(), x.y(), x.z(), x.w());
    detail::error_check("VertexAttribI4ui"sv);
}

/* Introduced in GL core 3.0 */
template<class span_const_vec_4_u32>
requires(
    MinimumVersion<Current, Version<3, 0>>&&
        semantic::concepts::Span<span_const_vec_4_u32>&& semantic::concepts::
            Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
    STATICINLINE
    void vertex_attrib_i4uiv(u32 index, span_const_vec_4_u32 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4uiv)
    }
    glVertexAttribI4uiv(index, reinterpret_cast<const GLuint*>(v.data()));
    detail::error_check("VertexAttribI4uiv"sv);
}

/* Introduced in GL core 3.0 */
template<class span_const_vec_4_u16>
requires(
    MinimumVersion<Current, Version<3, 0>>&&
        semantic::concepts::Span<span_const_vec_4_u16>&& semantic::concepts::
            Vector<typename span_const_vec_4_u16::value_type, u16, 4>)
    STATICINLINE
    void vertex_attrib_i4usv(u32 index, span_const_vec_4_u16 const& v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribI4usv)
    }
    glVertexAttribI4usv(index, reinterpret_cast<const GLushort*>(v.data()));
    detail::error_check("VertexAttribI4usv"sv);
}

/* Introduced in GL core 3.0 */
template<class span_const_void>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_void>) STATICINLINE
    void vertex_attrib_i_pointer(
        u32                      index,
        i32                      size,
        group::vertex_attrib_int type,
        i32                      stride,
        span_const_void const&   pointer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribIPointer)
    }
    glVertexAttribIPointer(
        index,
        size,
        static_cast<GLenum>(type),
        stride,
        pointer.size() ? reinterpret_cast<const void*>(pointer.data())
                       : nullptr);
    detail::error_check("VertexAttribIPointer"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void bind_framebuffer(group::framebuffer_target target, u32 framebuffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindFramebuffer)
        glIsFramebuffer(framebuffer);
    }
    glBindFramebuffer(static_cast<GLenum>(target), framebuffer);
    detail::error_check("BindFramebuffer"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void bind_renderbuffer(group::renderbuffer_target target, u32 renderbuffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindRenderbuffer)
        glIsRenderbuffer(renderbuffer);
    }
    glBindRenderbuffer(static_cast<GLenum>(target), renderbuffer);
    detail::error_check("BindRenderbuffer"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void blit_framebuffer(
        i32                            srcX0,
        i32                            srcY0,
        i32                            srcX1,
        i32                            srcY1,
        i32                            dstX0,
        i32                            dstY0,
        i32                            dstX1,
        i32                            dstY1,
        group::clear_buffer_mask       mask,
        group::blit_framebuffer_filter filter)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlitFramebuffer)
    }
    glBlitFramebuffer(
        srcX0,
        srcY0,
        srcX1,
        srcY1,
        dstX0,
        dstY0,
        dstX1,
        dstY1,
        static_cast<GLenum>(mask),
        static_cast<GLenum>(filter));
    detail::error_check("BlitFramebuffer"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE GLenum
    check_framebuffer_status(group::framebuffer_target target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CheckFramebufferStatus)
    }
    auto out = glCheckFramebufferStatus(static_cast<GLenum>(target));
    detail::error_check("CheckFramebufferStatus"sv);
    return out;
}

/* Introduced in GL core 3.0 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void delete_framebuffers(span_const_u32 const& framebuffers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteFramebuffers)
    }
    glDeleteFramebuffers(
        framebuffers.size(),
        framebuffers.size()
            ? reinterpret_cast<const GLuint*>(framebuffers.data())
            : nullptr);
    detail::error_check("DeleteFramebuffers"sv);
}

/* Introduced in GL core 3.0 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void delete_renderbuffers(span_const_u32 const& renderbuffers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteRenderbuffers)
    }
    glDeleteRenderbuffers(
        renderbuffers.size(),
        renderbuffers.size()
            ? reinterpret_cast<const GLuint*>(renderbuffers.data())
            : nullptr);
    detail::error_check("DeleteRenderbuffers"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void framebuffer_renderbuffer(
        group::framebuffer_target     target,
        group::framebuffer_attachment attachment,
        group::renderbuffer_target    renderbuffertarget,
        u32                           renderbuffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferRenderbuffer)
        glIsRenderbuffer(renderbuffer);
    }
    glFramebufferRenderbuffer(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(renderbuffertarget),
        renderbuffer);
    detail::error_check("FramebufferRenderbuffer"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void framebuffer_texture_1d(
        group::framebuffer_target     target,
        group::framebuffer_attachment attachment,
        group::texture_target         textarget,
        u32                           texture,
        i32                           level)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTexture1D)
        glIsTexture(texture);
    }
    glFramebufferTexture1D(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(textarget),
        texture,
        level);
    detail::error_check("FramebufferTexture1D"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void framebuffer_texture_2d(
        group::framebuffer_target     target,
        group::framebuffer_attachment attachment,
        group::texture_target         textarget,
        u32                           texture,
        i32                           level)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTexture2D)
        glIsTexture(texture);
    }
    glFramebufferTexture2D(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(textarget),
        texture,
        level);
    detail::error_check("FramebufferTexture2D"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void framebuffer_texture_3d(
        group::framebuffer_target     target,
        group::framebuffer_attachment attachment,
        group::texture_target         textarget,
        u32                           texture,
        i32                           level,
        i32                           zoffset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTexture3D)
        glIsTexture(texture);
    }
    glFramebufferTexture3D(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(textarget),
        texture,
        level,
        zoffset);
    detail::error_check("FramebufferTexture3D"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void framebuffer_texture_layer(
        group::framebuffer_target     target,
        group::framebuffer_attachment attachment,
        u32                           texture,
        i32                           level,
        i32                           layer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTextureLayer)
        glIsTexture(texture);
    }
    glFramebufferTextureLayer(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        texture,
        level,
        layer);
    detail::error_check("FramebufferTextureLayer"sv);
}

/* Introduced in GL core 3.0 */
template<class span_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void gen_framebuffers(span_u32 framebuffers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenFramebuffers)
    }
    glGenFramebuffers(
        framebuffers.size(),
        framebuffers.size() ? reinterpret_cast<GLuint*>(framebuffers.data())
                            : nullptr);
    detail::error_check("GenFramebuffers"sv);
}

/* Introduced in GL core 3.0 */
template<class span_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void gen_renderbuffers(span_u32 renderbuffers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenRenderbuffers)
    }
    glGenRenderbuffers(
        renderbuffers.size(),
        renderbuffers.size() ? reinterpret_cast<GLuint*>(renderbuffers.data())
                             : nullptr);
    detail::error_check("GenRenderbuffers"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void generate_mipmap(group::texture_target target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenerateMipmap)
    }
    glGenerateMipmap(static_cast<GLenum>(target));
    detail::error_check("GenerateMipmap"sv);
}

/* Introduced in GL core 3.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_framebuffer_attachment_parameter(
        group::framebuffer_target                    target,
        group::framebuffer_attachment                attachment,
        group::framebuffer_attachment_parameter_name pname,
        span_i32                                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFramebufferAttachmentParameteriv)
    }
    glGetFramebufferAttachmentParameteriv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetFramebufferAttachmentParameteriv"sv);
}

/* Introduced in GL core 3.0 */
template<class span_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_i32>&& std::is_same_v<
                 std::decay_t<typename span_i32::value_type>,
                 std::decay_t<i32>>) STATICINLINE
    void get_renderbuffer_parameter(
        group::renderbuffer_target         target,
        group::renderbuffer_parameter_name pname,
        span_i32                           params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetRenderbufferParameteriv)
    }
    glGetRenderbufferParameteriv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetRenderbufferParameteriv"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE GLboolean
    is_framebuffer(u32 framebuffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsFramebuffer)
        glIsFramebuffer(framebuffer);
    }
    auto out = glIsFramebuffer(framebuffer);
    detail::error_check("IsFramebuffer"sv);
    return out;
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE GLboolean
    is_renderbuffer(u32 renderbuffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsRenderbuffer)
        glIsRenderbuffer(renderbuffer);
    }
    auto out = glIsRenderbuffer(renderbuffer);
    detail::error_check("IsRenderbuffer"sv);
    return out;
}

/* Introduced in GL core 3.0 */
template<class size_2_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void renderbuffer_storage(
        group::renderbuffer_target target,
        group::internal_format     internalformat,
        size_2_i32 const&          width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RenderbufferStorage)
    }
    glRenderbufferStorage(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("RenderbufferStorage"sv);
}

/* Introduced in GL core 3.0 */
template<class size_2_i32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void renderbuffer_storage_multisample(
        group::renderbuffer_target target,
        i32                        samples,
        group::internal_format     internalformat,
        size_2_i32 const&          width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RenderbufferStorageMultisample)
    }
    glRenderbufferStorageMultisample(
        static_cast<GLenum>(target),
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("RenderbufferStorageMultisample"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void flush_mapped_buffer_range(
        group::buffer_target_arb target, GLintptr offset, GLsizeiptr length)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FlushMappedBufferRange)
    }
    glFlushMappedBufferRange(static_cast<GLenum>(target), offset, length);
    detail::error_check("FlushMappedBufferRange"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void* map_buffer_range(
        group::buffer_target_arb      target,
        GLintptr                      offset,
        GLsizeiptr                    length,
        group::map_buffer_access_mask access)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MapBufferRange)
    }
    auto out = glMapBufferRange(
        static_cast<GLenum>(target),
        offset,
        length,
        static_cast<GLenum>(access));
    detail::error_check("MapBufferRange"sv);
    return out;
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE
    void bind_vertex_array(u32 array)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindVertexArray)
        glIsVertexArray(array);
    }
    glBindVertexArray(array);
    detail::error_check("BindVertexArray"sv);
}

/* Introduced in GL core 3.0 */
template<class span_const_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_const_u32>&& std::is_same_v<
                 std::decay_t<typename span_const_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void delete_vertex_arrays(span_const_u32 const& arrays)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteVertexArrays)
    }
    glDeleteVertexArrays(
        arrays.size(),
        arrays.size() ? reinterpret_cast<const GLuint*>(arrays.data())
                      : nullptr);
    detail::error_check("DeleteVertexArrays"sv);
}

/* Introduced in GL core 3.0 */
template<class span_u32>
requires(MinimumVersion<Current, Version<3, 0>>&&
             semantic::concepts::Span<span_u32>&& std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>) STATICINLINE
    void gen_vertex_arrays(span_u32 arrays)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenVertexArrays)
    }
    glGenVertexArrays(
        arrays.size(),
        arrays.size() ? reinterpret_cast<GLuint*>(arrays.data()) : nullptr);
    detail::error_check("GenVertexArrays"sv);
}

/* Introduced in GL core 3.0 */
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 0>>) STATICINLINE GLboolean
    is_vertex_array(u32 array)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsVertexArray)
        glIsVertexArray(array);
    }
    auto out = glIsVertexArray(array);
    detail::error_check("IsVertexArray"sv);
    return out;
}

#endif // GL_VERSION_3_0