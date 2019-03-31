#pragma once

#include <peripherals/base.h>

#if defined(COFFEE_LINUX)

#include <platforms/posix/stacktrace.h>

namespace platform {

using Stacktracer = env::glibc::Stacktracer;

} // namespace platform

#endif
