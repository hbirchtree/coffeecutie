#include <coffee/core/internal_state.h>

#include <coffee/core/CDebug>
#include <coffee/core/debug/logging.h>
#include <coffee/core/printing/log_interface.h>
#include <platforms/environment.h>
#include <platforms/profiling/jsonprofile.h>
#include <cxxopts.hpp>

#include <coffee/core/debug/logging.h>

#if !defined(COFFEE_DISABLE_PROFILER)
#include <coffee/core/profiler/profiling-export.h>
#include <platforms/profiling.h>
#endif

using namespace ::platform;

namespace Coffee {
namespace State {

struct InternalState
{
    using StateStorage
        = std::map<std::string_view, std::shared_ptr<State::GlobalState>>;

    InternalState() :
        current_app(std::make_shared<platform::info::AppData>()),
#if PERIPHERAL_PROFILER_ENABLED
        profiler_store(std::make_shared<profiling::PContext>()),
#endif
        bits()
    {
    }

    DebugFun::LogInterface logger = {Logging::log};

#ifndef COFFEE_LOWFAT
    stl_types::Mutex printer_lock;
#endif

    /* Resources */
    std::string resource_prefix;

    /* Application info */
    std::shared_ptr<platform::info::AppData> current_app;

    BuildInfo build{};

    std::vector<const char*> initial_args{};

#if PERIPHERAL_PROFILER_ENABLED
    std::shared_ptr<profiling::PContext> profiler_store;
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
#if PERIPHERAL_PROFILER_ENABLED
    InternalThreadState() :
        current_thread_id(),
        profiler_data(std::make_shared<profiling::ThreadState>())
    {
        using RuntimeProperties = profiling::Profiler::runtime_options;
        auto runtimeProps       = std::make_unique<RuntimeProperties>();

        runtimeProps->push               = profiling::json::Push;
        runtimeProps->context            = profiler_data;
        runtimeProps->context->thread_id = current_thread_id.hash();

        std::lock_guard _(State::internal_state->profiler_store->access);

        auto& globalState
            = State::internal_state->profiler_store->thread_states;

        globalState[current_thread_id.hash()] = profiler_data;

        profiler_data->writer         = State::PeekState("jsonProfiler").get();
        profiler_data->internal_state = std::move(runtimeProps);
    }

    ~InternalThreadState()
    {
        using RuntimeOptions = profiling::Profiler::runtime_options;

        RuntimeOptions* internal_state
            = C_DCAST<RuntimeOptions>(profiler_data->internal_state.get());

        if(internal_state)
            internal_state->context.reset();

        profiler_data->internal_state = {};
    }

    stl_types::ThreadId                     current_thread_id;
    std::shared_ptr<profiling::ThreadState> profiler_data;
#endif
};

#define ISTATE internal_state
#define TSTATE thread_state

P<InternalState>                    internal_state;
thread_local P<InternalThreadState> thread_state;

P<InternalState> CreateNewState()
{
    return std::make_shared<InternalState>();
}

P<InternalThreadState> CreateNewThreadState()
{
    return std::make_shared<InternalThreadState>();
}

void SetInternalState(P<InternalState> state)
{
    ISTATE = state;
}

void ClearStates()
{
    if(ISTATE)
        ISTATE->pointer_storage = {};
}

P<InternalState>& GetInternalState()
{
    return ISTATE;
}

STATICINLINE void RegisterProfilerThreadState()
{
    if constexpr(!compile_info::profiler::enabled)
        return;

#if PERIPHERAL_PROFILER_ENABLED
    if(ISTATE)
    {
        auto tid   = stl_types::ThreadId().hash();
        auto store = GetProfilerStore();

        C_PTR_CHECK(store);

        std::lock_guard _(store->access);
        store->thread_states[tid] = TSTATE->profiler_data;
    }
#endif
}

void SetInternalThreadState(P<InternalThreadState> state)
{
    if constexpr(!compile_info::profiler::enabled)
        return;

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

std::shared_ptr<info::AppData> GetAppData()
{
    if(!ISTATE)
        return {};
    return ISTATE->current_app;
}

bool ProfilerEnabled()
{
    return compile_info::profiler::enabled;
}

std::shared_ptr<profiling::PContext> GetProfilerStore()
{
#if PERIPHERAL_PROFILER_ENABLED
    if(!ISTATE)
        return {};

    return ISTATE->profiler_store;
#else
    return {};
#endif
}

std::shared_ptr<platform::profiling::ThreadState> GetProfilerTStore()
{
    if constexpr(!compile_info::profiler::enabled)
        Throw(implementation_error("profiler disabled"));

#if PERIPHERAL_PROFILER_ENABLED
    if(!TSTATE)
        SetInternalThreadState(CreateNewThreadState());

    return TSTATE->profiler_data;
#else
    return {};
#endif
}

stl_types::Mutex& GetPrinterLock()
{
    if constexpr(compile_info::lowfat_mode)
        Throw(releasemode_error("not available in this mode"));

    C_PTR_CHECK(ISTATE);
    return ISTATE->printer_lock;
}

stl_types::ThreadId& GetCurrentThreadId()
{
    if constexpr(!compile_info::profiler::enabled)
        Throw(releasemode_error("thread ID is not available"));

#if PERIPHERAL_PROFILER_ENABLED
    if(!TSTATE)
        SetInternalThreadState(CreateNewThreadState());

    /* We check it just in case everything is really bad */
    C_PTR_CHECK(TSTATE);

    return TSTATE->current_thread_id;
#endif
}

std::unique_lock<std::mutex> LockState(std::string_view key)
{
    C_PTR_CHECK(ISTATE);

    if(ISTATE->pointer_storage.find(key) == ISTATE->pointer_storage.end())
        return std::unique_lock<std::mutex>();

    return std::unique_lock(ISTATE->pointer_storage[key]->access);
}

std::shared_ptr<GlobalState> SwapState(
    std::string_view key, std::shared_ptr<GlobalState> const& ptr)
{
    C_PTR_CHECK(ISTATE);

    std::shared_ptr<GlobalState> current
        = std::move(ISTATE->pointer_storage[key]);
    ISTATE->pointer_storage[key] = std::move(ptr);
    return current;
}

const std::shared_ptr<GlobalState>& PeekState(std::string_view key)
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

std::vector<const char*>& GetInitArgs()
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
        return {Logging::log};
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

void ResourcePrefix(std::string prefix)
{
    C_PTR_CHECK(State::ISTATE);
    State::ISTATE->resource_prefix = prefix;
}

std::optional<std::string> ResourcePrefix(bool /*fallback*/)
{
    if(!State::ISTATE)
        return std::nullopt;

    auto const& out = State::ISTATE->resource_prefix;

    if(out.size() == 0)
        return std::nullopt;

    return out;
}

} // namespace file
} // namespace platform

/*
 * Pointer to main() function to be used
 * This storage is for non-standard platforms
 */
//#if defined(COFFEE_CUSTOM_MAIN)
// Coffee::MainWithArgs coffee_main_function_ptr = nullptr;
//#endif

#if defined(COFFEE_APPLE_MOBILE)
void* uikit_appdelegate = nullptr;
void* uikit_window      = nullptr;
#endif
