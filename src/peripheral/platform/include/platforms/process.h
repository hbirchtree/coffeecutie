#pragma once

#include "base/process.h"
#include "linux/process.h"

namespace Coffee {

#if defined(COFFEE_LINUX)
using ProcessProperty = platform::ProcessProperty;
#else
using ProcessProperty = platform::env::ProcessPropertyDef;
#endif
}
