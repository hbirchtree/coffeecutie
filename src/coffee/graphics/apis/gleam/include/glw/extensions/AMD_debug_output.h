#pragma once

#ifdef GL_AMD_debug_output
#include "../enums/DebugSeverity.h"

namespace gl::amd::debug_output {
using gl::group::debug_severity;

namespace values {
constexpr u32 max_debug_message_length          = 0x9143;
constexpr u32 max_debug_logged_messages         = 0x9144;
constexpr u32 debug_logged_messages             = 0x9145;
constexpr u32 debug_category_api_error          = 0x9149;
constexpr u32 debug_category_window_system      = 0x914A;
constexpr u32 debug_category_deprecation        = 0x914B;
constexpr u32 debug_category_undefined_behavior = 0x914C;
constexpr u32 debug_category_performance        = 0x914D;
constexpr u32 debug_category_shader_compiler    = 0x914E;
constexpr u32 debug_category_application        = 0x914F;
constexpr u32 debug_category_other              = 0x9150;
} // namespace values

template<class span_void>
requires(concepts::span<span_void>)
/*!
 * \brief Part of GL_AMD_debug_output
 * \param callback GLDEBUGPROCAMD
 * \param userParam void *
 * \return void
 */
STATICINLINE void debug_message_callback(
    GLDEBUGPROCAMD callback,
    span_void      userParam,
    error_check    check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DebugMessageCallbackAMD)
    }
    glDebugMessageCallbackAMD(
        callback,
        userParam.size() ? reinterpret_cast<void*>(userParam.data()) : nullptr);
    detail::error_check("DebugMessageCallbackAMD"sv, check_errors);
}

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_AMD_debug_output
 * \param category GLenum
 * \param severity GLenum
 * \param count GLsizei
 * \param ids const GLuint *
 * \param enabled GLboolean
 * \return void
 */
STATICINLINE void debug_message_enable(
    GLenum                category,
    group::debug_severity severity,
    span_const_u32 const& ids,
    bool                  enabled,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DebugMessageEnableAMD)
    }
    glDebugMessageEnableAMD(
        category,
        static_cast<GLenum>(severity),
        ids.size(),
        ids.size() ? reinterpret_cast<const GLuint*>(ids.data()) : nullptr,
        enabled);
    detail::error_check("DebugMessageEnableAMD"sv, check_errors);
}

/*!
 * \brief Part of GL_AMD_debug_output
 * \param category GLenum
 * \param severity GLenum
 * \param id GLuint
 * \param length GLsizei
 * \param buf const GLchar *
 * \return void
 */
STATICINLINE void debug_message_insert(
    GLenum                  category,
    group::debug_severity   severity,
    u32                     id,
    std::string_view const& buf,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DebugMessageInsertAMD)
    }
    glDebugMessageInsertAMD(
        category, static_cast<GLenum>(severity), id, buf.size(), buf.data());
    detail::error_check("DebugMessageInsertAMD"sv, check_errors);
}

template<
    class span_GLchar,
    class span_GLenum,
    class span_debug_severity,
    class span_i32,
    class span_u32>
requires(
    concepts::span<span_GLenum> &&
    std::is_same_v<
        std::decay_t<typename span_GLenum::value_type>,
        std::decay_t<GLenum>> &&
    concepts::span<span_debug_severity> &&
    std::is_same_v<
        std::decay_t<typename span_debug_severity::value_type>,
        std::decay_t<group::debug_severity>> &&
    concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>> &&
    concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>> &&
    concepts::span<span_GLchar> &&
    std::is_same_v<
        std::decay_t<typename span_GLchar::value_type>,
        std::decay_t<GLchar>>)
/*!
 * \brief Part of GL_AMD_debug_output
 * \param count GLuint
 * \param bufSize GLsizei
 * \param categories GLenum *
 * \param severities GLenum *
 * \param ids GLuint *
 * \param lengths GLsizei *
 * \param message GLchar *
 * \return GLuint
 */
STATICINLINE GLuint get_debug_message_log(
    span_GLenum         categories,
    span_debug_severity severities,
    span_u32            ids,
    span_i32            lengths,
    span_GLchar         message,
    error_check         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetDebugMessageLogAMD)
    }
    auto out = glGetDebugMessageLogAMD(
        categories.size(),
        message.size(),
        categories.size() ? reinterpret_cast<GLenum*>(categories.data())
                          : nullptr,
        severities.size() ? reinterpret_cast<GLenum*>(severities.data())
                          : nullptr,
        ids.size() ? reinterpret_cast<GLuint*>(ids.data()) : nullptr,
        lengths.size() ? reinterpret_cast<GLsizei*>(lengths.data()) : nullptr,
        message.data());
    detail::error_check("GetDebugMessageLogAMD"sv, check_errors);
    return out;
}

} // namespace gl::amd::debug_output
#endif // GL_AMD_debug_output
namespace gl::amd::debug_output {
constexpr auto name = "GL_AMD_debug_output";
}
