#pragma once

#ifdef GL_ARB_debug_output
#include "../enums/DebugSeverity.h"
#include "../enums/DebugSource.h"
#include "../enums/DebugType.h"
#include "../enums/EnableCap.h"

namespace gl::arb::debug_output {
using gl::group::debug_severity;
using gl::group::debug_source;
using gl::group::debug_type;
using gl::group::enable_cap;

namespace values {
constexpr u32 debug_next_logged_message_length = 0x8243;
constexpr u32 debug_callback_function          = 0x8244;
constexpr u32 debug_callback_user_param        = 0x8245;
constexpr u32 debug_source_api                 = 0x8246;
constexpr u32 max_debug_message_length         = 0x9143;
constexpr u32 max_debug_logged_messages        = 0x9144;
constexpr u32 debug_logged_messages            = 0x9145;
} // namespace values

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_ARB_debug_output
 * \param callback GLDEBUGPROCARB
 * \param userParam const void *
 * \return void
 */
STATICINLINE void debug_message_callback(
    GLDEBUGPROCARB callback, span_const_void const& userParam)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DebugMessageCallbackARB)
    }
    glDebugMessageCallbackARB(
        callback,
        userParam.size() ? reinterpret_cast<const void*>(userParam.data())
                         : nullptr);
    detail::error_check("DebugMessageCallbackARB"sv);
}

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_ARB_debug_output
 * \param source GLenum
 * \param type GLenum
 * \param severity GLenum
 * \param count GLsizei
 * \param ids const GLuint *
 * \param enabled GLboolean
 * \return void
 */
STATICINLINE void debug_message_control(
    group::debug_source   source,
    group::debug_type     type,
    group::debug_severity severity,
    span_const_u32 const& ids,
    bool                  enabled)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DebugMessageControlARB)
    }
    glDebugMessageControlARB(
        static_cast<GLenum>(source),
        static_cast<GLenum>(type),
        static_cast<GLenum>(severity),
        ids.size(),
        ids.size() ? reinterpret_cast<const GLuint*>(ids.data()) : nullptr,
        enabled);
    detail::error_check("DebugMessageControlARB"sv);
}

/*!
 * \brief Part of GL_ARB_debug_output
 * \param source GLenum
 * \param type GLenum
 * \param id GLuint
 * \param severity GLenum
 * \param length GLsizei
 * \param buf const GLchar *
 * \return void
 */
STATICINLINE void debug_message_insert(
    group::debug_source     source,
    group::debug_type       type,
    u32                     id,
    group::debug_severity   severity,
    std::string_view const& buf)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DebugMessageInsertARB)
    }
    glDebugMessageInsertARB(
        static_cast<GLenum>(source),
        static_cast<GLenum>(type),
        id,
        static_cast<GLenum>(severity),
        buf.size(),
        buf.data());
    detail::error_check("DebugMessageInsertARB"sv);
}

template<
    class span_GLchar,
    class span_debug_severity,
    class span_debug_source,
    class span_debug_type,
    class span_i32,
    class span_u32>
requires(
    concepts::span<span_debug_source> &&
    std::is_same_v<
        std::decay_t<typename span_debug_source::value_type>,
        std::decay_t<group::debug_source>> &&
    concepts::span<span_debug_type> &&
    std::is_same_v<
        std::decay_t<typename span_debug_type::value_type>,
        std::decay_t<group::debug_type>> &&
    concepts::span<span_u32> &&
    std::is_same_v<
        std::decay_t<typename span_u32::value_type>,
        std::decay_t<u32>> &&
    concepts::span<span_debug_severity> &&
    std::is_same_v<
        std::decay_t<typename span_debug_severity::value_type>,
        std::decay_t<group::debug_severity>> &&
    concepts::span<span_i32> &&
    std::is_same_v<
        std::decay_t<typename span_i32::value_type>,
        std::decay_t<i32>> &&
    concepts::span<span_GLchar> &&
    std::is_same_v<
        std::decay_t<typename span_GLchar::value_type>,
        std::decay_t<GLchar>>)
/*!
 * \brief Part of GL_ARB_debug_output
 * \param count GLuint
 * \param bufSize GLsizei
 * \param sources GLenum *
 * \param types GLenum *
 * \param ids GLuint *
 * \param severities GLenum *
 * \param lengths GLsizei *
 * \param messageLog GLchar *
 * \return GLuint
 */
STATICINLINE GLuint get_debug_message_log(
    span_debug_source   sources,
    span_debug_type     types,
    span_u32            ids,
    span_debug_severity severities,
    span_i32            lengths,
    span_GLchar         messageLog)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetDebugMessageLogARB)
    }
    auto out = glGetDebugMessageLogARB(
        sources.size(),
        messageLog.size(),
        sources.size() ? reinterpret_cast<GLenum*>(sources.data()) : nullptr,
        types.size() ? reinterpret_cast<GLenum*>(types.data()) : nullptr,
        ids.size() ? reinterpret_cast<GLuint*>(ids.data()) : nullptr,
        severities.size() ? reinterpret_cast<GLenum*>(severities.data())
                          : nullptr,
        lengths.size() ? reinterpret_cast<GLsizei*>(lengths.data()) : nullptr,
        messageLog.data());
    detail::error_check("GetDebugMessageLogARB"sv);
    return out;
}

} // namespace gl::arb::debug_output
#endif // GL_ARB_debug_output
namespace gl::arb::debug_output {
constexpr auto name = "GL_ARB_debug_output";
}
