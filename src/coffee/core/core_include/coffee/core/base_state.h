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

extern std::shared_ptr<GlobalState> SwapState(
    std::string_view key, std::shared_ptr<GlobalState> const& ptr);
extern std::shared_ptr<GlobalState> const& PeekState(std::string_view key);

template<typename T>
inline std::shared_ptr<T> CastState(std::string_view key)
{
    return std::dynamic_pointer_cast<T>(PeekState(key));
}

extern bool ProfilerEnabled();

extern std::shared_ptr<platform::profiling::PContext>    GetProfilerStore();
extern std::shared_ptr<platform::profiling::ThreadState> GetProfilerTStore();

extern std::shared_ptr<platform::info::AppData> GetAppData();

} // namespace State
} // namespace Coffee
