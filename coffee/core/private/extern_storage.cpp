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
#ifndef COFFEE_LOWFAT
    Mutex printer_lock;
#endif

#if !defined(COFFEE_DISABLE_PROFILER)
    Profiling::ProfilerDataStore profiler_store;
#endif

#if defined(COFFEE_USE_UNWIND)
    unw_context_t* unwind_context = nullptr;
#endif

    /* Resources */
    CString _coffee_resource_prefix = "./";

    /* Application info */
    CoffeeApplicationData _coffee_current_app = {};

#if defined(COFFEE_USE_TERMINAL_CTL)
    bool terminal_alternate_buffer = false;
#endif

    byte_t padding[7];
};

struct InternalThreadState
{
#if !defined(NDEBUG)
    ThreadId current_thread_id;
#endif
};

namespace State{

static P<InternalState> internal_state = nullptr;
static thread_local P<InternalThreadState> thread_state = nullptr;

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

void SetInternalThreadState(P<InternalThreadState> state)
{
    thread_state = state;
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

Profiling::ProfilerDataStore* GetProfilerStore()
{
    C_PTR_CHECK(internal_state);
    return &internal_state->profiler_store;
}

#if defined(COFFEE_USE_TERMINAL_CTL)
bool& GetAlternateTerminal()
{
    C_PTR_CHECK(internal_state);
    return internal_state->terminal_alternate_buffer;
}
#endif

Mutex& GetPrinterLock()
{
    C_PTR_CHECK(internal_state);
    return internal_state->printer_lock;
}

ThreadId& GetCurrentThreadId()
{
#if !defined(NDEBUG)
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
    State::internal_state->_coffee_resource_prefix = prefix;
}

CString const& CResources::GetFileResourcePrefix()
{
    C_PTR_CHECK(State::internal_state);
    return State::internal_state->_coffee_resource_prefix;
}

void SetCurrentApp(CoffeeApplicationData const& app)
{
    C_PTR_CHECK(State::internal_state);
    State::internal_state->_coffee_current_app = app;
}

CoffeeApplicationData const& GetCurrentApp()
{
    C_PTR_CHECK(State::internal_state);
    return State::internal_state->_coffee_current_app;
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
