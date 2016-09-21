#pragma once

#include <signal.h>

namespace Coffee{

using exithandler_t = void(*)();

enum Signals
{
#ifndef COFFEE_WINDOWS
    Sig_Normal       = SIGQUIT,
#else
    Sig_Normal       = 0,
#endif
    Sig_GeneralError = 1,

    Sig_UnknownBad  = 100,

    Sig_IllegalOp   = SIGILL,
    Sig_Termination = SIGTERM,
    Sig_Interrupt   = SIGINT,

    Sig_FPE	    = SIGFPE,

#ifndef COFFEE_WINDOWS
    Sig_Kill        = SIGKILL,
#endif

    Sig_ShitMySelf   = SIGSEGV,
    Sig_PoopedABit   = SIGABRT,
};

extern void sig_dummy_handler(int);
extern void SetExitHandler(exithandler_t handler);
extern void InstallSignalHandler(Signals sig, sighandler_t handler);

}
