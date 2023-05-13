#pragma once

#ifdef GL_ARB_robustness
#include "../enums/ContextFlagMask.h"
#include "../enums/ErrorCode.h"
#include "../enums/GraphicsResetStatus.h"
#include "../enums/SpecialNumbers.h"
namespace gl::arb::robustness {
using gl::group::context_flag_mask;
using gl::group::error_code;
using gl::group::graphics_reset_status;
using gl::group::special_numbers;
namespace values {
constexpr u32 lose_context_on_reset       = 0x8252;
constexpr u32 reset_notification_strategy = 0x8256;
constexpr u32 no_reset_notification       = 0x8261;
} // namespace values
/*!
 * \brief Part of GL_ARB_robustness

 * \return GraphicsResetStatus
 */
STATICINLINE group::graphics_reset_status get_graphics_reset_status()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetGraphicsResetStatusARB)
    }
    auto out = glGetGraphicsResetStatusARB();
    detail::error_check("GetGraphicsResetStatusARB"sv);
    return static_cast<group::graphics_reset_status>(out);
}

template<class span_void>
requires(concepts::span<span_void>)
    /*!
     * \brief Part of GL_ARB_robustness
     * \param target GLenum
     * \param lod GLint
     * \param bufSize GLsizei
     * \param img void *
     * \return void
     */
    STATICINLINE void getn_compressed_tex_image(
        group::texture_target target, i32 lod, span_void img)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnCompressedTexImageARB)
    }
    glGetnCompressedTexImageARB(
        static_cast<GLenum>(target),
        lod,
        img.size() * sizeof(typename std::decay_t<span_void>::value_type),
        img.size() ? reinterpret_cast<void*>(img.data()) : nullptr);
    detail::error_check("GetnCompressedTexImageARB"sv);
}

template<class span_void>
requires(concepts::span<span_void>)
    /*!
     * \brief Part of GL_ARB_robustness
     * \param target GLenum
     * \param level GLint
     * \param format GLenum
     * \param type GLenum
     * \param bufSize GLsizei
     * \param img void *
     * \return void
     */
    STATICINLINE void getn_tex_image(
        group::texture_target target,
        i32                   level,
        group::pixel_format   format,
        group::pixel_type     type,
        span_void             img)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnTexImageARB)
    }
    glGetnTexImageARB(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        img.size() * sizeof(typename std::decay_t<span_void>::value_type),
        img.size() ? reinterpret_cast<void*>(img.data()) : nullptr);
    detail::error_check("GetnTexImageARB"sv);
}

template<class span_f64>
requires(concepts::span<span_f64>&& std::is_same_v<
         std::decay_t<typename span_f64::value_type>,
         std::decay_t<f64>>)
    /*!
     * \brief Part of GL_ARB_robustness
     * \param program GLuint
     * \param location GLint
     * \param bufSize GLsizei
     * \param params GLdouble *
     * \return void
     */
    STATICINLINE
    void getn_uniformdv(u32 program, i32 location, i32 bufSize, span_f64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformdvARB)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetnUniformdvARB(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLdouble*>(params.data()) : nullptr);
    detail::error_check("GetnUniformdvARB"sv);
}

template<class span_f32>
requires(concepts::span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>)
    /*!
     * \brief Part of GL_ARB_robustness
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
        GLW_FPTR_CHECK(GetnUniformfvARB)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetnUniformfvARB(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetnUniformfvARB"sv);
}

template<class span_i32>
requires(concepts::span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_ARB_robustness
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
        GLW_FPTR_CHECK(GetnUniformivARB)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetnUniformivARB(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetnUniformivARB"sv);
}

template<class span_u32>
requires(concepts::span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_ARB_robustness
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
        GLW_FPTR_CHECK(GetnUniformuivARB)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glGetnUniformuivARB(
        program,
        location,
        bufSize,
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetnUniformuivARB"sv);
}

template<class size_2_i32, class span_void, class vec_2_i32>
requires(concepts::vector<vec_2_i32, i32, 2>&&
             concepts::size_2d<size_2_i32, i32>&& concepts::span<span_void>)
    /*!
     * \brief Part of GL_ARB_robustness
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
        GLW_FPTR_CHECK(ReadnPixelsARB)
    }
    glReadnPixelsARB(
        x[0],
        x[1],
        width[0],
        width[1],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() * sizeof(typename std::decay_t<span_void>::value_type),
        data.size() ? reinterpret_cast<void*>(data.data()) : nullptr);
    detail::error_check("ReadnPixelsARB"sv);
}

} // namespace gl::arb::robustness
#endif // GL_ARB_robustness
namespace gl::arb::robustness {
constexpr auto name = "GL_ARB_robustness";
}
