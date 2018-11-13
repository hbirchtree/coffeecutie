#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

/* This is an API with limited access to CoffeeCore's internal state */

namespace Coffee {
namespace State {

struct GlobalState
{
    virtual ~GlobalState();
};

extern stl_types::ShPtr<GlobalState> SwapState(
    libc_types::cstring key, stl_types::ShPtr<GlobalState> const& ptr);
extern stl_types::ShPtr<GlobalState> const& PeekState(libc_types::cstring key);

} // namespace State
} // namespace Coffee
