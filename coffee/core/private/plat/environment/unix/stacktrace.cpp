#include <coffee/core/plat/environment/unix/stacktrace.h>

#if defined(COFFEE_UNIXPLAT)

#include <coffee/core/coffee_assert_macros.h>

#include <coffee/core/CRegex>

#include <cxxabi.h>     //Demangling function names

#if defined(COFFEE_USE_UNWIND)
#define UNW_LOCAL_ONLY
#include <libunwind.h>  //For retrieving the callstack
#endif

namespace Coffee{
namespace Environment{
namespace Posix{

CString PosixStacktracer::DemangleSymbol(const char *sym)
{
    int32 stat = 0;
    cstring_w symbol = abi::__cxa_demangle(sym, nullptr, nullptr, &stat);
    if(stat==0)
        return CString(symbol);
    else
        return sym;
}

StacktracerDef::Stacktrace PosixStacktracer::GetRawStackframes(uint32 start, int32 length)
{
    Stacktrace t;
#if defined(COFFEE_USE_UNWIND)
    if(!unwind_context)
    {
        unwind_context = new unw_context_t;
        unw_getcontext(unwind_context);
    }

    unw_cursor_t cursor;
    unw_init_local(&cursor,unwind_context);

    for(uint32 i=0;i<start;i++)
        unw_step(&cursor);

    CString temp_buf;
    temp_buf.resize(256);

    int32 depth = 0;

    while(unw_step(&cursor)>0)
    {
        unw_word_t offset,pc;
        unw_get_reg(&cursor,UNW_REG_IP,&pc);
        if(pc==0)
            break;
        if(unw_get_proc_name(&cursor,&temp_buf[0],temp_buf.size()*sizeof(char),&offset)==0)
        {
            CString fname = DemangleSymbol(temp_buf);
            t.push_back(fname);
        }

        depth++;
        if(length != (-1) && depth == length)
            break;
    }
#else
    C_UNUSED(start);
    C_UNUSED(length);
#endif
    return t;
}

CString PosixStacktracer::GetStackframeName(uint32 depth)
{
    Stacktrace trace = GetRawStackframes(depth, 2);
    if(!trace.size())
        return "???";
    return CString(trace[0]);
}

CString PosixStacktracer::GetStackFuncName(u32 depth)
{
    static bool rgx_compiled;
    static Regex::Pattern rgx;

    CString frame = GetStackframeName(depth + 1);

    if(!rgx_compiled)
    {
        rgx = Regex::Compile("^(.*)\\(.*$");
    }

    auto result = Regex::Match(rgx, frame, true);

    if(result.size() < 2 || result[1].s_match.size() < 1)
        return frame;

    return result[1].s_match.front();
}

}
}
}

#endif
