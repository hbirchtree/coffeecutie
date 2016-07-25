#include <coffee/core/coffee.h>

#include <coffee/core/CMD>
#include <coffee/core/CDebug>

#include <coffee/core/coffee_version.h>

#include <coffee/core/profiler/profiling-export.h>
#include <coffee/core/argument_handling.h>
#include <coffee/core/plat/environment/process_def.h>
#include <coffee/core/types/cdef/infotypes.h>

namespace Coffee{

CString plat_tmp_string;

/* Information that is not otherwise available */
ApplicationData_t CoffeeApplicationData;
cstring CoffeeCompilerString = C_COMPILER_DEFINE_STRING(C_COMPILER_NAME,
                                                        C_COMPILER_VER_MAJ,
                                                        C_COMPILER_VER_MIN,
                                                        C_COMPILER_VER_REV);
cstring CoffeeArchString = COFFEE_ARCH;
cstring CoffeeBuildString = COFFEE_BUILD_STRING;

cstring CoffeePlatformString = C_SYSTEM_STRING;

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

#if defined(COFFEE_ANDROID)
    plat_tmp_string = cStringFormat("Android ({0})",__ANDROID_API__);
    CoffeePlatformString = plat_tmp_string.c_str();
#endif

    cDebug("Build string: {0}",CoffeeBuildString);

    cDebug("Compiled for {0} on {1} ({2})",
           CoffeePlatformString,CoffeeCompilerString,CoffeeArchString);
    cDebug("Running on {0}",PlatformData::SystemDisplayString());
}

FORCEDINLINE void PrintVersionInfo()
{
    cOutputPrint("{0}, released by {1}, version {2}",
                CoffeeApplicationData.application_name,
                CoffeeApplicationData.organization_name,
                CoffeeApplicationData.version_code);
}

FORCEDINLINE void PrintBuildInfo()
{
    cOutputPrint("Running {0} build {1}",
                "Coffee",
                CoffeeBuildString);
}

FORCEDINLINE void PrintHelpInfo(ArgumentCollection const& arg)
{
    cOutputPrint("{0}",arg.helpMessage());
}

int32 CoffeeMain(CoffeeMainWithArgs mainfun, int32 argc, cstring_w*argv)
{
#ifndef NDEBUG
    Coffee::PrintingVerbosityLevel = 3;
#else
    Coffee::PrintingVerbosityLevel = 1;
#endif

    cDebug("Verbosity level: {0}",Coffee::PrintingVerbosityLevel);

#ifdef COFFEE_SLAP_LOWMEM
    /*
     * Dealing with non-PAE systems is a pain in the ass, fuck it
     * They are unlikely to have enough memory and processing power anyway.
     *
     */

    if(!SysInfo::HasPAE() && !PlatformData::IsMobile())
    {
        cOutputPrint("Unsupported system, insufficient addressing space");
        return 1;
    }
#endif

    initargs = AppArg(argc,argv);

    {
        ArgumentCollection parser;
        parser.registerArgument(ArgumentCollection::Switch,"help","h",
                                "Print help information and exit");
        parser.registerArgument(ArgumentCollection::Switch,"version",nullptr,
                                "Print version information and exit");
        parser.registerArgument(ArgumentCollection::Switch,nullptr,"v",
                                "Print verbose messages to terminal while running");
        parser.registerArgument(ArgumentCollection::Switch,"licenses",nullptr,
                                "Print license information and exit");

        parser.parseArguments(argc,argv);

        for(Pair<CString,bool> const& a : parser.getSwitchOptions())
        {
            if((a.first == "help" || a.first == "h") && a.second)
            {
                PrintVersionInfo();
                PrintHelpInfo(parser);
                return 0;
            }
            if(a.first == "v" && a.second)
            {
                Coffee::PrintingVerbosityLevel++;
            }
            if(a.first == "version" && a.second)
            {
                PrintVersionInfo();
                PrintBuildInfo();
                return 0;
            }
            if(a.first == "licenses" && a.second)
            {
                cOutputPrint("{0}",CoffeeLicenseString);
                return 0;
            }
        }
    }

    cVerbose("Initializing profiler");
    Profiler::InitProfiler();
    Profiler::LabelThread("Main");

    Profiler::PushContext("CoffeeMain");

    cVerbose("Initializing Coffee library");
    CoffeeInit(false);
    cVerbose("Calling Profile()");
    Profiler::Profile("Init");

    cVerbose("Entering main function");
    Profiler::PushContext("main()");
    int32 r = mainfun(argc,argv);
    Profiler::PopContext();
    Profiler::Profile("Runtime");

    cVerbose("Terminating library");
    CoffeeTerminate(false);
    Profiler::Profile("Termination");
    Profiler::PopContext();

    cVerbose("Unloading profiler");
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
        break;
    case SIGABRT:
        if(exit_handle)
            exit_handle();
        Cmd::Exit(CoffeeExit_PoopedABit);
        break;
    case SIGILL:
        if(exit_handle)
            exit_handle();
        Cmd::Exit(CoffeeExit_Termination);
        break;
    case SIGINT:
    {
        Profiling::ExitRoutine(GetInitArgs().argc,GetInitArgs().argv);
        if(exit_handle)
            exit_handle();
        Cmd::Exit(CoffeeExit_Interrupt);
        break;
    }
    case SIGTERM:
    {
        Profiling::ExitRoutine(GetInitArgs().argc,GetInitArgs().argv);
        if(exit_handle)
            exit_handle();
        Cmd::Exit(CoffeeExit_Termination);
        break;
    }
    default:
        Cmd::Exit(CoffeeExit_UnknownBad);
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

#if !defined(COFFEE_ANDROID) || !defined(ANDROID_DONT_USE_SDL2)

bool EventProcess(int)
{
    return true;
}

#endif

}
