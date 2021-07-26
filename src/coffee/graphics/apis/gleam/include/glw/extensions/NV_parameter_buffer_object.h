#pragma once

#ifdef GL_NV_parameter_buffer_object
namespace gl::nv::parameter_buffer_object {
namespace values {
constexpr libc_types::u32 max_program_parameter_buffer_bindings = 0x8DA0;
constexpr libc_types::u32 max_program_parameter_buffer_size     = 0x8DA1;
constexpr libc_types::u32 vertex_program_parameter_buffer       = 0x8DA2;
constexpr libc_types::u32 geometry_program_parameter_buffer     = 0x8DA3;
constexpr libc_types::u32 fragment_program_parameter_buffer     = 0x8DA4;
} // namespace values
template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void program_buffer_parameters_iiv(
        group::program_target target,
        u32                   bindingIndex,
        u32                   wordIndex,
        span_const_i32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramBufferParametersIivNV)
    }
    glProgramBufferParametersIivNV(
        static_cast<GLenum>(target),
        bindingIndex,
        wordIndex,
        params.size(),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("ProgramBufferParametersIivNV"sv);
}

template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void program_buffer_parameters_iuiv(
        group::program_target target,
        u32                   bindingIndex,
        u32                   wordIndex,
        span_const_u32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramBufferParametersIuivNV)
    }
    glProgramBufferParametersIuivNV(
        static_cast<GLenum>(target),
        bindingIndex,
        wordIndex,
        params.size(),
        params.size() ? reinterpret_cast<const GLuint*>(params.data())
                      : nullptr);
    detail::error_check("ProgramBufferParametersIuivNV"sv);
}

template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void program_buffer_parameter(
        group::program_target target,
        u32                   bindingIndex,
        u32                   wordIndex,
        span_const_f32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramBufferParametersfvNV)
    }
    glProgramBufferParametersfvNV(
        static_cast<GLenum>(target),
        bindingIndex,
        wordIndex,
        params.size(),
        params.size() ? reinterpret_cast<const GLfloat*>(params.data())
                      : nullptr);
    detail::error_check("ProgramBufferParametersfvNV"sv);
}

} // namespace gl::nv::parameter_buffer_object
#endif // GL_NV_parameter_buffer_object
namespace gl::nv::parameter_buffer_object {
constexpr auto name = "GL_NV_parameter_buffer_object";
}
