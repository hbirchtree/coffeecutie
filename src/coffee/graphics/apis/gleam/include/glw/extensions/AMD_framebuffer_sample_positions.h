#pragma once

#ifdef GL_AMD_framebuffer_sample_positions
#include "../enums/SpecialNumbers.h"
namespace gl::amd::framebuffer_sample_positions {
using gl::group::special_numbers;
namespace values {
constexpr u32 subsample_distance          = 0x883F;
constexpr u32 pixels_per_sample_pattern_x = 0x91AE;
constexpr u32 pixels_per_sample_pattern_y = 0x91AF;
} // namespace values
template<class span_const_f32>
requires(concepts::span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_AMD_framebuffer_sample_positions
     * \param target GLenum
     * \param numsamples GLuint
     * \param pixelindex GLuint
     * \param values const GLfloat *
     * \return void
     */
    STATICINLINE void framebuffer_sample_positionsfv(
        group::framebuffer_target target,
        u32                       numsamples,
        u32                       pixelindex,
        span_const_f32 const&     values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferSamplePositionsfvAMD)
    }
    glFramebufferSamplePositionsfvAMD(
        static_cast<GLenum>(target),
        numsamples,
        pixelindex,
        values.size() ? reinterpret_cast<const GLfloat*>(values.data())
                      : nullptr);
    detail::error_check("FramebufferSamplePositionsfvAMD"sv);
}

template<class span_f32>
requires(concepts::span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_AMD_framebuffer_sample_positions
     * \param target GLenum
     * \param pname GLenum
     * \param numsamples GLuint
     * \param pixelindex GLuint
     * \param size GLsizei
     * \param values GLfloat *
     * \return void
     */
    STATICINLINE void get_framebuffer_parameter(
        group::framebuffer_target                    target,
        group::framebuffer_attachment_parameter_name pname,
        u32                                          numsamples,
        u32                                          pixelindex,
        i32                                          size,
        span_f32                                     values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFramebufferParameterfvAMD)
    }
    glGetFramebufferParameterfvAMD(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        numsamples,
        pixelindex,
        size,
        values.size() ? reinterpret_cast<GLfloat*>(values.data()) : nullptr);
    detail::error_check("GetFramebufferParameterfvAMD"sv);
}

template<class span_f32>
requires(concepts::span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_AMD_framebuffer_sample_positions
     * \param framebuffer GLuint
     * \param pname GLenum
     * \param numsamples GLuint
     * \param pixelindex GLuint
     * \param size GLsizei
     * \param values GLfloat *
     * \return void
     */
    STATICINLINE void get_named_framebuffer_parameter(
        u32      framebuffer,
        GLenum   pname,
        u32      numsamples,
        u32      pixelindex,
        i32      size,
        span_f32 values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetNamedFramebufferParameterfvAMD)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
    }
    glGetNamedFramebufferParameterfvAMD(
        framebuffer,
        pname,
        numsamples,
        pixelindex,
        size,
        values.size() ? reinterpret_cast<GLfloat*>(values.data()) : nullptr);
    detail::error_check("GetNamedFramebufferParameterfvAMD"sv);
}

template<class span_const_f32>
requires(concepts::span<span_const_f32>&& std::is_same_v<
         std::decay_t<typename span_const_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_AMD_framebuffer_sample_positions
     * \param framebuffer GLuint
     * \param numsamples GLuint
     * \param pixelindex GLuint
     * \param values const GLfloat *
     * \return void
     */
    STATICINLINE void named_framebuffer_sample_positionsfv(
        u32                   framebuffer,
        u32                   numsamples,
        u32                   pixelindex,
        span_const_f32 const& values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(NamedFramebufferSamplePositionsfvAMD)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
    }
    glNamedFramebufferSamplePositionsfvAMD(
        framebuffer,
        numsamples,
        pixelindex,
        values.size() ? reinterpret_cast<const GLfloat*>(values.data())
                      : nullptr);
    detail::error_check("NamedFramebufferSamplePositionsfvAMD"sv);
}

} // namespace gl::amd::framebuffer_sample_positions
#endif // GL_AMD_framebuffer_sample_positions
namespace gl::amd::framebuffer_sample_positions {
constexpr auto name = "GL_AMD_framebuffer_sample_positions";
}
