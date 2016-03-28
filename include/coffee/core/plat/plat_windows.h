#pragma once

#include "plat_primary_identify.h"

#ifdef COFFEE_WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <Windows.h>

#undef far
#undef near
#undef minor
#undef major

#undef min
#undef max

#undef CreateWindow
#endif