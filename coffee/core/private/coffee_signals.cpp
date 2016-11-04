#include <coffee/core/coffee_signals.h>

#include <coffee/core/CMD>
#include <coffee/core/profiler/profiling-export.h>
#include <coffee/core/CDebug>

namespace Coffee{

exithandler_t exit_handle = nullptr;

/*!
 * \brief Default signal handler function
 * \param sig
 */
void sighandle(int sig)
{
    /* If we use an alternate buffer, switch back to primary */
    Cmd::ResetScreen();
    switch(sig)
    {
    case SIGFPE:
	cVerbose(4,"FPE occurred");
	break;
    case SIGSEGV:
	if(exit_handle)
	    exit_handle();
    Cmd::Exit(Sig_ShitMySelf);
	break;
    case SIGABRT:
	if(exit_handle)
	    exit_handle();
    Cmd::Exit(Sig_PoopedABit);
	break;
    case SIGILL:
	if(exit_handle)
	    exit_handle();
    Cmd::Exit(Sig_Termination);
	break;
    case SIGINT:
    {
	Profiling::ExitRoutine(GetInitArgs().argc,GetInitArgs().argv);
	if(exit_handle)
	    exit_handle();
    Cmd::Exit(Sig_Interrupt);
	break;
    }
    case SIGTERM:
    {
	Profiling::ExitRoutine(GetInitArgs().argc,GetInitArgs().argv);
	if(exit_handle)
	    exit_handle();
    Cmd::Exit(Sig_Termination);
	break;
    }
    default:
    Cmd::Exit(Sig_UnknownBad);
	break;
    }

    /* Implementation detail: SIGKILL might
     *  leave system with dirty state for certain hardware devices:
     *  - Input devices such as controllers
     *  - Haptic devices
     *  - Kinect devices (if using CNect)
     *
     * If any abnormal behavior occurs after
     *  a SIGKILL shutdown has happened, this is likely the cause.*/
}

void SetExitHandler(exithandler_t handler)
{
    exit_handle = handler;
}

void InstallSignalHandler(Signals sig, sighandler_t handler)
{
    /* Set up signal handlers, make the process more well-behaved */
    if(sig < 100 && sig > 1)
    {
	if(!handler)
	    handler = sighandle;
	signal(sig,handler);
    }
}

void sig_dummy_handler(int)
{
}

}
