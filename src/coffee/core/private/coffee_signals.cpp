#include <coffee/core/coffee_signals.h>

#include <coffee/core/CMD>
#include <coffee/core/profiler/profiling-export.h>
#include <coffee/core/CDebug>

namespace Coffee{

static exithandler_t exit_handle = nullptr;

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
    case SIGSEGV:
    case SIGABRT:
    case SIGILL:
    case SIGINT:
    case SIGTERM:
        if(exit_handle)
            exit_handle();
        break;
    default:
        break;
    }

    switch(sig)
    {
    case SIGTERM:
    case SIGINT:
        /* This place used to trigger the Profiler::ExitRoutine().
         *  Linking made it difficult to keep it. */
        break;
    default:
        break;
    }

    switch(sig)
    {
    case SIGFPE:
        cVerbose(4,"FPE occurred");
        break;

    case SIGSEGV:
    case SIGABRT:
    case SIGILL:
    case SIGINT:
    case SIGTERM:
        Cmd::Exit(sig);

    default:
        Cmd::Exit(Sig_UnknownBad);
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
#if !defined(COFFEE_ANDROID) || (defined(COFFEE_ANDROID) && !defined(COFFEE_ARCH_MIPS))
    /* Set up signal handlers, make the process more well-behaved */
    if(sig < 100 && sig > 1)
    {
        if(!handler)
            handler = sighandle;
        signal(sig, handler);
    }
#endif
}

void sig_dummy_handler(int)
{
}

}
