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
constexpr libc_types::u32 stack_overflow = 0x0503;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 stack_underflow = 0x0504;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 vertex_array = 0x8074;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 debug_output_synchronous = 0x8242;
#endif
#if defined(GL_VERSION_1_0)
constexpr libc_types::u32 debug_next_logged_message_length = 0x8243;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 debug_next_logged_message_length = 0x8243;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 debug_callback_function = 0x8244;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 debug_callback_user_param = 0x8245;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 debug_source_api = 0x8246;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 debug_source_window_system = 0x8247;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 debug_source_shader_compiler = 0x8248;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 debug_source_third_party = 0x8249;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 debug_source_application = 0x824A;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 debug_source_other = 0x824B;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 debug_type_error = 0x824C;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 debug_type_deprecated_behavior = 0x824D;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 debug_type_undefined_behavior = 0x824E;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 debug_type_portability = 0x824F;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 debug_type_performance = 0x8250;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 debug_type_other = 0x8251;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 debug_type_marker = 0x8268;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 debug_type_push_group = 0x8269;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 debug_type_pop_group = 0x826A;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 debug_severity_notification = 0x826B;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 max_debug_group_stack_depth = 0x826C;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 debug_group_stack_depth = 0x826D;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 buffer = 0x82E0;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 shader = 0x82E1;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 program = 0x82E2;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 query = 0x82E3;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 program_pipeline = 0x82E4;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 sampler = 0x82E6;
#endif
#if defined(GL_VERSION_1_0)
constexpr libc_types::u32 display_list = 0x82E7;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 max_label_length = 0x82E8;
#endif
#if defined(GL_VERSION_1_0)
constexpr libc_types::u32 max_debug_message_length = 0x9143;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 max_debug_message_length = 0x9143;
#endif
#if defined(GL_VERSION_1_0)
constexpr libc_types::u32 max_debug_logged_messages = 0x9144;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 max_debug_logged_messages = 0x9144;
#endif
#if defined(GL_VERSION_1_0)
constexpr libc_types::u32 debug_logged_messages = 0x9145;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 debug_logged_messages = 0x9145;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 debug_severity_high = 0x9146;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 debug_severity_medium = 0x9147;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 debug_severity_low = 0x9148;
#endif
#if defined(GL_ES_VERSION_2_0)
constexpr libc_types::u32 debug_output = 0x92E0;
#endif
} // namespace values
#if defined(GL_VERSION_1_0)
template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void debug_message_callback(
        GLDEBUGPROC callback, span_const_void const& userParam)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DebugMessageCallback)
    }
    glDebugMessageCallback(
        callback,
        userParam.size() ? reinterpret_cast<const void*>(userParam.data())
                         : nullptr);
    detail::error_check("DebugMessageCallback"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void debug_message_control(
        group::debug_source   source,
        group::debug_type     type,
        group::debug_severity severity,
        span_const_u32 const& ids,
        bool                  enabled)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DebugMessageControl)
    }
    glDebugMessageControl(
        static_cast<GLenum>(source),
        static_cast<GLenum>(type),
        static_cast<GLenum>(severity),
        ids.size(),
        ids.size() ? reinterpret_cast<const GLuint*>(ids.data()) : nullptr,
        enabled);
    detail::error_check("DebugMessageControl"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void debug_message_insert(
    group::debug_source     source,
    group::debug_type       type,
    u32                     id,
    group::debug_severity   severity,
    i32                     length,
    std::string_view const& buf)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DebugMessageInsert)
    }
    glDebugMessageInsert(
        static_cast<GLenum>(source),
        static_cast<GLenum>(type),
        id,
        static_cast<GLenum>(severity),
        length,
        buf.data());
    detail::error_check("DebugMessageInsert"sv);
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
    semantic::concepts::Span<span_debug_source>&& std::is_same_v<
        std::decay_t<typename span_debug_source::value_type>,
        std::decay_t<group::debug_source>>&&
        semantic::concepts::Span<span_debug_type>&& std::is_same_v<
            std::decay_t<typename span_debug_type::value_type>,
            std::decay_t<group::debug_type>>&&
            semantic::concepts::Span<span_u32>&& std::is_same_v<
                std::decay_t<typename span_u32::value_type>,
                std::decay_t<u32>>&&
                semantic::concepts::Span<span_debug_severity>&& std::is_same_v<
                    std::decay_t<typename span_debug_severity::value_type>,
                    std::decay_t<group::debug_severity>>&&
                    semantic::concepts::Span<span_i32>&& std::is_same_v<
                        std::decay_t<typename span_i32::value_type>,
                        std::decay_t<i32>>&&
                        semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                            std::decay_t<typename span_GLchar::value_type>,
                            std::decay_t<GLchar>>) STATICINLINE GLuint
    get_debug_message_log(
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
        GLW_FPTR_CHECK(GetDebugMessageLog)
    }
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
    detail::error_check("GetDebugMessageLog"sv);
    return out;
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_GLchar>
requires(semantic::concepts::Span<span_GLchar>&& std::is_same_v<
         std::decay_t<typename span_GLchar::value_type>,
         std::decay_t<GLchar>>) STATICINLINE
    void get_object_label(
        group::object_identifier identifier,
        u32                      name,
        i32&                     length,
        span_GLchar              label)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetObjectLabel)
    }
    glGetObjectLabel(
        static_cast<GLenum>(identifier),
        name,
        label.size(),
        &length,
        label.data());
    detail::error_check("GetObjectLabel"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_GLchar, class span_const_void>
requires(semantic::concepts::Span<span_const_void>&&
             semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>) STATICINLINE
    void get_object_ptr_label(
        span_const_void const& ptr, i32& length, span_GLchar label)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetObjectPtrLabel)
    }
    glGetObjectPtrLabel(
        ptr.size() ? reinterpret_cast<const void*>(ptr.data()) : nullptr,
        label.size(),
        &length,
        label.data());
    detail::error_check("GetObjectPtrLabel"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_void>
requires(semantic::concepts::Span<span_void>) STATICINLINE
    void get_pointerv(group::get_pointerv_prop pname, span_void params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPointerv)
    }
    glGetPointerv(
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<void**>(params.data()) : nullptr);
    detail::error_check("GetPointerv"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void object_label(
    group::object_identifier identifier,
    u32                      name,
    i32                      length,
    std::string_view const&  label)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ObjectLabel)
    }
    glObjectLabel(static_cast<GLenum>(identifier), name, length, label.data());
    detail::error_check("ObjectLabel"sv);
}

#endif
#if defined(GL_VERSION_1_0)
template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void object_ptr_label(
        span_const_void const& ptr, i32 length, std::string_view const& label)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ObjectPtrLabel)
    }
    glObjectPtrLabel(
        ptr.size() ? reinterpret_cast<const void*>(ptr.data()) : nullptr,
        length,
        label.data());
    detail::error_check("ObjectPtrLabel"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void pop_debug_group()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PopDebugGroup)
    }
    glPopDebugGroup();
    detail::error_check("PopDebugGroup"sv);
}

#endif
#if defined(GL_VERSION_1_0)
STATICINLINE void push_debug_group(
    group::debug_source     source,
    u32                     id,
    i32                     length,
    std::string_view const& message)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PushDebugGroup)
    }
    glPushDebugGroup(static_cast<GLenum>(source), id, length, message.data());
    detail::error_check("PushDebugGroup"sv);
}

#endif
#if defined(GL_ES_VERSION_2_0)
template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void debug_message_callback(
        GLDEBUGPROCKHR callback, span_const_void const& userParam)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DebugMessageCallbackKHR)
    }
    glDebugMessageCallbackKHR(
        callback,
        userParam.size() ? reinterpret_cast<const void*>(userParam.data())
                         : nullptr);
    detail::error_check("DebugMessageCallbackKHR"sv);
}

#endif
#if defined(GL_ES_VERSION_2_0)
template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void debug_message_control(
        group::debug_source   source,
        group::debug_type     type,
        group::debug_severity severity,
        i32                   count,
        span_const_u32 const& ids,
        bool                  enabled)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DebugMessageControlKHR)
    }
    glDebugMessageControlKHR(
        static_cast<GLenum>(source),
        static_cast<GLenum>(type),
        static_cast<GLenum>(severity),
        count,
        ids.size() ? reinterpret_cast<const GLuint*>(ids.data()) : nullptr,
        enabled);
    detail::error_check("DebugMessageControlKHR"sv);
}

#endif
#if defined(GL_ES_VERSION_2_0)
STATICINLINE void debug_message_insert(
    group::debug_source     source,
    group::debug_type       type,
    u32                     id,
    group::debug_severity   severity,
    i32                     length,
    std::string_view const& buf)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DebugMessageInsertKHR)
    }
    glDebugMessageInsertKHR(
        static_cast<GLenum>(source),
        static_cast<GLenum>(type),
        id,
        static_cast<GLenum>(severity),
        length,
        buf.data());
    detail::error_check("DebugMessageInsertKHR"sv);
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
    semantic::concepts::Span<span_debug_source>&& std::is_same_v<
        std::decay_t<typename span_debug_source::value_type>,
        std::decay_t<group::debug_source>>&&
        semantic::concepts::Span<span_debug_type>&& std::is_same_v<
            std::decay_t<typename span_debug_type::value_type>,
            std::decay_t<group::debug_type>>&&
            semantic::concepts::Span<span_u32>&& std::is_same_v<
                std::decay_t<typename span_u32::value_type>,
                std::decay_t<u32>>&&
                semantic::concepts::Span<span_debug_severity>&& std::is_same_v<
                    std::decay_t<typename span_debug_severity::value_type>,
                    std::decay_t<group::debug_severity>>&&
                    semantic::concepts::Span<span_i32>&& std::is_same_v<
                        std::decay_t<typename span_i32::value_type>,
                        std::decay_t<i32>>&&
                        semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                            std::decay_t<typename span_GLchar::value_type>,
                            std::decay_t<GLchar>>) STATICINLINE GLuint
    get_debug_message_log(
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
        GLW_FPTR_CHECK(GetDebugMessageLogKHR)
    }
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
    detail::error_check("GetDebugMessageLogKHR"sv);
    return out;
}

#endif
#if defined(GL_ES_VERSION_2_0)
template<class span_GLchar, class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>&& semantic::concepts::Span<span_GLchar>&&
                              std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>) STATICINLINE
    void get_object_label(
        GLenum identifier, u32 name, span_i32 length, span_GLchar label)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetObjectLabelKHR)
    }
    glGetObjectLabelKHR(
        identifier,
        name,
        label.size(),
        length.size() ? reinterpret_cast<GLsizei*>(length.data()) : nullptr,
        label.data());
    detail::error_check("GetObjectLabelKHR"sv);
}

#endif
#if defined(GL_ES_VERSION_2_0)
template<class span_GLchar, class span_const_void>
requires(semantic::concepts::Span<span_const_void>&&
             semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                 std::decay_t<typename span_GLchar::value_type>,
                 std::decay_t<GLchar>>) STATICINLINE
    void get_object_ptr_label(
        span_const_void const& ptr, i32& length, span_GLchar label)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetObjectPtrLabelKHR)
    }
    glGetObjectPtrLabelKHR(
        ptr.size() ? reinterpret_cast<const void*>(ptr.data()) : nullptr,
        label.size(),
        &length,
        label.data());
    detail::error_check("GetObjectPtrLabelKHR"sv);
}

#endif
#if defined(GL_ES_VERSION_2_0)
template<class span_void>
requires(semantic::concepts::Span<span_void>) STATICINLINE
    void get_pointerv(GLenum pname, span_void params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetPointervKHR)
    }
    glGetPointervKHR(
        pname,
        params.size() ? reinterpret_cast<void**>(params.data()) : nullptr);
    detail::error_check("GetPointervKHR"sv);
}

#endif
#if defined(GL_ES_VERSION_2_0)
STATICINLINE void object_label(
    group::object_identifier identifier,
    u32                      name,
    i32                      length,
    std::string_view const&  label)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ObjectLabelKHR)
    }
    glObjectLabelKHR(
        static_cast<GLenum>(identifier), name, length, label.data());
    detail::error_check("ObjectLabelKHR"sv);
}

#endif
#if defined(GL_ES_VERSION_2_0)
template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>) STATICINLINE
    void object_ptr_label(
        span_const_void const& ptr, i32 length, std::string_view const& label)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ObjectPtrLabelKHR)
    }
    glObjectPtrLabelKHR(
        ptr.size() ? reinterpret_cast<const void*>(ptr.data()) : nullptr,
        length,
        label.data());
    detail::error_check("ObjectPtrLabelKHR"sv);
}

#endif
#if defined(GL_ES_VERSION_2_0)
STATICINLINE void pop_debug_group()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PopDebugGroupKHR)
    }
    glPopDebugGroupKHR();
    detail::error_check("PopDebugGroupKHR"sv);
}

#endif
#if defined(GL_ES_VERSION_2_0)
STATICINLINE void push_debug_group(
    group::debug_source     source,
    u32                     id,
    i32                     length,
    std::string_view const& message)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(PushDebugGroupKHR)
    }
    glPushDebugGroupKHR(
        static_cast<GLenum>(source), id, length, message.data());
    detail::error_check("PushDebugGroupKHR"sv);
}

#endif
} // namespace gl::khr::debug
#endif // GL_KHR_debug
namespace gl::khr::debug {
constexpr auto name = "GL_KHR_debug";
}