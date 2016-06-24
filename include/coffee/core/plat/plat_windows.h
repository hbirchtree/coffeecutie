#pragma once

#include "plat_primary_identify.h"
#include "../coffee_mem_macros.h"
#include "../types/tdef/stltypes.h"

#ifdef COFFEE_WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <Windows.h>
#include <VersionHelpers.h>

#undef far
#undef near
#undef minor
#undef major

#undef min
#undef max

#undef CreateWindow
#endif

namespace Coffee {
	inline CString win_strerror(DWORD err)
	{
		LPSTR msgBuf = nullptr;
		size_t size = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER
			| FORMAT_MESSAGE_FROM_SYSTEM
			| FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, err,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR)&msgBuf, 0, nullptr);
		CString error(msgBuf, size);
		LocalFree(msgBuf);
		return error;
	}
}