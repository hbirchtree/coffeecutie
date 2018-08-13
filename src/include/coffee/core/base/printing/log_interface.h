#pragma once

#include <coffee/core/plat/memory/cmemory.h>
#include <coffee/core/types/edef/dbgenum.h>

namespace Coffee {
namespace DebugFun {

using LogInterfaceBasic = void (*)(FILE*, CString const&, Severity, u32, u32);
using LogInterfaceTagged =
    void (*)(FILE*, cstring, CString const&, Severity, u32, u32);

struct LogInterface
{
    LogInterfaceBasic  basic;
    LogInterfaceTagged tag;
};

} // namespace DebugFun
} // namespace Coffee
