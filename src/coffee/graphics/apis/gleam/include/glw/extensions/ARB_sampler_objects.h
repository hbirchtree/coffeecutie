#pragma once

#ifdef GL_ARB_sampler_objects
#include "../enums/GetPName.h"

namespace gl::arb::sampler_objects {
using gl::group::get_prop;

namespace values {
} // namespace values

/*!
 * \brief Part of GL_ARB_sampler_objects
 * \param unit GLuint
 * \param sampler GLuint
 * \return void
 */
STATICINLINE void bind_sampler(u32 unit, u32 sampler)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindSampler)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
    glBindSampler(unit, sampler);
    detail::error_check("BindSampler"sv);
}

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_ARB_sampler_objects
 * \param count GLsizei
 * \param samplers const GLuint *
 * \return void
 */
STATICINLINE void delete_samplers(span_const_u32 const& samplers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteSamplers)
    }
    glDeleteSamplers(
        samplers.size(),
        samplers.size() ? reinterpret_cast<const GLuint*>(samplers.data())
                        : nullptr);
    detail::error_check("DeleteSamplers"sv);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_ARB_sampler_objects
 * \param count GLsizei
 * \param samplers GLuint *
 * \return void
 */
STATICINLINE void gen_samplers(span_u32 samplers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenSamplers)
    }
    glGenSamplers(
        samplers.size(),
        samplers.size() ? reinterpret_cast<GLuint*>(samplers.data()) : nullptr);
    detail::error_check("GenSamplers"sv);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_ARB_sampler_objects
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
        GLW_FPTR_CHECK(GetSamplerParameterIiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
    glGetSamplerParameterIiv(
        sampler,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetSamplerParameterIiv"sv);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_ARB_sampler_objects
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
        GLW_FPTR_CHECK(GetSamplerParameterIuiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
    glGetSamplerParameterIuiv(
        sampler,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetSamplerParameterIuiv"sv);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_ARB_sampler_objects
 * \param sampler GLuint
 * \param pname GLenum
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_sampler_parameter(
    u32 sampler, group::sampler_parameter_f pname, span_f32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSamplerParameterfv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
    glGetSamplerParameterfv(
        sampler,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetSamplerParameterfv"sv);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_ARB_sampler_objects
 * \param sampler GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_sampler_parameter(
    u32 sampler, group::sampler_parameter_i pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSamplerParameteriv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
    glGetSamplerParameteriv(
        sampler,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetSamplerParameteriv"sv);
}

/*!
 * \brief Part of GL_ARB_sampler_objects
 * \param sampler GLuint
 * \return Boolean
 */
STATICINLINE bool is_sampler(u32 sampler)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsSampler)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
    auto out = glIsSampler(sampler);
    detail::error_check("IsSampler"sv);
    return out == GL_TRUE ? true : false;
}

template<class span_const_i32>
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_ARB_sampler_objects
 * \param sampler GLuint
 * \param pname GLenum
 * \param param const GLint *
 * \return void
 */
STATICINLINE void sampler_parameter_iiv(
    u32 sampler, group::sampler_parameter_i pname, span_const_i32 const& param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameterIiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
    glSamplerParameterIiv(
        sampler,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLint*>(param.data()) : nullptr);
    detail::error_check("SamplerParameterIiv"sv);
}

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_ARB_sampler_objects
 * \param sampler GLuint
 * \param pname GLenum
 * \param param const GLuint *
 * \return void
 */
STATICINLINE void sampler_parameter_iuiv(
    u32 sampler, group::sampler_parameter_i pname, span_const_u32 const& param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameterIuiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
    glSamplerParameterIuiv(
        sampler,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLuint*>(param.data()) : nullptr);
    detail::error_check("SamplerParameterIuiv"sv);
}

/*!
 * \brief Part of GL_ARB_sampler_objects
 * \param sampler GLuint
 * \param pname GLenum
 * \param param GLfloat
 * \return void
 */
STATICINLINE void sampler_parameter(
    u32 sampler, group::sampler_parameter_f pname, f32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameterf)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
    glSamplerParameterf(sampler, static_cast<GLenum>(pname), param);
    detail::error_check("SamplerParameterf"sv);
}

template<class span_const_f32>
requires(
    concepts::span<span_const_f32> &&
    std::is_same_v<
        std::decay_t<typename span_const_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_ARB_sampler_objects
 * \param sampler GLuint
 * \param pname GLenum
 * \param param const GLfloat *
 * \return void
 */
STATICINLINE void sampler_parameter(
    u32 sampler, group::sampler_parameter_f pname, span_const_f32 const& param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameterfv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
    glSamplerParameterfv(
        sampler,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLfloat*>(param.data())
                     : nullptr);
    detail::error_check("SamplerParameterfv"sv);
}

/*!
 * \brief Part of GL_ARB_sampler_objects
 * \param sampler GLuint
 * \param pname GLenum
 * \param param GLint
 * \return void
 */
STATICINLINE void sampler_parameter(
    u32 sampler, group::sampler_parameter_i pname, i32 param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameteri)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
    glSamplerParameteri(sampler, static_cast<GLenum>(pname), param);
    detail::error_check("SamplerParameteri"sv);
}

template<class span_const_i32>
requires(
    concepts::span<span_const_i32> &&
    std::is_same_v<
        std::decay_t<typename span_const_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_ARB_sampler_objects
 * \param sampler GLuint
 * \param pname GLenum
 * \param param const GLint *
 * \return void
 */
STATICINLINE void sampler_parameter(
    u32 sampler, group::sampler_parameter_i pname, span_const_i32 const& param)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(SamplerParameteriv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsSampler)
            glIsSampler(sampler);
#endif
    }
    glSamplerParameteriv(
        sampler,
        static_cast<GLenum>(pname),
        param.size() ? reinterpret_cast<const GLint*>(param.data()) : nullptr);
    detail::error_check("SamplerParameteriv"sv);
}

} // namespace gl::arb::sampler_objects
#endif // GL_ARB_sampler_objects
namespace gl::arb::sampler_objects {
constexpr auto name = "GL_ARB_sampler_objects";
}
