#include "plat_primary_identify.h"

#if defined(COFFEE_ANDROID)
#define COFFEE_ARCH_LLP64
#endif

/* STL on Android is neutered! */
#if !defined(COFFEE_ANDROID)
//#define COFFEE_USE_STL_TO_STRING
#endif

#if !defined(COFFEE_ANDROID)
//#define COFFEE_USE_IOSTREAMS
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
