#include "../../platform_detect.h"

#ifdef COFFEE_WINDOWS
#pragma once

#include "../process_def.h"

namespace Coffee{

namespace Windows{

struct WinProcessProperty
{
};

}

using ProcessProperty = Windows::WinProcessProperty;

}

#endif