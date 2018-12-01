#pragma once

#include <peripherals/build/application.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

/* This is an API with limited access to CoffeeCore's internal state */

namespace platform {
namespace profiling {

struct PContext;
struct ThreadState;

} // namespace profiling
} // namespace platform

namespace Coffee {
namespace State {

struct GlobalState
{
    virtual ~GlobalState();
};

extern stl_types::ShPtr<GlobalState> SwapState(
    libc_types::cstring key, stl_types::ShPtr<GlobalState> const& ptr);
extern stl_types::ShPtr<GlobalState> const& PeekState(libc_types::cstring key);

extern bool ProfilerEnabled();

extern platform::profiling::PContext*    GetProfilerStore();
extern platform::profiling::ThreadState* GetProfilerTStore();

extern platform::info::AppData& GetAppData();

} // namespace State
} // namespace Coffee
