#include <coffee/core/internal_state.h>

#include <coffee/core/CDebug>
#include <coffee/core/base/printing/log_interface.h>
#include <coffee/core/base/printing/outputprinter.h>
#include <coffee/core/base/renderer/eventapplication_wrapper.h>
#include <coffee/core/plat/plat_environment.h>

#if !defined(COFFEE_DISABLE_PROFILER)
#include <coffee/core/plat/timing/profiling.h>
#include <coffee/core/profiler/profiling-export.h>
#endif

#ifdef COFFEE_USE_TERMINAL_CTL
#include <coffee/core/plat/memory/cmd_unixterm.h>
#endif

namespace Coffee {

struct InternalState
{
    InternalState() : bits()
    {
    }

    LogInterface logger = {OutputPrinter::fprintf_platform,
                           OutputPrinter::fprintf_platform_tagged};

#ifndef COFFEE_LOWFAT
    Mutex printer_lock;
#endif

    /* Resources */
    CString resource_prefix = "./";

    /* Application info */
    CoffeeApplicationData current_app = {};

    BuildInfo build = {};

    AppArg initial_args = {};

#if defined(COFFEE_USE_UNWIND)
    unw_context_t* unwind_context = nullptr;
#endif

#if !defined(COFFEE_DISABLE_PROFILER)
    Profiling::PContext profiler_store;
#endif

    Map<CString, ShPtr<State::GlobalState>> pointer_storage;

    struct internal_bits_t
    {
        internal_bits_t() :
            printing_verbosity(0)
#if defined(COFFEE_USE_TERMINAL_CTL)
            ,
            terminal_alternate_buffer(false)
#endif
        {
        }

        u8 printing_verbosity;
#if defined(COFFEE_USE_TERMINAL_CTL)
        bool terminal_alternate_buffer;
#endif
    };

    union
    {
        internal_bits_t bits = {};
        u64             _do_not_touch;
    };
};

struct InternalThreadState
{
#if !defined(COFFEE_DISABLE_PROFILER)
    InternalThreadState() :
        current_thread_id(), profiler_data(MkShared<Profiling::ThreadState>())
    {
        using RuntimeProperties = Profiling::Profiler::runtime_options;
        auto runtimeProps       = MkUq<RuntimeProperties>();

        runtimeProps->push               = Profiling::JsonPush;
        runtimeProps->context            = profiler_data;
        runtimeProps->context->thread_id = current_thread_id.hash();

        Lock _(State::internal_state->profiler_store.access);

        auto& globalState = State::internal_state->profiler_store.thread_states;

        globalState[current_thread_id.hash()] = profiler_data;

        profiler_data->writer         = State::PeekState("jsonProfiler").get();
        profiler_data->internal_state = std::move(runtimeProps);
    }

    ~InternalThreadState()
    {
        using RuntimeOptions = Profiling::Profiler::runtime_options;

        RuntimeOptions* internal_state =
            C_DCAST<RuntimeOptions>(profiler_data->internal_state.get());

        if(internal_state)
            internal_state->context.reset();

        profiler_data->internal_state = {};
    }

    ThreadId                      current_thread_id;
    ShPtr<Profiling::ThreadState> profiler_data;
#endif
};

namespace State {

#define ISTATE internal_state
#define TSTATE thread_state

P<InternalState>                    internal_state;
thread_local P<InternalThreadState> thread_state;

P<InternalState> CreateNewState()
{
    return MkShared<InternalState>();
}

P<InternalThreadState> CreateNewThreadState()
{
    return MkShared<InternalThreadState>();
}

void SetInternalState(P<InternalState> state)
{
    ISTATE = state;
}

P<InternalState>& GetInternalState()
{
    return ISTATE;
}

STATICINLINE void RegisterProfilerThreadState()
{
#if !defined(COFFEE_DISABLE_PROFILER)
    if(ISTATE)
    {
        auto tid = ThreadId().hash();

        Lock _(GetProfilerStore()->access);
        GetProfilerStore()->thread_states[tid] = TSTATE->profiler_data;
    }
#endif
}

void SetInternalThreadState(P<InternalThreadState> state)
{
    TSTATE = state;

    if(state)
        RegisterProfilerThreadState();
}

P<InternalThreadState>& GetInternalThreadState()
{
    return TSTATE;
}

/*
 *
 * Accessors
 *
 */

BuildInfo& GetBuildInfo()
{
    C_PTR_CHECK(ISTATE);
    return ISTATE->build;
}

CoffeeApplicationData& GetAppData()
{
    C_PTR_CHECK(ISTATE);
    return ISTATE->current_app;
}

bool ProfilerEnabled()
{
#if !defined(COFFEE_DISABLE_PROFILER)
    return true;
#else
    return false;
#endif
}

Profiling::PContext* GetProfilerStore()
{
#if !defined(COFFEE_DISABLE_PROFILER)
    C_PTR_CHECK(ISTATE);
    return &ISTATE->profiler_store;
#else
    Throw(implementation_error("profiler disabled"));
#endif
}

Profiling::ThreadState* GetProfilerTStore()
{
#if !defined(COFFEE_DISABLE_PROFILER)
    if(!TSTATE)
        SetInternalThreadState(CreateNewThreadState());

    return TSTATE->profiler_data.get();
#else
    Throw(implementation_error("profiler disabled"));
#endif
}

#if defined(COFFEE_USE_TERMINAL_CTL)
bool& GetAlternateTerminal()
{
    C_PTR_CHECK(ISTATE);
    return ISTATE->bits.terminal_alternate_buffer;
}
#endif

Mutex& GetPrinterLock()
{
#ifndef COFFEE_LOWFAT
    C_PTR_CHECK(ISTATE);
    return ISTATE->printer_lock;
#else
    Throw(releasemode_error("not available in this mode"));
#endif
}

ThreadId& GetCurrentThreadId()
{
#if !defined(COFFEE_DISABLE_PROFILER)
    if(!TSTATE)
        SetInternalThreadState(CreateNewThreadState());

    /* We check it just in case everything is really bad */
    C_PTR_CHECK(TSTATE);

    return TSTATE->current_thread_id;
#else
    Throw(releasemode_error("thread ID is not available"));
#endif
}

ShPtr<GlobalState> SwapState(cstring key, ShPtr<GlobalState> const& ptr)
{
    C_PTR_CHECK(ISTATE);

    ShPtr<GlobalState> current   = std::move(ISTATE->pointer_storage[key]);
    ISTATE->pointer_storage[key] = std::move(ptr);
    return current;
}

const ShPtr<GlobalState>& PeekState(cstring key)
{
    C_PTR_CHECK(ISTATE);
    return ISTATE->pointer_storage[key];
}
}

namespace CResources {
/*
 *
 * File resource prefix stuff, used by file API
 *
 * This works as a proxy between CoffeeCore_Application
 *  and the core library
 *
 */

void FileResourcePrefix(cstring prefix)
{
    C_PTR_CHECK(State::ISTATE);
    State::ISTATE->resource_prefix = prefix;
}

CString const& GetFileResourcePrefix()
{
    //    fprintf(stdout, "GET: %p\n", &State::internal_state);
    fflush(stdout);
    C_PTR_CHECK(State::ISTATE);
    return State::ISTATE->resource_prefix;
}

} // namespace CResources

void SetCurrentApp(CoffeeApplicationData const& app)
{
    C_PTR_CHECK(State::ISTATE);
    State::ISTATE->current_app = app;
}

CoffeeApplicationData const& GetCurrentApp()
{
    C_PTR_CHECK(State::ISTATE);
    return State::ISTATE->current_app;
}

AppArg& GetInitArgs()
{
    C_PTR_CHECK(State::ISTATE);
    return State::ISTATE->initial_args;
}

u8& PrintingVerbosityLevel()
{
    /*!
     * \brief We need this for the middle-stage where nothing
     *  is set up yet
     */
    static u8 backup_verbosity;

    if(State::ISTATE)
        return State::ISTATE->bits.printing_verbosity;
    else
        return backup_verbosity;
}

namespace DebugFun {

void SetLogInterface(LogInterface intf)
{
    C_PTR_CHECK(State::ISTATE);
    State::ISTATE->logger = intf;
}

Coffee::DebugFun::LogInterface GetLogInterface()
{
    if(State::ISTATE)
        return State::ISTATE->logger;
    else
        return {OutputPrinter::fprintf_platform,
                OutputPrinter::fprintf_platform_tagged};
}
} // namespace DebugFun

namespace State {

GlobalState::~GlobalState()
{
}

} // namespace State

} // namespace Coffee

/*
 * These declarations are library-local storage for event handling
 *
 * They require compatibility with C linkage in order to work with
 *  Objective-C and other things.
 *
 * They shouldn't be used externally to the program anyway.
 *
 */
void* coffee_event_handling_data;

void (*CoffeeEventHandle)(void*, int);
void (*CoffeeEventHandleNA)(void*, int, void*, void*, void*);

void (*CoffeeForeignSignalHandle)(int);
void (*CoffeeForeignSignalHandleNA)(int, void*, void*, void*);
