#ifndef COFFEE_CORE_MEM_MACROS_H
#define COFFEE_CORE_MEM_MACROS_H

//#define COFFEE_USE_EXCEPTIONS

#include "plat/platform_detect.h"

#define C_DEFAULT_CONSTRUCTOR(ctype) ctype() = default
#define C_DELETE_COPY_CONSTRUCTOR(ctype) ctype(ctype const&) = delete
#define C_MOVE_CONSTRUCTOR(ctype) ctype(ctype&& x) = default; ctype& operator=(ctype&& a) = default

#if defined(COFFEE_GCC) || defined(COFFEE_CLANG)
#define C_FORCE_PACKING __attribute__((packed))
#elif defined(COFFEE_MSVCXX)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define C_FORCE_PACKING UNALIGNED
#else
#error "You done diddled it again; there is no packing defined!"
#endif

#if defined(COFFEE_GCC) || defined(COFFEE_CLANG)
#define C_FORCE_INLINE __attribute__((always_inline))
#elif defined(COFFEE_MSVCXX)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define C_FORCE_INLINE __forceinline
#else
#define C_FORCE_INLINE
#pragma message("Warning: Forced inlining not defined!")
#endif

#if defined(COFFEE_GCC) || defined(COFFEE_CLANG)
#define C_FORCE_NOTINLINE __attribute__((noinline))
#else
#define C_FORCE_NOTINLINE
#pragma message("Warning: Some functions may be inlined unintentionally because of this undefined macro.")
#endif

#define STATICINLINE inline C_FORCE_INLINE static
#define FORCEDINLINE inline C_FORCE_INLINE
#define PACKEDSTRUCT struct C_FORCE_PACKING
#define DENYINLINE C_FORCE_NOTINLINE

#endif

#undef major
#undef minor
#undef far
#undef near