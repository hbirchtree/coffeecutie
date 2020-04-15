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
namespace detail {

inline void closeHandle(HANDLE hnd)
{
    ::CloseHandle(hnd);
}

} // namespace detail
} // namespace win32

using win_handle = semantic::generic_handle_t<
    HANDLE, 
    semantic::handle_modes::auto_close, 
    INVALID_HANDLE_VALUE,
    win32::detail::closeHandle>;

} // namespace platform
