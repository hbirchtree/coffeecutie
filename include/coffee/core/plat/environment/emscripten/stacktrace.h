#pragma once

#include "../../platform_detect.h"


#if defined(__EMSCRIPTEN__) || defined(COFFEE_NACL)

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
