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

struct win_handle
{
    win_handle() : m_hnd(0)
    {
    }

    win_handle(HANDLE wHnd) : m_hnd(wHnd)
    {
    }

    win_handle(win_handle&& other) : m_hnd(other.m_hnd)
    {
        other.m_hnd = INVALID_HANDLE_VALUE;
    }

    ~win_handle()
    {
        if(*this)
            CloseHandle(m_hnd);
    }

    bool close()
    {
        bool stat = CloseHandle(m_hnd) == TRUE;
        m_hnd = 0;

		return stat;
    }

    win_handle& operator=(win_handle&& other)
    {
        m_hnd       = other.m_hnd;
        other.m_hnd = INVALID_HANDLE_VALUE;
        return *this;
    }

    win_handle& operator=(HANDLE wHnd)
    {
        m_hnd = wHnd;
        return *this;
    }

    explicit operator HANDLE() const
    {
        return m_hnd;
    }

    operator bool() const
    {
        return m_hnd != 0 && m_hnd != INVALID_HANDLE_VALUE;
    }

  private:
    HANDLE m_hnd;
};

} // namespace Coffee
