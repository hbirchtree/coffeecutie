#pragma once

#ifdef GL_KHR_robustness
#include "../enums/ErrorCode.h"
#include "../enums/GraphicsResetStatus.h"
namespace gl::khr::robustness {
using gl::group::error_code;
using gl::group::graphics_reset_status;
namespace values {
#if defined(GL_VERSION_1_0)
constexpr libc_types::u32 context_lost = 0x0507;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 context_lost = 0x0507;
#endif
#if defined(GL_VERSION_1_0)
constexpr libc_types::u32 lose_context_on_reset = 0x8252;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 lose_context_on_reset = 0x8252;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 guilty_context_reset = 0x8253;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 innocent_context_reset = 0x8254;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 unknown_context_reset = 0x8255;
#endif
#if defined(GL_VERSION_1_0)
constexpr libc_types::u32 reset_notification_strategy = 0x8256;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 reset_notification_strategy = 0x8256;
#endif
#if defined(GL_VERSION_1_0)
constexpr libc_types::u32 no_reset_notification = 0x8261;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 no_reset_notification = 0x8261;
#endif
#if defined(GL_VERSION_1_0)
constexpr libc_types::u32 context_robust_access = 0x90F3;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 context_robust_access = 0x90F3;
#endif
} // namespace values
#if defined(GL_VERSION_1_0)
STATICINLINE GLenum get_graphics_reset_status()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetGraphicsResetStatus)
    }
    auto out = glGetGraphicsResetStatus();
    detail::error_check("GetGraphicsResetStatus"sv);
    return out;
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void getn_uniformfv(u32 program, i32 location, i32 bufSize, span_f32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformfv)
        glIsProgram(program);
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
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void getn_uniformiv(u32 program, i32 location, i32 bufSize, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformiv)
        glIsProgram(program);
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
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void getn_uniformuiv(
        u32 program, i32 location, i32 bufSize, span_u32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformuiv)
        glIsProgram(program);
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
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                 semantic::concepts::Size2D<size_2_i32, i32>&&
                 semantic::concepts::Span<span_void>) STATICINLINE
    void readn_pixels(
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
        x.x(),
        x.y(),
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
STATICINLINE GLenum get_graphics_reset_status()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetGraphicsResetStatusKHR)
    }
    auto out = glGetGraphicsResetStatusKHR();
    detail::error_check("GetGraphicsResetStatusKHR"sv);
    return out;
}

#endif
#if defined(GL_ES_VERSION_2_0)
template<class span_f32>
requires(semantic::concepts::Span<span_f32>&& std::is_same_v<
         std::decay_t<typename span_f32::value_type>,
         std::decay_t<f32>>) STATICINLINE
    void getn_uniformfv(u32 program, i32 location, i32 bufSize, span_f32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformfvKHR)
        glIsProgram(program);
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
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void getn_uniformiv(u32 program, i32 location, i32 bufSize, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformivKHR)
        glIsProgram(program);
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
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void getn_uniformuiv(
        u32 program, i32 location, i32 bufSize, span_u32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetnUniformuivKHR)
        glIsProgram(program);
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
requires(semantic::concepts::Vector<vec_2_i32, i32, 2>&&
                 semantic::concepts::Size2D<size_2_i32, i32>&&
                 semantic::concepts::Span<span_void>) STATICINLINE
    void readn_pixels(
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
        x.x(),
        x.y(),
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
