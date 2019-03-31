#pragma once

#include "linux/environment.h"
#include "osx/environment.h"
#include "posix/environment.h"
#include "win32/environment.h"

#include "base/environment.h"

namespace platform {

#if defined(COFFEE_ANDROID)
using Env      = platform::env::EnvInterface;
using ColorMap = platform::env::EnvColorCodes;
#else
using Env      = platform::Env_;
using ColorMap = platform::ColorMap_;
#endif

} // namespace platform
