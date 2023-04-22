#pragma once

#ifdef GL_NV_gpu_program4
namespace gl::nv::gpu_program4 {
namespace values {
constexpr libc_types::u32 min_program_texel_offset      = 0x8904;
constexpr libc_types::u32 max_program_texel_offset      = 0x8905;
constexpr libc_types::u32 program_attrib_components     = 0x8906;
constexpr libc_types::u32 program_result_components     = 0x8907;
constexpr libc_types::u32 max_program_attrib_components = 0x8908;
constexpr libc_types::u32 max_program_result_components = 0x8909;
constexpr libc_types::u32 max_program_generic_attribs   = 0x8DA5;
constexpr libc_types::u32 max_program_generic_results   = 0x8DA6;
} // namespace values
template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_NV_gpu_program4
     * \param target GLenum
     * \param index GLuint
     * \param params GLint *
     * \return void
     */
    STATICINLINE void get_program_env_parameter_iiv(
        group::program_target target, u32 index, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramEnvParameterIivNV)
    }
    glGetProgramEnvParameterIivNV(
        static_cast<GLenum>(target),
        index,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetProgramEnvParameterIivNV"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_NV_gpu_program4
     * \param target GLenum
     * \param index GLuint
     * \param params GLuint *
     * \return void
     */
    STATICINLINE void get_program_env_parameter_iuiv(
        group::program_target target, u32 index, span_u32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramEnvParameterIuivNV)
    }
    glGetProgramEnvParameterIuivNV(
        static_cast<GLenum>(target),
        index,
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetProgramEnvParameterIuivNV"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_NV_gpu_program4
     * \param target GLenum
     * \param index GLuint
     * \param params GLint *
     * \return void
     */
    STATICINLINE void get_program_local_parameter_iiv(
        group::program_target target, u32 index, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramLocalParameterIivNV)
    }
    glGetProgramLocalParameterIivNV(
        static_cast<GLenum>(target),
        index,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetProgramLocalParameterIivNV"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_NV_gpu_program4
     * \param target GLenum
     * \param index GLuint
     * \param params GLuint *
     * \return void
     */
    STATICINLINE void get_program_local_parameter_iuiv(
        group::program_target target, u32 index, span_u32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetProgramLocalParameterIuivNV)
    }
    glGetProgramLocalParameterIuivNV(
        static_cast<GLenum>(target),
        index,
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetProgramLocalParameterIuivNV"sv);
}

template<class vec_4_i32>
requires(semantic::concepts::Vector<vec_4_i32, i32, 4>)
    /*!
     * \brief Part of GL_NV_gpu_program4
     * \param target GLenum
     * \param index GLuint
     * \param x GLint
     * \param y GLint
     * \param z GLint
     * \param w GLint
     * \return void
     */
    STATICINLINE void program_env_parameter_i4i(
        group::program_target target, u32 index, vec_4_i32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramEnvParameterI4iNV)
    }
    glProgramEnvParameterI4iNV(
        static_cast<GLenum>(target), index, x[0], x[1], x[2], x[3]);
    detail::error_check("ProgramEnvParameterI4iNV"sv);
}

template<class span_const_vec_4_i32>
requires(semantic::concepts::Span<span_const_vec_4_i32>&& semantic::concepts::
             Vector<typename span_const_vec_4_i32::value_type, i32, 4>)
    /*!
     * \brief Part of GL_NV_gpu_program4
     * \param target GLenum
     * \param index GLuint
     * \param params const GLint *
     * \return void
     */
    STATICINLINE void program_env_parameter_i4iv(
        group::program_target       target,
        u32                         index,
        span_const_vec_4_i32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramEnvParameterI4ivNV)
    }
    glProgramEnvParameterI4ivNV(
        static_cast<GLenum>(target),
        index,
        reinterpret_cast<const GLint*>(params.data()));
    detail::error_check("ProgramEnvParameterI4ivNV"sv);
}

template<class vec_4_u32>
requires(semantic::concepts::Vector<vec_4_u32, u32, 4>)
    /*!
     * \brief Part of GL_NV_gpu_program4
     * \param target GLenum
     * \param index GLuint
     * \param x GLuint
     * \param y GLuint
     * \param z GLuint
     * \param w GLuint
     * \return void
     */
    STATICINLINE void program_env_parameter_i4ui(
        group::program_target target, u32 index, vec_4_u32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramEnvParameterI4uiNV)
    }
    glProgramEnvParameterI4uiNV(
        static_cast<GLenum>(target), index, x[0], x[1], x[2], x[3]);
    detail::error_check("ProgramEnvParameterI4uiNV"sv);
}

template<class span_const_vec_4_u32>
requires(semantic::concepts::Span<span_const_vec_4_u32>&& semantic::concepts::
             Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
    /*!
     * \brief Part of GL_NV_gpu_program4
     * \param target GLenum
     * \param index GLuint
     * \param params const GLuint *
     * \return void
     */
    STATICINLINE void program_env_parameter_i4uiv(
        group::program_target       target,
        u32                         index,
        span_const_vec_4_u32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramEnvParameterI4uivNV)
    }
    glProgramEnvParameterI4uivNV(
        static_cast<GLenum>(target),
        index,
        reinterpret_cast<const GLuint*>(params.data()));
    detail::error_check("ProgramEnvParameterI4uivNV"sv);
}

template<class span_const_vec_4_i32>
requires(semantic::concepts::Span<span_const_vec_4_i32>&& semantic::concepts::
             Vector<typename span_const_vec_4_i32::value_type, i32, 4>)
    /*!
     * \brief Part of GL_NV_gpu_program4
     * \param target GLenum
     * \param index GLuint
     * \param count GLsizei
     * \param params const GLint *
     * \return void
     */
    STATICINLINE void program_env_parameters_i4iv(
        group::program_target       target,
        u32                         index,
        span_const_vec_4_i32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramEnvParametersI4ivNV)
    }
    glProgramEnvParametersI4ivNV(
        static_cast<GLenum>(target),
        index,
        params.size(),
        reinterpret_cast<const GLint*>(params.data()));
    detail::error_check("ProgramEnvParametersI4ivNV"sv);
}

template<class span_const_vec_4_u32>
requires(semantic::concepts::Span<span_const_vec_4_u32>&& semantic::concepts::
             Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
    /*!
     * \brief Part of GL_NV_gpu_program4
     * \param target GLenum
     * \param index GLuint
     * \param count GLsizei
     * \param params const GLuint *
     * \return void
     */
    STATICINLINE void program_env_parameters_i4uiv(
        group::program_target       target,
        u32                         index,
        span_const_vec_4_u32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramEnvParametersI4uivNV)
    }
    glProgramEnvParametersI4uivNV(
        static_cast<GLenum>(target),
        index,
        params.size(),
        reinterpret_cast<const GLuint*>(params.data()));
    detail::error_check("ProgramEnvParametersI4uivNV"sv);
}

template<class vec_4_i32>
requires(semantic::concepts::Vector<vec_4_i32, i32, 4>)
    /*!
     * \brief Part of GL_NV_gpu_program4
     * \param target GLenum
     * \param index GLuint
     * \param x GLint
     * \param y GLint
     * \param z GLint
     * \param w GLint
     * \return void
     */
    STATICINLINE void program_local_parameter_i4i(
        group::program_target target, u32 index, vec_4_i32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramLocalParameterI4iNV)
    }
    glProgramLocalParameterI4iNV(
        static_cast<GLenum>(target), index, x[0], x[1], x[2], x[3]);
    detail::error_check("ProgramLocalParameterI4iNV"sv);
}

template<class span_const_vec_4_i32>
requires(semantic::concepts::Span<span_const_vec_4_i32>&& semantic::concepts::
             Vector<typename span_const_vec_4_i32::value_type, i32, 4>)
    /*!
     * \brief Part of GL_NV_gpu_program4
     * \param target GLenum
     * \param index GLuint
     * \param params const GLint *
     * \return void
     */
    STATICINLINE void program_local_parameter_i4iv(
        group::program_target       target,
        u32                         index,
        span_const_vec_4_i32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramLocalParameterI4ivNV)
    }
    glProgramLocalParameterI4ivNV(
        static_cast<GLenum>(target),
        index,
        reinterpret_cast<const GLint*>(params.data()));
    detail::error_check("ProgramLocalParameterI4ivNV"sv);
}

template<class vec_4_u32>
requires(semantic::concepts::Vector<vec_4_u32, u32, 4>)
    /*!
     * \brief Part of GL_NV_gpu_program4
     * \param target GLenum
     * \param index GLuint
     * \param x GLuint
     * \param y GLuint
     * \param z GLuint
     * \param w GLuint
     * \return void
     */
    STATICINLINE void program_local_parameter_i4ui(
        group::program_target target, u32 index, vec_4_u32 const& x)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramLocalParameterI4uiNV)
    }
    glProgramLocalParameterI4uiNV(
        static_cast<GLenum>(target), index, x[0], x[1], x[2], x[3]);
    detail::error_check("ProgramLocalParameterI4uiNV"sv);
}

template<class span_const_vec_4_u32>
requires(semantic::concepts::Span<span_const_vec_4_u32>&& semantic::concepts::
             Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
    /*!
     * \brief Part of GL_NV_gpu_program4
     * \param target GLenum
     * \param index GLuint
     * \param params const GLuint *
     * \return void
     */
    STATICINLINE void program_local_parameter_i4uiv(
        group::program_target       target,
        u32                         index,
        span_const_vec_4_u32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramLocalParameterI4uivNV)
    }
    glProgramLocalParameterI4uivNV(
        static_cast<GLenum>(target),
        index,
        reinterpret_cast<const GLuint*>(params.data()));
    detail::error_check("ProgramLocalParameterI4uivNV"sv);
}

template<class span_const_vec_4_i32>
requires(semantic::concepts::Span<span_const_vec_4_i32>&& semantic::concepts::
             Vector<typename span_const_vec_4_i32::value_type, i32, 4>)
    /*!
     * \brief Part of GL_NV_gpu_program4
     * \param target GLenum
     * \param index GLuint
     * \param count GLsizei
     * \param params const GLint *
     * \return void
     */
    STATICINLINE void program_local_parameters_i4iv(
        group::program_target       target,
        u32                         index,
        span_const_vec_4_i32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramLocalParametersI4ivNV)
    }
    glProgramLocalParametersI4ivNV(
        static_cast<GLenum>(target),
        index,
        params.size(),
        reinterpret_cast<const GLint*>(params.data()));
    detail::error_check("ProgramLocalParametersI4ivNV"sv);
}

template<class span_const_vec_4_u32>
requires(semantic::concepts::Span<span_const_vec_4_u32>&& semantic::concepts::
             Vector<typename span_const_vec_4_u32::value_type, u32, 4>)
    /*!
     * \brief Part of GL_NV_gpu_program4
     * \param target GLenum
     * \param index GLuint
     * \param count GLsizei
     * \param params const GLuint *
     * \return void
     */
    STATICINLINE void program_local_parameters_i4uiv(
        group::program_target       target,
        u32                         index,
        span_const_vec_4_u32 const& params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramLocalParametersI4uivNV)
    }
    glProgramLocalParametersI4uivNV(
        static_cast<GLenum>(target),
        index,
        params.size(),
        reinterpret_cast<const GLuint*>(params.data()));
    detail::error_check("ProgramLocalParametersI4uivNV"sv);
}

} // namespace gl::nv::gpu_program4
#endif // GL_NV_gpu_program4
namespace gl::nv::gpu_program4 {
constexpr auto name = "GL_NV_gpu_program4";
}
