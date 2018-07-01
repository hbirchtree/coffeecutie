#pragma once

#include "../coffee_mem_macros.h"
#include "plat_primary_identify.h"
#include <string>

#ifdef COFFEE_WINDOWS

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
//#undef far
//#undef near
//#undef minor
//#undef major

//#undef CreateWindow
#endif

namespace Coffee {
inline std::string win_strerror(DWORD err)
{
#ifdef COFFEE_WINDOWS_UWP
    LPWSTR msgBuf = nullptr;
#else
    LPSTR msgBuf = nullptr;
#endif
    size_t size = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        err,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
#ifdef COFFEE_WINDOWS_UWP
        (LPWSTR)&msgBuf,
#else
        (LPSTR)&msgBuf,
#endif
        0,
        nullptr);

#ifdef COFFEE_WINDOWS_UWP
    std::wstring error_w(msgBuf, size);
    std::string  error(error_w.begin(), error_w.end());
#else
    std::string error(msgBuf, size);
#endif
    LocalFree(msgBuf);
    return error;
}

struct win_handle
{
    FORCEDINLINE win_handle() : m_hnd(0)
    {
    }

    FORCEDINLINE win_handle(HANDLE wHnd) : m_hnd(wHnd)
    {
    }

    FORCEDINLINE ~win_handle()
    {
        if(!(*this))
            CloseHandle(m_hnd);
    }

    FORCEDINLINE win_handle& operator=(HANDLE wHnd)
    {
        m_hnd = wHnd;
        return *this;
    }

    FORCEDINLINE operator HANDLE()
    {
        return m_hnd;
    }

    FORCEDINLINE operator bool()
    {
        return m_hnd != 0 && m_hnd != INVALID_HANDLE_VALUE;
    }

  private:
    HANDLE m_hnd;
};

} // namespace Coffee
