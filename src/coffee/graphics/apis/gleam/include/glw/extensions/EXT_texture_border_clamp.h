#pragma once

#ifdef GL_EXT_texture_border_clamp
namespace gl::ext::texture_border_clamp {
namespace values {
constexpr libc_types::u32 texture_border_color = 0x1004;
constexpr libc_types::u32 clamp_to_border      = 0x812D;
} // namespace values
template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_EXT_texture_border_clamp
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
        GLW_FPTR_CHECK(GetSamplerParameterIivEXT)
        glIsSampler(sampler);
    }
    glGetSamplerParameterIivEXT(
        sampler,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetSamplerParameterIivEXT"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_EXT_texture_border_clamp
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
        GLW_FPTR_CHECK(GetSamplerParameterIuivEXT)
        glIsSampler(sampler);
    }
    glGetSamplerParameterIuivEXT(
        sampler,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetSamplerParameterIuivEXT"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_EXT_texture_border_clamp
     * \param target GLenum
     * \param pname GLenum
     * \param params GLint *
     * \return void
     */
    STATICINLINE void get_tex_parameter_iiv(
        group::texture_target        target,
        group::get_texture_parameter pname,
        span_i32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTexParameterIivEXT)
    }
    glGetTexParameterIivEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetTexParameterIivEXT"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_EXT_texture_border_clamp
     * \param target GLenum
     * \param pname GLenum
     * \param params GLuint *
     * \return void
     */
    STATICINLINE void get_tex_parameter_iuiv(
        group::texture_target        target,
        group::get_texture_parameter pname,
        span_u32                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTexParameterIuivEXT)
    }
    glGetTexParameterIuivEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetTexParameterIuivEXT"sv);
}

template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_EXT_texture_border_clamp
     * \param sampler GLuint
     * \param pname GLenum
     * \param param const GLint *
     * \return void
     */
    STATICINLINE void sampler_parameter_iiv(
        u32                        sampler,
        group::sampler_parameter_i pname,
        span_const_i32 const&      param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameterIivEXT)
        glIsSampler(sampler);
    }
    glSamplerParameterIivEXT(
        sampler,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLint*>(param.data()) : nullptr);
    detail::error_check("SamplerParameterIivEXT"sv);
}

template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_EXT_texture_border_clamp
     * \param sampler GLuint
     * \param pname GLenum
     * \param param const GLuint *
     * \return void
     */
    STATICINLINE void sampler_parameter_iuiv(
        u32                        sampler,
        group::sampler_parameter_i pname,
        span_const_u32 const&      param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameterIuivEXT)
        glIsSampler(sampler);
    }
    glSamplerParameterIuivEXT(
        sampler,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLuint*>(param.data()) : nullptr);
    detail::error_check("SamplerParameterIuivEXT"sv);
}

template<class span_const_i32>
requires(semantic::concepts::Span<span_const_i32>&& std::is_same_v<
         std::decay_t<typename span_const_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_EXT_texture_border_clamp
     * \param target GLenum
     * \param pname GLenum
     * \param params const GLint *
     * \return void
     */
    STATICINLINE void tex_parameter_iiv(
        group::texture_target         target,
        group::texture_parameter_name pname,
        span_const_i32 const&         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexParameterIivEXT)
    }
    glTexParameterIivEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("TexParameterIivEXT"sv);
}

template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_EXT_texture_border_clamp
     * \param target GLenum
     * \param pname GLenum
     * \param params const GLuint *
     * \return void
     */
    STATICINLINE void tex_parameter_iuiv(
        group::texture_target         target,
        group::texture_parameter_name pname,
        span_const_u32 const&         params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexParameterIuivEXT)
    }
    glTexParameterIuivEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLuint*>(params.data())
                      : nullptr);
    detail::error_check("TexParameterIuivEXT"sv);
}

} // namespace gl::ext::texture_border_clamp
#endif // GL_EXT_texture_border_clamp
namespace gl::ext::texture_border_clamp {
constexpr auto name = "GL_EXT_texture_border_clamp";
}
