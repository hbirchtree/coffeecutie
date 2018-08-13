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

    static CString GetFuncName_Internal(void* funcPtr);

    template<typename T>
    STATICINLINE CString GetFuncName(T funcPtr)
    {
        return GetFuncName_Internal(C_RCAST<void*>(funcPtr));
    }
};

}
}

using Stacktracer = Environment::Linux::LinuxStacktracer;

}

#endif
