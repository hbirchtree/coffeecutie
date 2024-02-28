#pragma once

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

#if defined(__BEAGLEBONE__)
#define COFFEE_BEAGLEBONE
#if defined(__BEAGLEBONEBLACK__)
#define COFFEE_BEAGLEBONEBLACK
#endif
#endif

#endif

/* Android: the land of oddities */
#if defined(__ANDROID__) || defined(ANDROID)
#undef C_SYSTEM_STRING
#define C_SYSTEM_STRING "Android"
#define COFFEE_ANDROID
#undef COFFEE_LINUX
#endif

#if defined(__MAEMO__)
#undef C_SYSTEM_STRING
#define C_SYSTEM_STRING "Maemo"
#define COFFEE_MAEMO
#endif

/* Windows macro, only one for now */
#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__) || \
    defined(__MINGW32__) || defined(WIN32) || defined(WINUWP)
#define C_SYSTEM_STRING "Windows"
#if defined(__MINGW32__)
#define COFFEE_MINGW32
#endif
#if defined(WINUWP)
#define COFFEE_WINDOWS_UWP
#else
#define COFFEE_WINDOWS_WIN32
#define COFFEE_WIN32
#endif
#define COFFEE_WINDOWS
#undef COFFEE_LINUX
#endif

#if defined(__MINGW64__)
#define COFFEE_MINGW64
#endif

/* Mac OS X macro, not iOS */
#if defined(__APPLE__)

#include <TargetConditionals.h>

#define COFFEE_APPLE

#if TARGET_OS_OSX
#undef C_SYSTEM_STRING
#define C_SYSTEM_STRING "macOS"
#define COFFEE_APPLE_DESKTOP
#define COFFEE_MACOS

#elif TARGET_OS_IOS
#undef C_SYSTEM_STRING
#define C_SYSTEM_STRING "iOS"
#define COFFEE_ARCH_LLP64
#define COFFEE_APPLE_MOBILE
#define COFFEE_IOS

#else
#error Unrecognized Apple platform, please define it

#endif

#endif

#if defined(__EMSCRIPTEN__)
#if defined(__wasm32__)
#define C_SYSTEM_STRING "Emscripten/wasm32"
#elif defined(__wasm__)
#define C_SYSTEM_STRING "Emscripten/wasm"
#else
#define C_SYSTEM_STRING "Emscripten/asm.js"
#endif

#if defined(__wasm__)
#define COFFEE_WASM
#endif
#define COFFEE_EMSCRIPTEN
#endif

/* Generic UNIX, mostly for POSIX libraries */
#if(defined(__unix__) || defined(COFFEE_APPLE)) && !defined(COFFEE_WINDOWS)
#define COFFEE_UNIXPLAT
#endif

#if defined(GEKKO) || defined(__GEKKO__)
#define C_SYSTEM_STRING "Nintendo Gamecube"
#define COFFEE_GEKKO
#endif
