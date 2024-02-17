#pragma once

#if defined(COFFEE_LINUX) || defined(COFFEE_ANDROID) ||    \
    defined(COFFEE_APPLE) || defined(COFFEE_EMSCRIPTEN) || \
    defined(COFFEE_MINGW64)
#define USE_POSIX_API 1
#endif

#ifdef USE_POSIX_API
#include "posix/env.h"
#endif

namespace platform::env {

#if defined(USE_POSIX_API)
using posix::home_dir;
using posix::set_var;
using posix::var;
#endif

} // namespace platform::env
