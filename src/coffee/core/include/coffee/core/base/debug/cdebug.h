#pragma once

#include <coffee/core/base.h>

#include <peripherals/libc/output_ops.h>

#include "../printing/verbosity_level.h"

#ifndef COFFEE_LOWFAT
#include "../printing/outputprinter.h"
#endif

namespace Coffee {

namespace DebugFun {

/*!
 * \brief Different severities for debug messages
 */
enum DebugSeverity
{
    DebugMsgInfo,    /*!< Simple info, nothing serious*/
    DebugMsgDebug,   /*!< Debug info, should be disabled in release mode*/
    DebugMsgWarning, /*!< Warning, should be paid attention to*/
    DebugMsgFatal,   /*!< Fatal message, followed by a crash*/
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
    OutputPrinter::fprintf(
        libc::io::io_handles::err,
        Severity::Information,
        1,
        OutputPrinter::Flag_NoContext,
        str,
        args...);
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
    OutputPrinter::fprintf(
        DefaultDebugOutputPipe,
        Severity::Information,
        1,
        OutputPrinter::Flag_NoContext | OutputPrinter::Flag_NoNewline,
        str,
        args...);
#endif
}

template<typename... Arg>
FORCEDINLINE void cOutputPrint(cstring str, Arg... args)
{
#ifndef COFFEE_LOWFAT
    OutputPrinter::fprintf(
        DefaultPrintOutputPipe,
        Severity::Information,
        0,
        OutputPrinter::Flag_NoContext,
        str,
        args...);
#endif
}

template<typename... Arg>
FORCEDINLINE void cOutputPrintNoNL(cstring str, Arg... args)
{
#ifndef COFFEE_LOWFAT
    OutputPrinter::fprintf(
        DefaultPrintOutputPipe,
        Severity::Information,
        0,
        OutputPrinter::Flag_NoContext | OutputPrinter::Flag_NoNewline,
        str,
        args...);
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
    OutputPrinter::fprintf(
        DefaultDebugOutputPipe, Severity::Debug, 1, 0, str, args...);
#endif
}

#if MODE_DEBUG
template<typename... Arg>
/*!
 * \brief Verbose message
 * \param str
 * \param args
 */
FORCEDINLINE void cVerbose(u32 v, cstring str, Arg... args)
{
#ifndef COFFEE_LOWFAT
    OutputPrinter::fprintf(
        DefaultDebugOutputPipe, Severity::Verbose, v, 0, str, args...);
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
    OutputPrinter::fprintf(
        DefaultDebugOutputPipe, Severity::Verbose, 3, 0, str, args...);
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
    OutputPrinter::fprintf(
        DefaultDebugOutputPipe, Severity::Medium, 1, 0, str, args...);
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
    OutputPrinter::fprintf(
        DefaultDebugOutputPipe, Severity::Fatal, 0, 0, str, args...);
#endif
    Throw(std::runtime_error(str));
}

template<typename... Arg>
FORCEDINLINE void cTag(cstring tag, cstring str, Arg... args)
{
#ifndef COFFEE_LOWFAT
    OutputPrinter::fprintf_tagged(
        DefaultDebugOutputPipe, Severity::Information, 0, 0, tag, str, args...);
#endif
}

template<typename... Arg>
FORCEDINLINE void cTag(u32 level, cstring tag, cstring str, Arg... args)
{
#ifndef COFFEE_LOWFAT
    OutputPrinter::fprintf_tagged(
        DefaultDebugOutputPipe,
        Severity::Information,
        level,
        0,
        tag,
        str,
        args...);
#endif
}

template<typename... Arg>
C_DEPRECATED FORCEDINLINE void cLog(cstring, i64, cstring, cstring, Arg...)
{
}

template<typename... Arg>
/*!
 * \brief Print message with source and message
 * \param src
 * \param msg
 * \param args
 */
C_DEPRECATED FORCEDINLINE void cMsg(cstring src, cstring msg, Arg... args)
{
    cLog(nullptr, 0, src, msg, args...);
}
} // namespace DebugFun

#define CLOG_FUNCTION(LogFun)             \
    template<typename... Args>            \
    static void LogFun(Args... args)      \
    {                                     \
        using namespace Coffee::DebugFun; \
        LogFun(args...);                  \
    }

struct DebugLogger
{
    CLOG_FUNCTION(cOutputPrint)
    CLOG_FUNCTION(cBasicPrint)
    CLOG_FUNCTION(cOutputPrintNoNL)
    CLOG_FUNCTION(cBasicPrintNoNL)

    CLOG_FUNCTION(cTag)
    CLOG_FUNCTION(cFatal)
    CLOG_FUNCTION(cWarning)
    CLOG_FUNCTION(cDebug)
    CLOG_FUNCTION(cVerbose)
};

#undef LOG_FUNCTION

using namespace DebugFun;

} // namespace Coffee
