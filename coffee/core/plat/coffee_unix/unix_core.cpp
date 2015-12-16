#include "unix_core.h"

#ifdef COFFEE_UNIXPLAT

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/resource.h>
#include <cstdlib>
#include <malloc.h>
#include <cstring>
#include <ctime>

#define UNW_LOCAL_ONLY
#include <cxxabi.h>     //Demangling function names
#include <libunwind.h>  //For retrieving the callstack

namespace Coffee{

void coffee_enable_core_dump()
{
    struct rlimit core_limits;
    core_limits.rlim_cur = core_limits.rlim_max = RLIM_INFINITY;
    setrlimit(RLIMIT_CORE,&core_limits);
}

namespace CResources{
namespace CFiles{

bool coffee_file_mkdir(cstring dname, bool createParent)
{
    if(!createParent)
        return mkdir(dname,S_IRWXU|S_IRWXG)==0;

    char tmp[256];
    char *p = NULL;
    size_t len;

    snprintf(tmp,sizeof(tmp),"%s",dname);
    len = strlen(tmp);
    if(tmp[len-1] == '/')
        tmp[len-1] = 0;
    for(p = tmp+1; *p;p++)
        if(*p == '/')
        {
            *p = 0;
            mkdir(tmp,S_IRWXU);
            *p = '/';
        }
    return mkdir(tmp,S_IRWXU)==0;
}

}
szptr coffee_file_get_size(cstring file)
{
    struct stat sb;
    int fd = open(file,O_RDONLY);
    if(fstat(fd,&sb)==-1)
        return 0;
    return sb.st_size;
}
}

namespace CMemoryManagement{

void *coffee_memory_map_file(cstring filename, szptr offset, szptr size, int *error)
{
    szptr pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE)-1);
    int fd = open(filename,O_RDONLY);
    if(fd < 0)
    {
        *error = errno;
        return nullptr;
    }
    byte_t* addr = (byte_t*)mmap(
                NULL,
                offset+size-pa_offset,
                PROT_READ,MAP_PRIVATE,
                fd,pa_offset);
    if(addr == MAP_FAILED)
    {
        *error = errno;
        return nullptr;
    }
    return addr;
}

bool coffee_memory_unmap_file(void *ptr, szptr size)
{
    return munmap(ptr,size);
}

}

namespace CFunctional{
namespace CDebugHelpers{
cstring_w _coffee_posix_demangle_symbol(cstring_w sym, bool *success)
{
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
}

cstring_w *coffee_callstack(szptr *length, uint32 stackreduce)
{
    //Initial values, create a valid ptr for callstack
    szptr callstack_ptr = 0;
    cstring_w* callstack = (cstring_w*)c_alloc(0);
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
    byte_t sym[256];
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
            callstack = (cstring_w*)c_realloc(callstack,callstack_ptr*sizeof(cstring_w));
            if(success){
                //If demangling succeeded, we created a new cstring which we can bring with us
                callstack[callstack_ptr-1] = fname;
            }else{
                //If demangling failed, we copy the string and move on
                cstring_w str = (cstring_w)c_alloc(strlen(fname)+1);
                strcpy(str,fname);
                callstack[callstack_ptr-1] = str;
            }
        }
    }
    *length = callstack_ptr;
    return callstack;
}

cstring_w coffee_clock_string()
{
    cstring_w time_val = (cstring_w)c_alloc(17);
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
    c_free(arg);
}

}
}

}

#endif



