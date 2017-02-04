#include <coffee/core/coffee.h>

#include <coffee/core/CMD>
#include <coffee/core/CDebug>
#include <coffee/core/CProfiling>

#include <coffee/core/coffee_version.h>

#include <coffee/core/profiler/profiling-export.h>
#include <coffee/core/argument_handling.h>
#include <coffee/core/plat/environment/process_def.h>
#include <coffee/core/types/cdef/infotypes.h>
#include <coffee/core/plat/plat_file.h>

#include <coffee/core/coffee_signals.h>

namespace Coffee{

#if defined(COFFEE_ANDROID)
CString plat_tmp_string;
#endif

/* Information that is not otherwise available */
cstring CoffeeCompilerString = C_COMPILER_DEFINE_STRING(C_COMPILER_NAME,
                                                        C_COMPILER_VER_MAJ,
                                                        C_COMPILER_VER_MIN,
                                                        C_COMPILER_VER_REV);
cstring CoffeeArchString = COFFEE_ARCH;
cstring CoffeeBuildString = COFFEE_BUILD_STRING;

cstring CoffeePlatformString = C_SYSTEM_STRING;

CString CoffeeDefaultWindowName;

extern CoffeeApplicationData app_data;

FORCEDINLINE void PrintVersionInfo()
{
    cOutputPrint("{0}, released by {1}, version {2}",
                app_data.application_name,
                app_data.organization_name,
                app_data.version_code);
}

FORCEDINLINE void PrintBuildInfo()
{
    cOutputPrint("Running {0} build {1}",
                "Coffee",
                CoffeeBuildString);
}

FORCEDINLINE void PrintArchitectureInfo()
{
    cOutputPrint("Compiled for {0} on {1} ({2})",
                 CoffeePlatformString,CoffeeCompilerString,
                 CoffeeArchString);
    cOutputPrint("Executing on {0}",PlatformData::SystemDisplayString());
    cOutputPrint("Device: {0}",SysInfo::DeviceName());
}

FORCEDINLINE void PrintHelpInfo(ArgumentCollection const& arg)
{
    cOutputPrint("{0}",arg.helpMessage());
}

FORCEDINLINE void PrintLicenseInfo()
{
    cVerbose(6,"Number of licenses to print: {0}",CoffeeLicenseCount);
    for(unsigned int i=0;i<CoffeeLicenseCount;i++)
    {
        cOutputPrint("{0}",CoffeeLicenseString[i]);
        if(i<(CoffeeLicenseCount-1))
            cOutputPrint("\n\n--|END OF LICENSE|--\n\n");
    }
}

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

#if defined(COFFEE_ANDROID)
    plat_tmp_string = cStringFormat("Android ({0})",__ANDROID_API__);
    CoffeePlatformString = plat_tmp_string.c_str();
#endif

    PrintVersionInfo();
    PrintBuildInfo();
    PrintArchitectureInfo();

    cVerbose(1,"Verbosity level: {0}",Coffee::PrintingVerbosityLevel);
}

int32 CoffeeMain(CoffeeMainWithArgs mainfun, int32 argc, cstring_w*argv)
{
#ifndef NDEBUG
    Coffee::PrintingVerbosityLevel = 7;
    DefaultPrintOutputPipe = DefaultDebugOutputPipe;
#else
    Coffee::PrintingVerbosityLevel = 1;
#endif

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

    CoffeeDefaultWindowName = app_data.application_name + " [OpenGL]";

    initargs = AppArg(argc,argv);

    {
        ArgumentCollection parser;
        parser.registerArgument(ArgumentCollection::Switch,"help","h",
                                "Print help information and exit");
        parser.registerArgument(ArgumentCollection::Switch,"version",nullptr,
                                "Print version information and exit");
        parser.registerArgument(ArgumentCollection::Switch,nullptr,"v",
                                "Print verbose messages to terminal while running");
        parser.registerArgument(ArgumentCollection::Switch,nullptr,"q",
                                "Be quiet");
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
            if(a.first == "q" && a.second)
            {
                Coffee::PrintingVerbosityLevel = 0;
            }
            if(a.first == "version" && a.second)
            {
                PrintVersionInfo();
                PrintBuildInfo();
                return 0;
            }
            if(a.first == "licenses" && a.second)
            {
                PrintLicenseInfo();
                return 0;
            }
        }
    }

    cVerbose(8,"Initializing profiler");
    Profiler::InitProfiler();
    Profiler::LabelThread("Main");

    Profiler::PushContext("CoffeeMain");

    cVerbose(8,"Initializing Coffee library");
    CoffeeInit(false);
    cVerbose(8,"Calling Profile()");
    Profiler::Profile("Init");

    cVerbose(8,"Entering main function");
    Profiler::PushContext("main()");
    int32 r = mainfun(argc,argv);
    Profiler::PopContext();
    Profiler::Profile("Runtime");

    cVerbose(8,"Terminating library");
    CoffeeTerminate(false);
    Profiler::Profile("Termination");
    Profiler::PopContext();

    cVerbose(8,"Unloading profiler");
    Profiling::ExitRoutine();

    cVerbose(8,"Successfully reached end of main()");

    return r;
}

void CoffeeTerminate(bool profiler_destroy)
{
    if(profiler_destroy)
        Profiler::DestroyProfiler();
    Cmd::ResetScreen();
}

#if !defined(COFFEE_ANDROID) || !defined(ANDROID_DONT_USE_SDL2)

bool EventProcess(int)
{
    return true;
}

#endif

void GotoApplicationDir()
{
    CString dir = Env::ApplicationDir();
    DirFun::ChDir(dir.c_str());
}

void InstallDefaultSigHandlers()
{
//    InstallSignalHandler(Sig_Termination,nullptr);
//    InstallSignalHandler(Sig_PoopedABit,nullptr);
//    InstallSignalHandler(Sig_ShitMySelf,nullptr);
//    InstallSignalHandler(Sig_FPE,nullptr);
}

const CoffeeApplicationData &ApplicationData()
{
    return app_data;
}

}
