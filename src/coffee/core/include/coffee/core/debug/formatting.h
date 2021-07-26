#pragma once

#include "logging.h"

#include <coffee/strings/format.h>
#include <coffee/core/printing/log_interface.h>

namespace Coffee::Logging {

using semantic::debug::Severity;

template<typename... Args>
FORCEDINLINE void logInterface(Args... args)
{
    DebugFun::GetLogInterface().tag(std::forward<Args>(args)...);
}

template<typename... Arg>
FORCEDINLINE void cOutputPrint(std::string_view fmt, Arg... args)
{
    logInterface(
        libc::io::io_handles::out,
        compile_info::module::name,
        Strings::fmt(fmt, std::forward<Arg>(args)...),
        Severity::Information,
        1,
        print_context | print_newline);
}

template<typename... Arg>
FORCEDINLINE void cOutputPrintNoNL(std::string_view fmt, Arg... args)
{
    logInterface(
        libc::io::io_handles::out,
        compile_info::module::name,
        Strings::fmt(fmt, std::forward<Arg>(args)...),
        Severity::Information,
        1,
        print_none);
}

template<typename... Arg>
FORCEDINLINE void cBasicPrint(std::string_view fmt, Arg... args)
{
    logInterface(
        Logging::default_stream,
        compile_info::module::name,
        Strings::fmt(fmt, std::forward<Arg>(args)...),
        Severity::Information,
        1,
        print_context | print_newline);
}

template<typename... Arg>
FORCEDINLINE void cBasicPrintNoNL(std::string_view fmt, Arg... args)
{
    logInterface(
        Logging::default_stream,
        compile_info::module::name,
        Strings::fmt(fmt, std::forward<Arg>(args)...),
        Severity::Information,
        1,
        print_none);
}

template<typename... Arg>
FORCEDINLINE void cDebug(std::string_view fmt, Arg... args)
{
    logInterface(
        Logging::default_stream,
        compile_info::module::name,
        Strings::fmt(fmt, std::forward<Arg>(args)...),
        Severity::Debug,
        1,
        print_context | print_newline);
}

template<typename... Arg>
FORCEDINLINE void cWarning(std::string_view fmt, Arg... args)
{
    logInterface(
        Logging::default_stream,
        compile_info::module::name,
        Strings::fmt(fmt, std::forward<Arg>(args)...),
        Severity::Medium,
        1,
        print_context | print_newline);
}

template<typename... Arg>
FORCEDINLINE void cFatal(std::string_view fmt, Arg... args)
{
    logInterface(
        Logging::default_stream,
        compile_info::module::name,
        Strings::fmt(fmt, std::forward<Arg>(args)...),
        Severity::Fatal,
        1,
        print_context | print_newline);
}

template<typename... Arg>
FORCEDINLINE void cVerbose(u32 v, std::string_view fmt, Arg... args)
{
    logInterface(
        Logging::default_stream,
        compile_info::module::name,
        Strings::fmt(fmt, std::forward<Arg>(args)...),
        Severity::Verbose,
        1,
        print_context | print_newline);
}

} // namespace Coffee::Logging

namespace Coffee {

using Logging::cBasicPrint;
using Logging::cBasicPrintNoNL;
using Logging::cOutputPrint;
using Logging::cOutputPrintNoNL;
using Logging::cDebug;
using Logging::cWarning;
using Logging::cFatal;
using Logging::cVerbose;

}
