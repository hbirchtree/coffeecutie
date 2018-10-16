#include <coffee/core/plat/environment/unix/stacktrace.h>

#if defined(COFFEE_UNIXPLAT)

#include <coffee/core/coffee_assert_macros.h>
#include <coffee/core/types/cdef/memsafe.h>

#include <coffee/core/CRegex>

#include <cxxabi.h> //Demangling function names

#if defined(COFFEE_USE_UNWIND)
#define UNW_LOCAL_ONLY
#include <libunwind.h> //For retrieving the callstack
#endif

namespace Coffee {
namespace Environment {
namespace Posix {

CString PosixStacktracer::DemangleSymbol(const char* sym)
{
#ifndef COFFEE_LOWFAT
    i32       stat   = 0;
    Ptr<char> symbol = abi::__cxa_demangle(sym, nullptr, nullptr, &stat);
    if(stat == 0)
    {
        CString outSymbol = symbol.ptr;
        CFree(symbol);
        return outSymbol;
    } else
#endif
        return sym;
}

StacktracerDef::Stacktrace PosixStacktracer::GetRawStackframes(
    C_UNUSED(u32 start), C_UNUSED(i32 length))
{
    Stacktrace t;
#ifndef COFFEE_LOWFAT
#if defined(COFFEE_USE_UNWIND)
    if(!unwind_context)
    {
        unwind_context = new unw_context_t;
        unw_getcontext(unwind_context);
    }

    unw_cursor_t cursor;
    unw_init_local(&cursor, unwind_context);

    for(uint32 i = 0; i < start; i++)
        unw_step(&cursor);

    CString temp_buf;
    temp_buf.resize(256);

    int32 depth = 0;

    while(unw_step(&cursor) > 0)
    {
        unw_word_t offset, pc;
        unw_get_reg(&cursor, UNW_REG_IP, &pc);
        if(pc == 0)
            break;
        if(unw_get_proc_name(
               &cursor,
               &temp_buf[0],
               temp_buf.size() * sizeof(char),
               &offset) == 0)
        {
            CString fname = DemangleSymbol(temp_buf);
            t.push_back(fname);
        }

        depth++;
        if(length != (-1) && depth == length)
            break;
    }
#endif
#endif
    return t;
}

CString PosixStacktracer::GetStackframeName(u32 depth)
{
#ifndef COFFEE_LOWFAT
    Stacktrace trace = GetRawStackframes(depth, 2);
    if(!trace.size())
        return "???";
    return CString(trace[0]);
#else
    return {};
#endif
}

CString PosixStacktracer::GetStackFuncName(u32 depth)
{
#ifndef COFFEE_LOWFAT
    static bool           rgx_compiled;
    static Regex::Pattern rgx;

    CString frame = GetStackframeName(depth + 1);

    if(!rgx_compiled)
    {
        rgx = Regex::compile_pattern("^(.*)\\(.*$");
    }

    Vector<CString> result;

    if(!Regex::match(rgx, frame, result))
        return frame;

    return result[1];
#else
    return {};
#endif
}

} // namespace Posix

} // namespace Environment
} // namespace Coffee

#endif
