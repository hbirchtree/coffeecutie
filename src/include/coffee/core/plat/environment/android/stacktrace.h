#pragma once

#ifdef COFFEE_ANDROID

#include "../stacktrace_def.h"

namespace Coffee{
namespace Environment{
namespace Android{
struct AndroidStacktracer : StacktracerDef
{
    STATICINLINE CString GetStackframeName(uint32 = 0)
    {
        return CString();
    }
    STATICINLINE Stacktrace GetFullStack(uint32 = 0,int32 = -1)
    {
        return Stacktrace();
    }
};
}
}

using Stacktracer = Environment::Android::AndroidStacktracer;

}
#endif
