#ifndef COFFEE_DEBUG
#define COFFEE_DEBUG

#define UNW_LOCAL_ONLY
#include <cxxabi.h> //Demangling function names
#include <libunwind.h> //For retrieving the callstack

#include <vector>
#include <stdio.h>
#include <string.h>
#include <string>
#include <stdexcept>
#include <ctime>
#include <sys/time.h>

namespace Coffee{
namespace CFunctional{

template<typename... Arg>
static std::string cStringFormat(const char* fmt, Arg... args);

namespace CDebugHelpers
{
static char* coffee_demangle_symbol(char* sym,bool* success){
    int stat;
    char* demangled = abi::__cxa_demangle(sym,nullptr,nullptr,&stat);
    if(stat == 0){
        *success = true;
        return demangled;
    }else{
        *success = false;
        std::free(demangled);
        return sym;
    }
}

static char** coffee_callstack(size_t *length,uint stackreduce = 0){
    //Initial values, create a valid ptr for callstack
    size_t callstack_ptr = 0;
    char** callstack = reinterpret_cast<char**>(malloc(0));
    //Set up for unwind
    unw_cursor_t cur;
    unw_context_t ctx;
    unw_getcontext(&ctx);
    unw_init_local(&cur,&ctx);

    //Remove the observing functions
    for(uint i=0;i<stackreduce+1;i++)
        unw_step(&cur);

    //Allocate buffer for symbol
    bool success;
    char sym[256];
    while(unw_step(&cur)>0){
        //Unwinding stuff here
        unw_word_t offset,pc;
        unw_get_reg(&cur,UNW_REG_IP,&pc);
        if(pc == 0)
            break;
        if(unw_get_proc_name(&cur,sym,sizeof(sym),&offset)==0){
            //Demangle the symbol with CXXABI
            char* fname = coffee_demangle_symbol(sym,&success);
            //Increment the callstack pointer, also known as length of the array
            callstack_ptr++;
            //Resize the callstack array
            callstack = reinterpret_cast<char**>(realloc(callstack,callstack_ptr*sizeof(char*)));
            if(success){
                //If demangling succeeded, we created a new char* which we can bring with us
                callstack[callstack_ptr-1] = fname;
            }else{
                //If demangling failed, we copy the string and move on
                char *str = reinterpret_cast<char*>(malloc(strlen(fname)+1));
                strcpy(str,fname);
                callstack[callstack_ptr-1] = str;
            }
        }
    }
    *length = callstack_ptr;
    return callstack;
}

static void coffee_print_callstack(const char* header, const char* callfmt, char** callstack, size_t stacksize){
    fprintf(stderr,header);
    for(size_t i=0;i<stacksize;i++){
        fprintf(stderr,callfmt,callstack[i]);
        free(callstack[i]);
    }
    free(callstack);
}

static void coffee_free_callstack(char** callstack, size_t stacksize){
    for(size_t i=0;i<stacksize;i++)
        free(callstack[i]);
    free(callstack);
}

static char* coffee_clock_string(){
    char *time_val = reinterpret_cast<char*>(malloc(17));
    time_t t;
    struct tm *tm;
    time(&t);
    tm = localtime(&t);
    strftime(time_val,10,"%H:%M:%S.",tm);
    struct timeval tv;
    gettimeofday(&tv,0);
    sprintf(&time_val[9],"%03ld:",tv.tv_usec/1000);
    return time_val;
}
}

template<typename... Arg>
static void cDebugPrint(uint8_t severity,   //Whether we should stderr, stdout or crash
                        uint stackreduce,   //Removes parts of the displayed callstack
                        const char* str,    //The formatting string
                        Arg... args         //The arguments
                        )
{

    //Some settings for output
    FILE* strm = stdout;
    bool fail = false;
    char* time_val = CDebugHelpers::coffee_clock_string();
    std::string s_(time_val);
    free(time_val);

    //Get call stack
    const char* first = nullptr;
    char** callstack = nullptr;
    size_t cs_length;
    {
        callstack = CDebugHelpers::coffee_callstack(&cs_length,stackreduce);
        first = (cs_length>0) ? callstack[0] : "[callstack unavailable]";
    }
    //

    switch(severity){
    case 0: //Info
        s_ += "INFO:";
        strm = stderr;
        break;
    case 1: //Debug
        s_ += "DEBG:";
        strm = stderr;
        break;
    case 2: //Warning
        s_ += "WARN:";
        strm = stderr;
        break;
    case 3:{ //Fatal
        s_ += "FTAL:";
        strm = stderr;
        fail = true;
        break;
    }
    }

    s_ += first;
    s_ += ":";
    s_ += str;
    s_ += "\n";

    if(fail){
        CDebugHelpers::coffee_print_callstack("Callstack before crash: \n","-> %s\n",callstack,cs_length);
        throw std::runtime_error(cStringFormat(s_.c_str(),args...));
    }else
        fprintf(strm,s_.c_str(),args...);
    CDebugHelpers::coffee_free_callstack(callstack,cs_length);
}

template<typename... Arg>
static void cDebug(const char* str, Arg... args){
    cDebugPrint(1,1,str,args...);
}

template<typename... Arg>
static void cWarning(const char* str, Arg... args){
    cDebugPrint(2,1,str,args...);
}

template<typename... Arg>
static void cFatal(const char* str, Arg... args){
    cDebugPrint(3,1,str,args...);
}

template<typename... Arg>
static void cMsg(const char* src, const char* msg, Arg... args){
    char* str = reinterpret_cast<char*>(malloc(strlen(src)+strlen(msg)+2));
    memcpy(str,src,strlen(src)+1);
    strcat(str,":");
    strcat(str,msg);
    cDebugPrint(0,1,str,args...);
    free(str);
}

template<typename... Arg>
static std::string cStringFormat(const char* fmt, Arg... args){
    char* _s = new char[strlen(fmt)];
    sprintf(_s,fmt,args...);
    std::string _o(_s);
    return _o;
}

}
}

#endif // COFFEE_DEBUG

