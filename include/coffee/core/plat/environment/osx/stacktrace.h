#pragma once

#include "../../plat_primary_identify.h"

#if defined(COFFEE_APPLE)

#include "../stacktrace_def.h"

namespace Coffee{
namespace Environment{
namespace Mac{

struct MacStacktracer : StacktracerDef
{
};

}
}

using Stacktracer = Environment::Mac::MacStacktracer;

}
#endif
