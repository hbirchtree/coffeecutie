#pragma once

#include <peripherals/base.h>

#if(defined(COFFEE_UNIXPLAT) && !defined(COFFEE_EMSCRIPTEN)) || \
    defined(COFFEE_GEKKO)

#include <platforms/base/stacktrace.h>

#if defined(COFFEE_USE_UNWIND)
#define UNW_LOCAL_ONLY
#include <libunwind.h>
#endif

namespace platform {
namespace env {
namespace posix {

struct Stacktracer : StacktracerDef
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
        return GetRawStackframes(start, length);
    }
};

} // namespace posix

/* glibc-like stacktraces exist on Linux and Darwin */
#if(defined(COFFEE_LINUX) || defined(COFFEE_APPLE)) && \
    !defined(COFFEE_NO_EXCEPTION_RETHROW)

#define COFFEE_GLIBC_STACKTRACE 1

namespace glibc {

struct Stacktracer : posix::Stacktracer
{
    COFFEE_DISABLE_ASAN static void Backtrace(
        typing::logging::LogInterfaceBasic log    = nullptr,
        typing::logging::StackWriter       stackw = nullptr);

    static void ExceptionStacktrace(
        ExceptionPtr const&                exc_ptr,
        typing::logging::LogInterfaceBasic log,
        typing::logging::StackWriter       stackw = nullptr);

    static CString GetFuncName_Internal(void* funcPtr);

    template<typename T>
    STATICINLINE CString GetFuncName(T funcPtr)
    {
        return GetFuncName_Internal(C_RCAST<void*>(funcPtr));
    }
};

} // namespace glibc
#endif

#if defined(COFFEE_UNWIND_STACKTRACE)

namespace unwind {

struct Stacktracer : posix::Stacktracer
{
    COFFEE_DISABLE_ASAN static void Backtrace(
        typing::logging::LogInterfaceBasic log    = nullptr,
        typing::logging::StackWriter       stackw = nullptr);

    static void ExceptionStacktrace(
        ExceptionPtr const&                exc_ptr,
        typing::logging::LogInterfaceBasic log,
        typing::logging::StackWriter       stackw = nullptr);
};

} // namespace unwind

#endif

#if defined(COFFEE_UNWIND_STACKTRACE)
using Stacktracer = unwind::Stacktracer;
#elif defined(COFFEE_GLIBC_STACKTRACE)
using Stacktracer = glibc::Stacktracer;
#else
using Stacktracer = posix::Stacktracer;
#endif

} // namespace env
} // namespace platform

#endif
