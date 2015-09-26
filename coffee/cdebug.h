#ifndef COFFEE_DEBUG
#define COFFEE_DEBUG

#include "coffee.h"
#include "plat/plat_core.h"

//C++ headers
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdio>

//C libraries
#include <ctime>
#include <string.h>

namespace Coffee{
namespace CFunctional{

template<typename... Arg>
static CString cStringFormat(cstring fmt, Arg... args);

namespace CDebugHelpers
{

static void coffee_print_callstack(cstring header, cstring callfmt, cstring_w* callstack, szptr stacksize)
{
    fprintf(stderr,header);
    for(szptr i=0;i<stacksize;i++){
        fprintf(stderr,callfmt,callstack[i]);
        free(callstack[i]);
    }
    free(callstack);
}

static void coffee_free_callstack(cstring_w* callstack, szptr stacksize)
{
    for(szptr i=0;i<stacksize;i++)
        free(callstack[i]);
    free(callstack);
}

}

template<typename... Arg>
static void cDebugPrint(
        uint8 severity,       //Whether we should stderr, stdout or crash
        uint32 stackreduce,   //Removes parts of the displayed callstack
        cstring str,          //The formatting string
        Arg... args)          //The arguments
{
    //Some settings for output
    FILE* strm = stdout;
    bool fail = false;
    cstring_w time_val = CDebugHelpers::coffee_clock_string();
    CString s_ = time_val;
    CDebugHelpers::coffee_clock_free(time_val); //inlined

    //Get call stack
    cstring first = nullptr;
    cstring_w* callstack = nullptr;
    szptr cs_length;
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

    fprintf(strm,s_.c_str(),args...);

    if(fail){
        CDebugHelpers::coffee_print_callstack("Callstack before crash: \n","-> %s\n",callstack,cs_length);
        throw std::runtime_error(cStringFormat(str,args...));
    }

    CDebugHelpers::coffee_free_callstack(callstack,cs_length);
}

template<typename... Arg>
static void cBasicPrint(cstring str, Arg... args)
{
    cstring_w fmt = reinterpret_cast<cstring_w>(malloc(strlen(str)+2));
    strcpy(fmt,str);
    strcat(fmt,"\n");
    fprintf(stderr,fmt,args...);
    free(fmt);
}

template<typename... Arg>
static void cDebug(cstring str, Arg... args)
{
    cDebugPrint(1,1,str,args...);
}

template<typename... Arg>
static void cWarning(cstring str, Arg... args)
{
    cDebugPrint(2,1,str,args...);
}

template<typename... Arg>
static void cFatal(cstring str, Arg... args)
{
    cDebugPrint(3,1,str,args...);
}

template<typename... Arg>
static void cMsg(cstring src, cstring msg, Arg... args)
{
    cstring_w str = reinterpret_cast<cstring_w>(malloc(strlen(src)+strlen(msg)+2));
    memcpy(str,src,strlen(src)+1);
    strcat(str,":");
    strcat(str,msg);
    cDebugPrint(0,1,str,args...);
    free(str);
}

//cStringFormat should not be used for debug printing, it's slower than using printf
template<typename... Arg>
static CString cStringFormat(cstring fmt, Arg... args)
{
    szptr sz = snprintf(nullptr,0,fmt,args...);
    cstring_w _s = reinterpret_cast<cstring_w>(malloc(sz+1));
    sprintf(_s,fmt,args...);
    CString _o(_s);
    free(_s);
    return _o;
}

}
}

#endif // COFFEE_DEBUG

