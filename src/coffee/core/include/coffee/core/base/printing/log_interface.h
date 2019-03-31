#pragma once

#include <peripherals/typing/logging.h>

namespace Coffee {
namespace DebugFun {

using semantic::debug::Severity;
using typing::logging::LogInterfaceBasic;
using typing::logging::LogInterfaceTagged;

struct LogInterface
{
    LogInterfaceBasic  basic;
    LogInterfaceTagged tag;
};

} // namespace DebugFun
} // namespace Coffee
