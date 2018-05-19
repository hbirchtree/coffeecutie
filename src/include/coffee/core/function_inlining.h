#pragma once

//#define COFFEE_LIMIT_INLINE

#include "plat/plat_compiler_identify.h"

#if (defined(COFFEE_GCC) || defined(COFFEE_CLANG)) && !defined(COFFEE_ANDROID)
#define C_FORCE_INLINE __attribute__((always_inline))
#elif defined(COFFEE_MSVCXX)
//#include "plat/plat_windows.h"
//#define C_FORCE_INLINE __forceinline
#define C_FORCE_INLINE
#else
#define C_FORCE_INLINE
//#pragma message("Warning: Forced inlining not defined!")
#endif

#if defined(COFFEE_GCC) || defined(COFFEE_CLANG)
#define C_FORCE_NOTINLINE __attribute__((noinline))
#else
#define C_FORCE_NOTINLINE
//#pragma message("Warning: Some functions may be inlined unintentionally because of this undefined macro.")
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
