#pragma once

#include <peripherals/identify/compiler.h>

#if defined(COFFEE_GCC) || defined(COFFEE_CLANG)
#define C_FORCE_PACKING __attribute__((packed))
#elif defined(COFFEE_MSVCXX)
#include "plat/plat_windows.h"
//TODO: Fix struct packing on Windows, this is broken
#define C_FORCE_PACKING //UNALIGNED
#else
#error "You done diddled it again; there is no packing defined!"
#endif
/* Forced packing, required for some types to work (uint24 etc.) */
#define PACKEDSTRUCT struct C_FORCE_PACKING