#include <coffee/core/profiler/profiling-export.h>
#include <coffee/core/plat/plat_environment.h>
#include <coffee/core/plat/timing/profiling.h>
#include <coffee/core/CDebug>
#include <coffee/core/base/renderer/eventapplication_wrapper.h>

#include <coffee/core/internal_state.h>

#ifdef COFFEE_USE_TERMINAL_CTL
#include <coffee/core/plat/memory/cmd_unixterm.h>
#endif

namespace Coffee{

struct InternalState
{
    LogInterface logger = OutputPrinterImpl::fprintf_platform;

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
    Profiling::ProfilerDataStore profiler_store;
#endif

    struct internal_bits_t {
        internal_bits_t():
            printing_verbosity(0)
  #if defined(COFFEE_USE_TERMINAL_CTL)
            ,terminal_alternate_buffer(false)
  #endif
        {
        }

        u8 printing_verbosity;
#if defined(COFFEE_USE_TERMINAL_CTL)
        bool terminal_alternate_buffer;
#endif
    };

    union {
        internal_bits_t bits = {};
        u64 _do_not_touch;
    };
};

struct InternalThreadState
{

#if !defined(COFFEE_DISABLE_PROFILER)
    InternalThreadState():
        current_thread_id(),
        profiler_data(MkShared<Profiling::ThreadData>())
    {
    }

    ThreadId current_thread_id;
    ShPtr<Profiling::ThreadData> profiler_data;
#endif
};

namespace State{

static P<InternalState> internal_state;
static thread_local P<InternalThreadState> thread_state;

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
    internal_state = state;
}

P<InternalState>& GetInternalState()
{
    return internal_state;
}

STATICINLINE void RegisterProfilerThreadState()
{
#if !defined(COFFEE_DISABLE_PROFILER)
    if(internal_state)
    {
        Lock _(GetProfilerStore()->data_access_mutex);

        GetProfilerStore()->thread_refs[ThreadId().hash()] =
                thread_state->profiler_data;
    }
#endif
}

void SetInternalThreadState(P<InternalThreadState> state)
{
    thread_state = state;

    RegisterProfilerThreadState();
}

P<InternalThreadState>& GetInternalThreadState()
{
    return thread_state;
}

/*
 *
 * Accessors
 *
 */

BuildInfo& GetBuildInfo()
{
    return internal_state->build;
}

CoffeeApplicationData& GetAppData()
{
    return internal_state->current_app;
}

Profiling::ProfilerDataStore* GetProfilerStore()
{
#if !defined(COFFEE_DISABLE_PROFILER)
    C_PTR_CHECK(internal_state);
    return &internal_state->profiler_store;
#else
    throw implementation_error("profiler disabled");
#endif
}

Profiling::ThreadData *GetProfilerTStore()
{
#if !defined(COFFEE_DISABLE_PROFILER)
    if(!thread_state)
        SetInternalThreadState(CreateNewThreadState());

    return thread_state->profiler_data.get();
#else
    throw implementation_error("profiler disabled");
#endif
}

#if defined(COFFEE_USE_TERMINAL_CTL)
bool& GetAlternateTerminal()
{
    C_PTR_CHECK(internal_state);
    return internal_state->bits.terminal_alternate_buffer;
}
#endif

Mutex& GetPrinterLock()
{
    C_PTR_CHECK(internal_state);
    return internal_state->printer_lock;
}

ThreadId& GetCurrentThreadId()
{
#if !defined(COFFEE_DISABLE_PROFILER)
    if(!thread_state)
        SetInternalThreadState(CreateNewThreadState());

    /* We check it just in case everything is really bad */
    C_PTR_CHECK(thread_state);

    return thread_state->current_thread_id;
#else
    throw releasemode_error("thread ID is not available");
#endif
}

}

/*
 *
 * File resource prefix stuff, used by file API
 *
 * This works as a proxy between CoffeeCore_Application
 *  and the core library
 *
 */

void CResources::FileResourcePrefix(cstring prefix)
{
    C_PTR_CHECK(State::internal_state);
    State::internal_state->resource_prefix = prefix;
}

CString const& CResources::GetFileResourcePrefix()
{
    C_PTR_CHECK(State::internal_state);
    return State::internal_state->resource_prefix;
}

void SetCurrentApp(CoffeeApplicationData const& app)
{
    C_PTR_CHECK(State::internal_state);
    State::internal_state->current_app = app;
}

CoffeeApplicationData const& GetCurrentApp()
{
    C_PTR_CHECK(State::internal_state);
    return State::internal_state->current_app;
}

AppArg &GetInitArgs()
{
    C_PTR_CHECK(State::internal_state);
    return State::internal_state->initial_args;;
}

u8& PrintingVerbosityLevel()
{
    /*!
     * \brief We need this for the middle-stage where nothing
     *  is set up yet
     */
    static u8 backup_verbosity;


    if(State::internal_state)
        return State::internal_state->bits.printing_verbosity;
    else
        return backup_verbosity;
}

namespace DebugFun{

void SetLogInterface(LogInterface intf)
{
    C_PTR_CHECK(State::internal_state);
    State::internal_state->logger = intf;
}

LogInterface GetLogInterface()
{
    if(State::internal_state)
        return State::internal_state->logger;
    else
        return DebugFun::OutputPrinterImpl::fprintf_platform;
}

}

}

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

void(*CoffeeEventHandle)(void*, int);
void(*CoffeeEventHandleNA)(void*, int, void*, void*, void*);

void(*CoffeeForeignSignalHandle)(int);
void(*CoffeeForeignSignalHandleNA)(int, void*, void*, void*);
