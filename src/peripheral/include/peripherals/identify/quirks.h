#pragma once

#include <peripherals/identify/architecture.h>
#include <peripherals/identify/system.h>

/* Platform specifications:
 * COFFEE_ARCH_LLP64 - Uses ull as opposed to ul
 * COFFEE_USE_TERMINAL_CTL - insert ASCII commands for color and etc.
 * COFFEE_USE_UNWIND - use Unwind stack backtracing
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
 * COFFEE_LOWFAT - disable tons of features for size, most of these changes
 *  are not noticeable for end-user applications with GUIs
 *
 * COFFEE_USE_CHRONOTIME - use std::chrono in place of system-provided
 *  time facilities
 *
 */

#include "cpp_quirks.h"
#include "posix_quirks.h"

/* General configuration changers */
//#define COFFEE_LINUX_LIGHTWEIGHT_WM
//#define COFFEE_GLES20_MODE
//#define COFFEE_LOWFAT

/*
 *
 * Minor prohibiting flags
 *
 */

#define COFFEE_SLAP_LOWMEM

/*
 *
 * Build properties
 *
 */

#if !defined(NDEBUG)
#define MODE_DEBUG 1
#define MODE_RELEASE 0

#else
#define MODE_RELEASE 1
#define MODE_DEBUG 0
#endif

#if defined(COFFEE_LOWFAT)
#define MODE_LOWFAT 1
#else
#define MODE_LOWFAT 0
#endif

/*
 *
 * Extra architecture info
 *
 */

/* For Android 32-bit, we need this neat little trick. */
/* This might apply to win32 and lin32 as well, but they don't exist */
#if((defined(COFFEE_WINDOWS) || defined(COFFEE_EMSCRIPTEN) ||   \
     (defined(COFFEE_ANDROID) && defined(COFFEE_ARCH_ARM32)) || \
     (defined(COFFEE_ANDROID) && defined(COFFEE_ARCH_MIPS)) ||  \
     (defined(COFFEE_ANDROID) && defined(COFFEE_ARCH_X86)) ||   \
     (defined(COFFEE_LINUX) && __SIZEOF_PTRDIFF_T__ == 4)) &&   \
    __LP64__ != 1) ||                                           \
    defined(COFFEE_APPLE)
#define COFFEE_ARCH_LLP64
#endif

/* So far, no issues with big-endian vs lil' endian,
 *  but we will keep it in mind */
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define COFFEE_BIG_ENDIAN
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define COFFEE_LIL_ENDIAN
#endif

/*
 *
 * Signalize that we are running with a special startup process
 *
 */

#if defined(COFFEE_WINDOWS_UWP)
#define COFFEE_SDL_MAIN
#elif defined(COFFEE_APPLE_MOBILE) || defined(COFFEE_ANDROID)
#define COFFEE_CUSTOM_MAIN
#define COFFEE_CUSTOM_EXIT_HANDLING
#define COFFEE_CUSTOM_STACKTRACE
#endif

/*
 *
 * Exceptions and RTTI
 *
 */

#define COFFEE_USE_EXCEPTIONS
#define COFFEE_USE_RTTI

/*
 *
 * Platform-specific tweaks
 *
 */

#if defined(COFFEE_ANDROID) || defined(COFFEE_LOWFAT)
/* For Android, we limit inlining for size reasons */
#define COFFEE_LIMIT_INLINE
#endif

#if defined(COFFEE_EMSCRIPTEN) || defined(COFFEE_GEKKO)
#define COFFEE_DEFAULT_VERBOSITY
#endif

/*
 *
 * Library-bound switches
 *
 */

#if defined(COFFEE_ENABLE_SSL)
#define ASIO_USE_SSL
#endif

/*
 *
 * Profiler switches
 *
 */

/* Disable profiler in release mode */
#if MODE_RELEASE
#define COFFEE_DISABLE_PROFILER
#endif

#if defined(COFFEE_GEKKO) || defined(COFFEE_EMSCRIPTEN)
#define COFFEE_DISABLE_PROFILER
#endif

/*
 *
 * Filesystem switches
 *
 */

#if defined(COFFEE_ANDROID) || \
    (defined(COFFEE_WINDOWS) && !defined(COFFEE_WINDOWS_UWP))
#define COFFEE_VIRTUAL_ASSETS
#endif

#if defined(COFFEE_ANDROID)
#define COFFEE_DYNAMIC_TEMPFILES
#endif



/*
 *
 * Compression systems
 *
 */

#if !defined(COFFEE_WINDOWS) && !defined(COFFEE_GEKKO)
/* Zlib comes bundled with pretty much all POSIX systems
 *  except Windows */
#define COFFEE_BUILD_ZLIB
#elif defined(COFFEE_WINDOWS) && defined(_WIN32_WINNT) && _WIN32_WINNT >= 0x0602
#define COFFEE_BUILD_WINDOWS_DEFLATE
#else
#define COFFEE_BUILD_NO_COMPRESSION
#endif

/*
 *
 * System calls
 *
 */

/* Default to using std::chrono for conversion, clocks and etc. */
#define COFFEE_USE_CHRONOTIME

#if defined(COFFEE_MAEMO) || defined(COFFEE_GEKKO)
#define COFFEE_NO_TIME_FORMAT
#endif

/* Unwind and terminal control signals are desktop-only */
#if(defined(COFFEE_LINUX) || defined(COFFEE_APPLE)) &&           \
    !defined(COFFEE_ANDROID) && !defined(COFFEE_APPLE_MOBILE) && \
    !defined(COFFEE_MAEMO)
#define COFFEE_USE_TERMINAL_CTL
#if defined(LIBUNWIND_ENABLED)
#define COFFEE_USE_UNWIND
#endif
#endif

/*
 *
 * Platform output switching
 *
 */

#if defined(COFFEE_ANDROID) || defined(COFFEE_EMSCRIPTEN)
#define COFFEE_PLATFORM_OUTPUT_FORMAT
#endif

#if defined(COFFEE_GEKKO)
#define COFFEE_SIMPLE_PRINT
#endif

/*
 *
 * Implementation stubbing
 *
 */

#if defined(COFFEE_EMSCRIPTEN) || defined(COFFEE_GEKKO)

#if !defined(COFFEE_EMSCRIPTEN)
#define COFFEE_STUBBED_SYSINFO
#define COFFEE_STUBBED_ENVIRONMENT
#endif

#define COFFEE_STUBBED_PROCESS
#define COFFEE_STUBBED_STACKTRACE
#define COFFEE_STUBBED_CFILE
#define COFFEE_STUBBED_DYNLOADER

#endif

#if defined(COFFEE_GEKKO)
#define COFFEE_STUBBED_TIMING
#endif

/*
 *
 * Gekko switches, because it has nothing
 *
 */

#if defined(COFFEE_GEKKO)
/* For more correct types IRT what all the libraries use */
#define COFFEE_PLAIN_INT_TYPES
#endif

#include "video_quirks.h"

/*
 *
 * And here comes some simplification for configuration issues
 *
 */

#if defined(COFFEE_ONLY_GLES20) && defined(COFFEE_GLEAM_DESKTOP)
#error Invalid configuration detected, cannot use GLES2.0 mode with desktop GL!
#endif
#if defined(COFFEE_GLEAM_DESKTOP) && defined(COFFEE_USE_MAEMO_EGL)
#error Invalid configuration detected, cannot use EGL with OpenGL (non-ES)!
#endif
