#pragma once

#if defined(__EMSCRIPTEN__) || defined(COFFEE_NACL)

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
