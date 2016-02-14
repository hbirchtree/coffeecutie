#include <coffee/core/coffee.h>

#include <signal.h>

#include <coffee/core/plat/platform_detect.h>
#include <coffee/core/base/cdebug.h>
#include <coffee/core/plat/plat_core.h>
#include <coffee/core/unit_tests/data_types.h>
#include <coffee/core/unit_tests/memory_operations.h>
#include <coffee/core/plat/memory/cmd_interface.h>

#include <coffee/core/plat/environment/process_def.h>
#include <coffee/core/plat/environment/sysinfo_def.h>
#include <coffee/core/plat/timing/profiling.h>

static bool coffee_initialized = false;

namespace Coffee{

void sighandle(int sig)
{
    /* If we use an alternate buffer, switch back to primary */
    Cmd::ResetScreen();
    switch(sig)
    {
    case SIGILL:
        Cmd::Exit(CoffeeExit_Termination);
        break;
    case SIGINT:
        Cmd::Exit(CoffeeExit_Interrupt);
        break;
    case SIGTERM:
        Cmd::Exit(CoffeeExit_Termination);
        break;
#if defined(COFFEE_LINUX)
    case SIGKILL:
        Cmd::Exit(CoffeeExit_Kill);
#endif
    default:
        Cmd::Exit(CoffeeExit_UnknownBad);
    }
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

    coffee_initialized = true;

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

int32 CoffeeMain(CoffeeMainWithArgs mainfun, int32 argv, cstring_w*argc)
{
    //TODO: Handle the Windows case of not including the application name
    CoffeeInit();
    int32 r = mainfun(argv,argc);
    CoffeeTerminate();
    return r;
}

void CoffeeTerminate()
{
    Cmd::ResetScreen();
}

}

namespace Coffee{
/* Don't mind this, just some string storage */
thread_local CString LinuxSysInfo::cached_cpuinfo_string;

std::list<Profiler::DataPoint> Profiler::datapoints;
std::list<CString> Profiler::context_stack;
Mutex Profiler::data_access_mutex;

}