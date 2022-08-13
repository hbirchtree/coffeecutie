#pragma once

#ifdef GL_QCOM_extended_get2
namespace gl::qcom::extended_get2 {
namespace values {
} // namespace values
template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void ext_get_program_binary_source(
        u32 program, group::shader_type shadertype, span_i32 length)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ExtGetProgramBinarySourceQCOM)
        glIsProgram(program);
    }
    glExtGetProgramBinarySourceQCOM(
        program,
        static_cast<GLenum>(shadertype),
        length.size() ? reinterpret_cast<GLint*>(length.data()) : nullptr);
    detail::error_check("ExtGetProgramBinarySourceQCOM"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void ext_get_programs(span_u32 programs, i32 maxPrograms, i32& numPrograms)
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
    detail::error_check("ExtGetProgramsQCOM"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void ext_get_shaders(span_u32 shaders, i32 maxShaders, i32& numShaders)
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
    detail::error_check("ExtGetShadersQCOM"sv);
}

STATICINLINE GLboolean ext_is_program_binary(u32 program)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ExtIsProgramBinaryQCOM)
        glIsProgram(program);
    }
    auto out = glExtIsProgramBinaryQCOM(program);
    detail::error_check("ExtIsProgramBinaryQCOM"sv);
    return out;
}

} // namespace gl::qcom::extended_get2
#endif // GL_QCOM_extended_get2
namespace gl::qcom::extended_get2 {
constexpr auto name = "GL_QCOM_extended_get2";
}