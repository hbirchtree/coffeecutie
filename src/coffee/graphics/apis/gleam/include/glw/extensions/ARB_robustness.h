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
constexpr libc_types::u32 lose_context_on_reset       = 0x8252;
constexpr libc_types::u32 guilty_context_reset        = 0x8253;
constexpr libc_types::u32 innocent_context_reset      = 0x8254;
constexpr libc_types::u32 unknown_context_reset       = 0x8255;
constexpr libc_types::u32 reset_notification_strategy = 0x8256;
constexpr libc_types::u32 no_reset_notification       = 0x8261;
} // namespace values
/*!
 * \brief Part of GL_ARB_robustness

 * \return GraphicsResetStatus
 */
STATICINLINE GLenum get_graphics_reset_status()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetGraphicsResetStatusARB)
    }
    auto out = glGetGraphicsResetStatusARB();
    detail::error_check("GetGraphicsResetStatusARB"sv);
    return out;
}

template<class span_void>
requires(semantic::concepts::Span<span_void>)
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
requires(semantic::concepts::Span<span_void>)
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
requires(
    semantic::concepts::Span<span_f64> &&
    std::is_same_v<
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
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
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
requires(
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<
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
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
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
requires(
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<
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
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
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
requires(
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<
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
#if defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)
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
requires(
    semantic::concepts::Vector<vec_2_i32, i32, 2> &&
    semantic::concepts::Size2D<size_2_i32, i32> &&
    semantic::concepts::Span<span_void>)
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
        x.x(),
        x.y(),
        width[0],
        width[1],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() * sizeof(typename std::decay_t<span_void>::value_type),
        data.size() ? reinterpret_cast<void*>(data.data()) : nullptr);
    detail::error_check("ReadnPixelsARB"sv);
}

#if defined(GL_VERSION_1_0)
template<class span_void>
requires(semantic::concepts::Span<span_void>)
/*!
 * \brief Part of GL_ARB_robustness
 * \param target GLenum
 * \param format GLenum
 * \param type GLenum
 * \param bufSize GLsizei
 * \param table void *
 * \return void
 */
STATICINLINE void getn_color_table(
    group::color_table_target target,
    group::pixel_format       format,
    group::pixel_type         type,
    span_void                 table)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnColorTableARB)
    }
    glGetnColorTableARB(
        static_cast<GLenum>(target),
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        table.size() * sizeof(typename std::decay_t<span_void>::value_type),
        table.size() ? reinterpret_cast<void*>(table.data()) : nullptr);
    detail::error_check("GetnColorTableARB"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_void>
requires(semantic::concepts::Span<span_void>)
/*!
 * \brief Part of GL_ARB_robustness
 * \param target GLenum
 * \param format GLenum
 * \param type GLenum
 * \param bufSize GLsizei
 * \param image void *
 * \return void
 */
STATICINLINE void getn_convolution_filter(
    group::convolution_target target,
    group::pixel_format       format,
    group::pixel_type         type,
    span_void                 image)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnConvolutionFilterARB)
    }
    glGetnConvolutionFilterARB(
        static_cast<GLenum>(target),
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        image.size() * sizeof(typename std::decay_t<span_void>::value_type),
        image.size() ? reinterpret_cast<void*>(image.data()) : nullptr);
    detail::error_check("GetnConvolutionFilterARB"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_void>
requires(semantic::concepts::Span<span_void>)
/*!
 * \brief Part of GL_ARB_robustness
 * \param target GLenum
 * \param reset GLboolean
 * \param format GLenum
 * \param type GLenum
 * \param bufSize GLsizei
 * \param values void *
 * \return void
 */
STATICINLINE void getn_histogram(
    group::histogram_target_ext target,
    bool                        reset,
    group::pixel_format         format,
    group::pixel_type           type,
    span_void                   values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnHistogramARB)
    }
    glGetnHistogramARB(
        static_cast<GLenum>(target),
        reset,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        values.size() * sizeof(typename std::decay_t<span_void>::value_type),
        values.size() ? reinterpret_cast<void*>(values.data()) : nullptr);
    detail::error_check("GetnHistogramARB"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_f64>
requires(
    semantic::concepts::Span<span_f64> &&
    std::is_same_v<
        std::decay_t<typename span_f64::value_type>,
        std::decay_t<f64>>)
/*!
 * \brief Part of GL_ARB_robustness
 * \param target GLenum
 * \param query GLenum
 * \param bufSize GLsizei
 * \param v GLdouble *
 * \return void
 */
STATICINLINE void getn_mapdv(
    group::map_target target, group::map_query query, i32 bufSize, span_f64 v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnMapdvARB)
    }
    glGetnMapdvARB(
        static_cast<GLenum>(target),
        static_cast<GLenum>(query),
        bufSize,
        v.size() ? reinterpret_cast<GLdouble*>(v.data()) : nullptr);
    detail::error_check("GetnMapdvARB"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_f32>
requires(
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<
        std::decay_t<typename span_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_ARB_robustness
 * \param target GLenum
 * \param query GLenum
 * \param bufSize GLsizei
 * \param v GLfloat *
 * \return void
 */
STATICINLINE void getn_mapfv(
    group::map_target target, group::map_query query, span_f32 v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnMapfvARB)
    }
    glGetnMapfvARB(
        static_cast<GLenum>(target),
        static_cast<GLenum>(query),
        v.size(),
        v.size() ? reinterpret_cast<GLfloat*>(v.data()) : nullptr);
    detail::error_check("GetnMapfvARB"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_i32>
requires(
    semantic::concepts::Span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>>)
/*!
 * \brief Part of GL_ARB_robustness
 * \param target GLenum
 * \param query GLenum
 * \param bufSize GLsizei
 * \param v GLint *
 * \return void
 */
STATICINLINE void getn_mapiv(
    group::map_target target, group::map_query query, span_i32 v)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnMapivARB)
    }
    glGetnMapivARB(
        static_cast<GLenum>(target),
        static_cast<GLenum>(query),
        v.size(),
        v.size() ? reinterpret_cast<GLint*>(v.data()) : nullptr);
    detail::error_check("GetnMapivARB"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_void>
requires(semantic::concepts::Span<span_void>)
/*!
 * \brief Part of GL_ARB_robustness
 * \param target GLenum
 * \param reset GLboolean
 * \param format GLenum
 * \param type GLenum
 * \param bufSize GLsizei
 * \param values void *
 * \return void
 */
STATICINLINE void getn_minmax(
    group::minmax_target_ext target,
    bool                     reset,
    group::pixel_format      format,
    group::pixel_type        type,
    span_void                values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnMinmaxARB)
    }
    glGetnMinmaxARB(
        static_cast<GLenum>(target),
        reset,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        values.size() * sizeof(typename std::decay_t<span_void>::value_type),
        values.size() ? reinterpret_cast<void*>(values.data()) : nullptr);
    detail::error_check("GetnMinmaxARB"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_f32>
requires(
    semantic::concepts::Span<span_f32> &&
    std::is_same_v<
        std::decay_t<typename span_f32::value_type>,
        std::decay_t<f32>>)
/*!
 * \brief Part of GL_ARB_robustness
 * \param map GLenum
 * \param bufSize GLsizei
 * \param values GLfloat *
 * \return void
 */
STATICINLINE
    void getn_pixel_mapfv(group::pixel_map map, i32 bufSize, span_f32 values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnPixelMapfvARB)
    }
    glGetnPixelMapfvARB(
        static_cast<GLenum>(map),
        bufSize,
        values.size() ? reinterpret_cast<GLfloat*>(values.data()) : nullptr);
    detail::error_check("GetnPixelMapfvARB"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_u32>
requires(
    semantic::concepts::Span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_ARB_robustness
 * \param map GLenum
 * \param bufSize GLsizei
 * \param values GLuint *
 * \return void
 */
STATICINLINE void getn_pixel_mapuiv(group::pixel_map map, span_u32 values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnPixelMapuivARB)
    }
    glGetnPixelMapuivARB(
        static_cast<GLenum>(map),
        values.size(),
        values.size() ? reinterpret_cast<GLuint*>(values.data()) : nullptr);
    detail::error_check("GetnPixelMapuivARB"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_u16>
requires(
    semantic::concepts::Span<span_u16> &&
    std::is_same_v<
        std::decay_t<typename span_u16::value_type>,
        std::decay_t<u16>>)
/*!
 * \brief Part of GL_ARB_robustness
 * \param map GLenum
 * \param bufSize GLsizei
 * \param values GLushort *
 * \return void
 */
STATICINLINE void getn_pixel_mapusv(group::pixel_map map, span_u16 values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnPixelMapusvARB)
    }
    glGetnPixelMapusvARB(
        static_cast<GLenum>(map),
        values.size(),
        values.size() ? reinterpret_cast<GLushort*>(values.data()) : nullptr);
    detail::error_check("GetnPixelMapusvARB"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_u8>
requires(
    semantic::concepts::Span<span_u8> &&
    std::
        is_same_v<std::decay_t<typename span_u8::value_type>, std::decay_t<u8>>)
/*!
 * \brief Part of GL_ARB_robustness
 * \param bufSize GLsizei
 * \param pattern GLubyte *
 * \return void
 */
STATICINLINE void getn_polygon_stipple(span_u8 pattern)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnPolygonStippleARB)
    }
    glGetnPolygonStippleARB(
        pattern.size(),
        pattern.size() ? reinterpret_cast<GLubyte*>(pattern.data()) : nullptr);
    detail::error_check("GetnPolygonStippleARB"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_void>
requires(semantic::concepts::Span<span_void>)
/*!
 * \brief Part of GL_ARB_robustness
 * \param target GLenum
 * \param format GLenum
 * \param type GLenum
 * \param rowBufSize GLsizei
 * \param row void *
 * \param columnBufSize GLsizei
 * \param column void *
 * \param span void *
 * \return void
 */
STATICINLINE void getn_separable_filter(
    group::separable_target_ext target,
    group::pixel_format         format,
    group::pixel_type           type,
    span_void                   row,
    span_void                   column,
    span_void                   span)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnSeparableFilterARB)
    }
    glGetnSeparableFilterARB(
        static_cast<GLenum>(target),
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        row.size() * sizeof(typename std::decay_t<span_void>::value_type),
        row.size() ? reinterpret_cast<void*>(row.data()) : nullptr,
        column.size() * sizeof(typename std::decay_t<span_void>::value_type),
        column.size() ? reinterpret_cast<void*>(column.data()) : nullptr,
        span.size() ? reinterpret_cast<void*>(span.data()) : nullptr);
    detail::error_check("GetnSeparableFilterARB"sv);
}

#endif
} // namespace gl::arb::robustness
#endif // GL_ARB_robustness
namespace gl::arb::robustness {
constexpr auto name = "GL_ARB_robustness";
}
