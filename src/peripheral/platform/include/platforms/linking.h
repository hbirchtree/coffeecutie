#pragma once

#include "posix/loader.h"
#include "win32/loader.h"

#include "base/loader.h"

namespace platform {
namespace ld {

#if defined(COFFEE_EMSCRIPTEN) || defined(COFFEE_GEKKO)
using FunctionLoader = ld::FunctionLoad_def;

using ObjectLoader = ld::ObjectLoader_def<FunctionLoader>;
#endif
}
} // namespace platform
