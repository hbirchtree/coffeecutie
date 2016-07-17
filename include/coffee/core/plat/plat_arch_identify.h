/* AMD64 string, also used for Intel's 64-bit architectures */

#undef COFFEE_ARCH

#if defined(__amd64__) || defined(_M_X64) || defined(_M_AMD64)
#define COFFEE_ARCH "AMD64"

/* i386 is a weird one */
#elif defined(__i386__) || defined(_X86_)
#if defined(__i686__)
#define COFFEE_ARCH "i686"
#elif defined(__i586__)
#define COFFEE_ARCH "i586"
#elif defined(__i486__)
#define COFFEE_ARCH "i486"
#else
#define COFFEE_ARCH "i386"
#endif

/* ARM 64-bit is different from ARM 32-bit, but has the same traits */
#elif defined(__aarch64__)
#define COFFEE_ARCH "ARMv8-64"

/* The chain of identification is a long one for ARM */
#elif defined(__arm__)
#if defined(__ARM_ARCH_5__)
#define COFFEE_ARCH "ARMv5"
#elif defined(__ARM_ARCH_7A__)
#define COFFEE_ARCH "ARMv7a"
#elif defined(__thumb__)
#define COFFEE_ARCH "ARM Thumb mode"
#else
#define COFFEE_ARCH "ARM"
#endif

/* And if we don't know..? */
#else
#define COFFEE_ARCH "Magic"
#endif
