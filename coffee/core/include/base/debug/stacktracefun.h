#ifndef COFFEE_CORE_BASE_DEBUG_STACKTRACE_H
#define COFFEE_CORE_BASE_DEBUG_STACKTRACE_H

#include "debug_interface.h"

namespace Coffee{
namespace DebugFun{

struct StacktraceFunImpl : StacktraceFunDef
{
    static Stacktrace GetStacktrace()
    {
        return Stacktrace();
    }
};

using StacktraceFun = StacktraceFunImpl;

}
}

#endif