#pragma once

#ifdef GL_NV_gpu_program5
namespace gl::nv::gpu_program5 {
namespace values {
constexpr libc_types::u32 max_geometry_program_invocations           = 0x8E5A;
constexpr libc_types::u32 min_fragment_interpolation_offset          = 0x8E5B;
constexpr libc_types::u32 max_fragment_interpolation_offset          = 0x8E5C;
constexpr libc_types::u32 fragment_program_interpolation_offset_bits = 0x8E5D;
constexpr libc_types::u32 min_program_texture_gather_offset          = 0x8E5E;
constexpr libc_types::u32 max_program_texture_gather_offset          = 0x8E5F;
constexpr libc_types::u32 max_program_subroutine_parameters          = 0x8F44;
constexpr libc_types::u32 max_program_subroutine_num                 = 0x8F45;
} // namespace values
template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void get_program_subroutine_parameter(
        GLenum target, u32 index, span_u32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramSubroutineParameteruivNV)
    }
    glGetProgramSubroutineParameteruivNV(
        target,
        index,
        param.size() ? reinterpret_cast<GLuint*>(param.data()) : nullptr);
    detail::error_check("GetProgramSubroutineParameteruivNV"sv);
}

template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void program_subroutine_parameters(
        GLenum target, span_const_u32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramSubroutineParametersuivNV)
    }
    glProgramSubroutineParametersuivNV(
        target,
        params.size(),
        params.size() ? reinterpret_cast<const GLuint*>(params.data())
                      : nullptr);
    detail::error_check("ProgramSubroutineParametersuivNV"sv);
}

} // namespace gl::nv::gpu_program5
#endif // GL_NV_gpu_program5
namespace gl::nv::gpu_program5 {
constexpr auto name = "GL_NV_gpu_program5";
}
