#pragma once

#include <peripherals/identify/compiler.h>

#if defined(COFFEE_GCC) || defined(COFFEE_CLANG)
#define C_FORCE_PACKING __attribute__((packed))
#elif defined(COFFEE_MSVCXX)
//#include <peripherals/platform/windows.h>
//TODO: Fix struct packing on Windows, this is broken
#define C_FORCE_PACKING //UNALIGNED
#else
#error "You done diddled it again; there is no packing defined!"
#endif

/* Forced packing, required for some types to work (uint24 etc.) */
#if defined(COFFEE_MSVCXX)
#define PACKEDSTRUCT(struct_decl) __pragma( pack(push, 1) ) struct struct_decl __pragma( pack(pop) )
#else
#define PACKEDSTRUCT(struct_decl) struct C_FORCE_PACKING struct_decl
#endif

#define PACKED(decl) decl C_FORCE_PACKING
