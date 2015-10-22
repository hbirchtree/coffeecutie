#ifndef COFFEE_DEBUG
#define COFFEE_DEBUG

#include "coffee.h"
#include "plat/plat_core.h"
#include "coffee/cregex.h"

#include "cppformat/format.h"

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


template<typename... Args>
static void cfprintf(FILE* stream, cstring format, Args... args)
{
    try{
        fmt::fprintf(stream,format,args...);
    }catch(std::exception e)
    {
        fprintf(stderr,"Failed to print message: %s\n",e.what());
    }
}

namespace CDebugHelpers{
static void coffee_print_callstack(cstring header, cstring callfmt, cstring_w* callstack, szptr stacksize)
{
    cfprintf(stderr,header);
    for(szptr i=0;i<stacksize;i++){
        cfprintf(stderr,callfmt,callstack[i]);
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

/*!
 * \brief Different severities for debug messages
 */
enum DebugSeverity
{
    DebugMsgInfo, /*!< Simple info, nothing serious*/
    DebugMsgDebug, /*!< Debug info, should be disabled in release mode*/
    DebugMsgWarning, /*!< Warning, should be paid attention to*/
    DebugMsgFatal, /*!< Fatal message, followed by a crash*/
};

template<typename... Arg>
/*!
 * \brief cDebugPrint
 * \param severity Whether we should use stderr, stdout or crash
 * \param stackreduce How much of the stack should be reduced with respect to debug functions and etc.
 * \param str Format string to print with
 * \param args Variadic arguments to string format
 */
static void cDebugPrint(
        uint8 severity,
        uint32 stackreduce,
        cstring str,
        Arg... args)
{
    cstring sevstring = nullptr;
    cstring_w timestring = nullptr;
    cstring callstring = nullptr;

    //Some settings for output
    FILE* strm = stdout;
    bool fail = false;
    timestring = CDebugHelpers::coffee_clock_string();


    //Get call stack
    cstring_w* callstack = nullptr;
    szptr cs_length;
    {
        callstack = CDebugHelpers::coffee_callstack(&cs_length,stackreduce);
        callstring = (cs_length>0) ? callstack[0] : "[callstack unavailable]";
    }
    //

    cstring col = print_color_debug;

    switch(severity){
    case DebugMsgInfo:
        sevstring = "INFO:";
        strm = stderr;
        break;
    case DebugMsgDebug:
        sevstring = "DEBG:";
        strm = stderr;
        break;
    case DebugMsgWarning:
        sevstring = "WARN:";
	col = print_color_warning;
        strm = stderr;
        break;
    case DebugMsgFatal:{
        sevstring = "FTAL:";
	col = print_color_fatal;
        strm = stderr;
        fail = true;
        break;
    }
    }

    cfprintf(strm,"%s%s:%s:%s%s: %s\n",col,timestring,sevstring,callstring,print_color_reset,cStringFormat(str,args...));

    if(fail){
        CDebugHelpers::coffee_print_callstack("Callstack before crash: \n","-> %s\n",callstack,cs_length);
        throw std::runtime_error(cStringFormat(str,args...));
    }

    CDebugHelpers::coffee_free_callstack(callstack,cs_length);
    CDebugHelpers::coffee_clock_free(timestring);
}

template<typename... Arg>
/*!
 * \brief Prints message with newline, nothing else
 * \param str
 * \param args
 */
static void cBasicPrint(cstring str, Arg... args)
{
    cstring_w fmt = reinterpret_cast<cstring_w>(malloc(strlen(str)+2));
    strcpy(fmt,str);
    strcat(fmt,"\n");
    cfprintf(stderr,fmt,args...);
    free(fmt);
}

template<typename... Arg>
/*!
 * \brief Debug message
 * \param str
 * \param args
 */
static void cDebug(cstring str, Arg... args)
{
    cDebugPrint(1,1,str,args...);
}

template<typename... Arg>
/*!
 * \brief Warning message
 * \param str
 * \param args
 */
static void cWarning(cstring str, Arg... args)
{
    cDebugPrint(2,1,str,args...);
}

template<typename... Arg>
/*!
 * \brief Fatal message, will crash program
 * \param str
 * \param args
 */
static void cFatal(cstring str, Arg... args)
{
    cDebugPrint(3,1,str,args...);
}

template<typename... Arg>
/*!
 * \brief Print message with source and message
 * \param src
 * \param msg
 * \param args
 */
static void cMsg(cstring src, cstring msg, Arg... args)
{
    cstring_w str = reinterpret_cast<cstring_w>(malloc(strlen(src)+strlen(msg)+2));
    memcpy(str,src,strlen(src)+1);
    strcat(str,":");
    strcat(str,msg);
    cDebugPrint(0,1,str,args...);
    free(str);
}

template<typename... Arg>
/*!
 * \brief Should not be used for debug printing, it's slower than using printf
 * \param fmt
 * \param args
 * \return A readily formatted string
 */
static CString cStringFormat(cstring fmt, Arg... args)
{
//    szptr sz = snprintf(nullptr,0,fmt,args...);
//    cstring_w _s = reinterpret_cast<cstring_w>(malloc(sz+1));
//    sprintf(_s,fmt,args...);
//    CString _o(_s);
//    free(_s);
//    return _o;

    return fmt::sprintf(fmt,args...);
}

}
}

#endif // COFFEE_DEBUG
