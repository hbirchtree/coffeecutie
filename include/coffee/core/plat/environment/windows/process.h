#include "../../platform_detect.h"

#ifdef COFFEE_WINDOWS
#pragma once

#include "../process_def.h"

namespace Coffee{
namespace Environment{
namespace Windows{

struct WinProcessProperty
{
};

}
}

using ProcessProperty = Environment::Windows::WinProcessProperty;

}

#endif