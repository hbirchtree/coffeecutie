#pragma once

#ifdef GL_OES_texture_border_clamp
#include "../enums/TextureWrapMode.h"

namespace gl::oes::texture_border_clamp {
using gl::group::texture_wrap_mode;

namespace values {
constexpr u32 texture_border_color = 0x1004;
} // namespace values

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_OES_texture_border_clamp
 * \param sampler GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_sampler_parameter_iiv(
    u32                        sampler,
    group::sampler_parameter_i pname,
    span_i32                   params,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSamplerParameterIivOES)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetSamplerParameterIivOES,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        sampler,
        pname);
#endif
    glGetSamplerParameterIivOES(
        sampler,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetSamplerParameterIivOES"sv, check_errors);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_OES_texture_border_clamp
 * \param sampler GLuint
 * \param pname GLenum
 * \param params GLuint *
 * \return void
 */
STATICINLINE void get_sampler_parameter_iuiv(
    u32                        sampler,
    group::sampler_parameter_i pname,
    span_u32                   params,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSamplerParameterIuivOES)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetSamplerParameterIuivOES,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        sampler,
        pname);
#endif
    glGetSamplerParameterIuivOES(
        sampler,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetSamplerParameterIuivOES"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_OES_texture_border_clamp
 * \param target GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_tex_parameter_iiv(
    group::texture_target        target,
    group::get_texture_parameter pname,
    span_i32                     params,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTexParameterIivOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetTexParameterIivOES,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glGetTexParameterIivOES(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetTexParameterIivOES"sv, check_errors);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_OES_texture_border_clamp
 * \param target GLenum
 * \param pname GLenum
 * \param params GLuint *
 * \return void
 */
STATICINLINE void get_tex_parameter_iuiv(
    group::texture_target        target,
    group::get_texture_parameter pname,
    span_u32                     params,
    error_check                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetTexParameterIuivOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glGetTexParameterIuivOES,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glGetTexParameterIuivOES(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetTexParameterIuivOES"sv, check_errors);
}

template<class span_const_i32>
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_OES_texture_border_clamp
 * \param sampler GLuint
 * \param pname GLenum
 * \param param const GLint *
 * \return void
 */
STATICINLINE void sampler_parameter_iiv(
    u32                        sampler,
    group::sampler_parameter_i pname,
    span_const_i32 const&      param,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameterIivOES)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glSamplerParameterIivOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(param.data()), param.size_bytes()),
        sampler,
        pname);
#endif
    glSamplerParameterIivOES(
        sampler,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLint*>(param.data()) : nullptr);
    detail::error_check("SamplerParameterIivOES"sv, check_errors);
}

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_OES_texture_border_clamp
 * \param sampler GLuint
 * \param pname GLenum
 * \param param const GLuint *
 * \return void
 */
STATICINLINE void sampler_parameter_iuiv(
    u32                        sampler,
    group::sampler_parameter_i pname,
    span_const_u32 const&      param,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameterIuivOES)
#if (defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glSamplerParameterIuivOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(param.data()), param.size_bytes()),
        sampler,
        pname);
#endif
    glSamplerParameterIuivOES(
        sampler,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLuint*>(param.data()) : nullptr);
    detail::error_check("SamplerParameterIuivOES"sv, check_errors);
}

template<class span_const_i32>
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_OES_texture_border_clamp
 * \param target GLenum
 * \param pname GLenum
 * \param params const GLint *
 * \return void
 */
STATICINLINE void tex_parameter_iiv(
    group::texture_target         target,
    group::texture_parameter_name pname,
    span_const_i32 const&         params,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexParameterIivOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glTexParameterIivOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glTexParameterIivOES(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLint*>(params.data())
                      : nullptr);
    detail::error_check("TexParameterIivOES"sv, check_errors);
}

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_OES_texture_border_clamp
 * \param target GLenum
 * \param pname GLenum
 * \param params const GLuint *
 * \return void
 */
STATICINLINE void tex_parameter_iuiv(
    group::texture_target         target,
    group::texture_parameter_name pname,
    span_const_u32 const&         params,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(TexParameterIuivOES)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE_DATA(
        glTexParameterIuivOES,
        gsl::span<const char>(
            reinterpret_cast<const char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glTexParameterIuivOES(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<const GLuint*>(params.data())
                      : nullptr);
    detail::error_check("TexParameterIuivOES"sv, check_errors);
}

} // namespace gl::oes::texture_border_clamp
#endif // GL_OES_texture_border_clamp
namespace gl::oes::texture_border_clamp {
constexpr auto name = "GL_OES_texture_border_clamp";
}
