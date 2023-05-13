#pragma once

#ifdef GL_KHR_robustness
#include "../enums/ErrorCode.h"
#include "../enums/GraphicsResetStatus.h"
#include "../enums/SpecialNumbers.h"
namespace gl::khr::robustness {
using gl::group::error_code;
using gl::group::graphics_reset_status;
using gl::group::special_numbers;
namespace values {
#if defined(GL_VERSION_1_0)
constexpr u32 context_lost = 0x0507;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr u32 context_lost = 0x0507;
#endif
#if defined(GL_VERSION_1_0)
constexpr u32 lose_context_on_reset = 0x8252;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr u32 lose_context_on_reset = 0x8252;
#endif
#if defined(GL_VERSION_1_0)
constexpr u32 reset_notification_strategy = 0x8256;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr u32 reset_notification_strategy = 0x8256;
#endif
#if defined(GL_VERSION_1_0)
constexpr u32 no_reset_notification = 0x8261;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr u32 no_reset_notification = 0x8261;
#endif
#if defined(GL_VERSION_1_0)
constexpr u32 context_robust_access = 0x90F3;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr u32 context_robust_access = 0x90F3;
#endif
} // namespace values
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_KHR_robustness

 * \return GraphicsResetStatus
 */
STATICINLINE group::graphics_reset_status get_graphics_reset_status()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetGraphicsResetStatus)
    }
    auto out = glGetGraphicsResetStatus();
    detail::error_check("GetGraphicsResetStatus"sv);
    return static_cast<group::graphics_reset_status>(out);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_f32>
requires(concepts::span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_KHR_robustness
     * \param program GLuint
     * \param location GLint
     * \param bufSize GLsizei
     * \param params GLfloat *
     * \return void
     */
    STATICINLINE
    void getn_uniformfv(u32 program, i32 location, i32 bufSize, span_f32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformfv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetnUniformfv(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetnUniformfv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_i32>
requires(concepts::span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_KHR_robustness
     * \param program GLuint
     * \param location GLint
     * \param bufSize GLsizei
     * \param params GLint *
     * \return void
     */
    STATICINLINE
    void getn_uniformiv(u32 program, i32 location, i32 bufSize, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetnUniformiv(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetnUniformiv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_u32>
requires(concepts::span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_KHR_robustness
     * \param program GLuint
     * \param location GLint
     * \param bufSize GLsizei
     * \param params GLuint *
     * \return void
     */
    STATICINLINE void getn_uniformuiv(
        u32 program, i32 location, i32 bufSize, span_u32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformuiv)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetnUniformuiv(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetnUniformuiv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class size_2_i32, class span_void, class vec_2_i32>
requires(concepts::vector<vec_2_i32, i32, 2>&&
             concepts::size_2d<size_2_i32, i32>&& concepts::span<span_void>)
    /*!
     * \brief Part of GL_KHR_robustness
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
        span_void           data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ReadnPixels)
    }
    glReadnPixels(
        x[0],
        x[1],
        width[0],
        width[1],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() * sizeof(typename std::decay_t<span_void>::value_type),
        data.size() ? reinterpret_cast<void*>(data.data()) : nullptr);
    detail::error_check("ReadnPixels"sv);
}

#endif
#if defined(GL_ES_VERSION_2_0)
/*!
 * \brief Part of GL_KHR_robustness

 * \return GraphicsResetStatus
 */
STATICINLINE group::graphics_reset_status get_graphics_reset_status()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetGraphicsResetStatusKHR)
    }
    auto out = glGetGraphicsResetStatusKHR();
    detail::error_check("GetGraphicsResetStatusKHR"sv);
    return static_cast<group::graphics_reset_status>(out);
}

#endif
#if defined(GL_ES_VERSION_2_0)
template<class span_f32>
requires(concepts::span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_KHR_robustness
     * \param program GLuint
     * \param location GLint
     * \param bufSize GLsizei
     * \param params GLfloat *
     * \return void
     */
    STATICINLINE
    void getn_uniformfv(u32 program, i32 location, i32 bufSize, span_f32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformfvKHR)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetnUniformfvKHR(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetnUniformfvKHR"sv);
}

#endif
#if defined(GL_ES_VERSION_2_0)
template<class span_i32>
requires(concepts::span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_KHR_robustness
     * \param program GLuint
     * \param location GLint
     * \param bufSize GLsizei
     * \param params GLint *
     * \return void
     */
    STATICINLINE
    void getn_uniformiv(u32 program, i32 location, i32 bufSize, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformivKHR)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetnUniformivKHR(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetnUniformivKHR"sv);
}

#endif
#if defined(GL_ES_VERSION_2_0)
template<class span_u32>
requires(concepts::span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_KHR_robustness
     * \param program GLuint
     * \param location GLint
     * \param bufSize GLsizei
     * \param params GLuint *
     * \return void
     */
    STATICINLINE void getn_uniformuiv(
        u32 program, i32 location, i32 bufSize, span_u32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformuivKHR)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetnUniformuivKHR(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetnUniformuivKHR"sv);
}

#endif
#if defined(GL_ES_VERSION_2_0)
template<class size_2_i32, class span_void, class vec_2_i32>
requires(concepts::vector<vec_2_i32, i32, 2>&&
             concepts::size_2d<size_2_i32, i32>&& concepts::span<span_void>)
    /*!
     * \brief Part of GL_KHR_robustness
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
        span_void           data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ReadnPixelsKHR)
    }
    glReadnPixelsKHR(
        x[0],
        x[1],
        width[0],
        width[1],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() * sizeof(typename std::decay_t<span_void>::value_type),
        data.size() ? reinterpret_cast<void*>(data.data()) : nullptr);
    detail::error_check("ReadnPixelsKHR"sv);
}

#endif
} // namespace gl::khr::robustness
#endif // GL_KHR_robustness
namespace gl::khr::robustness {
constexpr auto name = "GL_KHR_robustness";
}
