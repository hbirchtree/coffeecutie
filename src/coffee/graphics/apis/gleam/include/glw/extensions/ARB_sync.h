#pragma once

#ifdef GL_ARB_sync
#include "../enums/GetPName.h"
#include "../enums/SyncCondition.h"
#include "../enums/SyncObjectMask.h"
#include "../enums/SyncParameterName.h"
#include "../enums/SyncStatus.h"
namespace gl::arb::sync {
using gl::group::get_prop;
using gl::group::sync_condition;
using gl::group::sync_object_mask;
using gl::group::sync_parameter_name;
using gl::group::sync_status;
namespace values {
constexpr libc_types::u32 timeout_ignored = 0xFFFFFFFFFFFFFFFF;
constexpr libc_types::u32 sync_fence      = 0x9116;
constexpr libc_types::u32 unsignaled      = 0x9118;
constexpr libc_types::u32 signaled        = 0x9119;
} // namespace values
STATICINLINE GLenum
client_wait_sync(GLsync sync, group::sync_object_mask flags, u64 timeout)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ClientWaitSync)
    }
    auto out = glClientWaitSync(sync, static_cast<GLenum>(flags), timeout);
    detail::error_check("ClientWaitSync"sv);
    return out;
}

STATICINLINE void delete_sync(GLsync sync)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteSync)
    }
    glDeleteSync(sync);
    detail::error_check("DeleteSync"sv);
}

STATICINLINE GLsync
fence_sync(group::sync_condition condition, group::sync_behavior_flags flags)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FenceSync)
    }
    auto out =
        glFenceSync(static_cast<GLenum>(condition), static_cast<GLenum>(flags));
    detail::error_check("FenceSync"sv);
    return out;
}

template<class span_i64>
requires(semantic::concepts::Span<span_i64>&& std::is_same_v<
         std::decay_t<typename span_i64::value_type>,
         std::decay_t<i64>>) STATICINLINE
    void get_integer64v(group::get_prop pname, span_i64 data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetInteger64v)
    }
    glGetInteger64v(
        static_cast<GLenum>(pname),
        data.size() ? reinterpret_cast<GLint64*>(data.data()) : nullptr);
    detail::error_check("GetInteger64v"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_synciv(
        GLsync                     sync,
        group::sync_parameter_name pname,
        i32&                       length,
        span_i32                   values)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetSynciv)
    }
    glGetSynciv(
        sync,
        static_cast<GLenum>(pname),
        values.size(),
        &length,
        values.size() ? reinterpret_cast<GLint*>(values.data()) : nullptr);
    detail::error_check("GetSynciv"sv);
}

STATICINLINE GLboolean is_sync(GLsync sync)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsSync)
    }
    auto out = glIsSync(sync);
    detail::error_check("IsSync"sv);
    return out;
}

STATICINLINE void wait_sync(
    GLsync sync, group::sync_behavior_flags flags, u64 timeout)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(WaitSync)
    }
    glWaitSync(sync, static_cast<GLenum>(flags), timeout);
    detail::error_check("WaitSync"sv);
}

} // namespace gl::arb::sync
#endif // GL_ARB_sync
namespace gl::arb::sync {
constexpr auto name = "GL_ARB_sync";
}
