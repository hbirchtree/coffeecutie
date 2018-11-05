#pragma once

#include <coffee/core/base.h>

#if defined(COFFEE_STUBBED_ENVIRONMENT)

#include "../environment_details.h"

namespace Coffee{
namespace Environment{
namespace Emscripten{

struct EmEnvInfo : EnvInterface
{
};

struct EmEnvColor : EnvColorCodes
{
};

}
}
using Env_ = Environment::Emscripten::EmEnvInfo;
using ColorMap_ = Environment::Emscripten::EmEnvColor;
}

#endif
