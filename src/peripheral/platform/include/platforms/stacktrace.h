#pragma once

#include <exception>
#include <optional>
#include <peripherals/base.h>
#include <peripherals/libc/output_ops.h>
#include <peripherals/stl/string_ops.h>
#include <peripherals/typing/logging.h>
#include <typeinfo>
#include <utility>

#if defined(COFFEE_UNIXPLAT)
#define BOOST_STACKTRACE_USE_BACKTRACE
#endif

#include <boost/stacktrace.hpp>

//#include "base/stacktrace.h"
//#include "posix/stacktrace.h"
//#include "linux/stacktrace.h"
//#include "win32/stacktrace.h"

namespace platform::stacktrace {
namespace detail {

template<typename... Args>
FORCEDINLINE void print(
    typing::logging::LogInterfaceBasic log, std::string const& line)
{
    log(libc::io::io_handles::err,
        line,
        semantic::debug::Severity::Critical,
        1,
        0);
}

} // namespace detail

namespace demangle {

FORCEDINLINE auto name(std::string const& symbol)
{
    boost::core::scoped_demangled_name demangler(symbol.c_str());
    return std::string(demangler.get());
}

template<typename T>
FORCEDINLINE auto type_name()
{
    return name(typeid(T).name());
}

template<typename T>
FORCEDINLINE auto type_name(T& e)
{
    return name(typeid(e).name());
}

} // namespace demangle

using boost::stacktrace::stacktrace;

FORCEDINLINE
std::optional<std::pair<std::string, stacktrace>> frames_of(
    stl_types::ExceptionPtr const& exc)
{
    try
    {
        std::rethrow_exception(exc);
    } catch(std::exception& e)
    {
        return std::make_optional(std::pair<std::string, stacktrace>(
            demangle::type_name(e) + ": " + e.what(), stacktrace()));
    }
    return std::nullopt;
}

FORCEDINLINE auto exception_frames()
{
    return frames_of(std::current_exception());
}

FORCEDINLINE auto frames()
{
    return boost::stacktrace::stacktrace();
}

void print_frames(
    stacktrace&&                       frames,
    typing::logging::LogInterfaceBasic print,
    typing::logging::StackWriterEx     stack_writer);

void print_exception(
    stl_types::Pair<std::string, stacktrace>&& exception,
    typing::logging::LogInterfaceBasic         print,
    typing::logging::StackWriterEx             stack_writer);

} // namespace platform::stacktrace

namespace platform::env {

struct Stacktracer
{
    C_DEPRECATED_S("using platform::stacktrace::frames_of")
    STATICINLINE void ExceptionStacktrace(
        std::exception_ptr const&          exc,
        typing::logging::LogInterfaceBasic log,
        typing::logging::StackWriter       stackw = nullptr)
    {
    }

    C_DEPRECATED_S("using platform::stacktrace::demangle::name")
    STATICINLINE std::string DemangleSymbol(const char* symbol)
    {
        return platform::stacktrace::demangle::name(symbol);
    }

    C_DEPRECATED_S("use platform::stacktrace::frames")
    STATICINLINE void Backtrace(typing::logging::LogInterfaceBasic = nullptr)
    {
    }
};

} // namespace platform::env
