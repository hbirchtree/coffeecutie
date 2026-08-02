#pragma once

#ifdef GL_KHR_debug
#include "../enums/ContextFlagMask.h"
#include "../enums/DebugSeverity.h"
#include "../enums/DebugSource.h"
#include "../enums/DebugType.h"
#include "../enums/EnableCap.h"
#include "../enums/ErrorCode.h"
#include "../enums/GetPName.h"
#include "../enums/GetPointervPName.h"
#include "../enums/ObjectIdentifier.h"

namespace gl::khr::debug {
using gl::group::context_flag_mask;
using gl::group::debug_severity;
using gl::group::debug_source;
using gl::group::debug_type;
using gl::group::enable_cap;
using gl::group::error_code;
using gl::group::get_pointerv_prop;
using gl::group::get_prop;
using gl::group::object_identifier;

namespace values {
#if defined(GL_ES_VERSION_2_0)
constexpr u32 stack_overflow = 0x0503;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr u32 stack_underflow = 0x0504;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr u32 vertex_array = 0x8074;
#endif
#if defined(GL_VERSION_1_0)
constexpr u32 debug_next_logged_message_length = 0x8243;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr u32 debug_next_logged_message_length = 0x8243;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr u32 debug_callback_function = 0x8244;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr u32 debug_callback_user_param = 0x8245;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr u32 debug_source_api = 0x8246;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr u32 max_debug_group_stack_depth = 0x826C;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr u32 debug_group_stack_depth = 0x826D;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr u32 buffer = 0x82E0;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr u32 shader = 0x82E1;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr u32 program = 0x82E2;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr u32 query = 0x82E3;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr u32 program_pipeline = 0x82E4;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr u32 sampler = 0x82E6;
#endif
#if defined(GL_VERSION_1_0)
constexpr u32 display_list = 0x82E7;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr u32 max_label_length = 0x82E8;
#endif
#if defined(GL_VERSION_1_0)
constexpr u32 max_debug_message_length = 0x9143;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr u32 max_debug_message_length = 0x9143;
#endif
#if defined(GL_VERSION_1_0)
constexpr u32 max_debug_logged_messages = 0x9144;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr u32 max_debug_logged_messages = 0x9144;
#endif
#if defined(GL_VERSION_1_0)
constexpr u32 debug_logged_messages = 0x9145;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr u32 debug_logged_messages = 0x9145;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr u32 debug_output = 0x92E0;
#endif
} // namespace values
#if defined(GL_VERSION_1_0)
template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_KHR_debug
 * \param callback GLDEBUGPROC
 * \param userParam const void *
 * \return void
 */
STATICINLINE void debug_message_callback(
    GLDEBUGPROC            callback,
    span_const_void const& userParam,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(DebugMessageCallback)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glDebugMessageCallback,
        gsl::span<const char>(
            reinterpret_cast<const char*>(userParam.data()),
            userParam.size_bytes()),
        callback);
#endif
    glDebugMessageCallback(
        callback,
        userParam.size() ? reinterpret_cast<const void*>(userParam.data())
                         : nullptr);
    detail::error_check("DebugMessageCallback"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_KHR_debug
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
    bool                  enabled,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(DebugMessageControl)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glDebugMessageControl,
        gsl::span<const char>(
            reinterpret_cast<const char*>(ids.data()), ids.size_bytes()),
        source,
        type,
        severity,
        enabled);
#endif
    glDebugMessageControl(
        static_cast<GLenum>(source),
        static_cast<GLenum>(type),
        static_cast<GLenum>(severity),
        ids.size(),
        ids.size() ? reinterpret_cast<const GLuint*>(ids.data()) : nullptr,
        enabled);
    detail::error_check("DebugMessageControl"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_KHR_debug
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
    i32                     length,
    std::string_view const& buf,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(DebugMessageInsert)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glDebugMessageInsert, source, type, id, severity, length, buf);
#endif
    glDebugMessageInsert(
        static_cast<GLenum>(source),
        static_cast<GLenum>(type),
        id,
        static_cast<GLenum>(severity),
        length,
        buf.data());
    detail::error_check("DebugMessageInsert"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
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
 * \brief Part of GL_KHR_debug
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
    span_GLchar         messageLog,
    error_check         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetDebugMessageLog)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glGetDebugMessageLog,
        gsl::span<char>(
            reinterpret_cast<char*>(sources.data()), sources.size_bytes()),
        gsl::span<char>(
            reinterpret_cast<char*>(types.data()), types.size_bytes()),
        gsl::span<char>(reinterpret_cast<char*>(ids.data()), ids.size_bytes()),
        gsl::span<char>(
            reinterpret_cast<char*>(severities.data()),
            severities.size_bytes()),
        gsl::span<char>(
            reinterpret_cast<char*>(lengths.data()), lengths.size_bytes()),
        gsl::span<char>(
            reinterpret_cast<char*>(messageLog.data()),
            messageLog.size_bytes()));
#endif
    auto out = glGetDebugMessageLog(
        sources.size(),
        messageLog.size(),
        sources.size() ? reinterpret_cast<GLenum*>(sources.data()) : nullptr,
        types.size() ? reinterpret_cast<GLenum*>(types.data()) : nullptr,
        ids.size() ? reinterpret_cast<GLuint*>(ids.data()) : nullptr,
        severities.size() ? reinterpret_cast<GLenum*>(severities.data())
                          : nullptr,
        lengths.size() ? reinterpret_cast<GLsizei*>(lengths.data()) : nullptr,
        messageLog.data());
    detail::error_check("GetDebugMessageLog"sv, check_errors);
    return out;
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_GLchar>
requires(
    concepts::span<span_GLchar> &&
    std::is_same_v<
        std::decay_t<typename span_GLchar::value_type>,
        std::decay_t<GLchar>>)
/*!
 * \brief Part of GL_KHR_debug
 * \param identifier GLenum
 * \param name GLuint
 * \param bufSize GLsizei
 * \param length GLsizei *
 * \param label GLchar *
 * \return void
 */
STATICINLINE void get_object_label(
    group::object_identifier identifier,
    u32                      name,
    i32&                     length,
    span_GLchar              label,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetObjectLabel)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetObjectLabel,
        gsl::span<char>(
            reinterpret_cast<char*>(label.data()), label.size_bytes()),
        identifier,
        name,
        length);
#endif
    glGetObjectLabel(
        static_cast<GLenum>(identifier),
        name,
        label.size(),
        &length,
        label.data());
    detail::error_check("GetObjectLabel"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_GLchar, class span_const_void>
requires(
    concepts::span<span_const_void> && concepts::span<span_GLchar> &&
    std::is_same_v<
        std::decay_t<typename span_GLchar::value_type>,
        std::decay_t<GLchar>>)
/*!
 * \brief Part of GL_KHR_debug
 * \param ptr const void *
 * \param bufSize GLsizei
 * \param length GLsizei *
 * \param label GLchar *
 * \return void
 */
STATICINLINE void get_object_ptr_label(
    span_const_void const& ptr,
    i32&                   length,
    span_GLchar            label,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetObjectPtrLabel)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glGetObjectPtrLabel,
        gsl::span<const char>(
            reinterpret_cast<const char*>(ptr.data()), ptr.size_bytes()),
        length,
        gsl::span<char>(
            reinterpret_cast<char*>(label.data()), label.size_bytes()));
#endif
    glGetObjectPtrLabel(
        ptr.size() ? reinterpret_cast<const void*>(ptr.data()) : nullptr,
        label.size(),
        &length,
        label.data());
    detail::error_check("GetObjectPtrLabel"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_void>
requires(concepts::span<span_void>)
/*!
 * \brief Part of GL_KHR_debug
 * \param pname GLenum
 * \param params void **
 * \return void
 */
STATICINLINE void get_pointerv(
    group::get_pointerv_prop pname,
    span_void                params,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetPointerv)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetPointerv,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        pname);
#endif
    glGetPointerv(
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<void**>(params.data()) : nullptr);
    detail::error_check("GetPointerv"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_KHR_debug
 * \param identifier GLenum
 * \param name GLuint
 * \param length GLsizei
 * \param label const GLchar *
 * \return void
 */
STATICINLINE void object_label(
    group::object_identifier identifier,
    u32                      name,
    i32                      length,
    std::string_view const&  label,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ObjectLabel)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glObjectLabel, identifier, name, length, label);
#endif
    glObjectLabel(static_cast<GLenum>(identifier), name, length, label.data());
    detail::error_check("ObjectLabel"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_KHR_debug
 * \param ptr const void *
 * \param length GLsizei
 * \param label const GLchar *
 * \return void
 */
STATICINLINE void object_ptr_label(
    span_const_void const&  ptr,
    i32                     length,
    std::string_view const& label,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ObjectPtrLabel)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glObjectPtrLabel,
        gsl::span<const char>(
            reinterpret_cast<const char*>(ptr.data()), ptr.size_bytes()),
        length,
        label);
#endif
    glObjectPtrLabel(
        ptr.size() ? reinterpret_cast<const void*>(ptr.data()) : nullptr,
        length,
        label.data());
    detail::error_check("ObjectPtrLabel"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_KHR_debug

 * \return void
 */
STATICINLINE void pop_debug_group(error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(PopDebugGroup)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glPopDebugGroup);
#endif
    glPopDebugGroup();
    detail::error_check("PopDebugGroup"sv, check_errors);
}

#endif
#if defined(GL_VERSION_1_0)
/*!
 * \brief Part of GL_KHR_debug
 * \param source GLenum
 * \param id GLuint
 * \param length GLsizei
 * \param message const GLchar *
 * \return void
 */
STATICINLINE void push_debug_group(
    group::debug_source     source,
    u32                     id,
    i32                     length,
    std::string_view const& message,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(PushDebugGroup)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glPushDebugGroup, source, id, length, message);
#endif
    glPushDebugGroup(static_cast<GLenum>(source), id, length, message.data());
    detail::error_check("PushDebugGroup"sv, check_errors);
}

#endif
#if defined(GL_ES_VERSION_2_0)
template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_KHR_debug
 * \param callback GLDEBUGPROCKHR
 * \param userParam const void *
 * \return void
 */
STATICINLINE void debug_message_callback(
    GLDEBUGPROCKHR         callback,
    span_const_void const& userParam,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(DebugMessageCallbackKHR)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glDebugMessageCallbackKHR,
        gsl::span<const char>(
            reinterpret_cast<const char*>(userParam.data()),
            userParam.size_bytes()),
        callback);
#endif
    glDebugMessageCallbackKHR(
        callback,
        userParam.size() ? reinterpret_cast<const void*>(userParam.data())
                         : nullptr);
    detail::error_check("DebugMessageCallbackKHR"sv, check_errors);
}

#endif
#if defined(GL_ES_VERSION_2_0)
template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_KHR_debug
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
    i32                   count,
    span_const_u32 const& ids,
    bool                  enabled,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(DebugMessageControlKHR)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glDebugMessageControlKHR,
        gsl::span<const char>(
            reinterpret_cast<const char*>(ids.data()), ids.size_bytes()),
        source,
        type,
        severity,
        count,
        enabled);
#endif
    glDebugMessageControlKHR(
        static_cast<GLenum>(source),
        static_cast<GLenum>(type),
        static_cast<GLenum>(severity),
        count,
        ids.size() ? reinterpret_cast<const GLuint*>(ids.data()) : nullptr,
        enabled);
    detail::error_check("DebugMessageControlKHR"sv, check_errors);
}

#endif
#if defined(GL_ES_VERSION_2_0)
/*!
 * \brief Part of GL_KHR_debug
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
    i32                     length,
    std::string_view const& buf,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(DebugMessageInsertKHR)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glDebugMessageInsertKHR, source, type, id, severity, length, buf);
#endif
    glDebugMessageInsertKHR(
        static_cast<GLenum>(source),
        static_cast<GLenum>(type),
        id,
        static_cast<GLenum>(severity),
        length,
        buf.data());
    detail::error_check("DebugMessageInsertKHR"sv, check_errors);
}

#endif
#if defined(GL_ES_VERSION_2_0)
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
 * \brief Part of GL_KHR_debug
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
    span_GLchar         messageLog,
    error_check         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetDebugMessageLogKHR)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glGetDebugMessageLogKHR,
        gsl::span<char>(
            reinterpret_cast<char*>(sources.data()), sources.size_bytes()),
        gsl::span<char>(
            reinterpret_cast<char*>(types.data()), types.size_bytes()),
        gsl::span<char>(reinterpret_cast<char*>(ids.data()), ids.size_bytes()),
        gsl::span<char>(
            reinterpret_cast<char*>(severities.data()),
            severities.size_bytes()),
        gsl::span<char>(
            reinterpret_cast<char*>(lengths.data()), lengths.size_bytes()),
        gsl::span<char>(
            reinterpret_cast<char*>(messageLog.data()),
            messageLog.size_bytes()));
#endif
    auto out = glGetDebugMessageLogKHR(
        sources.size(),
        messageLog.size(),
        sources.size() ? reinterpret_cast<GLenum*>(sources.data()) : nullptr,
        types.size() ? reinterpret_cast<GLenum*>(types.data()) : nullptr,
        ids.size() ? reinterpret_cast<GLuint*>(ids.data()) : nullptr,
        severities.size() ? reinterpret_cast<GLenum*>(severities.data())
                          : nullptr,
        lengths.size() ? reinterpret_cast<GLsizei*>(lengths.data()) : nullptr,
        messageLog.data());
    detail::error_check("GetDebugMessageLogKHR"sv, check_errors);
    return out;
}

#endif
#if defined(GL_ES_VERSION_2_0)
template<class span_GLchar>
requires(
    concepts::span<span_GLchar> &&
    std::is_same_v<
        std::decay_t<typename span_GLchar::value_type>,
        std::decay_t<GLchar>>)
/*!
 * \brief Part of GL_KHR_debug
 * \param identifier GLenum
 * \param name GLuint
 * \param bufSize GLsizei
 * \param length GLsizei *
 * \param label GLchar *
 * \return void
 */
STATICINLINE void get_object_label(
    GLenum      identifier,
    u32         name,
    i32&        length,
    span_GLchar label,
    error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetObjectLabelKHR)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetObjectLabelKHR,
        gsl::span<char>(
            reinterpret_cast<char*>(label.data()), label.size_bytes()),
        identifier,
        name,
        length);
#endif
    glGetObjectLabelKHR(identifier, name, label.size(), &length, label.data());
    detail::error_check("GetObjectLabelKHR"sv, check_errors);
}

#endif
#if defined(GL_ES_VERSION_2_0)
template<class span_GLchar, class span_const_void>
requires(
    concepts::span<span_const_void> && concepts::span<span_GLchar> &&
    std::is_same_v<
        std::decay_t<typename span_GLchar::value_type>,
        std::decay_t<GLchar>>)
/*!
 * \brief Part of GL_KHR_debug
 * \param ptr const void *
 * \param bufSize GLsizei
 * \param length GLsizei *
 * \param label GLchar *
 * \return void
 */
STATICINLINE void get_object_ptr_label(
    span_const_void const& ptr,
    i32&                   length,
    span_GLchar            label,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetObjectPtrLabelKHR)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(
        glGetObjectPtrLabelKHR,
        gsl::span<const char>(
            reinterpret_cast<const char*>(ptr.data()), ptr.size_bytes()),
        length,
        gsl::span<char>(
            reinterpret_cast<char*>(label.data()), label.size_bytes()));
#endif
    glGetObjectPtrLabelKHR(
        ptr.size() ? reinterpret_cast<const void*>(ptr.data()) : nullptr,
        label.size(),
        &length,
        label.data());
    detail::error_check("GetObjectPtrLabelKHR"sv, check_errors);
}

#endif
#if defined(GL_ES_VERSION_2_0)
template<class span_void>
requires(concepts::span<span_void>)
/*!
 * \brief Part of GL_KHR_debug
 * \param pname GLenum
 * \param params void **
 * \return void
 */
STATICINLINE void get_pointerv(
    GLenum pname, span_void params, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetPointervKHR)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetPointervKHR,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        pname);
#endif
    glGetPointervKHR(
        pname,
        params.size() ? reinterpret_cast<void**>(params.data()) : nullptr);
    detail::error_check("GetPointervKHR"sv, check_errors);
}

#endif
#if defined(GL_ES_VERSION_2_0)
/*!
 * \brief Part of GL_KHR_debug
 * \param identifier GLenum
 * \param name GLuint
 * \param length GLsizei
 * \param label const GLchar *
 * \return void
 */
STATICINLINE void object_label(
    group::object_identifier identifier,
    u32                      name,
    i32                      length,
    std::string_view const&  label,
    error_check              check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ObjectLabelKHR)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glObjectLabelKHR, identifier, name, length, label);
#endif
    glObjectLabelKHR(
        static_cast<GLenum>(identifier), name, length, label.data());
    detail::error_check("ObjectLabelKHR"sv, check_errors);
}

#endif
#if defined(GL_ES_VERSION_2_0)
template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_KHR_debug
 * \param ptr const void *
 * \param length GLsizei
 * \param label const GLchar *
 * \return void
 */
STATICINLINE void object_ptr_label(
    span_const_void const&  ptr,
    i32                     length,
    std::string_view const& label,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ObjectPtrLabelKHR)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glObjectPtrLabelKHR,
        gsl::span<const char>(
            reinterpret_cast<const char*>(ptr.data()), ptr.size_bytes()),
        length,
        label);
#endif
    glObjectPtrLabelKHR(
        ptr.size() ? reinterpret_cast<const void*>(ptr.data()) : nullptr,
        length,
        label.data());
    detail::error_check("ObjectPtrLabelKHR"sv, check_errors);
}

#endif
#if defined(GL_ES_VERSION_2_0)
/*!
 * \brief Part of GL_KHR_debug

 * \return void
 */
STATICINLINE void pop_debug_group(error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(PopDebugGroupKHR)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glPopDebugGroupKHR);
#endif
    glPopDebugGroupKHR();
    detail::error_check("PopDebugGroupKHR"sv, check_errors);
}

#endif
#if defined(GL_ES_VERSION_2_0)
/*!
 * \brief Part of GL_KHR_debug
 * \param source GLenum
 * \param id GLuint
 * \param length GLsizei
 * \param message const GLchar *
 * \return void
 */
STATICINLINE void push_debug_group(
    group::debug_source     source,
    u32                     id,
    i32                     length,
    std::string_view const& message,
    error_check             check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(PushDebugGroupKHR)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glPushDebugGroupKHR, source, id, length, message);
#endif
    glPushDebugGroupKHR(
        static_cast<GLenum>(source), id, length, message.data());
    detail::error_check("PushDebugGroupKHR"sv, check_errors);
}

#endif
} // namespace gl::khr::debug
#endif // GL_KHR_debug
namespace gl::khr::debug {
constexpr auto name = "GL_KHR_debug";
}
