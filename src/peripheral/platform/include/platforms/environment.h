#pragma once

#include "linux/environment.h"
#include "osx/environment.h"
#include "posix/environment.h"
#include "win32/environment.h"

#include "base/environment.h"

namespace platform {

using Env      = platform::Env_;
using ColorMap = platform::ColorMap_;

} // namespace platform
