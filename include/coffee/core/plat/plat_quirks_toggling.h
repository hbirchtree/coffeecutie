#include "plat_primary_identify.h"

#if defined(COFFEE_ANDROID)
#define COFFEE_ARCH_LLP64
#endif

/* Unwind and terminal control signals are desktop-only */
#if defined(COFFEE_UNIXPLAT) && !defined(COFFEE_ANDROID)
#define COFFEE_USE_UNWIND
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

/* Disabling exceptions and RTTI on Android for size reasons */
#if !defined(COFFEE_ANDROID)
#define COFFEE_USE_EXCEPTIONS
#define COFFEE_USE_RTTI
#endif

/* For Android, we limit inlining for size reasons */
#if defined(COFFEE_ANDROID)
#define COFFEE_LIMIT_INLINE
#endif
