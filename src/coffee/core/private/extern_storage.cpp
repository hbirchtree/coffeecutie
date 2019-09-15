#include <coffee/core/internal_state.h>

#include <coffee/core/CDebug>
#include <coffee/core/base/printing/log_interface.h>
#include <coffee/core/base/printing/outputprinter.h>
#include <coffee/foreign/foreign.h>
#include <platforms/argument_parse.h>
#include <platforms/environment.h>

#if !defined(COFFEE_DISABLE_PROFILER)
#include <coffee/core/profiler/profiling-export.h>
#include <platforms/profiling.h>
#endif

using namespace ::platform;

namespace Coffee {
namespace State {

struct InternalState
{
    using StateStorage = Map<CString, ShPtr<State::GlobalState>>;

    InternalState() :
        current_app(MkShared<platform::info::AppData>()),
    #if !defined(COFFEE_DISABLE_PROFILER)
        profiler_store(MkShared<profiling::PContext>()),
    #endif
        bits()
    {
    }

    LogInterface logger = {OutputPrinter::fprintf_platform,
                           OutputPrinter::fprintf_platform_tagged};

#ifndef COFFEE_LOWFAT
    Mutex printer_lock;
#endif

    /* Resources */
    CString resource_prefix;

    /* Application info */
    ShPtr<platform::info::AppData> current_app;

    BuildInfo build = {};

    platform::args::AppArg initial_args = {};

#if defined(COFFEE_USE_UNWIND)
    unw_context_t* unwind_context = nullptr;
#endif

#if !defined(COFFEE_DISABLE_PROFILER)
    ShPtr<profiling::PContext> profiler_store;
#endif

    StateStorage pointer_storage;

    struct internal_bits_t
    {
        internal_bits_t() : printing_verbosity(0)
        {
        }

        u8 printing_verbosity;
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
        current_thread_id(), profiler_data(MkShared<profiling::ThreadState>())
    {
        using RuntimeProperties = profiling::Profiler::runtime_options;
        auto runtimeProps       = MkUq<RuntimeProperties>();

        runtimeProps->push               = profiling::JsonPush;
        runtimeProps->context            = profiler_data;
        runtimeProps->context->thread_id = current_thread_id.hash();

        Lock _(State::internal_state->profiler_store->access);

        auto& globalState =
            State::internal_state->profiler_store->thread_states;

        globalState[current_thread_id.hash()] = profiler_data;

        profiler_data->writer         = State::PeekState("jsonProfiler").get();
        profiler_data->internal_state = std::move(runtimeProps);
    }

    ~InternalThreadState()
    {
        using RuntimeOptions = profiling::Profiler::runtime_options;

        RuntimeOptions* internal_state =
            C_DCAST<RuntimeOptions>(profiler_data->internal_state.get());

        if(internal_state)
            internal_state->context.reset();

        profiler_data->internal_state = {};
    }

    ThreadId                      current_thread_id;
    ShPtr<profiling::ThreadState> profiler_data;
#endif
};

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
        auto tid   = ThreadId().hash();
        auto store = GetProfilerStore();

        C_PTR_CHECK(store);

        Lock _(store->access);
        store->thread_states[tid] = TSTATE->profiler_data;
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

ShPtr<info::AppData> GetAppData()
{
    if(!ISTATE)
        return {};
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

ShPtr<profiling::PContext> GetProfilerStore()
{
#if !defined(COFFEE_DISABLE_PROFILER)
    if(!ISTATE)
        return {};

    return ISTATE->profiler_store;
#else
    Throw(implementation_error("profiler disabled"));
#endif
}

ShPtr<platform::profiling::ThreadState> GetProfilerTStore()
{
#if !defined(COFFEE_DISABLE_PROFILER)
    if(!TSTATE)
        SetInternalThreadState(CreateNewThreadState());

    return TSTATE->profiler_data;
#else
    Throw(implementation_error("profiler disabled"));
#endif
}

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

stl_types::UqLock LockState(libc_types::cstring key)
{
    C_PTR_CHECK(ISTATE);

    if(ISTATE->pointer_storage.find(key) == ISTATE->pointer_storage.end())
        return stl_types::UqLock();

    return UqLock(ISTATE->pointer_storage[key]->access);
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
} // namespace State

void SetCurrentApp(const info::AppData& app)
{
    C_PTR_CHECK(State::ISTATE);
    *State::ISTATE->current_app = app;
}

info::AppData const& GetCurrentApp()
{
    C_PTR_CHECK(State::ISTATE);
    return *State::ISTATE->current_app;
}

args::AppArg& GetInitArgs()
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
} // namespace Coffee

namespace platform {
namespace file {

using namespace ::libc_types;
using namespace ::Coffee;

/*
 *
 * File resource prefix stuff, used by file API
 *
 * This works as a proxy between CoffeeCore_Application
 *  and the core library
 *
 */

void ResourcePrefix(cstring prefix)
{
    C_PTR_CHECK(State::ISTATE);
    State::ISTATE->resource_prefix = prefix;
}

CString const& ResourcePrefix(bool fallback)
{
    //    fprintf(stdout, "GET: %p\n", &State::internal_state);
    fflush(stdout);
    C_PTR_CHECK(State::ISTATE);
    auto const& out = State::ISTATE->resource_prefix;

    if(fallback && out.size() == 0)
    {
        static CString fallback = "./";
        return fallback;
    }

    return out;
}

} // namespace file
} // namespace platform

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

bool CoffeeEventHandleCall(int event)
{
    if(CoffeeEventHandle && coffee_event_handling_data)
    {
        CoffeeEventHandle(coffee_event_handling_data, event);
        return true;
    } else
    {
        fprintf(
            stderr,
            "Event handler function called without valid configuration!\n");
        return false;
    }
}
bool CoffeeEventHandleNACall(int event, void* ptr1, void* ptr2, void* ptr3)
{
    if(CoffeeEventHandleNA && coffee_event_handling_data)
    {
        CoffeeEventHandleNA(
            coffee_event_handling_data, event, ptr1, ptr2, ptr3);
        return true;
    } else
    {
        fprintf(
            stderr,
            "Event handler function called without valid configuration!\n");
        return false;
    }
}
