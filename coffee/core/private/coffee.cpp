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

#ifndef COFFEE_LOADABLE_LIBRARY
extern CoffeeApplicationData app_data;
#endif

FORCEDINLINE void PrintVersionInfo()
{
#if !defined(COFFEE_LOWFAT) && !defined(COFFEE_LOADABLE_LIBRARY)
    cOutputPrint("{0}, released by {1}, version {2}",
                app_data.application_name,
                app_data.organization_name,
                app_data.version_code);
#endif
}

FORCEDINLINE void PrintBuildInfo()
{
#ifndef COFFEE_LOWFAT
    cOutputPrint("Running {0} build {1}",
                "Coffee",
                CoffeeBuildString);
#endif
}

FORCEDINLINE void PrintArchitectureInfo()
{
#ifndef COFFEE_LOWFAT
    cOutputPrint("Compiled for {0} on {1} ({2})",
                 CoffeePlatformString,CoffeeCompilerString,
                 CoffeeArchString);
    cOutputPrint("Executing on {0}",PlatformData::SystemDisplayString());
    cOutputPrint("Device: {0}",SysInfo::DeviceName());
#endif
}

FORCEDINLINE void PrintHelpInfo(ArgumentParser const& arg)
{
    cOutputPrint("{0}",arg.helpMessage());
}

FORCEDINLINE void PrintLicenseInfo()
{
#if !defined(COFFEE_LOWFAT) && !defined(COFFEE_LOADABLE_LIBRARY)
    cVerbose(6,"Number of licenses to print: {0}",CoffeeLicenseCount);
    for(unsigned int i=0;i<CoffeeLicenseCount;i++)
    {
        cOutputPrint("{0}",CoffeeLicenseString[i]);
        if(i<(CoffeeLicenseCount-1))
            cOutputPrint("\n\n--|END OF LICENSE|--\n\n");
    }
#endif
}

void CoffeeInit(bool profiler_init)
{
#ifndef COFFEE_LOWFAT
#ifndef NDEBUG
    Coffee::PrintingVerbosityLevel = 8;
    DefaultPrintOutputPipe = DefaultDebugOutputPipe;
#else
    Coffee::PrintingVerbosityLevel = 1;
#endif
#endif

#ifndef COFFEE_LOWFAT
    cVerbose(1,"Verbosity level: {0}",Coffee::PrintingVerbosityLevel);
#endif

#ifndef COFFEE_LOWFAT
    if(profiler_init)
    {
        Profiler::InitProfiler();
        Profiler::LabelThread("Main");
    }
#endif

#ifndef NDEBUG
    /* Allow core dump by default in debug mode */
    ProcessProperty::CoreDumpEnable();
#endif

#if defined(COFFEE_ANDROID)
    plat_tmp_string = cStringFormat("Android ({0})",__ANDROID_API__);
    CoffeePlatformString = plat_tmp_string.c_str();
#endif

    PrintVersionInfo();
    PrintBuildInfo();
    PrintArchitectureInfo();

#ifdef COFFEE_SLAP_LOWMEM
    /*
     * Dealing with non-PAE systems is a pain in the ass, fuck it
     * They are unlikely to have enough memory and processing power anyway.
     *
     */
    if(!SysInfo::HasPAE() && !PlatformData::IsMobile())
    {
        cOutputPrint("Unsupported system, insufficient addressing space");
        Cmd::Exit(1);
    }
#endif

#ifndef COFFEE_LOADABLE_LIBRARY
    CoffeeDefaultWindowName = app_data.application_name + " [OpenGL]";
#else
    CoffeeDefaultWindowName = "Coffee [OpenGL]";
#endif

    cVerbose(8,"Initializing profiler");
    Profiler::InitProfiler();
    Profiler::LabelThread("Main");
}

int32 CoffeeMain(CoffeeMainWithArgs mainfun, int32 argc, cstring_w*argv)
{
    initargs = AppArg::Clone(argc, argv);

#ifndef COFFEE_LOWFAT

    CoffeeInit(false);

    Profiler::PushContext("CoffeeMain");
    Profiler::Profile("Init");

    Profiler::PushContext("Argument parsing");
    {
        ArgumentParser parser;
        parser.addSwitch(
                    "help",
                    "help","h",
                    "Print help information and exit");

        parser.addSwitch(
                    "version","version", nullptr,
                    "Print version information and exit");

        parser.addSwitch(
                    "verbose", nullptr, "v",
                    "Print verbose messages to terminal while running");

        parser.addSwitch(
                    "quiet",nullptr,"q",
                    "Be quiet");

        parser.addSwitch(
                    "licenses","licenses", nullptr,
                    "Print license information and exit");

        parser.addSwitch(
                    "dprofile", "deep-profile", nullptr,
                    "Enable deep profiling");

        parser.addPositionalArgument(
                    "resource_prefix",

                    "Change resource prefix"
                    " (only works if application does not"
                    " override resource prefix)");

        auto args = parser.parseArguments(initargs);

        for(CString sw : args.switches)
        {
            if(sw == "help")
            {
                PrintVersionInfo();
                PrintHelpInfo(parser);
                return 0;
            }else if(sw == "verbose")
            {
                Coffee::PrintingVerbosityLevel++;
            }else if(sw == "quiet")
            {
                Coffee::PrintingVerbosityLevel = 0;
            }else if(sw == "version")
            {
                PrintVersionInfo();
                PrintBuildInfo();
                return 0;
            }else if(sw == "licenses")
            {
                PrintLicenseInfo();
                return 0;
            }else if(sw == "dprofile")
            {
                Profiler::SetDeepProfileMode(true);
            }
        }

        for(auto arg : args.arguments)
        {
            if(arg.first == "resource_prefix")
                CResources::FileResourcePrefix(arg.second.c_str());
        }

        for(auto pos : args.positional)
        {
            if(pos.first == "resource_prefix")
                CResources::FileResourcePrefix(pos.second.c_str());
        }
    }
    Profiler::PopContext();

    /* This is a bit more versatile than simple procedures
     */
    Cmd::RegisterAtExit(CoffeeTerminate);

    cVerbose(8,"Entering main function");
    Profiler::PushContext("main()");
#endif
    int32 r = mainfun(argc,argv);

#ifndef COFFEE_LOWFAT
    Profiler::PopContext();
    Profiler::PopContext();
#endif
    return r;
}

void CoffeeTerminate()
{
    cDebug("Terminating");

#ifndef COFFEE_LOWFAT
#ifndef COFFEE_CUSTOM_EXIT_HANDLING
    Profiling::ExitRoutine();
#endif
//    if(profiler_destroy)
//        Profiler::DestroyProfiler();
    Cmd::ResetScreen();
#endif
}

void GotoApplicationDir()
{
    CString dir = Env::ApplicationDir();
    DirFun::ChDir(MkUrl(dir.c_str()));
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
#ifdef COFFEE_LOADABLE_LIBRARY
    static CoffeeApplicationData app_data;
#endif
    return app_data;
}

void SetPrintingVerbosity(C_MAYBE_UNUSED u8 level)
{
#ifndef COFFEE_LOWFAT
    Coffee::PrintingVerbosityLevel = level;
#endif
}

}
