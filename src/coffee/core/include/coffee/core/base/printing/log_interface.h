#pragma once

#include <coffee/core/libc_types.h>
#include <coffee/core/stl_types.h>
#include <peripherals/typing/enum/debug/severity.h>

namespace Coffee {
namespace DebugFun {

using Severity = semantic::debug::Severity;

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
