#include <coffee/core/exit_handling.h>
#include <coffee/core/CMD>
#include <coffee/core/profiler/profiling-export.h>

#include <coffee/core/argument_handling.h>

namespace Coffee{

ExitCallback exit_handle = nullptr;

void SetExitFunction(ExitCallback f)
{
    exit_handle = f;
}

void sighandle(int sig)
{
    /* If we use an alternate buffer, switch back to primary */
    Cmd::ResetScreen();
    switch(sig)
    {
    case SIGFPE:
        cBasicPrint("FPE occurred");
        break;
    case SIGSEGV:
        if(exit_handle)
            exit_handle();
        Cmd::Exit(CoffeeExit_ShitMySelf);
    case SIGABRT:
        if(exit_handle)
            exit_handle();
        Cmd::Exit(CoffeeExit_PoopedABit);
    case SIGILL:
        if(exit_handle)
            exit_handle();
        Cmd::Exit(CoffeeExit_Termination);
    case SIGINT:
    {
        Profiling::ExitRoutine(GetInitArgs().argc,GetInitArgs().argv);
        if(exit_handle)
            exit_handle();
        Cmd::Exit(CoffeeExit_Interrupt);
    }
    case SIGTERM:
    {
        Profiling::ExitRoutine(GetInitArgs().argc,GetInitArgs().argv);
        if(exit_handle)
            exit_handle();
        Cmd::Exit(CoffeeExit_Termination);
    }
    default:
        Cmd::Exit(CoffeeExit_UnknownBad);
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

void InstallSignalHandler()
{
    /* Set up signal handlers, make the process more well-behaved */
    signal(SIGABRT,sighandle);
    signal(SIGSEGV,sighandle);

    signal(SIGFPE,sighandle);

    signal(SIGILL,sighandle);
    signal(SIGINT,sighandle);
    signal(SIGTERM,sighandle);
}

}