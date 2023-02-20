#pragma once

#include <peripherals/base.h>

#if defined(COFFEE_MACOS) || defined(COFFEE_LINUX)
#include "posix/dynamic.h"
#endif

namespace platform::dl {

#if defined(COFFEE_MACOS) || defined(COFFEE_LINUX)
static constexpr bool supported = true;

using posix::load;
using posix::symbol_from;
using posix::error_message;
#else
static constexpr bool supported = false;
#endif

}
