#pragma once

#ifdef GL_NV_register_combiners2
namespace gl::nv::register_combiners2 {
namespace values {
constexpr libc_types::u32 per_stage_constants = 0x8535;
} // namespace values
template<class span_const_f32>
requires(semantic::concepts::Span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void combiner_stage_parameter(
        group::combiner_stage_nv     stage,
        group::combiner_parameter_nv pname,
        span_const_f32 const&        params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CombinerStageParameterfvNV)
    }
    glCombinerStageParameterfvNV(
        static_cast<GLenum>(stage),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLfloat*>(params.data())
                      : nullptr);
    detail::error_check("CombinerStageParameterfvNV"sv);
}

template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void get_combiner_stage_parameter(
        group::combiner_stage_nv     stage,
        group::combiner_parameter_nv pname,
        span_f32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetCombinerStageParameterfvNV)
    }
    glGetCombinerStageParameterfvNV(
        static_cast<GLenum>(stage),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetCombinerStageParameterfvNV"sv);
}

} // namespace gl::nv::register_combiners2
#endif // GL_NV_register_combiners2
namespace gl::nv::register_combiners2 {
constexpr auto name = "GL_NV_register_combiners2";
}
