#ifndef COFFEE_DEBUG
#define COFFEE_DEBUG

#include "../../coffee_mem_macros.h"

#include "../strings/cdebug_print.h"

#ifndef COFFEE_LOWFAT
#include "debugprinter.h"
#endif

namespace Coffee{

namespace DebugFun{
using namespace Strings;

namespace CDebugHelpers{
//FORCEDINLINE void coffee_print_callstack(cstring header, cstring callfmt, cstring_w* callstack, szptr stacksize)
//{
//#ifndef COFFEE_LOWFAT
//    OutputPrinter::fprintf(DefaultDebugOutputPipe,header);
//    for(szptr i=0;i<stacksize;i++){
//        OutputPrinter::fprintf(DefaultDebugOutputPipe,callfmt,callstack[i]);
//        CFree(callstack[i]);
//    }
//    CFree(callstack);
//#endif
//}

//FORCEDINLINE void coffee_free_callstack(cstring_w* callstack, szptr stacksize)
//{
//#ifndef COFFEE_LOWFAT
//    for(szptr i=0;i<stacksize;i++)
//        CFree(callstack[i]);
//    CFree(callstack);
//#endif
//}

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
#ifndef COFFEE_LOWFAT
    CString out = cStringFormat(str,args...);
    OutputPrinter::fprintf(DefaultDebugOutputPipe,Severity::Information,DebugPrinterImpl::print_fmt,out.c_str());
#endif
}

template<typename... Arg>
/*!
 * \brief Prints message with newline, nothing else
 * \param str
 * \param args
 */
FORCEDINLINE void cBasicPrintNoNL(cstring str, Arg... args)
{
#ifndef COFFEE_LOWFAT
    CString out = cStringFormat(str,args...);
    OutputPrinter::fprintf(DefaultDebugOutputPipe,Severity::Information,"{0}",out.c_str());
#endif
}

template<typename... Arg> FORCEDINLINE
void cOutputPrint(cstring str, Arg... args)
{
#ifndef COFFEE_LOWFAT
    CString out = cStringFormat(str,args...);
    OutputPrinter::fprintf(DefaultPrintOutputPipe,Severity::Information,DebugPrinterImpl::print_fmt,out.c_str());
#endif
}

template<typename... Arg> FORCEDINLINE
void cOutputPrintNoNL(cstring str, Arg... args)
{
#ifndef COFFEE_LOWFAT
    CString out = cStringFormat(str,args...);
    OutputPrinter::fprintf(DefaultPrintOutputPipe,Severity::Information,"{0}",out.c_str());
#endif
}

template<typename... Arg>
/*!
 * \brief Debug message
 * \param str
 * \param args
 */
FORCEDINLINE void cDebug(cstring str, Arg... args)
{
#ifndef COFFEE_LOWFAT
    DebugPrinter::cDebug(str,args...);
#endif
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
#ifndef COFFEE_LOWFAT
    DebugPrinter::cVerbose(v,str,args...);
#endif
}

template<typename... Arg>
/*!
 * \brief Verbose message
 * \param str
 * \param args
 */
FORCEDINLINE void cVerbose(cstring str, Arg... args)
{
#ifndef COFFEE_LOWFAT
    DebugPrinter::cVerbose(3,str,args...);
#endif
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
#ifndef COFFEE_LOWFAT
    DebugPrinter::cWarning(str,args...);
#endif
}

template<typename... Arg>
/*!
 * \brief Fatal message, will crash program
 * \param str
 * \param args
 */
FORCEDINLINE void cFatal(cstring str, Arg... args)
{
#ifndef COFFEE_LOWFAT
    DebugPrinter::cFatal(str,args...);
#endif
    CASSERT(false);
}

template<typename...Arg>
FORCEDINLINE void cLog(cstring file,int64 line,cstring id, cstring msg, Arg... args)
{
#ifndef COFFEE_LOWFAT
    /* TODO: Pipe this to a proper logger */
#ifndef NDEBUG
    CString logr = cStringFormat("{0}:{1}@{2}", id, file, line);
    CString fmt = logr + msg;
    DebugPrinter::cDebug(fmt.c_str(), args...);
#endif
#endif
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
    cLog(nullptr, 0, src, msg, args...);
}
}

using namespace Coffee::DebugFun;

}

#endif // COFFEE_DEBUG
