#pragma once

/* Enables POSIX errno wrapper type */
#define COFFEE_POSIX_ERRNO

/* This enables safer, but a bit slower functions for some core functions */
/* dirname(), basename() */
#define COFFEE_USE_POSIX_BASENAME

/* Maemo uses an older kernel, and has some stricter policies */
#if defined(COFFEE_MAEMO)
#define COFFEE_NO_HUGETLB
#define COFFEE_NO_MMAP64
#define COFFEE_NO_CANONICALIZE
#define COFFEE_NO_RUSAGE_THREAD
#endif

/* Linux desktop-specific threadname function */
#if defined(COFFEE_ANDROID)
#define COFFEE_NO_PTHREAD_GETNAME_NP
#endif

#if defined(COFFEE_EMSCRIPTEN) || defined(COFFEE_ANDROID) || \
    defined(COFFEE_WINDOWS_UWP) || defined(COFFEE_APPLE_MOBILE)
#define COFFEE_NO_SYSTEM_CMD
#endif

/* ::atexit() does not work as intended on these platforms */
#if defined(COFFEE_ANDROID) || defined(COFFEE_APPLE_MOBILE)
#define COFFEE_NO_ATEXIT
#endif

/* Not safe to use on mobile/web platforms */
#if defined(COFFEE_ANDROID) || defined(COFFEE_EMSCRIPTEN) || \
    defined(COFFEE_MAEMO) || defined(COFFEE_APPLE)
#define COFFEE_NO_EXECVPE
#endif

#if defined(COFFEE_UNIXPLAT) && !defined(COFFEE_NO_EXECVPE)
#define COFFEE_USE_EXECVPE
#endif

#if defined(COFFEE_EMSCRIPTEN) || defined(COFFEE_MAEMO)
#define COFFEE_NO_PTHREAD_SETNAME_NP
#define COFFEE_NO_PTHREAD_GETNAME_NP
#endif

#if defined(COFFEE_EMSCRIPTEN)
#define COFFEE_NO_WAITPID
#endif
