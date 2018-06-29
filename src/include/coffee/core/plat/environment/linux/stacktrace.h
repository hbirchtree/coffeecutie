#pragma once

#include "../../platform_detect.h"

#ifdef COFFEE_LINUX

#include "../unix/stacktrace.h"

namespace Coffee{
namespace Environment{
namespace Linux{

struct LinuxStacktracer : Posix::PosixStacktracer
{
    static void ExceptionStacktrace(ExceptionPtr const& exc_ptr);
};

}
}

using Stacktracer = Environment::Linux::LinuxStacktracer;

}

#endif
