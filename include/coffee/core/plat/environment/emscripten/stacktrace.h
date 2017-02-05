#pragma once

#include "../../platform_detect.h"


#ifdef __EMSCRIPTEN__

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
