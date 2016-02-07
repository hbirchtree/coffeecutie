#include "../../platform_detect.h"

#ifdef COFFEE_WINDOWS
#ifndef COFFEE_CORE_PLAT_ENVIRONMENT_DETAILS_WINDOWS_H
#define COFFEE_CORE_PLAT_ENVIRONMENT_DETAILS_WINDOWS_H

#include "../environment_details.h"

namespace Coffee{
namespace Environment{

struct WindowsEnvFun : EnvInterface
{

};

}

using Env = Environment::WindowsEnvFun;

}

#endif
#endif