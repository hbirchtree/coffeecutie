#pragma once

#ifdef GL_QCOM_extended_get2
namespace gl::qcom::extended_get2 {
namespace values {
} // namespace values

template<class span_GLchar, class span_i32>
requires(
    concepts::span<span_GLchar> &&
    std::is_same_v<
        std::decay_t<typename span_GLchar::value_type>,
        std::decay_t<GLchar>> &&
    concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_QCOM_extended_get2
 * \param program GLuint
 * \param shadertype GLenum
 * \param source GLchar *
 * \param length GLint *
 * \return void
 */
STATICINLINE void ext_get_program_binary_source(
    u32                program,
    group::shader_type shadertype,
    span_GLchar        source,
    span_i32           length,
    error_check        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ExtGetProgramBinarySourceQCOM)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glExtGetProgramBinarySourceQCOM(
        program,
        static_cast<GLenum>(shadertype),
        source.data(),
        length.size() ? reinterpret_cast<GLint*>(length.data()) : nullptr);
    detail::error_check("ExtGetProgramBinarySourceQCOM"sv, check_errors);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_QCOM_extended_get2
 * \param programs GLuint *
 * \param maxPrograms GLint
 * \param numPrograms GLint *
 * \return void
 */
STATICINLINE void ext_get_programs(
    span_u32    programs,
    i32         maxPrograms,
    i32&        numPrograms,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ExtGetProgramsQCOM)
    }
    glExtGetProgramsQCOM(
        programs.size() ? reinterpret_cast<GLuint*>(programs.data()) : nullptr,
        maxPrograms,
        &numPrograms);
    detail::error_check("ExtGetProgramsQCOM"sv, check_errors);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_QCOM_extended_get2
 * \param shaders GLuint *
 * \param maxShaders GLint
 * \param numShaders GLint *
 * \return void
 */
STATICINLINE void ext_get_shaders(
    span_u32    shaders,
    i32         maxShaders,
    i32&        numShaders,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ExtGetShadersQCOM)
    }
    glExtGetShadersQCOM(
        shaders.size() ? reinterpret_cast<GLuint*>(shaders.data()) : nullptr,
        maxShaders,
        &numShaders);
    detail::error_check("ExtGetShadersQCOM"sv, check_errors);
}

/*!
 * \brief Part of GL_QCOM_extended_get2
 * \param program GLuint
 * \return Boolean
 */
STATICINLINE bool ext_is_program_binary(
    u32 program, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ExtIsProgramBinaryQCOM)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    auto out = glExtIsProgramBinaryQCOM(program);
    detail::error_check("ExtIsProgramBinaryQCOM"sv, check_errors);
    return out == GL_TRUE ? true : false;
}

} // namespace gl::qcom::extended_get2
#endif // GL_QCOM_extended_get2
namespace gl::qcom::extended_get2 {
constexpr auto name = "GL_QCOM_extended_get2";
}
