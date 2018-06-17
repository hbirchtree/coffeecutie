#include <coffee/core/coffee.h>

#include <coffee/core/CMD>
#include <coffee/core/CProfiling>

#include <coffee/core/coffee_signals.h>
#include <coffee/core/coffee_version.h>

#include <coffee/core/CFiles>
#include <coffee/core/argument_handling.h>
#include <coffee/core/plat/environment/process_def.h>
#include <coffee/core/plat/plat_file.h>
#include <coffee/core/profiler/profiling-export.h>
#include <coffee/core/types/cdef/infotypes.h>

#include <coffee/core/internal_state.h>
#include <coffee/core/task_queue/task.h>

#include <coffee/core/CDebug>

#if defined(COFFEE_LINUX)
#include <execinfo.h>
#endif

#if defined(COFFEE_ANDROID)
#include <android_native_app_glue.h>
#endif

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
extern void SetApplicationData(CoffeeApplicationData& appdata);

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
    cOutputPrint("Executing on {0}", PlatformData::SystemDisplayString());
    cOutputPrint("Device: {0}", SysInfo::DeviceName());
#endif
}

FORCEDINLINE void PrintHelpInfo(ArgumentParser const& arg)
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
#ifndef NDEBUG
    PrintingVerbosityLevel() = 8;
//    DefaultPrintOutputPipe   = DefaultDebugOutputPipe;
#else
    Coffee::PrintingVerbosityLevel()          = 1;
#endif

#ifndef NDEBUG
    /* Allow core dump by default in debug mode */
    ProcessProperty::CoreDumpEnable();
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

#ifndef NDEBUG
    InstallDefaultSigHandlers();
#endif

    Profiler::InitProfiler();
    Profiler::LabelThread("Main");
#endif
}

void CoffeeInit(bool)
{
    CoffeeInit_Internal(0x0);
}

int32 CoffeeMain(
    CoffeeMainWithArgs mainfun, int32 argc, cstring_w* argv, u32 flags)
{
    /* Contains all global* state
     *  (*except RuntimeQueue, which is separate) */
    State::SetInternalState(State::CreateNewState());
    State::SetInternalThreadState(State::CreateNewThreadState());

#ifndef COFFEE_LOWFAT
    /* AppData contains the application name and etc. from AppInfo_*.cpp */
    SetApplicationData(State::GetAppData());
    /* BuildInfo contains information on the compiler, architecture
     *  and platform */
    SetBuildInfo(State::GetBuildInfo());
#endif

    /* Create initial RuntimeQueue context for the user */
    RuntimeQueue::SetQueueContext(RuntimeQueue::CreateContext());

    /* Set the program arguments so that we can look at them later */
    GetInitArgs() = AppArg::Clone(argc, argv);

#if defined(COFFEE_ANDROID)
    app_dummy();
#endif

#if defined(COFFEE_APPLE)
    FileResourcePrefix(GetAppleStoragePath().internUrl.c_str());
#endif

#ifndef COFFEE_LOWFAT

    CoffeeInit_Internal(flags);

    Profiler::PushContext("CoffeeMain");
    Profiler::Profile("Init");

    Profiler::PushContext("Argument parsing");

    if(!(flags & DiscardArgumentHandler))
    {
        ArgumentParser parser;
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

        parser.addPositionalArgument(
            "resource_prefix",

            "Change resource prefix"
            " (only works if application does not"
            " override resource prefix)");

        auto args = parser.parseArguments(GetInitArgs());

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
    } else
    {
        Coffee::PrintingVerbosityLevel() = 1;
    }

    Profiler::PopContext();

    if(!(flags & SilentInit))
        cVerbose(1, "Verbosity level: {0}", Coffee::PrintingVerbosityLevel());

    /* This is a bit more versatile than simple procedures
     */
    Cmd::RegisterAtExit(CoffeeTerminate);

    cVerbose(8, "Entering main function");
    Profiler::PushContext("main()");
#endif

    int32 r = mainfun(argc, argv);

#ifndef COFFEE_LOWFAT
    Profiler::PopContext();
    Profiler::PopContext();
#endif
    return r;
}

void CoffeeTerminate()
{
    cVerbose(5, "Terminating");

//    runtime_queue_error qec;
//    RuntimeQueue::TerminateThreads(qec);

#ifndef COFFEE_LOWFAT

#ifndef NDEBUG
#if defined(COFFEE_LINUX)
    /* Do runtime leak checks for POSIX resources,
     *  useful when debugging resource APIs */

    Vector<DirFun::DirItem_t> file_descs;
    auto procFd = Path("/proc/self/fd");

    file_error ec;

    cDebug("Open POSIX file descriptors:");
    if(DirFun::Ls(MkUrl(procFd, RSCA::SystemFile), file_descs, ec))
        for(auto const& f : file_descs)
        {
            cBasicPrint(
                "{0} : {1}",
                f.name,
                (procFd + f.name).canonical());
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
            StrUtil::pointerify(e.start),
            e.end - e.start,
            StrUtil::pointerify(C_CAST<u32>(e.access)));
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

#if defined(COFFEE_LINUX) && !defined(COFFEE_NO_EXCEPTION_RETHROW)
static void glibc_backtrace()
{
    static constexpr szptr MAX_CONTEXT = 20;

    void* tracestore[MAX_CONTEXT];

    auto exc_ptr = std::current_exception();
    try {
        if(exc_ptr)
            std::rethrow_exception(exc_ptr);
    } catch(std::exception& e)
    {
        if(Cmd::Interactive())
            cBasicPrint("{0}", StrUtil::multiply('-', Cmd::TerminalSize().w));
        cBasicPrint("exception encountered:");
        cBasicPrint(" >> {0}: {1}",
                 Stacktracer::DemangleSymbol(typeid(e).name()),
                 e.what()); 
        auto num = backtrace(tracestore, MAX_CONTEXT);
        auto syms = backtrace_symbols(tracestore, num);
        if(syms && num)
        {
            cBasicPrint("dumping stacktrace:");
            for(auto i : Range<>(num))
            {
                if(syms[i])
                {
                    CString sym_ = syms[i];
                    auto sym_end = sym_.rfind('+');
                    auto sym_begin = sym_.rfind('(', sym_end);
                    if(sym_end != CString::npos && sym_begin != CString::npos)
                    {
                        auto sym_length = sym_end - sym_begin - 1;
                        auto sym_target = sym_.substr(sym_begin + 1, sym_length);
                        sym_ = CStrReplace(
                                 sym_, 
                                 sym_target, 
                                 Stacktracer::DemangleSymbol(sym_target));
                        cBasicPrint(" >> {0}", sym_);
                    }
                }else
                    cBasicPrint(" >> {0}", Stacktracer::DemangleSymbol(syms[i]));
            }
        }
        abort();
    }
}
#endif

void InstallDefaultSigHandlers()
{
    #if defined(COFFEE_LINUX) && !defined(COFFEE_NO_TERMINATION_HANDLER)
    std::set_terminate(glibc_backtrace);
    #endif
    //    InstallSignalHandler(Sig_Termination,nullptr);
    //    InstallSignalHandler(Sig_PoopedABit,nullptr);
    //    InstallSignalHandler(Sig_ShitMySelf,nullptr);
    //    InstallSignalHandler(Sig_FPE,nullptr);
}

void SetPrintingVerbosity(C_MAYBE_UNUSED u8 level)
{
#ifndef COFFEE_LOWFAT
    Coffee::PrintingVerbosityLevel() = level;
#endif
}

} // namespace Coffee
