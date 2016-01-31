#ifndef COFFEE_CORE_BASE_DEBUG_INTERFACE_H
#define COFFEE_CORE_BASE_DEBUG_INTERFACE_H

#include "types/basetypes.h"

namespace Coffee{
namespace DebugFun{

struct StacktraceFunDef
{
    using Stacktrace = std::vector<CString>;
    static Stacktrace GetStacktrace();
};

struct OutputPrinterDef
{
    static Mutex PrinterLock;

    template<typename... Args>
    static void fprintf(FILE*, cstring, Args...){}
};

struct DebugPrinterDef
{
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

}
}

#endif