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
//#undef far
//#undef near
//#undef minor
//#undef major

//#undef CreateWindow

namespace Coffee {

using win_handle = semantic::generic_handle_t<HANDLE>;

} // namespace Coffee
