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
    case SIGSEGV:
        Cmd::Exit(CoffeeExit_Pooped);
    case SIGABRT:
        Cmd::Exit(CoffeeExit_Pooped);
    case SIGILL:
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
#if defined(COFFEE_LINUX)
    case SIGKILL:
        Cmd::Exit(CoffeeExit_Kill);
#endif
    default:
        Cmd::Exit(CoffeeExit_UnknownBad);
    }
}

void SetExitFunction(ExitCallback f)
{
    exit_handle = f;
}

void CoffeeInit()
{
    static_assert(sizeof(uint8)==1,"uint8 size is inconsistent");
    static_assert(sizeof(int8) ==1, "int8 size is inconsistent");

    static_assert(sizeof(uint16)==2,"uint16 size is inconsistent");
    static_assert(sizeof(int16) ==2, "int16 size is inconsistent");

    static_assert(sizeof(uint32)==4,"uint32 size is inconsistent");
    static_assert(sizeof(int32) ==4, "int32 size is inconsistent");

    static_assert(sizeof(uint64)==8,"uint64 size is inconsistent");
    static_assert(sizeof(int64) ==8, "int64 size is inconsistent");

    static_assert(sizeof(scalar)    ==4, "scalar size is inconsistent");
    static_assert(sizeof(bigscalar) ==8, "bigscalar size is inconsistent");

    /* Allow core dump by default */
#ifndef NDEBUG
    ProcessProperty::CoreDumpEnable();
#endif

    /* Set up signal handlers, make the process more well-behaved */
    signal(SIGILL,sighandle);
    signal(SIGINT,sighandle);
#if defined(COFFEE_LINUX)
    signal(SIGKILL,sighandle);
#endif
    signal(SIGTERM,sighandle);

#ifndef NDEBUG
    /* Run unit tests, ensuring that the system and compilation process is sane */
//    CoffeeTests::run_type_tests();
    CoffeeTests::run_memory_tests();
#endif

#ifndef NDEBUG
    cDebug("Build string: {0}",CoffeeBuildString);
#endif
    cDebug("Running on {0} ({1})",CoffeeCompilerString,CoffeeArchString);
}

int32 CoffeeMain(CoffeeMainWithArgs mainfun, int32 argc, cstring_w*argv)
{
    Profiler::InitProfiler();

    /*TODO: Handle the Windows case of not including the application name*/

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

//    Profiling::PrintProfilerData();
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

std::list<Profiler::DataPoint>* Profiler::datapoints = nullptr;
std::list<CString>* Profiler::context_stack = nullptr;
Mutex* Profiler::data_access_mutex = nullptr;
Timestamp* Profiler::start_time = nullptr;

}
