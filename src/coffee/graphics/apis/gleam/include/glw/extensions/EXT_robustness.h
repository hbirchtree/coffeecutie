#pragma once

#ifdef GL_EXT_robustness
#include "../enums/ErrorCode.h"
#include "../enums/GraphicsResetStatus.h"
#include "../enums/SpecialNumbers.h"

namespace gl::ext::robustness {
using gl::group::error_code;
using gl::group::graphics_reset_status;
using gl::group::special_numbers;

namespace values {
constexpr u32 lose_context_on_reset       = 0x8252;
constexpr u32 reset_notification_strategy = 0x8256;
constexpr u32 no_reset_notification       = 0x8261;
constexpr u32 context_robust_access       = 0x90F3;
} // namespace values

/*!
 * \brief Part of GL_EXT_robustness

 * \return GraphicsResetStatus
 */
STATICINLINE group::graphics_reset_status get_graphics_reset_status(
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetGraphicsResetStatusEXT)
    }
    auto out = glGetGraphicsResetStatusEXT();
    detail::error_check("GetGraphicsResetStatusEXT"sv, check_errors);
    return static_cast<group::graphics_reset_status>(out);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_robustness
 * \param program GLuint
 * \param location GLint
 * \param bufSize GLsizei
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void getn_uniformfv(
    u32         program,
    i32         location,
    i32         bufSize,
    span_f32    params,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformfvEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetnUniformfvEXT(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetnUniformfvEXT"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_robustness
 * \param program GLuint
 * \param location GLint
 * \param bufSize GLsizei
 * \param params GLint *
 * \return void
 */
STATICINLINE void getn_uniformiv(
    u32         program,
    i32         location,
    i32         bufSize,
    span_i32    params,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformivEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetnUniformivEXT(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetnUniformivEXT"sv, check_errors);
}

template<class size_2_i32, class span_void, class vec_2_i32>
requires(
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32> &&
    concepts::span<span_void>)
/*!
 * \brief Part of GL_EXT_robustness
 * \param x GLint
 * \param y GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param format GLenum
 * \param type GLenum
 * \param bufSize GLsizei
 * \param data void *
 * \return void
 */
STATICINLINE void readn_pixels(
    vec_2_i32 const&    x,
    size_2_i32 const&   width,
    group::pixel_format format,
    group::pixel_type   type,
    span_void           data,
    error_check         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ReadnPixelsEXT)
    }
    glReadnPixelsEXT(
        x[0],
        x[1],
        width[0],
        width[1],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() * sizeof(typename std::decay_t<span_void>::value_type),
        data.size() ? reinterpret_cast<void*>(data.data()) : nullptr);
    detail::error_check("ReadnPixelsEXT"sv, check_errors);
}

} // namespace gl::ext::robustness
#endif // GL_EXT_robustness
namespace gl::ext::robustness {
constexpr auto name = "GL_EXT_robustness";
}
