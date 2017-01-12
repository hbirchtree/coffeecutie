#pragma once

#include "../../platform_detect.h"

#ifdef COFFEE_LINUX

#include "../../../coffee_macros.h"
#include "../stacktrace_def.h"

#if defined(COFFEE_USE_UNWIND)
#define UNW_LOCAL_ONLY
#include <cxxabi.h>     //Demangling function names
#include <libunwind.h>  //For retrieving the callstack
#endif

namespace Coffee{
namespace Environment{
namespace Linux{
struct LinuxStacktracer : StacktracerDef
{
    STATICINLINE CString DemangleSymbol(CString const& sym)
    {
#if defined(COFFEE_USE_UNWIND)
        int32 stat = 0;
        cstring_w symbol = abi::__cxa_demangle(sym.c_str(),nullptr,nullptr,&stat);
        if(stat==0)
            return CString(symbol);
        else
            return sym;
#endif
        return {};
    }
#if defined(COFFEE_USE_UNWIND)
    STATICINLINE Stacktrace GetRawStackframes(uint32 start = 0, int32 length = 1)
#else
    STATICINLINE Stacktrace GetRawStackframes(uint32 = 0, int32 = 1)
#endif
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

        uint32 depth = 0;

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
            if(length!=(-1) && depth==length)
                break;
        }
#endif
        return t;
    }

    STATICINLINE CString GetStackframeName(uint32 depth = 0)
    {
        Stacktrace trace = GetRawStackframes(depth,1);
        if(!trace.size())
            return "???";
        return CString(trace[0]);
    }
    STATICINLINE Stacktrace GetFullStack(uint32 start = 0, int32 length = -1)
    {
        return GetRawStackframes(start,length);
    }
private:
#if defined(COFFEE_USE_UNWIND)
    static unw_context_t* unwind_context;
#endif
};
}
}

using Stacktracer = Environment::Linux::LinuxStacktracer;

}

#endif
