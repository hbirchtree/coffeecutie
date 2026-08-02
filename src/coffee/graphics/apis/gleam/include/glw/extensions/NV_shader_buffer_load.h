#pragma once

#ifdef GL_NV_shader_buffer_load
namespace gl::nv::shader_buffer_load {
namespace values {
constexpr u32 buffer_gpu_address        = 0x8F1D;
constexpr u32 gpu_address               = 0x8F34;
constexpr u32 max_shader_buffer_address = 0x8F35;
} // namespace values

template<class span_GLuint64EXT>
requires(
    concepts::span<span_GLuint64EXT> &&
    std::is_same_v<
        std::decay_t<typename span_GLuint64EXT::value_type>,
        std::decay_t<GLuint64EXT>>)
/*!
 * \brief Part of GL_NV_shader_buffer_load
 * \param target GLenum
 * \param pname GLenum
 * \param params GLuint64EXT *
 * \return void
 */
STATICINLINE void get_buffer_parameterui64v(
    group::buffer_target_arb target,
    GLenum                   pname,
    span_GLuint64EXT         params,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetBufferParameterui64vNV)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetBufferParameterui64vNV,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glGetBufferParameterui64vNV(
        static_cast<GLenum>(target),
        pname,
        params.size() ? reinterpret_cast<GLuint64EXT*>(params.data())
                      : nullptr);
    detail::error_check("GetBufferParameterui64vNV"sv, check_errors);
}

template<class span_GLuint64EXT>
requires(
    concepts::span<span_GLuint64EXT> &&
    std::is_same_v<
        std::decay_t<typename span_GLuint64EXT::value_type>,
        std::decay_t<GLuint64EXT>>)
/*!
 * \brief Part of GL_NV_shader_buffer_load
 * \param value GLenum
 * \param result GLuint64EXT *
 * \return void
 */
STATICINLINE void get_integerui64v(
    GLenum           value,
    span_GLuint64EXT result,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetIntegerui64vNV)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetIntegerui64vNV,
        gsl::span<char>(
            reinterpret_cast<char*>(result.data()), result.size_bytes()),
        value);
#endif
    glGetIntegerui64vNV(
        value,
        result.size() ? reinterpret_cast<GLuint64EXT*>(result.data())
                      : nullptr);
    detail::error_check("GetIntegerui64vNV"sv, check_errors);
}

template<class span_GLuint64EXT>
requires(
    concepts::span<span_GLuint64EXT> &&
    std::is_same_v<
        std::decay_t<typename span_GLuint64EXT::value_type>,
        std::decay_t<GLuint64EXT>>)
/*!
 * \brief Part of GL_NV_shader_buffer_load
 * \param buffer GLuint
 * \param pname GLenum
 * \param params GLuint64EXT *
 * \return void
 */
STATICINLINE void get_named_buffer_parameterui64v(
    u32                    buffer,
    group::buffer_prop_arb pname,
    span_GLuint64EXT       params,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetNamedBufferParameterui64vNV)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetNamedBufferParameterui64vNV,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        buffer,
        pname);
#endif
    glGetNamedBufferParameterui64vNV(
        buffer,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint64EXT*>(params.data())
                      : nullptr);
    detail::error_check("GetNamedBufferParameterui64vNV"sv, check_errors);
}

template<class span_GLuint64EXT>
requires(
    concepts::span<span_GLuint64EXT> &&
    std::is_same_v<
        std::decay_t<typename span_GLuint64EXT::value_type>,
        std::decay_t<GLuint64EXT>>)
/*!
 * \brief Part of GL_NV_shader_buffer_load
 * \param program GLuint
 * \param location GLint
 * \param params GLuint64EXT *
 * \return void
 */
STATICINLINE void get_uniformui64v(
    u32              program,
    i32              location,
    span_GLuint64EXT params,
    error_check      check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetUniformui64vNV)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetUniformui64vNV,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        program,
        location);
#endif
    glGetUniformui64vNV(
        program,
        location,
        params.size() ? reinterpret_cast<GLuint64EXT*>(params.data())
                      : nullptr);
    detail::error_check("GetUniformui64vNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_shader_buffer_load
 * \param target GLenum
 * \return GLboolean
 */
STATICINLINE GLboolean
is_buffer_resident(GLenum target, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(IsBufferResidentNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glIsBufferResidentNV, target);
#endif
    auto out = glIsBufferResidentNV(target);
    detail::error_check("IsBufferResidentNV"sv, check_errors);
    return out;
}

/*!
 * \brief Part of GL_NV_shader_buffer_load
 * \param buffer GLuint
 * \return GLboolean
 */
STATICINLINE GLboolean
is_named_buffer_resident(u32 buffer, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(IsNamedBufferResidentNV)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glIsNamedBufferResidentNV, buffer);
#endif
    auto out = glIsNamedBufferResidentNV(buffer);
    detail::error_check("IsNamedBufferResidentNV"sv, check_errors);
    return out;
}

/*!
 * \brief Part of GL_NV_shader_buffer_load
 * \param target GLenum
 * \return void
 */
STATICINLINE void make_buffer_non_resident(
    GLenum target, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(MakeBufferNonResidentNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glMakeBufferNonResidentNV, target);
#endif
    glMakeBufferNonResidentNV(target);
    detail::error_check("MakeBufferNonResidentNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_shader_buffer_load
 * \param target GLenum
 * \param access GLenum
 * \return void
 */
STATICINLINE void make_buffer_resident(
    GLenum target, GLenum access, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(MakeBufferResidentNV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glMakeBufferResidentNV, target, access);
#endif
    glMakeBufferResidentNV(target, access);
    detail::error_check("MakeBufferResidentNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_shader_buffer_load
 * \param buffer GLuint
 * \return void
 */
STATICINLINE void make_named_buffer_non_resident(
    u32 buffer, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(MakeNamedBufferNonResidentNV)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glMakeNamedBufferNonResidentNV, buffer);
#endif
    glMakeNamedBufferNonResidentNV(buffer);
    detail::error_check("MakeNamedBufferNonResidentNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_shader_buffer_load
 * \param buffer GLuint
 * \param access GLenum
 * \return void
 */
STATICINLINE void make_named_buffer_resident(
    u32 buffer, GLenum access, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(MakeNamedBufferResidentNV)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glMakeNamedBufferResidentNV, buffer, access);
#endif
    glMakeNamedBufferResidentNV(buffer, access);
    detail::error_check("MakeNamedBufferResidentNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_shader_buffer_load
 * \param program GLuint
 * \param location GLint
 * \param value GLuint64EXT
 * \return void
 */
STATICINLINE void program_uniform(
    u32         program,
    i32         location,
    GLuint64EXT value,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniformui64NV)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glProgramUniformui64NV, program, location, value);
#endif
    glProgramUniformui64NV(program, location, value);
    detail::error_check("ProgramUniformui64NV"sv, check_errors);
}

template<class span_const_GLuint64EXT>
requires(
    concepts::span<span_const_GLuint64EXT> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLuint64EXT::value_type>,
        std::decay_t<GLuint64EXT>>)
/*!
 * \brief Part of GL_NV_shader_buffer_load
 * \param program GLuint
 * \param location GLint
 * \param count GLsizei
 * \param value const GLuint64EXT *
 * \return void
 */
STATICINLINE void program_uniform(
    u32                           program,
    i32                           location,
    span_const_GLuint64EXT const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ProgramUniformui64vNV)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glProgramUniformui64vNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        program,
        location);
#endif
    glProgramUniformui64vNV(
        program,
        location,
        value.size(),
        value.size() ? reinterpret_cast<const GLuint64EXT*>(value.data())
                     : nullptr);
    detail::error_check("ProgramUniformui64vNV"sv, check_errors);
}

/*!
 * \brief Part of GL_NV_shader_buffer_load
 * \param location GLint
 * \param value GLuint64EXT
 * \return void
 */
STATICINLINE void uniform(
    i32 location, GLuint64EXT value, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(Uniformui64NV)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glUniformui64NV, location, value);
#endif
    glUniformui64NV(location, value);
    detail::error_check("Uniformui64NV"sv, check_errors);
}

template<class span_const_GLuint64EXT>
requires(
    concepts::span<span_const_GLuint64EXT> &&
    std::is_same_v<
        std::decay_t<typename span_const_GLuint64EXT::value_type>,
        std::decay_t<GLuint64EXT>>)
/*!
 * \brief Part of GL_NV_shader_buffer_load
 * \param location GLint
 * \param count GLsizei
 * \param value const GLuint64EXT *
 * \return void
 */
STATICINLINE void uniform(
    i32                           location,
    i32                           count,
    span_const_GLuint64EXT const& value,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(Uniformui64vNV)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glUniformui64vNV,
        gsl::span<const char>(
            reinterpret_cast<const char*>(value.data()), value.size_bytes()),
        location,
        count);
#endif
    glUniformui64vNV(
        location,
        count,
        value.size() ? reinterpret_cast<const GLuint64EXT*>(value.data())
                     : nullptr);
    detail::error_check("Uniformui64vNV"sv, check_errors);
}

} // namespace gl::nv::shader_buffer_load
#endif // GL_NV_shader_buffer_load
namespace gl::nv::shader_buffer_load {
constexpr auto name = "GL_NV_shader_buffer_load";
}
