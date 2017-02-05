#pragma once

#include "../../platform_detect.h"

#ifdef COFFEE_LINUX

#include "../unix/stacktrace.h"

namespace Coffee{
namespace Environment{
namespace Linux{

struct LinuxStacktracer : Posix::PosixStacktracer
{
};

}
}

using Stacktracer = Environment::Linux::LinuxStacktracer;

}

#endif
