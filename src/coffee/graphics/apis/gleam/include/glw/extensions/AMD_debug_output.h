#pragma once

#ifdef GL_AMD_debug_output
namespace gl::amd::debug_output {
namespace values {
constexpr libc_types::u32 max_debug_message_length          = 0x9143;
constexpr libc_types::u32 max_debug_logged_messages         = 0x9144;
constexpr libc_types::u32 debug_logged_messages             = 0x9145;
constexpr libc_types::u32 debug_severity_high               = 0x9146;
constexpr libc_types::u32 debug_severity_medium             = 0x9147;
constexpr libc_types::u32 debug_severity_low                = 0x9148;
constexpr libc_types::u32 debug_category_api_error          = 0x9149;
constexpr libc_types::u32 debug_category_window_system      = 0x914A;
constexpr libc_types::u32 debug_category_deprecation        = 0x914B;
constexpr libc_types::u32 debug_category_undefined_behavior = 0x914C;
constexpr libc_types::u32 debug_category_performance        = 0x914D;
constexpr libc_types::u32 debug_category_shader_compiler    = 0x914E;
constexpr libc_types::u32 debug_category_application        = 0x914F;
constexpr libc_types::u32 debug_category_other              = 0x9150;
} // namespace values
template<class span_void>
requires(semantic::concepts::Span<span_void>) STATICINLINE
    void debug_message_callback(GLDEBUGPROCAMD callback, span_void userParam)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DebugMessageCallbackAMD)
    }
    glDebugMessageCallbackAMD(
        callback,
        userParam.size() ? reinterpret_cast<void*>(userParam.data()) : nullptr);
    detail::error_check("DebugMessageCallbackAMD"sv);
}

template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void debug_message_enable(
        GLenum                category,
        group::debug_severity severity,
        span_const_u32 const& ids,
        bool                  enabled)
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
    detail::error_check("DebugMessageEnableAMD"sv);
}

STATICINLINE void debug_message_insert(
    GLenum                  category,
    group::debug_severity   severity,
    u32                     id,
    std::string_view const& buf)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DebugMessageInsertAMD)
    }
    glDebugMessageInsertAMD(
        category, static_cast<GLenum>(severity), id, buf.size(), buf.data());
    detail::error_check("DebugMessageInsertAMD"sv);
}

template<class span_GLchar, class span_GLenum, class span_i32, class span_u32>
requires(semantic::concepts::Span<span_GLenum>&& std::is_same_v<
         std::decay_t<typename span_GLenum::value_type>,
         std::decay_t<GLenum>>&&      semantic::concepts::Span<span_u32>&&
                                      std::is_same_v<
                 std::decay_t<typename span_u32::value_type>,
                 std::decay_t<u32>>&& semantic::concepts::Span<span_i32>&&
                                      std::is_same_v<
                     std::decay_t<typename span_i32::value_type>,
                     std::decay_t<i32>>&&
                     semantic::concepts::Span<span_GLchar>&& std::is_same_v<
                         std::decay_t<typename span_GLchar::value_type>,
                         std::decay_t<GLchar>>) STATICINLINE GLuint
    get_debug_message_log(
        span_GLenum categories,
        span_u32    severities,
        span_u32    ids,
        span_i32    lengths,
        span_GLchar message)
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
        severities.size() ? reinterpret_cast<GLuint*>(severities.data())
                          : nullptr,
        ids.size() ? reinterpret_cast<GLuint*>(ids.data()) : nullptr,
        lengths.size() ? reinterpret_cast<GLsizei*>(lengths.data()) : nullptr,
        message.data());
    detail::error_check("GetDebugMessageLogAMD"sv);
    return out;
}

} // namespace gl::amd::debug_output
#endif // GL_AMD_debug_output
namespace gl::amd::debug_output {
constexpr auto name = "GL_AMD_debug_output";
}
