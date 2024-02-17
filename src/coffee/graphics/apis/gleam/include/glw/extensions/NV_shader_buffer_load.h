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
    group::buffer_target_arb target, GLenum pname, span_GLuint64EXT params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetBufferParameterui64vNV)
    }
    glGetBufferParameterui64vNV(
        static_cast<GLenum>(target),
        pname,
        params.size() ? reinterpret_cast<GLuint64EXT*>(params.data())
                      : nullptr);
    detail::error_check("GetBufferParameterui64vNV"sv);
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
STATICINLINE void get_integerui64v(GLenum value, span_GLuint64EXT result)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetIntegerui64vNV)
    }
    glGetIntegerui64vNV(
        value,
        result.size() ? reinterpret_cast<GLuint64EXT*>(result.data())
                      : nullptr);
    detail::error_check("GetIntegerui64vNV"sv);
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
    u32 buffer, group::buffer_prop_arb pname, span_GLuint64EXT params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedBufferParameterui64vNV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glGetNamedBufferParameterui64vNV(
        buffer,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint64EXT*>(params.data())
                      : nullptr);
    detail::error_check("GetNamedBufferParameterui64vNV"sv);
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
    u32 program, i32 location, span_GLuint64EXT params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetUniformui64vNV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetUniformui64vNV(
        program,
        location,
        params.size() ? reinterpret_cast<GLuint64EXT*>(params.data())
                      : nullptr);
    detail::error_check("GetUniformui64vNV"sv);
}

/*!
 * \brief Part of GL_NV_shader_buffer_load
 * \param target GLenum
 * \return Boolean
 */
STATICINLINE bool is_buffer_resident(GLenum target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsBufferResidentNV)
    }
    auto out = glIsBufferResidentNV(target);
    detail::error_check("IsBufferResidentNV"sv);
    return out == GL_TRUE ? true : false;
}

/*!
 * \brief Part of GL_NV_shader_buffer_load
 * \param buffer GLuint
 * \return Boolean
 */
STATICINLINE bool is_named_buffer_resident(u32 buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsNamedBufferResidentNV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    auto out = glIsNamedBufferResidentNV(buffer);
    detail::error_check("IsNamedBufferResidentNV"sv);
    return out == GL_TRUE ? true : false;
}

/*!
 * \brief Part of GL_NV_shader_buffer_load
 * \param target GLenum
 * \return void
 */
STATICINLINE void make_buffer_non_resident(GLenum target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MakeBufferNonResidentNV)
    }
    glMakeBufferNonResidentNV(target);
    detail::error_check("MakeBufferNonResidentNV"sv);
}

/*!
 * \brief Part of GL_NV_shader_buffer_load
 * \param target GLenum
 * \param access GLenum
 * \return void
 */
STATICINLINE void make_buffer_resident(GLenum target, GLenum access)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MakeBufferResidentNV)
    }
    glMakeBufferResidentNV(target, access);
    detail::error_check("MakeBufferResidentNV"sv);
}

/*!
 * \brief Part of GL_NV_shader_buffer_load
 * \param buffer GLuint
 * \return void
 */
STATICINLINE void make_named_buffer_non_resident(u32 buffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MakeNamedBufferNonResidentNV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glMakeNamedBufferNonResidentNV(buffer);
    detail::error_check("MakeNamedBufferNonResidentNV"sv);
}

/*!
 * \brief Part of GL_NV_shader_buffer_load
 * \param buffer GLuint
 * \param access GLenum
 * \return void
 */
STATICINLINE void make_named_buffer_resident(u32 buffer, GLenum access)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(MakeNamedBufferResidentNV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsBuffer)
            glIsBuffer(buffer);
#endif
    }
    glMakeNamedBufferResidentNV(buffer, access);
    detail::error_check("MakeNamedBufferResidentNV"sv);
}

/*!
 * \brief Part of GL_NV_shader_buffer_load
 * \param program GLuint
 * \param location GLint
 * \param value GLuint64EXT
 * \return void
 */
STATICINLINE void program_uniform(u32 program, i32 location, GLuint64EXT value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformui64NV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniformui64NV(program, location, value);
    detail::error_check("ProgramUniformui64NV"sv);
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
    u32 program, i32 location, span_const_GLuint64EXT const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramUniformui64vNV)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramUniformui64vNV(
        program,
        location,
        value.size(),
        value.size() ? reinterpret_cast<const GLuint64EXT*>(value.data())
                     : nullptr);
    detail::error_check("ProgramUniformui64vNV"sv);
}

/*!
 * \brief Part of GL_NV_shader_buffer_load
 * \param location GLint
 * \param value GLuint64EXT
 * \return void
 */
STATICINLINE void uniform(i32 location, GLuint64EXT value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniformui64NV)
    }
    glUniformui64NV(location, value);
    detail::error_check("Uniformui64NV"sv);
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
    i32 location, i32 count, span_const_GLuint64EXT const& value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(Uniformui64vNV)
    }
    glUniformui64vNV(
        location,
        count,
        value.size() ? reinterpret_cast<const GLuint64EXT*>(value.data())
                     : nullptr);
    detail::error_check("Uniformui64vNV"sv);
}

} // namespace gl::nv::shader_buffer_load
#endif // GL_NV_shader_buffer_load
namespace gl::nv::shader_buffer_load {
constexpr auto name = "GL_NV_shader_buffer_load";
}
