#pragma once

#include <coffee/core/base.h>

#if defined(COFFEE_UNIXPLAT)

#include "../stacktrace_def.h"

#if defined(COFFEE_USE_UNWIND)
#define UNW_LOCAL_ONLY
#include <libunwind.h>
#endif

namespace Coffee{
namespace Environment{
namespace Posix{

struct PosixStacktracer : StacktracerDef
{
    template<typename T>
    STATICINLINE CString GetTypeName()
    {
#ifndef COFFEE_LOWFAT
        return DemangleSymbol(typeid(T).name());
#else
        return {};
#endif
    }

    STATICINLINE CString DemangleSymbol(CString const& sym)
    {
#ifndef COFFEE_LOWFAT
        return DemangleSymbol(sym.c_str());
#else
        return {};
#endif
    }

    static CString DemangleSymbol(const char* sym);

    static Stacktrace GetRawStackframes(u32 start = 0, i32 length = 1);

    static CString GetStackframeName(u32 depth = 0);

    static CString GetStackFuncName(u32 depth = 0);

    STATICINLINE Stacktrace GetFullStack(u32 start = 0, i32 length = -1)
    {
        return GetRawStackframes(start,length);
    }
};

}
}
}

#endif
