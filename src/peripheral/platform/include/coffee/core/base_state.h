#pragma once

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

using namespace ::platform::profiling;

struct GlobalState
{
    virtual ~GlobalState();
};

extern stl_types::ShPtr<GlobalState> SwapState(
    libc_types::cstring key, stl_types::ShPtr<GlobalState> const& ptr);
extern stl_types::ShPtr<GlobalState> const& PeekState(libc_types::cstring key);

extern bool ProfilerEnabled();

extern PContext*    GetProfilerStore();
extern ThreadState* GetProfilerTStore();

} // namespace State
} // namespace Coffee
