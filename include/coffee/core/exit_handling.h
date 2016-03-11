#pragma once

#include "plat/plat_primary_identify.h"
#include <signal.h>

namespace Coffee{

using ExitCallback = void(*)();

enum CoffeeExitCode
{
#ifndef COFFEE_WINDOWS
    CoffeeExit_Normal       = SIGQUIT,
#else
    CoffeeExit_Normal       = 0,
#endif
    CoffeeExit_GeneralError = 1,

    CoffeeExit_UnknownBad  = 100,

    CoffeeExit_IllegalOp   = SIGILL,
    CoffeeExit_Termination = SIGTERM,
    CoffeeExit_Interrupt   = SIGINT,

#ifndef COFFEE_WINDOWS
    CoffeeExit_Kill        = SIGKILL,
#endif

    CoffeeExit_ShitMySelf   = SIGSEGV,
    CoffeeExit_PoopedABit   = SIGABRT,
};

extern void SetExitFunction(ExitCallback f);

extern void InstallSignalHandler();

}