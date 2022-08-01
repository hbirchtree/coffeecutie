#pragma once

#include <coffee/core/printing/verbosity_level.h>
#include <peripherals/constants.h>
#include <peripherals/libc/output_ops.h>
#include <peripherals/stl/time_types.h>
#include <peripherals/stl/time_formatting.h>
#include <peripherals/typing/enum/debug/severity.h>
#include <platforms/environment.h>

#if defined(COFFEE_ANDROID)
#include <android/log.h>
#endif

#if defined(COFFEE_EMSCRIPTEN)
#include <emscripten.h>
#endif

namespace Coffee::Logging {
namespace detail {

template<typename Dummy = void>
requires compile_info::platform::is_android
    //
    void log(
        libc::io::output_fd,
        UNUSED_PARAM(std::string_view, tag),
        UNUSED_PARAM(std::string_view, formatted),
        UNUSED_PARAM(semantic::debug::Severity, severity))
{
#if defined(COFFEE_ANDROID)
    using S  = semantic::debug::Severity;
    int flag = ANDROID_LOG_INFO;
    switch(severity)
    {
    case S::Debug:
        flag = ANDROID_LOG_DEBUG;
        break;
    case S::Medium:
        flag = ANDROID_LOG_WARN;
        break;
    case S::Fatal:
        flag = ANDROID_LOG_ERROR;
        break;
    case S::Verbose:
        flag = ANDROID_LOG_VERBOSE;
        break;
    }

    __android_log_print(
        flag, tag.data(), "%s", formatted.data());
#endif
}

template<typename Dummy = void>
requires compile_info::platform::is_emscripten
    //
    void log(
        libc::io::output_fd,
        std::string_view,
        UNUSED_PARAM(std::string_view, formatted),
        UNUSED_PARAM(semantic::debug::Severity, severity))
{
#if defined(COFFEE_EMSCRIPTEN)
    using S  = semantic::debug::Severity;
    int flag = EM_LOG_INFO;
    switch(severity)
    {
    case S::Debug:
        flag = EM_LOG_DEBUG;
        break;
    case S::Medium:
        flag = EM_LOG_WARN | EM_LOG_C_STACK;
        break;
    case S::Fatal:
        flag = EM_LOG_ERROR | EM_LOG_C_STACK;
        break;
    default:
        break;
    }
    emscripten_log(flag, "%.*s", formatted.size(), formatted.data());
#endif
}

template<typename Dummy = void>
requires compile_info::platform::is_windows void log(
    libc::io::output_fd stream,
    std::string_view,
    UNUSED_PARAM(std::string_view, formatted),
    semantic::debug::Severity)
{
    auto formatted_w = stl_types::str::encode::to<wchar_t>(formatted);
    if(platform::env::var("VisualStudioVersion").has_value())
#if defined(COFFEE_WINDOWS)
        OutputDebugStringW(formatted_w.c_str());
#else
        Throw(unimplemented_path("Visual Studio path not available"));
#endif
    else
        libc::io::put(stream, formatted.data());
}

template<typename Dummy = void>
requires(
    (compile_info::platform::is_ios || compile_info::platform::is_linux ||
     compile_info::platform::is_macos) &&
    !compile_info::platform::is_android &&
    !compile_info::platform::is_emscripten)
    //
    void log(
        libc::io::output_fd stream,
        std::string_view,
        std::string_view formatted,
        semantic::debug::Severity)
{
    if constexpr(compile_info::platform::is_ios)
        stream = libc::io::io_handles::out;
    libc::io::put(stream, formatted.data());
}

constexpr libc_types::u32 context_len = 4 + 2 * 3 + 3;

inline stl_types::String context_string(semantic::debug::Severity severity)
{
    using S = semantic::debug::Severity;
    stl_types::String context{};
    context.reserve(context_len);

    if(severity == S::Verbose)
        context.append("VERB");
    else if(severity == S::Information)
        context.append("INFO");
    else if(severity == S::Debug)
        context.append("DEBG");
    else if(severity == S::Medium)
        context.append("WARN");
    else if(severity == S::Fatal)
        context.append("FTAL");

    context.append(":");
    auto clock = stl_types::TimeFormatter<>::String("%H:%M:%S");
    if(auto end = clock.find('\0'); end != stl_types::String::npos)
        context.append(clock.begin(), clock.begin() + end);
    else
        context.append(clock);

    return context;
}

} // namespace detail

enum print_flags
{
    print_none,
    print_context,
    print_newline,
};

template<typename Dummy = void>
requires(!compile_info::lowfat_mode)
    //
    void log(
        libc::io::output_fd       stream,
        cstring                   tag,
        stl_types::String const&  message,
        semantic::debug::Severity severity = semantic::debug::Severity::Debug,
        u32                       level = 0,
        u32                       flags = print_context | print_newline)
{
    if(PrintingVerbosityLevel() < level)
        return;

    if constexpr(compile_info::printing::is_syslogged)
    {
        detail::log(stream, tag, message, severity);
        return;
    }

    stl_types::String output;

    bool has_context =
        (flags & print_context) && !compile_info::printing::is_simple;
    bool has_newline = flags & print_newline;

    output.reserve(
        message.size() +
        (has_context ? (detail::context_len + 1 +
                        (tag ? 1 + libc::str::len(tag) : 0) + 2)
                     : 0) +
        (has_newline ? 1 : 0));

    if(has_context)
    {
        auto context = detail::context_string(severity);
        output.append(context);
        output.append(":");
        output.append(tag);
        output.append(": ");
    }
    output.append(message);
    if(has_newline)
        output.append("\n");

    detail::log(stream, tag, output, severity);
}

template<typename Dummy = void>
requires(compile_info::lowfat_mode)
    //
    void log(
        libc::io::output_fd      stream,
        cstring                  tag,
        stl_types::String const& message,
        semantic::debug::Severity,
        u32,
        u32 flags)
{
    fprintf(
        stream,
        "%s: %s%s",
        tag,
        message.c_str(),
        (flags & print_newline) ? "\n" : "");
}

static auto default_stream = libc::io::io_handles::err;

} // namespace Coffee::Logging
