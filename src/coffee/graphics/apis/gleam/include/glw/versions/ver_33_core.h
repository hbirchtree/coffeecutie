#ifdef GL_VERSION_3_3
template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 3>>)
/*!
 * \brief Wraps around glBindFragDataLocationIndexed. Introduced in GL core 3.3
 * \param program GLuint
 * \param colorNumber GLuint
 * \param index GLuint
 * \param name const GLchar *
 * \return void
 */
STATICINLINE void bind_frag_data_location_indexed(
    u32 program, u32 colorNumber, u32 index, std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindFragDataLocationIndexed)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glBindFragDataLocationIndexed(program, colorNumber, index, name.data());
    detail::error_check("BindFragDataLocationIndexed"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 3>>)
/*!
 * \brief Wraps around glGetFragDataIndex. Introduced in GL core 3.3
 * \param program GLuint
 * \param name const GLchar *
 * \return GLint
 */
STATICINLINE GLint
    get_frag_data_index(u32 program, std::string_view const& name)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFragDataIndex)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    auto out = glGetFragDataIndex(program, name.data());
    detail::error_check("GetFragDataIndex"sv);
    return out;
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 3>>)
/*!
 * \brief Wraps around glBindSampler. Introduced in GL core 3.3
 * \param unit GLuint
 * \param sampler GLuint
 * \return void
 */
STATICINLINE void bind_sampler(u32 unit, u32 sampler)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindSampler)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
    glBindSampler(unit, sampler);
    detail::error_check("BindSampler"sv);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glDeleteSamplers. Introduced in GL core 3.3
 * \param count GLsizei
 * \param samplers const GLuint *
 * \return void
 */
STATICINLINE void delete_samplers(span_const_u32 const& samplers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteSamplers)
    }
    glDeleteSamplers(
        samplers.size(),
        samplers.size() ? reinterpret_cast<const GLuint*>(samplers.data())
                        : nullptr);
    detail::error_check("DeleteSamplers"sv);
}

template<class span_u32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glGenSamplers. Introduced in GL core 3.3
 * \param count GLsizei
 * \param samplers GLuint *
 * \return void
 */
STATICINLINE void gen_samplers(span_u32 samplers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenSamplers)
    }
    glGenSamplers(
        samplers.size(),
        samplers.size() ? reinterpret_cast<GLuint*>(samplers.data()) : nullptr);
    detail::error_check("GenSamplers"sv);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetSamplerParameterIiv. Introduced in GL core 3.3
 * \param sampler GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_sampler_parameter_iiv(
    u32 sampler, group::sampler_parameter_i pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSamplerParameterIiv)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
    glGetSamplerParameterIiv(
        sampler,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetSamplerParameterIiv"sv);
}

template<class span_u32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glGetSamplerParameterIuiv. Introduced in GL core 3.3
 * \param sampler GLuint
 * \param pname GLenum
 * \param params GLuint *
 * \return void
 */
STATICINLINE void get_sampler_parameter_iuiv(
    u32 sampler, group::sampler_parameter_i pname, span_u32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSamplerParameterIuiv)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
    glGetSamplerParameterIuiv(
        sampler,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetSamplerParameterIuiv"sv);
}

template<class span_f32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<
        std::decay_t<typename span_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Wraps around glGetSamplerParameterfv. Introduced in GL core 3.3
 * \param sampler GLuint
 * \param pname GLenum
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_sampler_parameter(
    u32 sampler, group::sampler_parameter_f pname, span_f32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSamplerParameterfv)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
    glGetSamplerParameterfv(
        sampler,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetSamplerParameterfv"sv);
}

template<class span_i32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glGetSamplerParameteriv. Introduced in GL core 3.3
 * \param sampler GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_sampler_parameter(
    u32 sampler, group::sampler_parameter_i pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSamplerParameteriv)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
    glGetSamplerParameteriv(
        sampler,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetSamplerParameteriv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 3>>)
/*!
 * \brief Wraps around glIsSampler. Introduced in GL core 3.3
 * \param sampler GLuint
 * \return Boolean
 */
STATICINLINE GLboolean is_sampler(u32 sampler)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsSampler)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
    auto out = glIsSampler(sampler);
    detail::error_check("IsSampler"sv);
    return out;
}

template<class span_const_i32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glSamplerParameterIiv. Introduced in GL core 3.3
 * \param sampler GLuint
 * \param pname GLenum
 * \param param const GLint *
 * \return void
 */
STATICINLINE void sampler_parameter_iiv(
    u32 sampler, group::sampler_parameter_i pname, span_const_i32 const& param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameterIiv)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
    glSamplerParameterIiv(
        sampler,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLint*>(param.data()) : nullptr);
    detail::error_check("SamplerParameterIiv"sv);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glSamplerParameterIuiv. Introduced in GL core 3.3
 * \param sampler GLuint
 * \param pname GLenum
 * \param param const GLuint *
 * \return void
 */
STATICINLINE void sampler_parameter_iuiv(
    u32 sampler, group::sampler_parameter_i pname, span_const_u32 const& param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameterIuiv)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
    glSamplerParameterIuiv(
        sampler,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLuint*>(param.data()) : nullptr);
    detail::error_check("SamplerParameterIuiv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 3>>)
/*!
 * \brief Wraps around glSamplerParameterf. Introduced in GL core 3.3
 * \param sampler GLuint
 * \param pname GLenum
 * \param param GLfloat
 * \return void
 */
STATICINLINE void sampler_parameter(
    u32 sampler, group::sampler_parameter_f pname, f32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameterf)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
    glSamplerParameterf(sampler, static_cast<GLenum>(pname), param);
    detail::error_check("SamplerParameterf"sv);
}

template<class span_const_f32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Wraps around glSamplerParameterfv. Introduced in GL core 3.3
 * \param sampler GLuint
 * \param pname GLenum
 * \param param const GLfloat *
 * \return void
 */
STATICINLINE void sampler_parameter(
    u32 sampler, group::sampler_parameter_f pname, span_const_f32 const& param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameterfv)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
    glSamplerParameterfv(
        sampler,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLfloat*>(param.data())
                     : nullptr);
    detail::error_check("SamplerParameterfv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 3>>)
/*!
 * \brief Wraps around glSamplerParameteri. Introduced in GL core 3.3
 * \param sampler GLuint
 * \param pname GLenum
 * \param param GLint
 * \return void
 */
STATICINLINE void sampler_parameter(
    u32 sampler, group::sampler_parameter_i pname, i32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameteri)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
    glSamplerParameteri(sampler, static_cast<GLenum>(pname), param);
    detail::error_check("SamplerParameteri"sv);
}

template<class span_const_i32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Wraps around glSamplerParameteriv. Introduced in GL core 3.3
 * \param sampler GLuint
 * \param pname GLenum
 * \param param const GLint *
 * \return void
 */
STATICINLINE void sampler_parameter(
    u32 sampler, group::sampler_parameter_i pname, span_const_i32 const& param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameteriv)
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
    glSamplerParameteriv(
        sampler,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLint*>(param.data()) : nullptr);
    detail::error_check("SamplerParameteriv"sv);
}

template<class span_i64>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_i64> &&
    std::is_same_v<
        std::decay_t<typename span_i64::value_type>,
        std::decay_t<i64>>)
/*!
 * \brief Wraps around glGetQueryObjecti64v. Introduced in GL core 3.3
 * \param id GLuint
 * \param pname GLenum
 * \param params GLint64 *
 * \return void
 */
STATICINLINE void get_query_objecti64v(
    u32 id, group::query_object_parameter_name pname, span_i64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryObjecti64v)
    }
    glGetQueryObjecti64v(
        id,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint64*>(params.data()) : nullptr);
    detail::error_check("GetQueryObjecti64v"sv);
}

template<class span_u64>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_u64> &&
    std::is_same_v<
        std::decay_t<typename span_u64::value_type>,
        std::decay_t<u64>>)
/*!
 * \brief Wraps around glGetQueryObjectui64v. Introduced in GL core 3.3
 * \param id GLuint
 * \param pname GLenum
 * \param params GLuint64 *
 * \return void
 */
STATICINLINE void get_query_objectui64v(
    u32 id, group::query_object_parameter_name pname, span_u64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryObjectui64v)
    }
    glGetQueryObjectui64v(
        id,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint64*>(params.data()) : nullptr);
    detail::error_check("GetQueryObjectui64v"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 3>>)
/*!
 * \brief Wraps around glQueryCounter. Introduced in GL core 3.3
 * \param id GLuint
 * \param target GLenum
 * \return void
 */
STATICINLINE void query_counter(u32 id, group::query_counter_target target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(QueryCounter)
    }
    glQueryCounter(id, static_cast<GLenum>(target));
    detail::error_check("QueryCounter"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 3>>)
/*!
 * \brief Wraps around glVertexAttribDivisor. Introduced in GL core 3.3
 * \param index GLuint
 * \param divisor GLuint
 * \return void
 */
STATICINLINE void vertex_attrib_divisor(u32 index, u32 divisor)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribDivisor)
    }
    glVertexAttribDivisor(index, divisor);
    detail::error_check("VertexAttribDivisor"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 3>>)
/*!
 * \brief Wraps around glVertexAttribP1ui. Introduced in GL core 3.3
 * \param index GLuint
 * \param type GLenum
 * \param normalized GLboolean
 * \param value GLuint
 * \return void
 */
STATICINLINE void vertex_attrib_p1ui(
    u32                               index,
    group::vertex_attrib_pointer_type type,
    bool                              normalized,
    u32                               value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribP1ui)
    }
    glVertexAttribP1ui(index, static_cast<GLenum>(type), normalized, value);
    detail::error_check("VertexAttribP1ui"sv);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glVertexAttribP1uiv. Introduced in GL core 3.3
 * \param index GLuint
 * \param type GLenum
 * \param normalized GLboolean
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void vertex_attrib_p1uiv(
    u32                               index,
    group::vertex_attrib_pointer_type type,
    bool                              normalized,
    span_const_u32 const&             value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribP1uiv)
    }
    glVertexAttribP1uiv(
        index,
        static_cast<GLenum>(type),
        normalized,
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("VertexAttribP1uiv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 3>>)
/*!
 * \brief Wraps around glVertexAttribP2ui. Introduced in GL core 3.3
 * \param index GLuint
 * \param type GLenum
 * \param normalized GLboolean
 * \param value GLuint
 * \return void
 */
STATICINLINE void vertex_attrib_p2ui(
    u32                               index,
    group::vertex_attrib_pointer_type type,
    bool                              normalized,
    u32                               value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribP2ui)
    }
    glVertexAttribP2ui(index, static_cast<GLenum>(type), normalized, value);
    detail::error_check("VertexAttribP2ui"sv);
}

template<class span_const_vec_2_u32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_2_u32> &&
    semantic::concepts::
        Vector<typename span_const_vec_2_u32::value_type, u32, 2>)
/*!
 * \brief Wraps around glVertexAttribP2uiv. Introduced in GL core 3.3
 * \param index GLuint
 * \param type GLenum
 * \param normalized GLboolean
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void vertex_attrib_p2uiv(
    u32                               index,
    group::vertex_attrib_pointer_type type,
    bool                              normalized,
    span_const_vec_2_u32 const&       value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribP2uiv)
    }
    glVertexAttribP2uiv(
        index,
        static_cast<GLenum>(type),
        normalized,
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("VertexAttribP2uiv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 3>>)
/*!
 * \brief Wraps around glVertexAttribP3ui. Introduced in GL core 3.3
 * \param index GLuint
 * \param type GLenum
 * \param normalized GLboolean
 * \param value GLuint
 * \return void
 */
STATICINLINE void vertex_attrib_p3ui(
    u32                               index,
    group::vertex_attrib_pointer_type type,
    bool                              normalized,
    u32                               value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribP3ui)
    }
    glVertexAttribP3ui(index, static_cast<GLenum>(type), normalized, value);
    detail::error_check("VertexAttribP3ui"sv);
}

template<class span_const_vec_3_u32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_3_u32> &&
    semantic::concepts::
        Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
/*!
 * \brief Wraps around glVertexAttribP3uiv. Introduced in GL core 3.3
 * \param index GLuint
 * \param type GLenum
 * \param normalized GLboolean
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void vertex_attrib_p3uiv(
    u32                               index,
    group::vertex_attrib_pointer_type type,
    bool                              normalized,
    span_const_vec_3_u32 const&       value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribP3uiv)
    }
    glVertexAttribP3uiv(
        index,
        static_cast<GLenum>(type),
        normalized,
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("VertexAttribP3uiv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 3>>)
/*!
 * \brief Wraps around glVertexAttribP4ui. Introduced in GL core 3.3
 * \param index GLuint
 * \param type GLenum
 * \param normalized GLboolean
 * \param value GLuint
 * \return void
 */
STATICINLINE void vertex_attrib_p4ui(
    u32                               index,
    group::vertex_attrib_pointer_type type,
    bool                              normalized,
    u32                               value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribP4ui)
    }
    glVertexAttribP4ui(index, static_cast<GLenum>(type), normalized, value);
    detail::error_check("VertexAttribP4ui"sv);
}

template<class span_const_vec_4_u32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_4_u32> &&
    semantic::concepts::
        Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
/*!
 * \brief Wraps around glVertexAttribP4uiv. Introduced in GL core 3.3
 * \param index GLuint
 * \param type GLenum
 * \param normalized GLboolean
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void vertex_attrib_p4uiv(
    u32                               index,
    group::vertex_attrib_pointer_type type,
    bool                              normalized,
    span_const_vec_4_u32 const&       value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexAttribP4uiv)
    }
    glVertexAttribP4uiv(
        index,
        static_cast<GLenum>(type),
        normalized,
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("VertexAttribP4uiv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 3>>)
/*!
 * \brief Wraps around glColorP3ui. Introduced in GL core 3.3
 * \param type GLenum
 * \param color GLuint
 * \return void
 */
STATICINLINE void color_p3ui(group::color_pointer_type type, u32 color)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ColorP3ui)
    }
    glColorP3ui(static_cast<GLenum>(type), color);
    detail::error_check("ColorP3ui"sv);
}

template<class span_const_vec_3_u32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_3_u32> &&
    semantic::concepts::
        Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
/*!
 * \brief Wraps around glColorP3uiv. Introduced in GL core 3.3
 * \param type GLenum
 * \param color const GLuint *
 * \return void
 */
STATICINLINE void color_p3uiv(
    group::color_pointer_type type, span_const_vec_3_u32 const& color)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ColorP3uiv)
    }
    glColorP3uiv(
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(color.data()));
    detail::error_check("ColorP3uiv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 3>>)
/*!
 * \brief Wraps around glColorP4ui. Introduced in GL core 3.3
 * \param type GLenum
 * \param color GLuint
 * \return void
 */
STATICINLINE void color_p4ui(group::color_pointer_type type, u32 color)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ColorP4ui)
    }
    glColorP4ui(static_cast<GLenum>(type), color);
    detail::error_check("ColorP4ui"sv);
}

template<class span_const_vec_4_u32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_4_u32> &&
    semantic::concepts::
        Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
/*!
 * \brief Wraps around glColorP4uiv. Introduced in GL core 3.3
 * \param type GLenum
 * \param color const GLuint *
 * \return void
 */
STATICINLINE void color_p4uiv(
    group::color_pointer_type type, span_const_vec_4_u32 const& color)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ColorP4uiv)
    }
    glColorP4uiv(
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(color.data()));
    detail::error_check("ColorP4uiv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 3>>)
/*!
 * \brief Wraps around glMultiTexCoordP1ui. Introduced in GL core 3.3
 * \param texture GLenum
 * \param type GLenum
 * \param coords GLuint
 * \return void
 */
STATICINLINE void multi_tex_coord_p1ui(
    group::texture_unit texture, group::tex_coord_pointer_type type, u32 coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoordP1ui)
    }
    glMultiTexCoordP1ui(
        static_cast<GLenum>(texture), static_cast<GLenum>(type), coords);
    detail::error_check("MultiTexCoordP1ui"sv);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glMultiTexCoordP1uiv. Introduced in GL core 3.3
 * \param texture GLenum
 * \param type GLenum
 * \param coords const GLuint *
 * \return void
 */
STATICINLINE void multi_tex_coord_p1uiv(
    group::texture_unit           texture,
    group::tex_coord_pointer_type type,
    span_const_u32 const&         coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoordP1uiv)
    }
    glMultiTexCoordP1uiv(
        static_cast<GLenum>(texture),
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(coords.data()));
    detail::error_check("MultiTexCoordP1uiv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 3>>)
/*!
 * \brief Wraps around glMultiTexCoordP2ui. Introduced in GL core 3.3
 * \param texture GLenum
 * \param type GLenum
 * \param coords GLuint
 * \return void
 */
STATICINLINE void multi_tex_coord_p2ui(
    group::texture_unit texture, group::tex_coord_pointer_type type, u32 coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoordP2ui)
    }
    glMultiTexCoordP2ui(
        static_cast<GLenum>(texture), static_cast<GLenum>(type), coords);
    detail::error_check("MultiTexCoordP2ui"sv);
}

template<class span_const_vec_2_u32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_2_u32> &&
    semantic::concepts::
        Vector<typename span_const_vec_2_u32::value_type, u32, 2>)
/*!
 * \brief Wraps around glMultiTexCoordP2uiv. Introduced in GL core 3.3
 * \param texture GLenum
 * \param type GLenum
 * \param coords const GLuint *
 * \return void
 */
STATICINLINE void multi_tex_coord_p2uiv(
    group::texture_unit           texture,
    group::tex_coord_pointer_type type,
    span_const_vec_2_u32 const&   coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoordP2uiv)
    }
    glMultiTexCoordP2uiv(
        static_cast<GLenum>(texture),
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(coords.data()));
    detail::error_check("MultiTexCoordP2uiv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 3>>)
/*!
 * \brief Wraps around glMultiTexCoordP3ui. Introduced in GL core 3.3
 * \param texture GLenum
 * \param type GLenum
 * \param coords GLuint
 * \return void
 */
STATICINLINE void multi_tex_coord_p3ui(
    group::texture_unit texture, group::tex_coord_pointer_type type, u32 coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoordP3ui)
    }
    glMultiTexCoordP3ui(
        static_cast<GLenum>(texture), static_cast<GLenum>(type), coords);
    detail::error_check("MultiTexCoordP3ui"sv);
}

template<class span_const_vec_3_u32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_3_u32> &&
    semantic::concepts::
        Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
/*!
 * \brief Wraps around glMultiTexCoordP3uiv. Introduced in GL core 3.3
 * \param texture GLenum
 * \param type GLenum
 * \param coords const GLuint *
 * \return void
 */
STATICINLINE void multi_tex_coord_p3uiv(
    group::texture_unit           texture,
    group::tex_coord_pointer_type type,
    span_const_vec_3_u32 const&   coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoordP3uiv)
    }
    glMultiTexCoordP3uiv(
        static_cast<GLenum>(texture),
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(coords.data()));
    detail::error_check("MultiTexCoordP3uiv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 3>>)
/*!
 * \brief Wraps around glMultiTexCoordP4ui. Introduced in GL core 3.3
 * \param texture GLenum
 * \param type GLenum
 * \param coords GLuint
 * \return void
 */
STATICINLINE void multi_tex_coord_p4ui(
    group::texture_unit texture, group::tex_coord_pointer_type type, u32 coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoordP4ui)
    }
    glMultiTexCoordP4ui(
        static_cast<GLenum>(texture), static_cast<GLenum>(type), coords);
    detail::error_check("MultiTexCoordP4ui"sv);
}

template<class span_const_vec_4_u32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_4_u32> &&
    semantic::concepts::
        Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
/*!
 * \brief Wraps around glMultiTexCoordP4uiv. Introduced in GL core 3.3
 * \param texture GLenum
 * \param type GLenum
 * \param coords const GLuint *
 * \return void
 */
STATICINLINE void multi_tex_coord_p4uiv(
    group::texture_unit           texture,
    group::tex_coord_pointer_type type,
    span_const_vec_4_u32 const&   coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MultiTexCoordP4uiv)
    }
    glMultiTexCoordP4uiv(
        static_cast<GLenum>(texture),
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(coords.data()));
    detail::error_check("MultiTexCoordP4uiv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 3>>)
/*!
 * \brief Wraps around glNormalP3ui. Introduced in GL core 3.3
 * \param type GLenum
 * \param coords GLuint
 * \return void
 */
STATICINLINE void normal_p3ui(group::normal_pointer_type type, u32 coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NormalP3ui)
    }
    glNormalP3ui(static_cast<GLenum>(type), coords);
    detail::error_check("NormalP3ui"sv);
}

template<class span_const_vec_3_u32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_3_u32> &&
    semantic::concepts::
        Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
/*!
 * \brief Wraps around glNormalP3uiv. Introduced in GL core 3.3
 * \param type GLenum
 * \param coords const GLuint *
 * \return void
 */
STATICINLINE void normal_p3uiv(
    group::normal_pointer_type type, span_const_vec_3_u32 const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NormalP3uiv)
    }
    glNormalP3uiv(
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(coords.data()));
    detail::error_check("NormalP3uiv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 3>>)
/*!
 * \brief Wraps around glSecondaryColorP3ui. Introduced in GL core 3.3
 * \param type GLenum
 * \param color GLuint
 * \return void
 */
STATICINLINE
    void secondary_color_p3ui(group::color_pointer_type type, u32 color)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SecondaryColorP3ui)
    }
    glSecondaryColorP3ui(static_cast<GLenum>(type), color);
    detail::error_check("SecondaryColorP3ui"sv);
}

template<class span_const_vec_3_u32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_3_u32> &&
    semantic::concepts::
        Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
/*!
 * \brief Wraps around glSecondaryColorP3uiv. Introduced in GL core 3.3
 * \param type GLenum
 * \param color const GLuint *
 * \return void
 */
STATICINLINE void secondary_color_p3uiv(
    group::color_pointer_type type, span_const_vec_3_u32 const& color)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SecondaryColorP3uiv)
    }
    glSecondaryColorP3uiv(
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(color.data()));
    detail::error_check("SecondaryColorP3uiv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 3>>)
/*!
 * \brief Wraps around glTexCoordP1ui. Introduced in GL core 3.3
 * \param type GLenum
 * \param coords GLuint
 * \return void
 */
STATICINLINE void tex_coord_p1ui(group::tex_coord_pointer_type type, u32 coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoordP1ui)
    }
    glTexCoordP1ui(static_cast<GLenum>(type), coords);
    detail::error_check("TexCoordP1ui"sv);
}

template<class span_const_u32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Wraps around glTexCoordP1uiv. Introduced in GL core 3.3
 * \param type GLenum
 * \param coords const GLuint *
 * \return void
 */
STATICINLINE void tex_coord_p1uiv(
    group::tex_coord_pointer_type type, span_const_u32 const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoordP1uiv)
    }
    glTexCoordP1uiv(
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(coords.data()));
    detail::error_check("TexCoordP1uiv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 3>>)
/*!
 * \brief Wraps around glTexCoordP2ui. Introduced in GL core 3.3
 * \param type GLenum
 * \param coords GLuint
 * \return void
 */
STATICINLINE void tex_coord_p2ui(group::tex_coord_pointer_type type, u32 coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoordP2ui)
    }
    glTexCoordP2ui(static_cast<GLenum>(type), coords);
    detail::error_check("TexCoordP2ui"sv);
}

template<class span_const_vec_2_u32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_2_u32> &&
    semantic::concepts::
        Vector<typename span_const_vec_2_u32::value_type, u32, 2>)
/*!
 * \brief Wraps around glTexCoordP2uiv. Introduced in GL core 3.3
 * \param type GLenum
 * \param coords const GLuint *
 * \return void
 */
STATICINLINE void tex_coord_p2uiv(
    group::tex_coord_pointer_type type, span_const_vec_2_u32 const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoordP2uiv)
    }
    glTexCoordP2uiv(
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(coords.data()));
    detail::error_check("TexCoordP2uiv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 3>>)
/*!
 * \brief Wraps around glTexCoordP3ui. Introduced in GL core 3.3
 * \param type GLenum
 * \param coords GLuint
 * \return void
 */
STATICINLINE void tex_coord_p3ui(group::tex_coord_pointer_type type, u32 coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoordP3ui)
    }
    glTexCoordP3ui(static_cast<GLenum>(type), coords);
    detail::error_check("TexCoordP3ui"sv);
}

template<class span_const_vec_3_u32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_3_u32> &&
    semantic::concepts::
        Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
/*!
 * \brief Wraps around glTexCoordP3uiv. Introduced in GL core 3.3
 * \param type GLenum
 * \param coords const GLuint *
 * \return void
 */
STATICINLINE void tex_coord_p3uiv(
    group::tex_coord_pointer_type type, span_const_vec_3_u32 const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoordP3uiv)
    }
    glTexCoordP3uiv(
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(coords.data()));
    detail::error_check("TexCoordP3uiv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 3>>)
/*!
 * \brief Wraps around glTexCoordP4ui. Introduced in GL core 3.3
 * \param type GLenum
 * \param coords GLuint
 * \return void
 */
STATICINLINE void tex_coord_p4ui(group::tex_coord_pointer_type type, u32 coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoordP4ui)
    }
    glTexCoordP4ui(static_cast<GLenum>(type), coords);
    detail::error_check("TexCoordP4ui"sv);
}

template<class span_const_vec_4_u32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_4_u32> &&
    semantic::concepts::
        Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
/*!
 * \brief Wraps around glTexCoordP4uiv. Introduced in GL core 3.3
 * \param type GLenum
 * \param coords const GLuint *
 * \return void
 */
STATICINLINE void tex_coord_p4uiv(
    group::tex_coord_pointer_type type, span_const_vec_4_u32 const& coords)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexCoordP4uiv)
    }
    glTexCoordP4uiv(
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(coords.data()));
    detail::error_check("TexCoordP4uiv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 3>>)
/*!
 * \brief Wraps around glVertexP2ui. Introduced in GL core 3.3
 * \param type GLenum
 * \param value GLuint
 * \return void
 */
STATICINLINE void vertex_p2ui(group::vertex_pointer_type type, u32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexP2ui)
    }
    glVertexP2ui(static_cast<GLenum>(type), value);
    detail::error_check("VertexP2ui"sv);
}

template<class span_const_vec_2_u32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_2_u32> &&
    semantic::concepts::
        Vector<typename span_const_vec_2_u32::value_type, u32, 2>)
/*!
 * \brief Wraps around glVertexP2uiv. Introduced in GL core 3.3
 * \param type GLenum
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void vertex_p2uiv(
    group::vertex_pointer_type type, span_const_vec_2_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexP2uiv)
    }
    glVertexP2uiv(
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("VertexP2uiv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 3>>)
/*!
 * \brief Wraps around glVertexP3ui. Introduced in GL core 3.3
 * \param type GLenum
 * \param value GLuint
 * \return void
 */
STATICINLINE void vertex_p3ui(group::vertex_pointer_type type, u32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexP3ui)
    }
    glVertexP3ui(static_cast<GLenum>(type), value);
    detail::error_check("VertexP3ui"sv);
}

template<class span_const_vec_3_u32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_3_u32> &&
    semantic::concepts::
        Vector<typename span_const_vec_3_u32::value_type, u32, 3>)
/*!
 * \brief Wraps around glVertexP3uiv. Introduced in GL core 3.3
 * \param type GLenum
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void vertex_p3uiv(
    group::vertex_pointer_type type, span_const_vec_3_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexP3uiv)
    }
    glVertexP3uiv(
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("VertexP3uiv"sv);
}

template<typename Dummy = void>
requires(MinimumVersion<Current, Version<3, 3>>)
/*!
 * \brief Wraps around glVertexP4ui. Introduced in GL core 3.3
 * \param type GLenum
 * \param value GLuint
 * \return void
 */
STATICINLINE void vertex_p4ui(group::vertex_pointer_type type, u32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexP4ui)
    }
    glVertexP4ui(static_cast<GLenum>(type), value);
    detail::error_check("VertexP4ui"sv);
}

template<class span_const_vec_4_u32>
requires(
    MinimumVersion<Current, Version<3, 3>> &&
    semantic::concepts::Span<span_const_vec_4_u32> &&
    semantic::concepts::
        Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
/*!
 * \brief Wraps around glVertexP4uiv. Introduced in GL core 3.3
 * \param type GLenum
 * \param value const GLuint *
 * \return void
 */
STATICINLINE void vertex_p4uiv(
    group::vertex_pointer_type type, span_const_vec_4_u32 const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(VertexP4uiv)
    }
    glVertexP4uiv(
        static_cast<GLenum>(type),
        reinterpret_cast<const GLuint*>(value.data()));
    detail::error_check("VertexP4uiv"sv);
}

#endif // GL_VERSION_3_3
