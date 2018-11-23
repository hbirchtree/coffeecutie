#include <coffee/core/coffee.h>

#include <coffee/core/CFiles>
#include <coffee/core/CMD>
#include <coffee/core/argument_handling.h>
#include <coffee/core/base/jsonlogger.h>
#include <coffee/core/base/printing/outputprinter.h>
#include <coffee/core/coffee_args.h>
#include <coffee/core/formatting.h>
#include <coffee/core/internal_state.h>
#include <coffee/core/platform_data.h>
#include <coffee/core/profiler/profiling-export.h>
#include <coffee/core/task_queue/task.h>
#include <coffee/core/types/cdef/infotypes.h>
#include <peripherals/build/license.h>
#include <platforms/environment.h>
#include <platforms/file.h>
#include <platforms/process.h>
#include <platforms/sysinfo.h>

#include <coffee/core/CDebug>
#include <coffee/core/CProfiling>

#if defined(COFFEE_ANDROID)
#include <android_native_app_glue.h>
#endif

using namespace ::platform::info;

namespace Coffee {

enum StartFlags
{
    None                   = 0x0,
    DiscardArgumentHandler = 0x1,
    SilentInit             = 0x2,
};

/*!
 * \brief We use this internally to apply compiler-provided info
 * This information is "stored" in a function
 * \param binfo
 */
extern void SetBuildInfo(BuildInfo& binfo);

/*!
 * \brief Provides application-specific information
 * \param appdata
 */
extern void SetApplicationData(AppData& appdata);

#if defined(COFFEE_APPLE)
extern Url GetAppleStoragePath();
#endif

FORCEDINLINE void PrintVersionInfo()
{
#if !defined(COFFEE_LOWFAT) && !defined(COFFEE_LOADABLE_LIBRARY)
    auto const& app_data = ApplicationData();

    cOutputPrint(
        "{0}, released by {1}, version {2}",
        app_data.application_name,
        app_data.organization_name,
        app_data.version_code);
#endif
}

FORCEDINLINE void PrintBuildInfo()
{
#ifndef COFFEE_LOWFAT
    cOutputPrint(
        "Running {0} build {1}", "Coffee", State::GetBuildInfo().build_version);
#endif
}

FORCEDINLINE void PrintArchitectureInfo()
{
#ifndef COFFEE_LOWFAT
    cOutputPrint(
        "Compiled for {0} on {1} ({2})",
        State::GetBuildInfo().platform,
        State::GetBuildInfo().compiler,
        State::GetBuildInfo().architecture);
    cOutputPrint("Executing on {0}", info::system_name());
    cOutputPrint("Device: {0}", SysInfo::DeviceName());
#endif
}

FORCEDINLINE void PrintHelpInfo(args::ArgumentParser const& arg)
{
    cOutputPrint("{0}", arg.helpMessage());
}

FORCEDINLINE void PrintLicenseInfo()
{
#if !defined(COFFEE_LOWFAT) && !defined(COFFEE_LOADABLE_LIBRARY)
    cVerbose(6, "Number of licenses to print: {0}", CoffeeLicenseCount);
    for(unsigned int i = 0; i < CoffeeLicenseCount; i++)
    {
        cOutputPrint("{0}", CoffeeLicenseString[i]);
        if(i < (CoffeeLicenseCount - 1))
            cOutputPrint("\n\n--|END OF LICENSE|--\n\n");
    }
#endif
}

static void CoffeeInit_Internal(u32 flags)
{
#ifndef COFFEE_LOWFAT
#if MODE_DEBUG

    /* Allow core dump by default in debug mode */
    ProcessProperty::CoreDumpEnable();

#if !defined(COFFEE_DISABLE_PROFILER)
    State::GetProfilerStore()->enable();
#endif
#else
    Coffee::PrintingVerbosityLevel()          = 1;
#endif

#if defined(COFFEE_ANDROID)
    State::GetBuildInfo().plat_tmp_string =
        cStringFormat("Android ({0})", __ANDROID_API__);
    State::GetBuildInfo().platform =
        State::GetBuildInfo().plat_tmp_string.c_str();
#endif

    if(!(flags & SilentInit))
    {
        PrintVersionInfo();
        PrintBuildInfo();
        PrintArchitectureInfo();
    }

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
    State::GetBuildInfo().default_window_name =
        ApplicationData().application_name + " [OpenGL]";
#else
    State::GetBuildInfo().default_window_name = "Coffee [OpenGL]";
#endif

    CurrentThread::SetName(State::GetAppData().application_name);
#endif
}

void CoffeeInit(bool)
{
    CoffeeInit_Internal(0x0);
}

i32 CoffeeMain(CoffeeMainWithArgs mainfun, i32 argc, cstring_w* argv, u32 flags)
{
    auto start_time = Chrono::high_resolution_clock::now();

    /* Contains all global* state
     *  (*except RuntimeQueue, which is separate) */
    State::SetInternalState(State::CreateNewState());

    Coffee::PrintingVerbosityLevel() = 1;

#ifndef COFFEE_LOWFAT
    /* AppData contains the application name and etc. from AppInfo_*.cpp */
    SetApplicationData(State::GetAppData());
    /* BuildInfo contains information on the compiler, architecture
     *  and platform */
    SetBuildInfo(State::GetBuildInfo());

#endif

    /* Must be created before ThreadState, but after internal state */
    State::SwapState("jsonProfiler", Profiling::CreateJsonProfiler());

#if defined(COFFEE_CUSTOM_EXIT_HANDLING)
    /* On Android and iOS, we want to terminate the profiler early */
    CmdInterface::BasicTerm::RegisterAtExit([]() {
        State::SwapState("jsonProfiler", ShPtr<State::GlobalState>());
    });
#endif

    State::SetInternalThreadState(State::CreateNewThreadState());

    /* Create initial RuntimeQueue context for the user */
    RuntimeQueue::SetQueueContext(RuntimeQueue::CreateContext());

#if defined(COFFEE_CUSTOM_EXIT_HANDLING)
    CmdInterface::BasicTerm::RegisterAtExit([]() {
        runtime_queue_error ec;
        RuntimeQueue::TerminateThreads(ec);
    });
#endif

    /* Set the program arguments so that we can look at them later */
    GetInitArgs() = AppArg::Clone(argc, argv);

#if defined(COFFEE_ANDROID)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    app_dummy();
#pragma clang diagnostic pop
#endif

#if defined(COFFEE_APPLE)
    FileResourcePrefix(GetAppleStoragePath().internUrl.c_str());
#endif

#if MODE_DEBUG
    InstallDefaultSigHandlers();
#endif

#if !defined(COFFEE_LOWFAT)
#if !defined(COFFEE_CUSTOM_EXIT_HANDLING)
    if(!(flags & DiscardArgumentHandler))
    {
        auto parser = BaseArgParser::GetBase();
        auto args   = parser.parseArguments(GetInitArgs());
        auto ret    = BaseArgParser::PerformDefaults(parser, args);

        if(ret > 0)
            return ret;
    } else
    {
        Coffee::PrintingVerbosityLevel() = 1;
    }
#endif

#if defined(COFFEE_DEFAULT_VERBOSITY)
    Coffee::PrintingVerbosityLevel() = 12;
#endif

    Profiler::PushContext("CoffeeInit");
    CoffeeInit_Internal(flags);
    Profiler::PopContext();

    if(Env::ExistsVar("COFFEE_DEEP_PROFILE"))
        State::GetProfilerStore()->flags.deep_enabled = true;

    if(!(flags & SilentInit))
        cVerbose(1, "Verbosity level: {0}", Coffee::PrintingVerbosityLevel());

    /* This is a bit more versatile than simple procedures
     */
    Cmd::RegisterAtExit(CoffeeTerminate);

    cVerbose(8, "Entering main function");
    Profiler::PushContext("main()");
#endif

    i32 result = mainfun(argc, argv);

#ifndef COFFEE_LOWFAT
    Profiler::PopContext();

    cBasicPrint(
        "Execution time: {0}",
        Chrono::duration_cast<Chrono::seconds_double>(
            Chrono::high_resolution_clock::now() - start_time)
            .count());
#endif
    return result;
}

void CoffeeTerminate()
{
    cVerbose(5, "Terminating");

#ifndef COFFEE_LOWFAT

#if MODE_DEBUG
#if defined(COFFEE_LINUX)
    /* Do runtime leak checks for POSIX resources,
     *  useful when debugging resource APIs */

    Vector<DirFun::DirItem_t> file_descs;
    auto                      procFd = Path("/proc/self/fd");

    file_error ec;

    cDebug("Open POSIX file descriptors:");
    if(DirFun::Ls(MkUrl(procFd, RSCA::SystemFile), file_descs, ec))
        for(auto const& f : file_descs)
        {
            cBasicPrint("{0} : {1}", f.name, (procFd + f.name).canonical());
        }

    using MMAP = Environment::Linux::MemMap;

    MMAP::ProcMap mem_map;
    MMAP::GetProcMap(ProcessProperty::Pid(), mem_map);

    cDebug("Open POSIX mmap() regions:");
    for(auto const& e : mem_map)
    {
        /* Shared libraries are not our concern most of the time */
        if(e.name.find(".so") != CString::npos)
            continue;

        cBasicPrint(
            "{0} => {1}+{2}, {3}",
            e.name.size() ? e.name : "[anon]",
            str::print::pointerify(e.start),
            e.end - e.start,
            str::print::pointerify(C_CAST<u32>(e.access)));
    }
#endif
#endif

#ifndef COFFEE_CUSTOM_EXIT_HANDLING
    Profiling::ExitRoutine();
#endif
    Cmd::ResetScreen();
#endif
}

void GotoApplicationDir()
{
    file_error ec;

    Url dir = Env::ApplicationDir();
    DirFun::ChDir(MkUrl(dir), ec);
}

void InstallDefaultSigHandlers()
{
#if !defined(COFFEE_CUSTOM_STACKTRACE)
    std::set_terminate([]() {
        Stacktracer::ExceptionStacktrace(std::current_exception());
        abort();
    });
#endif
}

void SetPrintingVerbosity(C_UNUSED(u8 level))
{
    Coffee::PrintingVerbosityLevel() = level;
}

namespace BaseArgParser {

ArgumentParser& GetBase()
{
    static ArgumentParser parser;

    if(!parser.arguments.size())
    {
        parser.addSwitch(
            "help", "help", "h", "Print help information and exit");

        parser.addSwitch(
            "version",
            "version",
            nullptr,
            "Print version information and exit");

        parser.addSwitch(
            "verbose",
            nullptr,
            "v",
            "Print verbose messages to terminal while running");

        parser.addSwitch("quiet", nullptr, "q", "Be quiet");

        parser.addSwitch(
            "licenses",
            "licenses",
            nullptr,
            "Print license information and exit");

        parser.addSwitch(
            "dprofile", "deep-profile", nullptr, "Enable deep profiling");

        parser.addSwitch("json", "json", nullptr, "Output information as JSON");

        parser.addPositionalArgument(
            "resource_prefix",

            "Change resource prefix"
            " (only works if application does not"
            " override resource prefix)");
    }

    return parser;
}

int PerformDefaults(ArgumentParser& parser, ArgumentResult& args)
{
    for(Pair<CString, u32> sw_ : args.switches)
    {
        auto sw = sw_.first;
        if(sw == "help")
        {
            PrintVersionInfo();
            PrintHelpInfo(parser);
            return 0;
        } else if(sw == "verbose")
        {
            Coffee::PrintingVerbosityLevel() += sw_.second;
        } else if(sw == "quiet")
        {
            Coffee::PrintingVerbosityLevel() -= sw_.second;
        } else if(sw == "version")
        {
            PrintVersionInfo();
            PrintBuildInfo();
            return 0;
        } else if(sw == "licenses")
        {
            PrintLicenseInfo();
            return 0;
        } else if(sw == "dprofile")
        {
            State::GetProfilerStore()->flags.deep_enabled = true;
        } else if(sw == "json")
        {
            DebugFun::SetLogInterface(SetupJsonLogger("application.json"_tmp));
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

    return -1;
}

} // namespace BaseArgParser

} // namespace Coffee
