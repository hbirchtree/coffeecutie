#pragma once

/* Gekko needs its own std::thread implementation,
 *  as well as std::mutex and friends */
#if defined(COFFEE_GEKKO)
#define COFFEE_NO_THREADLIB
#define COFFEE_NO_FUTURES
#endif

/* Some exception features don't work on Gekko */
#if defined(COFFEE_GEKKO)
#define COFFEE_NO_EXCEPTION_QUIET
#define COFFEE_NO_EXCEPTION_DUMP
#endif

/* Maemo is missing a couple of new features */
#if defined(COFFEE_MAEMO)
#define COFFEE_NO_FUTURES
#define COFFEE_NO_EXCEPTION_RETHROW
#define COFFEE_NO_TERMINATION_HANDLER
#endif

/*
 *
 * thread_local "workarounds"
 *
 */

#if defined(COFFEE_EMSCRIPTEN)
#define COFFEE_NO_TLS
#endif

/*
 * Support for thread_local arrived in Xcode 8.
 * If you are on Xcode 7 or below, take this.
 * #define COFFEE_XCODE_NO_TLS
 */

#if defined(COFFEE_EMSCRIPTEN) || defined(COFFEE_RASPBERRYPI) || \
    defined(COFFEE_MAEMO)
/* In this case, it doesn't exist at all, with no fallback */
#define thread_local
#elif(defined(COFFEE_APPLE) && defined(COFFEE_XCODE_NO_TLS))
/* In this case it only supports POD types, __thread is an old feature */
#define thread_local __thread
#endif

#if defined(COFFEE_EMSCRIPTEN)
#define COFFEE_NO_EXECUTION_H
#endif
