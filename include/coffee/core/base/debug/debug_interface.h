#pragma once

#include "../../types/basetypes.h"

namespace Coffee{
namespace DebugFun{

struct StacktraceFunDef
{
    using Stacktrace = Vector<CString>;
    static Stacktrace GetStacktrace();
};

using LogInterface = void(*)(FILE*, CString, Severity, bool);

struct OutputPrinterDef
{
    static Mutex PrinterLock;

    template<typename... Args>
    static void fprintf(FILE*, cstring, Args...){}
};

struct DebugPrinterDef
{
    static CString& Colorize(CString&, Color, Color);

    template<typename... Args>
    static CString FormatPrintString(cstring, Args...){}

    template<typename... Args>
    static void cDebug(cstring, Args...){}
    template<typename... Args>
    static void cWarning(cstring, Args...){}
    template<typename... Args>
    static void cFatal(cstring, Args...){}

    template<typename... Args>
    static void cBasicPrint(cstring, Args...){}
    template<typename... Args>
    static void cBasicPrintNoNL(cstring, Args...){}
};

struct LogPrinterDef
{
    template<typename... Args>
    static void cLog(cstring, int64, cstring, cstring, Args...){}
    template<typename... Args>
    static void cMsg(cstring, cstring, Args...){}
};

/*!
 * \brief Get the currently set logging interface
 * By default this is set to OutputPrinterImpl::fprintf_platform
 * \return
 */

extern LogInterface GetLogInterface();
extern void SetLogInterface(LogInterface intf);

}
}
