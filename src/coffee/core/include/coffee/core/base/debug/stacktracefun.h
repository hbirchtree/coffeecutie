#pragma once

#include "../../plat/environment/stacktrace_def.h"
#include "debug_interface.h"

namespace Coffee {
namespace DebugFun {

struct StacktraceFunImpl : StacktraceFunDef
{
    static Stacktrace GetStacktrace()
    {
        return Stacktrace();
    }
};

using StacktraceFun = StacktraceFunImpl;

} // namespace DebugFun
} // namespace Coffee
