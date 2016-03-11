#include <coffee/core/coffee.h>

#include <coffee/core/CMD>
#include <coffee/core/CDebug>

#include <coffee/core/profiler/profiling-export.h>
#include <coffee/core/unit_tests/memory_operations.h>
#include <coffee/core/argument_handling.h>

namespace Coffee{

/* We make certain assumptions about the system
 *  which must be fulfilled for proper execution */
//static_assert(sizeof(uint8)==1,
//              "uint8 size is inconsistent");
//static_assert(sizeof(int8)==1,
//              "int8 size is inconsistent");

//static_assert(sizeof(uint16)==2,
//              "uint16 size is inconsistent");
//static_assert(sizeof(int16)==2,
//              "int16 size is inconsistent");

//static_assert(sizeof(uint32)==4,
//              "uint32 size is inconsistent");
//static_assert(sizeof(int32)==4,
//              "int32 size is inconsistent");

//static_assert(sizeof(uint64)==8,
//              "uint64 size is inconsistent");
//static_assert(sizeof(int64)==8,
//              "int64 size is inconsistent");

//static_assert(sizeof(scalar)==4,
//              "scalar size is inconsistent");
//static_assert(sizeof(bigscalar)==8,
//              "bigscalar size is inconsistent");

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

#ifndef NDEBUG
    /* Run unit tests, ensuring that the system
     *  and compilation process is sane */
    CoffeeTests::run_memory_tests();
#endif

#ifndef NDEBUG
    cDebug("Build string: {0}",CoffeeBuildString);
#endif
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
