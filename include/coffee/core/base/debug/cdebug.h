#ifndef COFFEE_DEBUG
#define COFFEE_DEBUG

//#define CPPFORMAT_PRINTING

#ifdef CPPFORMAT_PRINTING
#include <cppformat/format.h>
#endif

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

template<typename... Args>
FORCEDINLINE void cfprintf(FILE* stream, cstring format, Args... args)
{
    bool locking = false;

    /* If printing to file, don't lock IO */
    if(stream==stdout||stream==stderr)
        locking = true;

    CString formatted = cStringFormat(format,args...);
    if(locking)
        DebuggingState::PrinterLock.lock();
    Puts(stream,formatted.c_str());
    if(locking)
        DebuggingState::PrinterLock.unlock();
}

namespace CDebugHelpers{
FORCEDINLINE void coffee_print_callstack(cstring header, cstring callfmt, cstring_w* callstack, szptr stacksize)
{
    cfprintf(DefaultDebugOutputPipe,header);
    for(szptr i=0;i<stacksize;i++){
        cfprintf(DefaultDebugOutputPipe,callfmt,callstack[i]);
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
    cfprintf(DefaultDebugOutputPipe,"{0}\n",out.c_str());
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
    cfprintf(DefaultDebugOutputPipe,"{0}",out.c_str());
}

template<typename... Arg> FORCEDINLINE
void cOutputPrint(cstring str, Arg... args)
{
    CString out = cStringFormat(str,args...);
    cfprintf(DefaultPrintOutputPipe,"{0}\n",out.c_str());
}

template<typename... Arg> FORCEDINLINE
void cOutputPrintNoNL(cstring str, Arg... args)
{
    CString out = cStringFormat(str,args...);
    cfprintf(DefaultPrintOutputPipe,"{0}",out.c_str());
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

template<typename... Arg>
/*!
 * \brief Verbose message
 * \param str
 * \param args
 */
FORCEDINLINE void cVerbose(cstring str, Arg... args)
{
    DebugPrinter::cVerbose(str,args...);
}

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
                Severity::Information,1,"{0}: {1}",src,msg_out.c_str());
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
