#include <coffee/core/coffee.h>


#include <coffee/core/plat/platform_detect.h>
#include <coffee/core/CDebug>
#include <coffee/core/unit_tests/data_types.h>
#include <coffee/core/unit_tests/memory_operations.h>
#include <coffee/core/plat/memory/cmd_interface.h>

#include <coffee/core/plat/environment/stacktrace_def.h>
#include <coffee/core/plat/timing/timing_def.h>
#include <coffee/core/plat/environment/process_def.h>
#include <coffee/core/plat/environment/sysinfo_def.h>
#include <coffee/core/plat/timing/profiling.h>

#include <coffee/core/profiler/profiling-export.h>

namespace Coffee{

static ExitCallback exit_handle = nullptr;

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
        Cmd::Exit(CoffeeExit_Pooped);
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
        if(exit_handle)
            exit_handle();
        Cmd::Exit(CoffeeExit_Interrupt);
    }
    case SIGTERM:
    {
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

void SetExitFunction(ExitCallback f)
{
    exit_handle = f;
}

void CoffeeInit()
{
    /* We make certain assumptions about the system
     *  which must be fulfilled for proper execution */
    static_assert(sizeof(uint8)==1,
                  "uint8 size is inconsistent");
    static_assert(sizeof(int8)==1,
                  "int8 size is inconsistent");

    static_assert(sizeof(uint16)==2,
                  "uint16 size is inconsistent");
    static_assert(sizeof(int16)==2,
                  "int16 size is inconsistent");

    static_assert(sizeof(uint32)==4,
                  "uint32 size is inconsistent");
    static_assert(sizeof(int32)==4,
                  "int32 size is inconsistent");

    static_assert(sizeof(uint64)==8,
                  "uint64 size is inconsistent");
    static_assert(sizeof(int64)==8,
                  "int64 size is inconsistent");

    static_assert(sizeof(scalar)==4,
                  "scalar size is inconsistent");
    static_assert(sizeof(bigscalar)==8,
                  "bigscalar size is inconsistent");

    /* Allow core dump by default */
#ifndef NDEBUG
    ProcessProperty::CoreDumpEnable();
#endif

    /* Set up signal handlers, make the process more well-behaved */
    signal(SIGABRT,sighandle);
    signal(SIGSEGV,sighandle);

    signal(SIGFPE,sighandle);

    signal(SIGILL,sighandle);
    signal(SIGINT,sighandle);
    signal(SIGTERM,sighandle);

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

    Profiler::InitProfiler();
    Profiler::LabelThread("Main");
    Profiler::PushContext("CoffeeMain");

    CoffeeInit();
    Profiler::Profile("Init");

    Profiler::PushContext("main()");
    int32 r = mainfun(argc,argv);
    Profiler::PopContext();
    Profiler::Profile("Runtime");

    CoffeeTerminate();
    Profiler::Profile("Termination");
    Profiler::PopContext();

    CString profile_log_name = cStringFormat("{0}-profile.xml",Env::ExecutableName());
    Profiling::ExportProfilerData(profile_log_name.c_str(),argc,argv);
    Profiler::DestroyProfiler();

    return r;
}

void CoffeeTerminate()
{
    Cmd::ResetScreen();
}

}

namespace Coffee{
/* Don't mind this, just some string storage */
#ifdef COFFEE_LINUX
thread_local CString LinuxSysInfo::cached_cpuinfo_string;
#endif

#if defined(COFFEE_USE_UNWIND)
unw_context_t* LinuxStacktracer::unwind_context = nullptr;
#endif

#ifdef COFFEE_WINDOWS
//CFunctional::WindowsPerformanceCounterData CFunctional::_win_perfcounter_data;
#endif

/* Storage for profiler data */
LinkList<Profiler::DataPoint>* Profiler::datapoints = nullptr;
thread_local LinkList<CString>* Profiler::context_stack = nullptr;
Mutex* Profiler::data_access_mutex = nullptr;
Timestamp* Profiler::start_time = nullptr;
Profiler::ThreadListing* Profiler::threadnames = nullptr;
std::atomic_int *Profiler::global_init = nullptr;

}
