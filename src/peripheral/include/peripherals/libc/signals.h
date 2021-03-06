#pragma once

#include <peripherals/identify/compiler/function_inlining.h>
#include <peripherals/identify/system.h>

#include <csignal>
#include <cstdlib>

namespace libc {
namespace signal {

using sig_handler  = void(int);
using exit_handler = void(*)();

enum class sig
{
    /* May be returned */
    no_error      = EXIT_SUCCESS,
    general_error = EXIT_FAILURE,
    unknown_error = 100,

    /* May be received or sent */
    illegal_operation = SIGILL,
    terminate         = SIGTERM,
    interrupt         = SIGINT,
    fpe               = SIGFPE,
    segfault          = SIGSEGV,
    abort             = SIGABRT,

#if !defined(COFFEE_WINDOWS)
    bus_error         = SIGBUS,
#endif

    ill_op = illegal_operation,

/* May only be sent */
#if defined(COFFEE_WINDOWS)
    kill = 101,
#else
    kill = SIGKILL,
#endif
};

FORCEDINLINE void install(sig signal, sig_handler h)
{
#if !defined(COFFEE_ANDROID) && !defined(COFFEE_APPLE_MOBILE)
    ::signal(static_cast<int>(signal), h);
#endif
}

FORCEDINLINE void exit(sig signal)
{
    ::exit(static_cast<int>(signal));
}

namespace handlers {
FORCEDINLINE void default_noop(int)
{
}

FORCEDINLINE void default_exit(int)
{
    signal::exit(sig::general_error);
}
} // namespace handlers

template<void(*Handler)()>
struct scope_exit_handler
{
    ~scope_exit_handler()
    {
        Handler();
    }
};

extern void register_atexit(exit_handler hnd);

} // namespace signal
} // namespace libc
