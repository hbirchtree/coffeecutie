#pragma once

#include "base/power.h"

namespace platform {

#if defined(COFFEE_EMSCRIPTEN)
using PowerInfo = env::PowerInfoDef;
#endif

}
