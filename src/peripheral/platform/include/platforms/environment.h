#pragma once

//#include "linux/environment.h"
//#include "osx/environment.h"
//#include "posix/environment.h"
//#include "win32/environment.h"

//#include "base/environment.h"

#include "posix/env.h"

namespace platform::env {

using posix::var;
using posix::set_var;
using posix::home_dir;

} // namespace platform
