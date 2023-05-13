#pragma once

#ifdef GL_EXT_gpu_program_parameters
namespace gl::ext::gpu_program_parameters {
namespace values {
} // namespace values
template<class span_const_vec_4_f32>
requires(
    concepts::span<span_const_vec_4_f32>&&
        concepts::vector<typename span_const_vec_4_f32::value_type, f32, 4>)
    /*!
     * \brief Part of GL_EXT_gpu_program_parameters
     * \param target GLenum
     * \param index GLuint
     * \param count GLsizei
     * \param params const GLfloat *
     * \return void
     */
    STATICINLINE void program_env_parameters4fv(
        group::program_target       target,
        u32                         index,
        span_const_vec_4_f32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramEnvParameters4fvEXT)
    }
    glProgramEnvParameters4fvEXT(
        static_cast<GLenum>(target),
        index,
        params.size(),
        reinterpret_cast<const GLfloat*>(params.data()));
    detail::error_check("ProgramEnvParameters4fvEXT"sv);
}

template<class span_const_vec_4_f32>
requires(
    concepts::span<span_const_vec_4_f32>&&
        concepts::vector<typename span_const_vec_4_f32::value_type, f32, 4>)
    /*!
     * \brief Part of GL_EXT_gpu_program_parameters
     * \param target GLenum
     * \param index GLuint
     * \param count GLsizei
     * \param params const GLfloat *
     * \return void
     */
    STATICINLINE void program_local_parameters4fv(
        group::program_target       target,
        u32                         index,
        span_const_vec_4_f32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramLocalParameters4fvEXT)
    }
    glProgramLocalParameters4fvEXT(
        static_cast<GLenum>(target),
        index,
        params.size(),
        reinterpret_cast<const GLfloat*>(params.data()));
    detail::error_check("ProgramLocalParameters4fvEXT"sv);
}

} // namespace gl::ext::gpu_program_parameters
#endif // GL_EXT_gpu_program_parameters
namespace gl::ext::gpu_program_parameters {
constexpr auto name = "GL_EXT_gpu_program_parameters";
}
