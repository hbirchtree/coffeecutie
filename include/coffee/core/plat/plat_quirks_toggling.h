#include "plat_primary_identify.h"

#define COFFEE_SLAP_LOWMEM

/* For Android 32-bit, we need this neat little trick. */
/* This might apply to win32 and lin32 as well, but they don't exist */
#if defined(COFFEE_ANDROID) && (__INTPTR_WIDTH__ != 8)
#define COFFEE_ARCH_LLP64
#endif

#if defined(COFFEE_WINDOWS) || defined(COFFEE_APPLE)
#define COFFEE_ARCH_LLP64
#endif

/* Unwind and terminal control signals are desktop-only */
#if defined(COFFEE_LINUX) && !defined(COFFEE_ANDROID)
#define COFFEE_USE_TERMINAL_CTL
#endif

/* Terminal size: useless on Android */
#if defined(COFFEE_LINUX) && !defined(COFFEE_ANDROID)
#define COFFEE_USE_IOCTL_TERM_SIZE
#endif

/* So far, no issues with big-endian vs lil' endian, but we will keep it in mind */
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define COFFEE_BIG_ENDIAN
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define COFFEE_LIL_ENDIAN
#endif

/* Disabling exceptions and RTTI in release for size reasons */
#if !defined(COFFEE_ANDROID) && !defined(NDEBUG)
#define COFFEE_USE_EXCEPTIONS
#define COFFEE_USE_RTTI
#endif

/* For Android, we limit inlining for size reasons */
#if defined(COFFEE_ANDROID)
#define COFFEE_LIMIT_INLINE
#endif

/* This enables safer, but a bit slower functions for some core functions */
/* dirname(), basename() */
#define COFFEE_USE_POSIX_BASENAME

#if defined(COFFEE_ANDROID)
//#define COFFEE_DISABLE_SRGB_SUPPORT
#endif
