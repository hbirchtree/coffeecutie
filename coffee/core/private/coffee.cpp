#include <coffee/core/coffee.h>

#include <coffee/core/CMD>
#include <coffee/core/CDebug>

#include <coffee/core/profiler/profiling-export.h>
#include <coffee/core/argument_handling.h>
#include <coffee/core/plat/environment/process_def.h>
#include <coffee/core/types/cdef/infotypes.h>

namespace Coffee{

/* Information that is not otherwise available */
ApplicationData_t CoffeeApplicationData;

void CoffeeInit(bool profiler_init)
{
    if(profiler_init)
    {
        Profiler::InitProfiler();
        Profiler::LabelThread("Main");
    }

    /* Allow core dump by default */
#ifndef NDEBUG
    ProcessProperty::CoreDumpEnable();
#endif

    InstallSignalHandler();

    cDebug("Build string: {0}",CoffeeBuildString);

    cDebug("Running on {0} ({1})",CoffeeCompilerString,CoffeeArchString);
}

int32 CoffeeMain(CoffeeMainWithArgs mainfun, int32 argc, cstring_w*argv)
{
    /* Fix argument format on Windows */

    initargs = AppArg(argc,argv);

    Profiler::InitProfiler();
    Profiler::LabelThread("Main");

    Profiler::PushContext("CoffeeMain");

    CoffeeInit();
    Profiler::Profile("Init");

    Profiler::PushContext("main()");
    int32 r = mainfun(argc,argv);
    Profiler::PopContext();
    Profiler::Profile("Runtime");

    CoffeeTerminate(false);
    Profiler::Profile("Termination");
    Profiler::PopContext();

    Profiling::ExitRoutine(initargs.argc,initargs.argv);

    return r;
}

void CoffeeTerminate(bool profiler_destroy)
{
    if(profiler_destroy)
        Profiler::DestroyProfiler();
    Cmd::ResetScreen();
}

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
