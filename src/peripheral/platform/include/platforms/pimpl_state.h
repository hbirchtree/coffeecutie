#pragma once

#include <peripherals/build/application.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

namespace platform {
namespace profiling {

struct PContext;
struct ThreadState;

}
namespace detail {

struct GlobalState
{
    stl_types::Mutex access;
    virtual ~GlobalState();
};

using global_state_ptr = stl_types::ShPtr<GlobalState>;

using lock_state_fn = stl_types::UqLock(*)(libc_types::cstring);
using swap_state_fn = global_state_ptr(*)(libc_types::cstring, global_state_ptr const&);
using peek_state_fn = global_state_ptr const&(*)(libc_types::cstring);

using profiler_enabled_fn = bool(*)();
using profiler_store_fn = stl_types::ShPtr<platform::profiling::PContext>(*)();
using thread_store_fn = stl_types::ShPtr<platform::profiling::ThreadState>(*)();

using app_data_fn = stl_types::ShPtr<platform::info::AppData>(*)();

struct state_pimpl
{
    state_pimpl();

    using GlobalState = GlobalState;

    FORCEDINLINE stl_types::UqLock LockState(GlobalState& state)
    {
        return stl_types::UqLock(state.access);
    }
    FORCEDINLINE stl_types::UqLock LockState(libc_types::cstring state)
    {
        return m_LockState(state);
    }

    lock_state_fn m_LockState;
    swap_state_fn SwapState;
    peek_state_fn PeekState;

    profiler_enabled_fn ProfilerEnabled;
    profiler_store_fn GetProfilerStore;
    thread_store_fn GetProfilerTStore;

    app_data_fn GetAppData;
};

}

extern stl_types::UqPtr<detail::state_pimpl> state;
using GlobalState = detail::GlobalState;

}
