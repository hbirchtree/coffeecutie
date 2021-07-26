#pragma once

#include <peripherals/typing/logging.h>

namespace Coffee::DebugFun {

using semantic::debug::Severity;
using typing::logging::LogInterfaceBasic;
using typing::logging::LogInterfaceTagged;

struct LogInterface
{
    LogInterfaceTagged tag;
};

/*!
 * \brief Get the currently set logging interface
 * By default this is set to OutputPrinterImpl::fprintf_platform
 * \return
 */
extern LogInterface GetLogInterface();
extern void         SetLogInterface(LogInterface intf);

} // namespace Coffee::DebugFun
