#pragma once

#ifdef COFFEE_ANDROID
#include "../environment_details.h"

namespace Coffee{
namespace Environment{
namespace Android{
struct AndroidEnv : EnvInterface
{
};

}
}

using ColorMap = Environment::EnvColorCodes;
using Env = Environment::Android::AndroidEnv;

}
#endif