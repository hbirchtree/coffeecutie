#pragma once

#include "../../platform_detect.h"

#ifdef COFFEE_WINDOWS

#include "../process_def.h"

namespace Coffee{
namespace Environment{
namespace Windows{

struct WinProcessProperty : Environment::ProcessPropertyDef
{
};

}
}

using ProcessProperty = Environment::Windows::WinProcessProperty;

}

#endif