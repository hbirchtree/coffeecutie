#pragma once

#include <functional>
#include <peripherals/build/application.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/mutex.h>
#include <peripherals/stl/types.h>

namespace platform {
namespace profiling {

struct PContext;
struct ThreadState;

} // namespace profiling

namespace detail {

struct GlobalState
{
    std::mutex access;
    virtual ~GlobalState();
};

using global_state_ptr = std::shared_ptr<GlobalState>;

using lock_state_fn = std::function<std::unique_lock<std::mutex>(std::string_view)>;
using swap_state_fn =
    std::function<global_state_ptr(std::string_view, global_state_ptr const&)>;
using peek_state_fn = std::function<global_state_ptr const&(std::string_view)>;

using profiler_enabled_fn = bool (*)();
using profiler_store_fn = std::shared_ptr<platform::profiling::PContext> (*)();
using thread_store_fn = std::shared_ptr<platform::profiling::ThreadState> (*)();

using app_data_fn = std::shared_ptr<platform::info::AppData> (*)();

struct state_pimpl
{
    state_pimpl();

    using GlobalState = detail::GlobalState;

    FORCEDINLINE std::unique_lock<std::mutex> LockState(GlobalState& state)
    {
        return std::unique_lock<std::mutex>(state.access);
    }

    FORCEDINLINE std::unique_lock<std::mutex> LockState(libc_types::cstring state)
    {
        return m_LockState(state);
    }

    lock_state_fn m_LockState;
    swap_state_fn SwapState;
    peek_state_fn PeekState;

    profiler_enabled_fn ProfilerEnabled;
    profiler_store_fn   GetProfilerStore;
    thread_store_fn     GetProfilerTStore;

    app_data_fn GetAppData;
};

} // namespace detail

extern std::unique_ptr<detail::state_pimpl> state;
using GlobalState = detail::GlobalState;

} // namespace platform
