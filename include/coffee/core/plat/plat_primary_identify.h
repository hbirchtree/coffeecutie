/* Linux macro, also defines window system target */
#if defined(__linux__)
#undef C_SYSTEM_STRING
#define C_SYSTEM_STRING "Linux"
#define COFFEE_LINUX

#if !defined(__ANDROID__)
#define COFFEE_X11
/*#define COFFEE_WAYLAND*/
/*#define COFFEE_MIR*/
#endif
#endif

/* Android: the land of oddities */
#if defined(__ANDROID__)
#undef C_SYSTEM_STRING
#define C_SYSTEM_STRING "Android"
#define COFFEE_ANDROID
#undef COFFEE_LINUX
#endif

/* Windows macro, only one for now */
#if defined(_WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(WIN32)
#define C_SYSTEM_STRING "Windows"
#define COFFEE_WINDOWS
#undef COFFEE_LINUX
#endif

/* Mac OS X macro, not iOS */
#if defined(__APPLE__)
#define C_SYSTEM_STRING "Mac OS X"
#define COFFEE_APPLE

#if defined(TARGET_OS_MAC)
#define COFFEE_APPLE_DESKTOP
#elif defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
#undef C_SYSTEM_STRING
#define C_SYSTEM_STRING "iOS"
#define COFFEE_APPLE_MOBILE
#endif

#endif

/* Generic UNIX, mostly for POSIX libraries */
#if defined(__unix__) || defined(COFFEE_APPLE)
#define COFFEE_UNIXPLAT
#endif
