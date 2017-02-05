#pragma once

#include "../../platform_detect.h"

#ifdef COFFEE_WINDOWS

#include "../stacktrace_def.h"

namespace Coffee{
namespace Environment{
namespace Windows{

struct WindowsStacktracer : StacktracerDef
{

};

}
}

using Stacktracer = Environment::Windows::WindowsStacktracer;

}

#endif
