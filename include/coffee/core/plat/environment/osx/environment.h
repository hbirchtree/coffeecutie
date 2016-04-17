#pragma once

#include "../../plat_primary_identify.h"

#if defined(COFFEE_APPLE)

#include "../environment_details.h"
#include "../posix/environment.h"

namespace Coffee{
namespace Environment{
namespace Mac{

struct MacEnv : EnvInterface
{
};

struct MacColorMap : Posix::PosixTerminalColorCodes
{
};

}
}

using Env_ = Environment::Mac::MacEnv;
using ColorMap_ = Environment::Mac::MacColorMap;

}
#endif
