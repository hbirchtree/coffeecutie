#pragma once

#include <peripherals/base.h>

#include <exception>
#include <optional>
#include <peripherals/constants.h>
#include <peripherals/libc/output_ops.h>
#include <peripherals/stl/string_ops.h>
#include <peripherals/typing/logging.h>
#include <typeinfo>
#include <utility>

#if __has_include(<cxxabi.h>)
#include <cstdlib>
#include <cxxabi.h>
#include <memory>
#endif

/* libstdc++ provides std::stacktrace (COFFEE_STD_STACKTRACE is set by CMake
 * when it links), libc++ does not, so Android and Apple keep boost.
 * Release builds and everything else fall back to empty frames. */
#if MODE_RELEASE == 1
#elif defined(COFFEE_STD_STACKTRACE)
#define COFFEE_STACKTRACE_STD
#include <stacktrace>
#elif defined(COFFEE_ANDROID) || defined(COFFEE_APPLE)
#define COFFEE_STACKTRACE_BOOST
#if defined(COFFEE_APPLE)
#define BOOST_STACKTRACE_USE_LIBC_BACKTRACE_FUNCTION
#endif
#include <boost/stacktrace.hpp>
#endif

namespace platform::stacktrace {

constexpr bool supports_stacktrace = !compile_info::platform::is_emscripten &&
                                     !compile_info::platform::is_windows;

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

FORCEDINLINE std::string name(std::string const& symbol)
{
#if __has_include(<cxxabi.h>)
    int status = 0;
    std::unique_ptr<char, decltype(&std::free)> demangled(
        abi::__cxa_demangle(symbol.c_str(), nullptr, nullptr, &status),
        &std::free);
    if(status == 0 && demangled)
        return demangled.get();
#endif
    return symbol;
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

#if defined(COFFEE_STACKTRACE_STD)
using stacktrace = std::stacktrace;
#elif defined(COFFEE_STACKTRACE_BOOST)
using stacktrace = boost::stacktrace::stacktrace;
#else
struct frame_t
{
    std::string name() const
    {
        return {};
    }

    std::string source_file() const
    {
        return {};
    }

    uint32_t source_line() const
    {
        return 0;
    }
};

using stacktrace = std::vector<frame_t>;
#endif

FORCEDINLINE auto frames()
{
#if defined(COFFEE_STACKTRACE_STD)
    return std::stacktrace::current(); /* default-constructed is empty */
#else
    return stacktrace();
#endif
}

template<typename Frame>
FORCEDINLINE std::string frame_name(Frame const& frame)
{
    if constexpr(requires { frame.description(); })
        return frame.description();
    else
        return frame.name();
}

FORCEDINLINE
std::optional<std::pair<std::string, stacktrace>> frames_of(
    std::exception_ptr const& exc)
{
    if constexpr(!supports_stacktrace)
        return std::nullopt;

    try
    {
        std::rethrow_exception(exc);
    } catch(std::exception& e)
    {
        return std::make_optional(
            std::pair<std::string, stacktrace>(
                demangle::type_name(e) + ": " + e.what(), frames()));
    }
    return std::nullopt;
}

FORCEDINLINE declreturntype(frames_of) exception_frames()
{
    if constexpr(!supports_stacktrace)
        return std::nullopt;
    return frames_of(std::current_exception());
}

void print_frames(
    stacktrace&&                       frames,
    typing::logging::LogInterfaceBasic print,
    typing::logging::StackWriterEx     stack_writer);

void print_exception(
    std::pair<std::string, stacktrace>&& exception,
    typing::logging::LogInterfaceBasic   print,
    typing::logging::StackWriterEx       stack_writer);

} // namespace platform::stacktrace
