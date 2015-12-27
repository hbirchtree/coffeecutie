#ifndef COFFEE_DEBUG
#define COFFEE_DEBUG

//#define CPPFORMAT_PRINTING

#ifdef CPPFORMAT_PRINTING
#include <cppformat/format.h>
#endif

//C++ headers
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

//C libraries
#include <stdio.h>

#include "cdebug_print.h"
#include "coffee/core/coffee_macros.h"

#include "coffee/core/coffee.h"
#include "counter.h"

namespace Coffee{
namespace CFunctional{

extern cstring_w coffee_debug_get_clock_string();
extern void coffee_debug_clear_clock_string(cstring_w str);
extern cstring_w* coffee_debug_get_callstack(szptr *cs_length, uint32 stackreduce);

template<typename... Args>
inline void cfprintf(FILE* stream, cstring format, Args... args)
{
    try{
        CString formatted = cStringFormat(format,args...);
        fputs(formatted.c_str(),stream);
    }
    catch(std::exception e)
    {
        fprintf(stderr,"Failed to print message: %s\n",e.what());
    }
}

namespace CDebugHelpers{
inline void coffee_print_callstack(cstring header, cstring callfmt, cstring_w* callstack, szptr stacksize)
{
    cfprintf(stderr,header);
    for(szptr i=0;i<stacksize;i++){
        cfprintf(stderr,callfmt,callstack[i]);
        c_free(callstack[i]);
    }
    c_free(callstack);
}

inline void coffee_free_callstack(cstring_w* callstack, szptr stacksize)
{
    for(szptr i=0;i<stacksize;i++)
        c_free(callstack[i]);
    c_free(callstack);
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
inline void cDebugPrint(
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
    timestring = coffee_debug_get_clock_string();

    //Get call stack
    cstring_w* callstack = nullptr;
    szptr cs_length;
    {
        callstack = coffee_debug_get_callstack(&cs_length,stackreduce+1);
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

    cfprintf(strm,"{0}{1}:{2}:{3}{4}: {5}\n",
             col,timestring,sevstring,callstring,print_color_reset,
             cStringFormat(str,args...).c_str());

    if(fail){
        CDebugHelpers::coffee_print_callstack(
                    "Callstack before crash: \n","-> {0}\n",
                    callstack,cs_length);
        throw std::runtime_error(cStringFormat(str,args...));
    }

    CDebugHelpers::coffee_free_callstack(callstack,cs_length);
    coffee_debug_clear_clock_string(timestring);
}

template<typename... Arg>
/*!
 * \brief Prints message with newline, nothing else
 * \param str
 * \param args
 */
inline void cBasicPrint(cstring str, Arg... args)
{
    cstring_w fmt = c_cpy_string(str);
    c_realloc(fmt,c_strlen(str)+2);
    c_strcat(fmt,"\n");
    cfprintf(stderr,fmt,args...);
    c_free(fmt);
}

template<typename... Arg>
/*!
 * \brief Debug message
 * \param str
 * \param args
 */
inline void cDebug(cstring str, Arg... args)
{
    cDebugPrint(1,1,str,args...);
}

template<typename... Arg>
/*!
 * \brief Warning message
 * \param str
 * \param args
 */
inline void cWarning(cstring str, Arg... args)
{
    cDebugPrint(2,1,str,args...);
}

template<typename... Arg>
/*!
 * \brief Fatal message, will crash program
 * \param str
 * \param args
 */
inline void cFatal(cstring str, Arg... args)
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
inline void cMsg(cstring src, cstring msg, Arg... args)
{
    CString msg_out = cStringFormat(msg,args...);
    cDebugPrint(0,1,"{0}: {1}",src,msg_out.c_str());
}

template<typename...Arg>
inline void cLog(cstring file,int64 line,cstring id, cstring msg, Arg... args)
{
    CString msg_out = cStringFormat(msg,args...);
    cDebugPrint(0,1,"LOGGER:{0}:{3}:{1}: {2}",file,id,msg_out.c_str(),line);
}

}
}

#endif // COFFEE_DEBUG
