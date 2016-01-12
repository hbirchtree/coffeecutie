#include "coffee.h"

#include <signal.h>

#include "coffee/core/plat/platform_detect.h"
#include "base/cdebug.h"
#include "coffee/core/plat/plat_core.h"
#include "coffee/core/unit_tests/data_types.h"
#include "coffee/core/unit_tests/memory_operations.h"

static bool coffee_initialized = false;

namespace Coffee{

void sighandle(int sig)
{
    switch(sig)
    {
    case SIGILL:
        exit(CoffeeExit_Termination);
        break;
    case SIGINT:
        exit(CoffeeExit_Interrupt);
        break;
    case SIGTERM:
        exit(CoffeeExit_Termination);
        break;
#if defined(COFFEE_LINUX)
    case SIGKILL:
        exit(CoffeeExit_Kill);
#endif
    default:
        exit(CoffeeExit_UnknownBad);
    }
}

void CoffeeInit()
{
    coffee_initialized = true;

    //Allow core dump by default
    coffee_enable_core_dump();

    //Set up signal handlers, make the process more well-behaved
    signal(SIGILL,sighandle);
    signal(SIGINT,sighandle);
#if defined(COFFEE_LINUX)
    signal(SIGKILL,sighandle);
#endif
    signal(SIGTERM,sighandle);

#ifndef NDEBUG
    //Run unit tests, ensuring that the system and compilation process is sane
    CoffeeTests::run_type_tests();
    CoffeeTests::run_memory_tests();
#endif
}

int32 CoffeeMain(CoffeeMainWithArgs mainfun, int32 argv, byte_t **argc)
{
    //TODO: Handle the Windows case of not including the application name
    CoffeeInit();
#ifndef NDEBUG
    cDebug("Build string: {0}",CoffeeBuildString);
#endif
    if(!coffee_initialized)
        cWarning("CoffeeInit was not called");
    cDebug("Running on {0} ({1})",CoffeeCompilerString,CoffeeArchString);
    int32 r = mainfun(argv,argc);
    CoffeeTerminate();
    return r;
}

void CoffeeTerminate()
{

}

}
