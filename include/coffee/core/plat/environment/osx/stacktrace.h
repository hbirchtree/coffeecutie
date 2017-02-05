#pragma once

#include "../../plat_primary_identify.h"

#if defined(COFFEE_APPLE)

#include "../unix/stacktrace.h"

namespace Coffee{
namespace Environment{
namespace Mac{

struct MacStacktracer : Posix::PosixStacktracer
{
};

}
}

using Stacktracer = Environment::Mac::MacStacktracer;

}
#endif
