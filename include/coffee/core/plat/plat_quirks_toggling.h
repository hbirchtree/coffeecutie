#pragma once

#include "plat_primary_identify.h"
#include "plat_arch_identify.h"

/* Platform specifications:
 * COFFEE_ARCH_LLP64 - Uses ull as opposed to ul
 * COFFEE_USE_TERMINAL_CTL - insert ASCII commands for color and etc.
 * COFFEE_USE_UNWIND - use Unwind stack backtracing
 * COFFEE_USE_IOCTL_TERM_SIZE - allow requesting terminal size from ioctl()
 *
 * COFFEE_LIL_ENDIAN - Little endian system
 * COFFEE_BIG_ENDIAN - Big endian system, quite rare
 *
 * COFFEE_USE_EXCEPTIONS - do use exceptions
 * COFFEE_USE_RTTI - do use RTTI
 *
 * COFFEE_LIMIT_INLINE - disable inlining for code size reduction
 *
 * System-level quirks:
 * COFFEE_SLAP_LOWMEM - if system is x86 and does not PAE, give up
 * COFFEE_NO_FUTURES - std::future does not exist
 * COFFEE_USE_POSIX_BASENAME - using POSIX basename() or a custom implementation
 * COFFEE_DISABLE_SRGB_SUPPORT - deny sRGB framebuffers, some platforms
 *  shouldn't check for it
 * COFFEE_USE_IMMERSIVE_VIEW - use fullscreen at all times
 * ASIO_USE_SSL - whether to disable ASIO's SSL support, useful if SSL
 *  does not exist on a platform
 * COFFEE_NO_HUGETLB - disable non-standard HUGE_TLB flag for file mapping
 * COFFEE_NO_RUSAGE_THREAD - disable rusage statistics per-thread
 *
 * COFFEE_NO_PTHREAD_SETNAME_NP - platforms that do not support setting
 *  thread names
 * COFFEE_NO_PTHREAD_GETNAME_NP - platforms that do not support getting
 *  thread names
 *
 * Graphics quirks:
 * COFFEE_LINKED_GLES - disables dynamic loading of GLES symbols, links them.
 *  Android, Maemo, RPi and Apple do this
 * COFFEE_LINKED_GLES30 - GLES 3.0 linked headers
 * COFFEE_LINKED_GLES31 - GLES 3.1 linked headers
 * COFFEE_LINKED_GLES32 - GLES 3.2 linked headers
 * COFFEE_ONLY_GLES20 - allow only GLES 2.0, for RPi, old Android and Maemo
 *
 * Windowing/graphical quirks:
 * COFFEE_USE_SDL_GL - use SDL for GL initialization
 * COFFEE_USE_SDL_EVENT - use SDL for event handling (joysticks, input, etc.)
 * COFFEE_USE_SDL_WINDOW - use SDL for creating a window
 *
 * COFFEE_USE_MAEMO_EGL - uses a piece of EGL to load a GL context, quite
 *  fast, only ~100 EGL calls as opposed to SDL's thousands
 * COFFEE_USE_MAEMO_X11 - uses a tiny loader for X11, creates a window
 *  with minimal functionality
 * COFFEE_RASPBERRY_DMX - uses a tiny loader for DISPMANX, takes the
 *  entire screen on Raspberry Pi devices for application
 * COFFEE_FRAGILE_FRAMEBUFFER - make windowing system more careful with
 *  resizes that may crash some devices (N900)
 * COFFEE_ALWAYS_VSYNC - for devices which force VSYNC
 *
 * COFFEE_STUBBED_ENVIRONMENT - stubs environment functions, because they
 *  are impossible to fulfull
 * COFFEE_STUBBED_PROCESS - stubs process functions, same reason as above
 * COFFEE_STUBBED_STACKTRACE - stubs stacktracing
 * COFFEE_STUBBED_CFILE - stubs FILE functionality
 * COFFEE_STUBBED_DYNLOADER - stubs dynamic library loading
 *
 * COFFEE_NO_EXECVPE - platforms that do not support execvpe
 * COFFEE_NO_SYSTEM_CMD - disables system() code, either if it does not
 *  exist or for security
 *
 * COFFEE_PLATFORM_OUTPUT_FORMAT - platforms that do logging in a special way,
 *  less processing
 *
 * COFFEE_NO_TLS - disallowing thread-local storage
 *
 * COFFEE_DISABLE_PROFILER - disable the profiler, useful for platforms
 *  with isolation
 *
 * COFFEE_LOWFAT - disable or remove a lot of code, reduces library size a lot
 * COFFEE_LOADABLE_LIBRARY - remove some features which require static linkage
 *
 * COFFEE_SDL_MAIN - use SDL_main on start, allowing SDL to create its state
 * COFFEE_CUSTOM_MAIN - use a magical main entry point somewhere else
 * COFFEE_CUSTOM_EXIT_HANDLING - use magical event handling, exiting main()
 *
 * COFFEE_INJECTS_EVENTS_EXTERNALLY - whether it is necessary to allow an
 *      external library to inject events into the program's event loop
 *
 * COFFEE_LOWFAT - disable tons of features for size, most of these changes
 *  are not noticeable for end-user applications with GUIs
 *
 * COFFEE_USE_CHRONOTIME - use std::chrono in place of system-provided
 *  time facilities
 *
 */

/* General configuration changers */
//#define COFFEE_LINUX_LIGHTWEIGHT_WM
//#define COFFEE_GLES20_MODE
//#define COFFEE_LOWFAT

/* Minor prohibiting flags */
#define COFFEE_SLAP_LOWMEM

/* Standard flags */
#define COFFEE_USE_CHRONOTIME

/* For Android 32-bit, we need this neat little trick. */
/* This might apply to win32 and lin32 as well, but they don't exist */
#if ((defined(COFFEE_WINDOWS) \
    || defined(COFFEE_EMSCRIPTEN) \
    || (defined(COFFEE_ANDROID) && defined(COFFEE_ARCH_ARM32)) \
    || (defined(COFFEE_ANDROID) && defined(COFFEE_ARCH_MIPS)) \
    || (defined(COFFEE_ANDROID) && defined(COFFEE_ARCH_X86)) \
    || defined(COFFEE_NACL) \
    || (defined(COFFEE_LINUX) && __SIZEOF_PTRDIFF_T__ == 4) ) \
    && __LP64__ != 1) \
    || defined(COFFEE_APPLE)
#define COFFEE_ARCH_LLP64
#endif

/* Unwind and terminal control signals are desktop-only */
#if (defined(COFFEE_LINUX) || defined(COFFEE_APPLE)) \
    && !defined(COFFEE_ANDROID) && !defined(__STEAMOS__) \
    && !defined(COFFEE_APPLE_MOBILE) \
    && !defined(COFFEE_MAEMO)
#define COFFEE_USE_TERMINAL_CTL
#if defined(LIBUNWIND_ENABLED)
#define COFFEE_USE_UNWIND
#endif
#endif

#if (defined(COFFEE_ANDROID) && defined(COFFEE_USE_SDL2)) \
    || defined(COFFEE_WINDOWS_UWP) \
    || defined(COFFEE_NACL)
#define COFFEE_SDL_MAIN
#elif defined(COFFEE_APPLE_MOBILE) || defined(COFFEE_ANDROID)
#define COFFEE_CUSTOM_MAIN
#define COFFEE_CUSTOM_EXIT_HANDLING
#define COFFEE_INJECTS_EVENTS_EXTERNALLY
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

#if (__GNUC__ == 4 && (__GNUC_MINOR__ == 8 || __GNUC_MINOR__ ==  9)) || \
    defined(COFFEE_ANDROID)
#define COFFEE_BAD_REGEX
#endif

/* Disabling exceptions and RTTI in release for size reasons */
#if !defined(COFFEE_ANDROID) && !defined(NDEBUG)
#define COFFEE_USE_EXCEPTIONS
#define COFFEE_USE_RTTI
#endif

#if defined(COFFEE_ANDROID) || defined(COFFEE_LOWFAT)
/* For Android, we limit inlining for size reasons */
#define COFFEE_LIMIT_INLINE
#define COFFEE_DISABLE_PROFILER
#endif

/* This enables safer, but a bit slower functions for some core functions */
/* dirname(), basename() */
#define COFFEE_USE_POSIX_BASENAME

/* OpenSSL is difficult on Windows... */
#if defined(COFFEE_ENABLE_SSL)
#define ASIO_USE_SSL
#endif

/* thread_local workarounds */
#if defined(COFFEE_APPLE_MOBILE) || defined(COFFEE_EMSCRIPTEN)
#define thread_local
#elif defined(COFFEE_APPLE)
#define thread_local __thread
#endif

#if defined(COFFEE_RASPBERRYPI) || defined(COFFEE_MAEMO)
#define COFFEE_LINKED_GLES
#endif

#if defined(COFFEE_RASPBERRYPI) || defined(COFFEE_MAEMO) || defined(COFFEE_ANDROID)
#define COFFEE_DISABLE_SRGB_SUPPORT
#define COFFEE_USE_IMMERSIVE_VIEW
#define COFFEE_ALWAYS_VSYNC
#endif

#if defined(COFFEE_RASPBERRYPI) || defined(COFFEE_MAEMO)
#define COFFEE_ONLY_GLES20
#endif

#if defined(COFFEE_APPLE_MOBILE)\
    || defined(COFFEE_GLES20_MODE) || defined(COFFEE_EMSCRIPTEN)
#define COFFEE_LINKED_GLES
#define COFFEE_ONLY_GLES20
#endif

#if defined(COFFEE_MAEMO)
#define COFFEE_NO_FUTURES
#define COFFEE_NO_HUGETLB

#define COFFEE_NO_RUSAGE_THREAD
#define COFFEE_FRAGILE_FRAMEBUFFER
#define COFFEE_ALWAYS_VSYNC
#define COFFEE_X11_HILDON
#endif

#if defined(COFFEE_WINDOWS) && defined(COFFEE_GLES20_MODE)
#define COFFEE_USE_MAEMO_EGL
#define COFFEE_USE_WINDOWS_ANGLE
#endif

#if defined(COFFEE_ANDROID) && !defined(COFFEE_USE_SDL2)
#define COFFEE_USE_ANDROID_NATIVEWIN
#define COFFEE_USE_MAEMO_EGL
#endif

#if defined(COFFEE_APPLE_MOBILE)
#define COFFEE_USE_APPLE_GLKIT
#define COFFEE_USE_MAEMO_EGL

#define COFFEE_NO_SYSTEM_CMD
#endif

#if defined(COFFEE_LINUX_LIGHTWEIGHT_WM) || defined(COFFEE_MAEMO)
// This is the super-fast X11 combination, SDL2 can't even be compared to this
// The window is guaranteed to show within 500ms
#if !defined(COFFEE_GLEAM_DESKTOP) //You cannot load OpenGL (non-ES) with EGL :(
#define COFFEE_USE_MAEMO_EGL
#else
#define COFFEE_USE_LINUX_GLX
#endif
#define COFFEE_USE_MAEMO_X11
#endif

#if defined(COFFEE_RASPBERRYPI)
#define COFFEE_RASPBERRY_DMX
#define COFFEE_USE_MAEMO_EGL
#endif

#if defined(COFFEE_ANDROID) || defined(COFFEE_APPLE_MOBILE)
#define COFFEE_NO_ATEXIT
#endif

#if defined(COFFEE_ANDROID) || defined(COFFEE_EMSCRIPTEN) || defined(COFFEE_NACL) \
    || defined(COFFEE_MAEMO) || defined(COFFEE_APPLE)
#define COFFEE_NO_EXECVPE
#endif

#if defined(COFFEE_UNIXPLAT) && !defined(COFFEE_NO_EXECVPE)
#define COFFEE_USE_EXECVPE
#endif

#if defined(COFFEE_EMSCRIPTEN) || defined(COFFEE_NACL) || defined(COFFEE_MAEMO)
#define COFFEE_NO_PTHREAD_SETNAME_NP
#define COFFEE_NO_PTHREAD_GETNAME_NP
#endif

#if defined(COFFEE_ANDROID)
#define COFFEE_NO_PTHREAD_GETNAME_NP
#endif

#if defined(COFFEE_EMSCRIPTEN)
#define COFFEE_NO_TLS
#endif

#if defined(COFFEE_ANDROID) || defined(COFFEE_EMSCRIPTEN) || defined(COFFEE_NACL)
#define COFFEE_PLATFORM_OUTPUT_FORMAT
#endif

#if defined(COFFEE_EMSCRIPTEN) || defined(COFFEE_NACL)\
    || defined(COFFEE_GEKKO)

#if !defined(COFFEE_EMSCRIPTEN)
#define COFFEE_STUBBED_SYSINFO
#endif

#define COFFEE_STUBBED_PROCESS
#define COFFEE_STUBBED_STACKTRACE
#define COFFEE_STUBBED_CFILE
#define COFFEE_STUBBED_DYNLOADER
#define COFFEE_STUBBED_ENVIRONMENT
#endif

#if defined(COFFEE_GEKKO)
#define COFFEE_STUBBED_TIMING
#endif

#if defined(COFFEE_GEKKO)
#define COFFEE_NO_THREADLIB
#define COFFEE_NO_FUTURES
#define COFFEE_PLAIN_INT_TYPES
#endif

#if defined(COFFEE_EMSCRIPTEN) || defined(COFFEE_NACL) \
    || defined(COFFEE_ANDROID) || defined(COFFEE_WINDOWS_UWP)
#define COFFEE_NO_SYSTEM_CMD
#endif

#if !defined(COFFEE_RASPBERRY_DMX) && \
    !defined(COFFEE_USE_LINUX_GLX) && \
    !defined(COFFEE_USE_MAEMO_EGL) && \
    !defined(COFFEE_USE_MAEMO_X11) && \
    defined(COFFEE_USE_SDL2)
#define COFFEE_USE_SDL_GL
#endif

#if !defined(COFFEE_RASPBERRY_DMX) && \
    !defined(COFFEE_USE_LINUX_GLX) && \
    !defined(COFFEE_USE_MAEMO_X11) && \
    defined(COFFEE_USE_SDL2)
#define COFFEE_USE_SDL_WINDOW
#endif

#if !defined(COFFEE_RASPBERRY_DMX) && \
    !defined(COFFEE_USE_MAEMO_X11) && \
    defined(COFFEE_USE_SDL2)
#define COFFEE_USE_SDL_EVENT
#endif

/* And here comes some simplification for configuration issues */

#if defined(COFFEE_ONLY_GLES20) && defined(COFFEE_GLEAM_DESKTOP)
#error Invalid configuration detected, cannot use GLES2.0 mode with desktop GL!
#endif
#if defined(COFFEE_GLEAM_DESKTOP) && defined(COFFEE_USE_MAEMO_EGL)
#error Invalid configuration detected, cannot use EGL with OpenGL (non-ES)!
#endif
