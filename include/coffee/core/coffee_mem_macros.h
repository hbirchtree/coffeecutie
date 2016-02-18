#ifndef COFFEE_CORE_MEM_MACROS_H
#define COFFEE_CORE_MEM_MACROS_H

#include "plat/platform_detect.h"

#define C_DELETE_COPY_CONSTRUCTOR(ctype) ctype(ctype const&) = delete

#if defined(COFFEE_GCC) || defined(COFFEE_CLANG)
#define C_FORCE_PACKING __attribute__((packed))
#elif defined(COFFEE_MSVCXX)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define C_FORCE_PACKING UNALIGNED
#else
#error "Packing of structures not defined! Cannot function correctly!"
#endif

#if defined(COFFEE_GCC) || defined(COFFEE_CLANG)
#define C_FORCE_INLINE __attribute__((always_inline))
#elif defined(COFFEE_MSVCXX)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define C_FORCE_INLINE __forceinline
#else
#define C_FORCE_INLINE
#endif

#define STATICINLINE inline C_FORCE_INLINE static
#define FORCEDINLINE inline C_FORCE_INLINE
#define PACKEDSTRUCT struct C_FORCE_PACKING

#endif
