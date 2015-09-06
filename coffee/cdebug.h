#ifndef COFFEE_DEBUG
#define COFFEE_DEBUG

#include "coffee.h"

//Callstack unwinding
#define UNW_LOCAL_ONLY
#include <cxxabi.h> //Demangling function names
#include <libunwind.h> //For retrieving the callstack

//C++ headers
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdio>

//C libraries
#include <ctime>
#include <sys/time.h>
#include <string.h>

namespace Coffee{
namespace CFunctional{

template<typename... Arg>
static CString cStringFormat(cstring fmt, Arg... args);

namespace CDebugHelpers
{
static cstring_w coffee_demangle_symbol(cstring_w sym,bool* success){
    int stat = 0;
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

static cstring_w* coffee_callstack(size_t *length,uint stackreduce = 0){
    //Initial values, create a valid ptr for callstack
    size_t callstack_ptr = 0;
    cstring_w* callstack = reinterpret_cast<cstring_w*>(malloc(0));
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
    byte sym[256];
    while(unw_step(&cur)>0){
        //Unwinding stuff here
        unw_word_t offset,pc;
        unw_get_reg(&cur,UNW_REG_IP,&pc);
        if(pc == 0)
            break;
        if(unw_get_proc_name(&cur,sym,sizeof(sym),&offset)==0){
            //Demangle the symbol with CXXABI
            cstring_w fname = coffee_demangle_symbol(sym,&success);
            //Increment the callstack pointer, also known as length of the array
            callstack_ptr++;
            //Resize the callstack array
            callstack = reinterpret_cast<cstring_w*>(realloc(callstack,callstack_ptr*sizeof(cstring_w)));
            if(success){
                //If demangling succeeded, we created a new cstring which we can bring with us
                callstack[callstack_ptr-1] = fname;
            }else{
                //If demangling failed, we copy the string and move on
                cstring_w str = reinterpret_cast<cstring_w>(malloc(strlen(fname)+1));
                strcpy(str,fname);
                callstack[callstack_ptr-1] = str;
            }
        }
    }
    *length = callstack_ptr;
    return callstack;
}

static void coffee_print_callstack(cstring header, cstring callfmt, cstring_w* callstack, size_t stacksize){
    fprintf(stderr,header);
    for(size_t i=0;i<stacksize;i++){
        fprintf(stderr,callfmt,callstack[i]);
        free(callstack[i]);
    }
    free(callstack);
}

static void coffee_free_callstack(cstring_w* callstack, size_t stacksize){
    for(size_t i=0;i<stacksize;i++)
        free(callstack[i]);
    free(callstack);
}

static cstring_w coffee_clock_string(){
    cstring_w time_val = reinterpret_cast<cstring_w>(malloc(17));
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
                        cstring str,    //The formatting string
                        Arg... args         //The arguments
                        )
{
    //Some settings for output
    FILE* strm = stdout;
    bool fail = false;
    cstring_w time_val = CDebugHelpers::coffee_clock_string();
    CString s_(time_val);
    free(time_val);

    //Get call stack
    cstring first = nullptr;
    cstring_w* callstack = nullptr;
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
    if(str)
        s_ += str;
    else
        fail = true;
    s_ += "\n";

    if(fail){
        CDebugHelpers::coffee_print_callstack("Callstack before crash: \n","-> %s\n",callstack,cs_length);
        throw std::runtime_error(cStringFormat(s_.c_str(),args...));
    }else
        fprintf(strm,s_.c_str(),args...);
    CDebugHelpers::coffee_free_callstack(callstack,cs_length);
}

template<typename... Arg>
static void cDebug(cstring str, Arg... args){
    cDebugPrint(1,1,str,args...);
}

template<typename... Arg>
static void cWarning(cstring str, Arg... args){
    cDebugPrint(2,1,str,args...);
}

template<typename... Arg>
static void cFatal(cstring str, Arg... args){
    cDebugPrint(3,1,str,args...);
}

template<typename... Arg>
static void cMsg(cstring src, cstring msg, Arg... args){
    cstring_w str = reinterpret_cast<cstring_w>(malloc(strlen(src)+strlen(msg)+2));
    memcpy(str,src,strlen(src)+1);
    strcat(str,":");
    strcat(str,msg);
    cDebugPrint(0,1,str,args...);
    free(str);
}

//cStringFormat should not be used for debug printing, it's slower than using printf
template<typename... Arg>
static CString cStringFormat(cstring fmt, Arg... args){
    int sz = snprintf(nullptr,0,fmt,args...);
    cstring_w _s = reinterpret_cast<cstring_w>(malloc(sz+1));
    sprintf(_s,fmt,args...);
    CString _o(_s);
    free(_s);
    return _o;
}

template<typename T> class _coffee_elapsed_timer_template
{
public:
    void start(){
        _start = _curr_time();
    }
    T elapsed(){
        return _curr_time()-_start;
    }
protected:
    timeval _tv(){
        struct timeval tv;
        gettimeofday(&tv,0);
        return tv;
    }
    virtual T _curr_time() = 0;
private:
    T _start = 0;
};
class CElapsedTimer : public _coffee_elapsed_timer_template<uint64_t>
{
private:
    uint64_t _curr_time(){
        timeval tv = _tv();
        return tv.tv_sec*1000+tv.tv_usec/1000;
    }
};
class CElapsedTimerMicro : public _coffee_elapsed_timer_template<uint64_t>
{
private:
    uint64_t _curr_time(){
        timeval tv = _tv();
        return tv.tv_sec*1000000+tv.tv_usec;
    }
};
class CElapsedTimerD : public _coffee_elapsed_timer_template<bigscalar>
{
private:
    bigscalar _curr_time(){
        timeval tv = _tv();
        return (bigscalar)tv.tv_sec+(bigscalar)tv.tv_usec/1000000.0;
    }
};

}
}

#endif // COFFEE_DEBUG

