#ifndef COFFEE_DEBUG
#define COFFEE_DEBUG

#include "cdebug_print.h"
#include "debugprinter.h"
#include "../../coffee_mem_macros.h"

#include "../types/counter.h"

namespace Coffee{
namespace DebugFun{

struct DebuggingState
{
    static Mutex PrinterLock;
};

namespace CDebugHelpers{
FORCEDINLINE void coffee_print_callstack(cstring header, cstring callfmt, cstring_w* callstack, szptr stacksize)
{
    OutputPrinter::fprintf(DefaultDebugOutputPipe,header);
    for(szptr i=0;i<stacksize;i++){
        OutputPrinter::fprintf(DefaultDebugOutputPipe,callfmt,callstack[i]);
        CFree(callstack[i]);
    }
    CFree(callstack);
}

FORCEDINLINE void coffee_free_callstack(cstring_w* callstack, szptr stacksize)
{
    for(szptr i=0;i<stacksize;i++)
        CFree(callstack[i]);
    CFree(callstack);
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
 * \brief Prints message with newline, nothing else
 * \param str
 * \param args
 */
FORCEDINLINE void cBasicPrint(cstring str, Arg... args)
{
    CString out = cStringFormat(str,args...);
    OutputPrinter::fprintf(DefaultDebugOutputPipe,"{0}\n",out.c_str());
}

template<typename... Arg>
/*!
 * \brief Prints message with newline, nothing else
 * \param str
 * \param args
 */
FORCEDINLINE void cBasicPrintNoNL(cstring str, Arg... args)
{
    CString out = cStringFormat(str,args...);
    OutputPrinter::fprintf(DefaultDebugOutputPipe,"{0}",out.c_str());
}

template<typename... Arg> FORCEDINLINE
void cOutputPrint(cstring str, Arg... args)
{
    CString out = cStringFormat(str,args...);
    OutputPrinter::fprintf(DefaultPrintOutputPipe,"{0}\n",out.c_str());
}

template<typename... Arg> FORCEDINLINE
void cOutputPrintNoNL(cstring str, Arg... args)
{
    CString out = cStringFormat(str,args...);
    OutputPrinter::fprintf(DefaultPrintOutputPipe,"{0}",out.c_str());
}

template<typename... Arg>
/*!
 * \brief Debug message
 * \param str
 * \param args
 */
FORCEDINLINE void cDebug(cstring str, Arg... args)
{
    DebugPrinter::cDebug(str,args...);
}

#ifndef NDEBUG
template<typename... Arg>
/*!
 * \brief Verbose message
 * \param str
 * \param args
 */
FORCEDINLINE void cVerbose(uint32 v, cstring str, Arg... args)
{
    DebugPrinter::cVerbose(v,str,args...);
}

template<typename... Arg>
/*!
 * \brief Verbose message
 * \param str
 * \param args
 */
FORCEDINLINE void cVerbose(cstring str, Arg... args)
{
    DebugPrinter::cVerbose(3,str,args...);
}
#else
/* Disable the function, not even compiling it */
/* We take no chances */
#define cVerbose(...)
#endif

template<typename... Arg>
/*!
 * \brief Warning message
 * \param str
 * \param args
 */
FORCEDINLINE void cWarning(cstring str, Arg... args)
{
    DebugPrinter::cWarning(str,args...);
}

template<typename... Arg>
/*!
 * \brief Fatal message, will crash program
 * \param str
 * \param args
 */
FORCEDINLINE void cFatal(cstring str, Arg... args)
{
    DebugPrinter::cFatal(str,args...);
    CASSERT(false);
}

template<typename... Arg>
/*!
 * \brief Print message with source and message
 * \param src
 * \param msg
 * \param args
 */
FORCEDINLINE void cMsg(cstring src, cstring msg, Arg... args)
{
    CString msg_out = cStringFormat(msg,args...);
    CString out = DebugPrinter::FormatPrintString(
                Severity::Verbose,1,"{0}: {1}",src,msg_out.c_str());
    DebugPrinter::cBasicPrint("{0}",out);
}

template<typename...Arg>
FORCEDINLINE void cLog(cstring file,int64 line,cstring id, cstring msg, Arg... args)
{
    /* TODO: Pipe this to a proper logger */
#ifndef NDEBUG
    CString msg_out = cStringFormat(msg,args...);
    msg_out = cStringFormat("LOGR:{0}:{1}@{2}: {3}",id,file,line,msg_out.c_str());
    msg_out = DebugPrinter::Colorize(
                msg_out,
                Color::Blue,
                Color::Red);
    cBasicPrint("{0}",msg_out);
#endif
}

}

using namespace Coffee::DebugFun;

}

#endif // COFFEE_DEBUG
