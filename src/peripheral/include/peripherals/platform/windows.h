#pragma once

#include <peripherals/base.h>
#include <peripherals/semantic/handle.h>
#include <string>

#undef NOMINMAX
#define NOMINMAX

#undef DELETE

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif

#include <winsock2.h>

#include <processthreadsapi.h>
#include <windows.h>

#undef ERROR

namespace platform {
namespace win32 {

constexpr libc_types::u64 invalid_handle_value = (libc_types::u64)-1L;

inline libc_types::u64 handle_to_u64(HANDLE hnd)
{
    return C_RCAST<libc_types::u64>(hnd);
}

inline HANDLE u64_to_handle(libc_types::u64 hnd)
{
    return C_RCAST<HANDLE>(hnd);
}
    
namespace detail {
inline void closeHandle(libc_types::u64 hnd)
{
    ::CloseHandle(u64_to_handle(hnd));
}

} // namespace detail
} // namespace win32

struct win_handle : semantic::generic_handle_t<
    libc_types::u64,
    semantic::handle_modes::auto_close,
    win32::invalid_handle_value,
    win32::detail::closeHandle>
{
    using semantic::generic_handle_t<
        libc_types::u64,
        semantic::handle_modes::auto_close,
        win32::invalid_handle_value,
        win32::detail::closeHandle>::generic_handle_t;

    operator HANDLE() const
    {
        return C_RCAST<HANDLE>(hnd);
    }
};

} // namespace platform
