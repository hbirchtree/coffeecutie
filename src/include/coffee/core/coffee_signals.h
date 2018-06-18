#pragma once

#include <signal.h>
#include <coffee/core/plat/plat_primary_identify.h>

namespace Coffee{

using exithandler_t = void(*)();
using sighandler_t = void(*)(int);

enum Signals
{
#ifdef COFFEE_WINDOWS
	Sig_Normal		= 0,
#else
	Sig_Normal		= SIGQUIT,
#endif

	Sig_GeneralError = 1,

	Sig_UnknownBad	= 100,

	Sig_IllegalOp	= SIGILL,
	Sig_Termination = SIGTERM,
	Sig_Interrupt	= SIGINT,

	Sig_FPE			= SIGFPE,

#ifdef COFFEE_WINDOWS
    Sig_Kill		= Sig_GeneralError,
#else
    Sig_Kill        = SIGKILL,
#endif

    Sig_ShitMySelf   = SIGSEGV,
    Sig_PoopedABit   = SIGABRT,
};

extern void sig_dummy_handler(int);
extern void SetExitHandler(exithandler_t handler);
extern void InstallSignalHandler(Signals sig, sighandler_t handler);

}
