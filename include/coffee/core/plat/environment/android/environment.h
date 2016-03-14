#pragma once

#ifdef COFFEE_ANDROID
#include "../environment_details.h"

namespace Coffee{
namespace Environment{

struct AndroidEnv : EnvInterface
{
};

}

using ColorMap = EnvColorCodes;
using Env = Environment::AndroidEnv;

}
#endif