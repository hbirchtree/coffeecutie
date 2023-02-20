#pragma once

#ifdef GL_ARB_cl_event
namespace gl::arb::cl_event {
namespace values {
constexpr libc_types::u32 sync_cl_event          = 0x8240;
constexpr libc_types::u32 sync_cl_event_complete = 0x8241;
} // namespace values
template<class span__cl_context, class span__cl_event>
requires(semantic::concepts::Span<span__cl_context>&& std::is_same_v<
         std::decay_t<typename span__cl_context::value_type>,
         std::decay_t<_cl_context>>&& semantic::concepts::Span<span__cl_event>&&
                                      std::is_same_v<
                 std::decay_t<typename span__cl_event::value_type>,
                 std::decay_t<_cl_event>>)
    /*!
     * \brief Part of GL_ARB_cl_event
     * \param context struct _cl_context *
     * \param event struct _cl_event *
     * \param flags GLbitfield
     * \return sync
     */
    STATICINLINE GLsync create_sync_from_c_levent(
        span__cl_context const& context,
        span__cl_event const&   event,
        GLbitfield              flags)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CreateSyncFromCLeventARB)
    }
    auto out = glCreateSyncFromCLeventARB(
        context.size() ? reinterpret_cast<struct _cl_context*>(context.data())
                       : nullptr,
        event.size() ? reinterpret_cast<struct _cl_event*>(event.data())
                     : nullptr,
        flags);
    detail::error_check("CreateSyncFromCLeventARB"sv);
    return out;
}

} // namespace gl::arb::cl_event
#endif // GL_ARB_cl_event
namespace gl::arb::cl_event {
constexpr auto name = "GL_ARB_cl_event";
}
