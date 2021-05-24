#pragma once

#include <peripherals/base.h>

#if defined(COFFEE_MACOS) || defined(COFFEE_LINUX)
#include "posix/dynamic.h"
#endif

namespace platform::dl {

#if defined(COFFEE_MACOS) || defined(COFFEE_LINUX)
static constexpr bool supported = true;

using posix::load;
using posix::error_message;
#else
static constexpr bool supported = false;
#endif

}

//#include "posix/loader.h"
//#include "win32/loader.h"

//#include "base/loader.h"

//namespace platform {
//namespace ld {

//#if defined(COFFEE_EMSCRIPTEN) || defined(COFFEE_GEKKO)
//using FunctionLoader = ld::FunctionLoad_def;

//using ObjectLoader = ld::ObjectLoader_def<FunctionLoader>;
//#endif
//}
//} // namespace platform
