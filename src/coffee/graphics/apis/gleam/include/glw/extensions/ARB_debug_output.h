#pragma once

#ifdef GL_ARB_debug_output
namespace gl::arb::debug_output {
namespace values {
constexpr libc_types::u32 debug_output_synchronous         = 0x8242;
constexpr libc_types::u32 debug_next_logged_message_length = 0x8243;
constexpr libc_types::u32 debug_callback_function          = 0x8244;
constexpr libc_types::u32 debug_callback_user_param        = 0x8245;
constexpr libc_types::u32 debug_source_api                 = 0x8246;
constexpr libc_types::u32 debug_source_window_system       = 0x8247;
constexpr libc_types::u32 debug_source_shader_compiler     = 0x8248;
constexpr libc_types::u32 debug_source_third_party         = 0x8249;
constexpr libc_types::u32 debug_source_application         = 0x824A;
constexpr libc_types::u32 debug_source_other               = 0x824B;
constexpr libc_types::u32 debug_type_error                 = 0x824C;
constexpr libc_types::u32 debug_type_deprecated_behavior   = 0x824D;
constexpr libc_types::u32 debug_type_undefined_behavior    = 0x824E;
constexpr libc_types::u32 debug_type_portability           = 0x824F;
constexpr libc_types::u32 debug_type_performance           = 0x8250;
constexpr libc_types::u32 debug_type_other                 = 0x8251;
constexpr libc_types::u32 max_debug_message_length         = 0x9143;
constexpr libc_types::u32 max_debug_logged_messages        = 0x9144;
constexpr libc_types::u32 debug_logged_messages            = 0x9145;
constexpr libc_types::u32 debug_severity_high              = 0x9146;
constexpr libc_types::u32 debug_severity_medium            = 0x9147;
constexpr libc_types::u32 debug_severity_low               = 0x9148;
} // namespace values
template<class span_const_void>
requires(semantic::concepts::Span<span_const_void>)
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
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
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
