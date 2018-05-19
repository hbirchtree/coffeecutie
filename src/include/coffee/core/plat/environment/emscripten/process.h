#pragma once

#include <coffee/core/plat/plat_quirks_toggling.h>

#if defined(COFFEE_STUBBED_PROCESS)

#include "../process_def.h"

namespace Coffee{
namespace Environment{
namespace Emscripten{

struct EmProcessProperty : ProcessPropertyDef
{
};

}
}

using ProcessProperty = Environment::Emscripten::EmProcessProperty;

}
#endif
