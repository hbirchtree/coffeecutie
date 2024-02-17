#pragma once

/* AMD64 string, also used for Intel's 64-bit architectures */

#undef COFFEE_ARCH

#if defined(__amd64__) || defined(_M_X64) || defined(_M_AMD64)
#define COFFEE_ARCH "AMD64"

#define COFFEE_ARCH_AMD64

/* i386 is a weird one */
#elif defined(__i386__) || defined(_X86_)

#define COFFEE_ARCH_X86

#if defined(__i686__)
#define COFFEE_ARCH "i686"
#elif defined(__i586__)
#define COFFEE_ARCH "i586"
#elif defined(__i486__)
#define COFFEE_ARCH "i486"
#else
#define COFFEE_ARCH "i386"
#endif

/* I have no idea who uses this */
#elif defined(__mips__) || defined(__mips) || defined(__MIPS__)
#define COFFEE_MIPS_BASENAME "MIPS"

#define COFFEE_ARCH_MIPS

/* Too lazy to set up string conversion... */
#if __mips == 1
#define COFFEE_MIPS_PREFIX COFFEE_MIPS_BASENAME "1"
#elif __mips == 2
#define COFFEE_MIPS_PREFIX COFFEE_MIPS_BASENAME "2"
#elif __mips == 3
#define COFFEE_MIPS_PREFIX COFFEE_MIPS_BASENAME "3"
#elif __mips == 4
#define COFFEE_MIPS_PREFIX COFFEE_MIPS_BASENAME "4"
#else
#define COFFEE_MIPS_PREFIX COFFEE_MIPS_BASENAME
#endif

#if defined(_R3000)
#define COFFEE_ARCH COFFEE_MIPS_PREFIX " R3000"
#elif defined(_R4000)
#define COFFEE_ARCH COFFEE_MIPS_PREFIX " R4000"
#elif defined(_R5900)
#define COFFEE_ARCH COFFEE_MIPS_PREFIX " R5900"
#else
#define COFFEE_ARCH COFFEE_MIPS_PREFIX " ???"
#endif

/* Nobody will use this */
#elif defined(__powerpc__) || defined(__POWERPC__) || defined(__PPC64__) || \
    defined(__PPC__)

#define COFFEE_ARCH_POWERPC

#if defined(_ARCH_440)
#define COFFEE_POWERPC_CPU "440"
#elif defined(_ARCH_450)
#define COFFEE_POWERPC_CPU "450"
#elif defined(_ARCH_601) || defined(__ppc601__) || _M_PPC == 601
#define COFFEE_POWERPC_CPU "601"
#elif defined(_ARCH_603) || defined(__ppc603__) || _M_PPC == 603
#define COFFEE_POWERPC_CPU "603"
#elif defined(_ARCH_604) || defined(__ppc604__) || _M_PPC == 604
#define COFFEE_POWERPC_CPU "604"
#elif defined(_ARCH_620) || defined(__ppc620__) || _M_PPC == 620
#define COFFEE_POWERPC_CPU "620"
#elif defined(__GEKKO__)
#define COFFEE_POWERPC_CPU "750CXe/Gekko"
#elif defined(_ARCH_PPCGR)
#define COFFEE_POWERPC_CPU "603/604"
#else
#define COFFEE_POWERPC_CPU "???"
#endif

#define COFFEE_POWERPC_PREFIX "PowerPC " COFFEE_POWERPC_CPU

#if defined(__PPCGECKO__)
#define COFFEE_ARCH COFFEE_POWERPC_PREFIX " Gecko"
#elif defined(__PPCBROADWAY__)
#define COFFEE_ARCH COFFEE_POWERPC_PREFIX " Broadway"
#elif defined(_XENON)
#define COFFEE_ARCH COFFEE_POWERPC_PREFIX " Xenon"
#elif defined(__powerpc64__)
#define COFFEE_ARCH COFFEE_POWERPC_PREFIX " 64-bit"
#else
#define COFFEE_ARCH COFFEE_POWERPC_PREFIX
#endif

/* ARM 64-bit is different from ARM 32-bit, but has the same traits */
#elif defined(__aarch64__)
#define COFFEE_ARCH "ARMv8-64"

#define COFFEE_ARCH_ARM64

/* The chain of identification is a long one for ARM */
#elif defined(__arm__)

#define COFFEE_ARCH_ARM32

#if defined(__ARM_ARCH_5__)
#define COFFEE_ARCH "ARMv5"
#elif defined(__ARM_ARCH_7A__)
#define COFFEE_ARCH "ARMv7a"
#elif defined(__thumb__)
#define COFFEE_ARCH "ARM Thumb mode"
#else
#define COFFEE_ARCH "ARM"
#endif

#elif defined(__wasm64__)
#define COFFEE_ARCH "WebAssembly/64"

#elif defined(__wasm32__)
#define COFFEE_ARCH "WebAssembly/32"

#elif defined(__wasm__)
#define COFFEE_ARCH "WebAssembly"

#elif defined(__EMSCRIPTEN__)
#define COFFEE_ARCH "asm.js"

/* And if we don't know..? */
#else
#define COFFEE_ARCH "Magic"
#endif
