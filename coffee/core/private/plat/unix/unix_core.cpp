#include <coffee/core/plat/unix/unix_core.h>

#include <coffee/core/base/cdebug.h>
#include <coffee/core/coffee_strings.h>
#include <coffee/core/plat/plat_core.h>

#ifdef COFFEE_UNIXPLAT

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/resource.h>
#include <sys/sysinfo.h>
#include <cstdlib>
#include <malloc.h>
#include <cstring>
#include <ctime>

#if defined(COFFEE_USE_UNWIND)
#define UNW_LOCAL_ONLY
#include <cxxabi.h>     //Demangling function names
#include <libunwind.h>  //For retrieving the callstack
#endif

namespace Coffee{

namespace CFunctional{
namespace CDebugHelpers{
cstring_w _coffee_posix_demangle_symbol(cstring_w sym, bool *success)
{
#if defined(COFFEE_USE_UNWIND)
    int32 stat = 0;
    cstring_w demangled = abi::__cxa_demangle(sym,nullptr,nullptr,&stat);
    if(stat == 0){
        *success = true;
        return demangled;
    }else{
        *success = false;
        std::free(demangled);
        return sym;
    }
#else
    return nullptr;
#endif
}

cstring_w *coffee_callstack(szptr *length, uint32 stackreduce)
{
#if defined(COFFEE_USE_UNWIND)
    //Initial values, create a valid ptr for callstack
    szptr callstack_ptr = 0;
    cstring_w* callstack = (cstring_w*)Alloc(0);
    //Set up for unwind
    unw_cursor_t cur;
    unw_context_t ctx;
    unw_getcontext(&ctx);
    unw_init_local(&cur,&ctx);

    //Remove the observing functions
    for(uint32 i=0;i<stackreduce+1;i++)
        unw_step(&cur);

    //Allocate buffer for symbol
    bool success;
    sbyte_t sym[256];
    while(unw_step(&cur)>0){
        //Unwinding stuff here
        unw_word_t offset,pc;
        unw_get_reg(&cur,UNW_REG_IP,&pc);
        if(pc == 0)
            break;
        if(unw_get_proc_name(&cur,sym,sizeof(sym),&offset)==0){
            //Demangle the symbol with CXXABI
            cstring_w fname = _coffee_posix_demangle_symbol(sym,&success);
            //Increment the callstack pointer, also known as length of the array
            callstack_ptr++;
            //Resize the callstack array
            callstack = (cstring_w*)CRealloc(callstack,callstack_ptr*sizeof(cstring_w));
            if(success){
                //If demangling succeeded, we created a new cstring which we can bring with us
                callstack[callstack_ptr-1] = fname;
            }else{
                //If demangling failed, we copy the string and move on
                cstring_w str = (cstring_w)Alloc(strlen(fname)+1);
                strcpy(str,fname);
                callstack[callstack_ptr-1] = str;
            }
        }
    }
    *length = callstack_ptr;
    return callstack;
#else
    *length = 0;
    return nullptr;
#endif
}

cstring_w coffee_clock_string()
{
    cstring_w time_val = (cstring_w)Alloc(17);
    time_t t;
    struct tm *tm;
    time(&t);
    tm = localtime(&t);
    strftime(time_val,10,"%H:%M:%S.",tm);
    struct timeval tv;
    gettimeofday(&tv,0);
    sprintf(&time_val[9],"%03ld",tv.tv_usec/1000);
    return time_val;
}

void coffee_clock_free(cstring_w arg)
{
    CFree(arg);
}

}
}

}

#endif



