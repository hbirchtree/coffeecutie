#include <coffee/core/coffee.h>

#include <coffee/core/CFiles>
#include <coffee/core/CProfiling>
#include <coffee/core/argument_handling.h>
#include <coffee/core/coffee_args.h>
#include <coffee/core/internal_state.h>
#include <coffee/core/logging/jsonlogger.h>
#include <coffee/core/platform_data.h>
#include <coffee/core/profiler/profiling-export.h>
#include <coffee/core/resource_prefix.h>
#include <coffee/core/task_queue/task.h>
#include <url/url.h>

#include <peripherals/build/build_info.h>
#include <peripherals/build/license.h>
#include <peripherals/libc/signals.h>
#include <peripherals/stl/string_ops.h>

#include <platforms/environment.h>
#include <platforms/file.h>
#include <platforms/profiling/jsonprofile.h>
#include <platforms/stacktrace.h>
#include <platforms/sysinfo.h>

#include <coffee/strings/info.h>
#include <coffee/strings/libc_types.h>
#include <coffee/strings/url_types.h>

#include <coffee/core/CDebug>

#if defined(COFFEE_ANDROID)
#include <android_native_app_glue.h>
#endif

#if defined(COFFEE_UNIXPLAT)
#include <peripherals/posix/process.h>
#endif

#if defined(COFFEE_WINDOWS) && !defined(__MINGW64__)
#if !defined(COFFEE_WINDOWS_UWP)
extern bool WMI_Query(
    const char* query, const wchar_t* property, std::string& target);
#endif
extern int InitCOMInterface();
#endif

#if defined(COFFEE_GEKKO)
#include <coffee/gexxo/gexxo_api.h>
#endif

using namespace ::platform;

namespace platform {
namespace url {

#if defined(COFFEE_APPLE)
extern Url GetAppleStoragePath();
#endif
} // namespace url
} // namespace platform

namespace Coffee {

enum StartFlags
{
    None                   = 0x0,
    DiscardArgumentHandler = 0x1,
    SilentInit             = 0x2,
};

MainStorage main_functions;
static bool silent_init{false};

void CaptureMainFunction(MainWithArgs mainfun)
{
    main_functions.with_args  = mainfun;
    main_functions.is_no_args = false;
}

void CaptureMainFunction(MainNoArgs mainfun)
{
    main_functions.no_args    = mainfun;
    main_functions.is_no_args = true;
}

int MainSetup(MainWithArgs mainfun, int argc, char** argv, u32 flags)
{
#if !defined(COFFEE_LOWFAT) && 0
    cDebug("Entering MainSetup() at {0}", str::print::pointerify(MainSetup));
#endif

#if defined(COFFEE_WINDOWS) && !defined(COFFEE_WINDOWS_UWP) \
    && !defined(__MINGW64__)
#if MODE_RELEASE
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#else
    if(platform::Env::GetVar("VisualStudioVersion").size())
        ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
    InitCOMInterface();
#elif defined(COFFEE_WINDOWS_UWP)
    InitCOMInterface();
#elif defined(COFFEE_GEKKO)
    gexxo::initialize();
    printf("- Gamecube video initialized\n");
#endif

    int stat = Coffee::CoffeeMain(mainfun, argc, argv, flags);

#if defined(COFFEE_GEKKO)
    gexxo::infiniteLoop();
#endif

#ifndef COFFEE_CUSTOM_EXIT_HANDLING
#if MODE_DEBUG
    platform::profiling::PContext::ProfilerStore()->disable();
#endif
    exit(stat);
#else
    return stat;
#endif
}

int MainSetup(MainNoArgs mainfun, int argc, char** argv, u32 flags)
{
    static MainNoArgs main_storage = mainfun;

    return MainSetup(
        [](libc_types::i32, char**) { return main_storage(); },
        argc,
        argv,
        flags);
}

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
extern void SetApplicationData(info::AppData& appdata);

FORCEDINLINE void PrintVersionInfo()
{
    if constexpr(compile_info::lowfat_mode)
        return;

    auto const& app_data = GetCurrentApp();

    cOutputPrint(
        "{0}, released by {1}, version {2} ({3} mode)",
        app_data.application_name,
        app_data.organization_name,
        app_data.version_code,
        compile_info::module::build_mode);
}

FORCEDINLINE void PrintBuildInfo()
{
    if constexpr(compile_info::lowfat_mode)
        return;

    cOutputPrint(
        "Running {0} build {1}", "Coffee", compile_info::engine_version);
}

FORCEDINLINE void PrintArchitectureInfo()
{
    if constexpr(compile_info::lowfat_mode)
        return;

    cOutputPrint(
        "Compiled for {0} on {1} {2} ({3})",
        compile_info::target,
        compile_info::compiler::name,
        compile_info::compiler::version_str,
        compile_info::architecture);
    cOutputPrint(
        "Executing on {0} {1} ({2})",
        info::os::kernel(),
        info::os::kernel_version(),
        info::os::architecture());
    if(auto device = info::device::device(); device.has_value())
        cOutputPrint(
            "Device: {0} {1}", device.value().first, device.value().second);
    if(auto device = info::device::motherboard(); device.has_value())
        cOutputPrint(
            "Board: {0} {1}", device.value().first, device.value().second);
    if(auto cpu = info::proc::model(); cpu.has_value())
        cOutputPrint(
            "Processor: {0} {1} @ {2} GHz",
            cpu.value().first,
            cpu.value().second,
            info::proc::frequency() / 1000000.f);
}

FORCEDINLINE void PrintHelpInfo(args::ArgumentParser const& arg)
{
    cOutputPrint("{0}", arg.helpMessage());
}

static void PrintLicenseInfo()
{
    if constexpr(compile_info::lowfat_mode)
        return;

    cVerbose(6, "Number of licenses to print: {0}", CoffeeLicenseCount);
    for(unsigned int i = 0; i < CoffeeLicenseCount; i++)
    {
        cOutputPrint("{0}", CoffeeLicenseString[i]);
        if(i < (CoffeeLicenseCount - 1))
            cOutputPrint("\n\n--|END OF LICENSE|--\n\n");
    }
}

static void CoffeeInit_Internal(u32)
{
    if constexpr(compile_info::lowfat_mode)
        return;

    if constexpr(compile_info::debug_mode)
    {
        /* Allow core dump by default in debug mode */
#if defined(COFFEE_UNIXPLAT)
        platform::common::posix::proc::core_dump_enable();
#endif

        if constexpr(compile_info::profiler::enabled)
        {
            WkPtrUnwrap<platform::profiling::PContext> store(
                State::GetProfilerStore());
            store([](platform::profiling::PContext* context) {
                context->enable();
            });
        }
    } else
        Coffee::PrintingVerbosityLevel() = 1;

    if(!silent_init)
    {
        PrintVersionInfo();
        PrintBuildInfo();
        PrintArchitectureInfo();
    }

    State::GetBuildInfo().default_window_name
        = GetCurrentApp().application_name;
}

void SetPlatformState()
{
    /* Initialize state management in ::platform namespace */
    auto& platState = platform::state;

    platState->m_LockState
        = static_cast<stl_types::UqLock (*)(std::string_view)>(
            State::LockState);
    platState->SwapState = State::SwapState;
    platState->PeekState = State::PeekState;

    platState->ProfilerEnabled   = State::ProfilerEnabled;
    platState->GetProfilerStore  = State::GetProfilerStore;
    platState->GetProfilerTStore = State::GetProfilerTStore;

    platState->GetAppData = State::GetAppData;
}

void CoffeeInit(bool)
{
    CoffeeInit_Internal(0x0);
}

i32 CoffeeMain(MainWithArgs mainfun, i32 argc, cstring_w* argv, u32 flags)
{
    auto start_time = Chrono::high_resolution_clock::now();

    if constexpr(compile_info::debug_mode)
        InstallDefaultSigHandlers();

    /* Contains all global* state
     *  (*except RuntimeQueue, which is separate) */
    State::SetInternalState(State::CreateNewState());

    libc::signal::register_atexit([]() {
        /* On exit, set InternalState pointer to null */
        State::SetInternalState({});
    });

    SetPlatformState();

#if !defined(COFFEE_CUSTOM_EXIT_HANDLING)
    Coffee::PrintingVerbosityLevel() = 1;
#endif

    /* AppData contains the application name and etc. from AppInfo_*.cpp */
    if constexpr(!compile_info::lowfat_mode)
        if(auto appData = State::GetAppData())
        {
            SetApplicationData(*appData);
            CurrentThread::SetName(appData->application_name);
        }

    InstallStacktraceWriter();
    /* Must be created before ThreadState, but after internal state */
    if constexpr(compile_info::profiler::enabled)
        State::SwapState("jsonProfiler", profiling::json::CreateProfiler());

#if defined(COFFEE_CUSTOM_EXIT_HANDLING)
    /* On Android and iOS, we want to terminate the profiler early */
    libc::signal::register_atexit([]() {
        State::SwapState("jsonProfiler", ShPtr<State::GlobalState>());
    });
#endif

    State::SetInternalThreadState(State::CreateNewThreadState());

    /* Create initial RuntimeQueue context for the user */
    rq::runtime_queue::SetQueueContext(rq::runtime_queue::CreateContext());

#if defined(COFFEE_CUSTOM_EXIT_HANDLING)
    libc::signal::register_atexit(
        []() { rq::runtime_queue::TerminateThreads(); });
#endif

    /* Set the program arguments so that we can look at them later */
    GetInitArgs() = ::platform::args::AppArg::Clone(argc, argv);

    silent_init = (flags & SilentInit) == SilentInit;

    if constexpr(!compile_info::lowfat_mode)
    {
#if !defined(COFFEE_CUSTOM_EXIT_HANDLING)
        if((flags & DiscardArgumentHandler) == 0)
        {
            auto parser = BaseArgParser::GetBase();
            auto args   = parser.parseArguments(GetInitArgs());
            auto ret    = BaseArgParser::PerformDefaults(parser, args);

            if(ret > 0)
                return ret;
        } else
            Coffee::PrintingVerbosityLevel() = 1;
#endif

#if defined(COFFEE_DEFAULT_VERBOSITY)
        Coffee::PrintingVerbosityLevel() = 12;
#endif

        Profiler::PushContext("CoffeeInit");
        CoffeeInit_Internal(flags);
        Profiler::PopContext();

        if(env::var("COFFEE_DEEP_PROFILE"))
        {
            auto profilerState = State::GetProfilerStore();

            if(profilerState)
                profilerState->flags.deep_enabled = true;
        }

        if(!silent_init)
            cVerbose(
                1, "Verbosity level: {0}", Coffee::PrintingVerbosityLevel());

        /* This is a bit more versatile than simple procedures
         */
        libc::signal::register_atexit(CoffeeTerminate);

        if(!silent_init)
            cVerbose(8, "Entering main function");
        Profiler::PushContext("main()");
    }

    i32 result = -1;

    if constexpr(
        compile_info::platform::is_emscripten
        || compile_info::platform::is_gekko)
        try
        {
            result = mainfun(argc, argv);
        } catch(std::exception const& ex)
        {
            Logging::log(
                libc::io::io_handles::err,
                "Main",
                String("Exception encountered: ") + ex.what(),
                semantic::debug::Severity::Fatal);
            try
            {
                std::rethrow_if_nested(ex);
            } catch(std::exception const& ex2)
            {
                Logging::log(
                    libc::io::io_handles::err,
                    "Main",
                    String("Further information: ") + ex2.what(),
                    semantic::debug::Severity::Fatal);
            } catch(...)
            {
            }
        }
    else
        result = mainfun(argc, argv);

    if constexpr(!compile_info::lowfat_mode)
    {
        Profiler::PopContext();

        if(!silent_init)
            cDebug(
                "Execution time: {0}",
                Chrono::duration_cast<Chrono::seconds_double>(
                    Chrono::high_resolution_clock::now() - start_time)
                    .count());
    }

    if constexpr(
        compile_info::profiler::enabled && !compile_info::platform::custom_exit)
    {
        State::SwapState("jsonProfiler", {});
    }

    return result;
} // namespace Coffee

void CoffeeTerminate()
{
    using namespace ::platform::file;
    using namespace ::platform::url::constructors;

    if(!silent_init)
        cVerbose(5, "Terminating");

    if constexpr(compile_info::lowfat_mode)
        return;

#if MODE_DEBUG && defined(COFFEE_LINUX) && 0
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

    using MMAP = env::Linux::MemMap;

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

#ifndef COFFEE_CUSTOM_EXIT_HANDLING
    Profiling::ExitRoutine();
#endif
}

void GotoApplicationDir()
{
    path::change_dir(path::app_dir().value());
}

static file::file_handle stack_file;

#if !defined(COFFEE_CUSTOM_STACKTRACE)
static void stack_writer(
    std::string_view frame,
    std::string_view ip,
    std::string_view filename,
    u32              line)
{
    auto out = Strings::fmt(
        R"({"frame": "{0}", "ip": "{1}", "file": "{2}", "line": {3}},)",
        frame,
        ip,
        filename,
        line);
    if(auto error = file::write(stack_file, Bytes::ofContainer(out)))
        Throw(undefined_behavior(
            platform::file::error_message(error).value_or("")));
}

void generic_stacktrace(int sig)
{
    using platform::common::posix::proc::code_to_string;
    using platform::common::posix::proc::send_sig;
    using semantic::debug::Severity;
    using namespace libc::io;
    using namespace platform::stacktrace;

    auto sig_string = code_to_string(sig);

    fprintf(stderr, "signal encountered:\n");
    fprintf(stderr, " >> %s\n", sig_string);

    if constexpr(supports_stacktrace)
        print_frames(frames(), typing::logging::fprintf_logger, stack_writer);

    send_sig(getpid(), libc::signal::sig::kill);
}
#endif

void InstallDefaultSigHandlers()
{
    if constexpr(compile_info::platform::is_android)
        return;

    std::set_terminate([]() {
#if !defined(COFFEE_CUSTOM_STACKTRACE)
        if(auto frames = platform::stacktrace::exception_frames();
           frames.has_value())
            platform::stacktrace::print_exception(
                std::move(frames.value()),
                typing::logging::fprintf_logger,
                stack_writer);
#endif
        platform::common::posix::proc::breakpoint();
        libc::signal::exit(libc::signal::sig::kill);
    });

#if !defined(COFFEE_CUSTOM_STACKTRACE)
    using libc::signal::sig;

    libc::signal::install(sig::ill_op, generic_stacktrace);
    libc::signal::install(sig::abort, generic_stacktrace);
    libc::signal::install(sig::bus_error, generic_stacktrace);
    libc::signal::install(sig::fpe, generic_stacktrace);
    libc::signal::install(sig::segfault, generic_stacktrace);

    libc::signal::install(sig::terminate, [](i32) {
        CoffeeTerminate();
        libc::signal::exit(sig::terminate);
    });
    libc::signal::install(sig::interrupt, [](i32) {
        CoffeeTerminate();
        libc::signal::exit(sig::interrupt);
    });
#endif
}

void InstallStacktraceWriter()
{
#if COFFEE_GLIBC_STACKTRACE || COFFEE_UNWIND_STACKTRACE
    using namespace platform::url::constructors;
    if(auto fd = file::open_file(
           "stacktrace.json"_tmp,
           RSCA::Discard | RSCA::Append | RSCA::WriteOnly | RSCA::NewFile);
       fd.has_error())
    {
        if(auto error = file::error_message(fd.error()); error.has_value())
            Throw(undefined_behavior(error.value()));
        else
            Throw(undefined_behavior("unknown error"));
    } else
        stack_file = std::move(fd.value());
    file::truncate("stacktrace.json"_tmp);
    file::write(stack_file, BytesConst::ofString("["));

    libc::signal::register_atexit([]() {
        file::write(stack_file, BytesConst::ofString("{}]"));
        stack_file = {};
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

#if !COFFEE_FIXED_RESOURCE_DIR
        parser.addPositionalArgument(
            "resource_prefix",

            "Change resource prefix"
            " (only works if application does not"
            " override resource prefix)");
#endif
    }

    return parser;
}

int PerformDefaults(ArgumentParser& parser, ArgumentResult& args)
{
    using namespace ::platform::url::constructors;

    for(auto const& sw_ : args.switches)
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
        }
#if MODE_DEBUG
        else if(sw == "dprofile")
        {
            auto profilerState = State::GetProfilerStore();

            if(profilerState)
                profilerState->flags.deep_enabled = true;
        }
#endif
        else if(sw == "json")
        {
            if constexpr(!compile_info::lowfat_mode)
                DebugFun::SetLogInterface(
                    SetupJsonLogger("application.json"_tmp));
        }
    }

    for(auto const& arg : args.arguments)
    {
        if(arg.first == "resource_prefix")
            file::ResourcePrefix(arg.second.c_str());
    }

    for(auto const& pos : args.positional)
    {
        if(pos.first == "resource_prefix")
            file::ResourcePrefix(pos.second.c_str());
    }

    return -1;
}

} // namespace BaseArgParser

} // namespace Coffee
