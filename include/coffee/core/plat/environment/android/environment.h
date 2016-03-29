#pragma once

#include "../../plat_primary_identify.h"

#ifdef COFFEE_ANDROID
#include "../environment_details.h"

namespace Coffee{
namespace Environment{
namespace Android{
struct AndroidEnv : EnvInterface
{
    static CString GetUserData(cstring,cstring);
};

}
}

using ColorMap = Environment::EnvColorCodes;
using Env = Environment::Android::AndroidEnv;

}
#endif