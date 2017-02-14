/* Linux macro, also defines window system target */
#if defined(__linux__) && !defined(__NATIVE_CLIENT__)
#undef C_SYSTEM_STRING
#define C_SYSTEM_STRING "Linux"
#define COFFEE_LINUX

#if !defined(__ANDROID__) && !defined(__RASPBERRYPI__)
#define COFFEE_X11
/*#define COFFEE_WAYLAND*/
/*#define COFFEE_MIR*/
#endif

#if defined(__RASPBERRYPI__)
#define COFFEE_RASPBERRYPI
#endif

#endif

/* Android: the land of oddities */
#if defined(__ANDROID__)
#undef C_SYSTEM_STRING
#define C_SYSTEM_STRING "Android"
#define COFFEE_ANDROID
#undef COFFEE_LINUX
#endif

#if defined(__MAEMO__)
#define COFFEE_MAEMO
#endif

/* Windows macro, only one for now */
#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(WIN32) || defined(WINUWP)
#define C_SYSTEM_STRING "Windows"
#if defined(WINUWP)
#define COFFEE_WINDOWS_UWP
#endif
#define COFFEE_WINDOWS
#undef COFFEE_LINUX
#endif

/* Mac OS X macro, not iOS */
#if defined(__APPLE__)

#undef C_SYSTEM_STRING
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

#if defined(__EMSCRIPTEN__)
#if defined(__EMSCRIPTEN_WASM__)
#define C_SYSTEM_STRING "Emscripten/WebAsm"
#else
#define C_SYSTEM_STRING "Emscripten/asm.js"
#endif
#define COFFEE_EMSCRIPTEN
#endif

#if defined(__NATIVE_CLIENT__)
#define C_SYSTEM_STRING "Native Client"
#define COFFEE_NACL
#endif

/* Generic UNIX, mostly for POSIX libraries */
#if (defined(__unix__) || defined(COFFEE_APPLE))
#define COFFEE_UNIXPLAT
#endif
