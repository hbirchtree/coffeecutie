#pragma once

#include "../../platform_detect.h"


#if defined(COFFEE_STUBBED_STACKTRACE)

#include "../stacktrace_def.h"

namespace Coffee{
namespace Environment{
namespace Emscripten{

struct EmscriptenStacktracer : StacktracerDef
{

};

}
}

using Stacktracer = Environment::Emscripten::EmscriptenStacktracer;

}
#endif
