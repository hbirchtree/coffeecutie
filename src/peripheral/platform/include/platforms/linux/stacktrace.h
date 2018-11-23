#pragma once

#include <peripherals/base.h>

#ifdef COFFEE_LINUX

#include <platforms/posix/stacktrace.h>

namespace platform {
namespace env {
namespace Linux {

struct Stacktracer : posix::Stacktracer
{
    static void ExceptionStacktrace(ExceptionPtr const& exc_ptr);

    static CString GetFuncName_Internal(void* funcPtr);

    template<typename T>
    STATICINLINE CString GetFuncName(T funcPtr)
    {
        return GetFuncName_Internal(C_RCAST<void*>(funcPtr));
    }
};

} // namespace Linux
} // namespace env

using Stacktracer = env::Linux::Stacktracer;

} // namespace platform

#endif
