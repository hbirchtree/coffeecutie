#pragma once

#include <peripherals/build/application.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>
#include <platforms/pimpl_state.h>

/* This is an API with limited access to CoffeeCore's internal state */

namespace platform {
namespace profiling {

struct PContext;
struct ThreadState;

} // namespace profiling
} // namespace platform

namespace Coffee {
namespace State {

using GlobalState = platform::detail::GlobalState;

extern stl_types::UqLock LockState(std::string_view key);
inline stl_types::UqLock LockState(GlobalState& state)
{
    return stl_types::UqLock(state.access);
}

extern stl_types::ShPtr<GlobalState> SwapState(
    std::string_view key, stl_types::ShPtr<GlobalState> const& ptr);
extern stl_types::ShPtr<GlobalState> const& PeekState(std::string_view key);

template<typename T>
inline stl_types::ShPtr<T> CastState(std::string_view key)
{
    return std::dynamic_pointer_cast<T>(PeekState(key));
}

extern bool ProfilerEnabled();

extern stl_types::ShPtr<platform::profiling::PContext>    GetProfilerStore();
extern stl_types::ShPtr<platform::profiling::ThreadState> GetProfilerTStore();

extern stl_types::ShPtr<platform::info::AppData> GetAppData();

} // namespace State
} // namespace Coffee
