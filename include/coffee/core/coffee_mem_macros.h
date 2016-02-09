#ifndef COFFEE_CORE_MEM_MACROS_H
#define COFFEE_CORE_MEM_MACROS_H

#include "plat/platform_detect.h"

#define C_DELETE_COPY_CONSTRUCTOR(ctype) ctype(ctype const&) = delete

#if defined(COFFEE_GCC) || defined(COFFEE_CLANG)
#define C_FORCE_PACKING __attribute__((packed))
#elif defined(COFFEE_MSVCXX)
#include <windef.h>
#define C_FORCE_PACKING UNALIGNED
#else
#define C_FORCE_PACKING
#endif

#if defined(COFFEE_GCC) || defined(COFFEE_CLANG)
#define C_FORCE_INLINE __attribute__((always_inline))
#elif defined(COFFEE_MSVCXX)
#include <windef.h>
#define C_FORCE_INLINE __forceinline
#else
#define C_FORCE_INLINE
#endif

#define STATICINLINE inline C_FORCE_INLINE static
#define FORCEDINLINE inline C_FORCE_INLINE
#define PACKEDSTRUCT struct C_FORCE_PACKING

#endif
