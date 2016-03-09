#include "plat_primary_identify.h"

#ifndef COFFEE_ESMODE
#define COFFEE_GLEAM_DESKTOP
#endif

/* STL on Android is neutered! */
#if !defined(COFFEE_ANDROID)
#define COFFEE_USE_STL_TO_STRING
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

/* Some ARM platforms are LLP64 */
#if defined(COFFEE_ANDROID)
#define COFFEE_USE_LONGLONG_INT64
#endif

/* ...files are even worse! */
#if defined(COFFEE_ANDROID)
#define COFFEE_ANDROID_FILE_ASSET_API
#else
#define COFFEE_C_FILE_API
#endif

/* So far, no issues with big-endian vs lil' endian, but we will keep it in mind */
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define COFFEE_BIG_ENDIAN
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define COFFEE_LIL_ENDIAN
#endif