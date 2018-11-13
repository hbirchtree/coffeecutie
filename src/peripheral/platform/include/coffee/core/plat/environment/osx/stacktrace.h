#pragma once

#include <coffee/core/base.h>

#if defined(COFFEE_APPLE)

#include "../unix/stacktrace.h"

namespace Coffee {
namespace Environment {
namespace Mac {

struct MacStacktracer : Posix::PosixStacktracer
{
};

} // namespace Mac
} // namespace Environment

using Stacktracer = Environment::Mac::MacStacktracer;

} // namespace Coffee
#endif
