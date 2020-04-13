#pragma once

#include <peripherals/base.h>

#if defined(COFFEE_WINDOWS)

#include <platforms/base/stacktrace.h>

namespace platform {
namespace env {
namespace win32 {

struct Stacktracer : StacktracerDef
{
};

} // namespace win32

using Stacktracer = win32::Stacktracer;

} // namespace env
} // namespace platform

#endif
