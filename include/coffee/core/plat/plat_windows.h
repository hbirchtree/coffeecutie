#pragma once

#include "plat_primary_identify.h"
#include "../coffee_mem_macros.h"
#include "../types/tdef/stltypes.h"

#ifdef COFFEE_WINDOWS

#undef NOMINMAX
#define NOMINMAX

#undef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>

#include <windows.h>

#undef far
#undef near
#undef minor
#undef major

#undef CreateWindow
#endif

namespace Coffee {
	inline CString win_strerror(DWORD err)
	{
#ifdef COFFEE_WINDOWS_UWP
		LPWSTR msgBuf = nullptr;
#else
		LPSTR msgBuf = nullptr;
#endif
		size_t size = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER
			| FORMAT_MESSAGE_FROM_SYSTEM
			| FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, err,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
#ifdef COFFEE_WINDOWS_UWP
			(LPWSTR)&msgBuf, 0, nullptr);
#else
			(LPSTR)&msgBuf, 0, nullptr);
#endif

#ifdef COFFEE_WINDOWS_UWP
		CWString error_w(msgBuf, size);
		LocalFree(msgBuf);
		CString error(error_w.begin(),error_w.end());
#else
		CString error(msgBuf, size);
		LocalFree(msgBuf);
#endif
		return error;
	}
}
