#ifndef COFFEE_CORE_MEM_MACROS_H
#define COFFEE_CORE_MEM_MACROS_H

#include "plat/plat_compiler_identify.h"
#include "plat/plat_quirks_toggling.h"

#define C_DEFAULT_CONSTRUCTOR(ctype) ctype() = default
#define C_DELETE_COPY_CONSTRUCTOR(ctype) ctype(ctype const&) = delete
#define C_MOVE_CONSTRUCTOR(ctype) ctype(ctype&& x) = default; ctype& operator=(ctype&& a) = default

/* Force packing, compiler flags */
#if defined(COFFEE_GCC) || defined(COFFEE_CLANG)
#define C_FORCE_PACKING __attribute__((packed))
#elif defined(COFFEE_MSVCXX)
#include "plat/plat_windows.h"
//TODO: Fix struct packing on Windows, this is broken
#define C_FORCE_PACKING //UNALIGNED
#else
#error "You done diddled it again; there is no packing defined!"
#endif

/* Force inlining, compiler flags */
#if defined(COFFEE_GCC) || defined(COFFEE_CLANG) && !defined(COFFEE_ANDROID)
#define C_FORCE_INLINE __attribute__((always_inline))
#elif defined(COFFEE_MSVCXX)
#include "plat/plat_windows.h"
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

/* To disable inline */
#define DENYINLINE C_FORCE_NOTINLINE

/* Force inlining macros */
#if !defined(COFFEE_LIMIT_INLINE)
#define STATICINLINE inline C_FORCE_INLINE static
#define FORCEDINLINE inline C_FORCE_INLINE
#else
#define STATICINLINE inline DENYINLINE static
#define FORCEDINLINE inline DENYINLINE
#endif

/* Forced packing, required for some types to work (uint24 etc.) */
#define PACKEDSTRUCT struct C_FORCE_PACKING

#endif

/* Apparently, a lot of compilers define these shits */
#undef major
#undef minor
