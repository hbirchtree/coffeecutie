#include <platforms/stacktrace.h>

#include <platforms/pimpl_state.h>

#include <peripherals/stl/functional_types.h>
#include <peripherals/stl/stlstring_ops.h>

#include <peripherals/stl/string_casting.h>

namespace platform::stacktrace {

void print_frames(
    stacktrace&&                       frames,
    typing::logging::LogInterfaceBasic print,
    typing::logging::StackWriterEx)
{
    detail::print(print, "dumping stacktrace:");
    for(auto const& frame : frames)
    {
        // clang-format off
        detail::print(
            print,
            stl_types::String(" >> ") +
            "exec(" + frame.name() + ") [0x????] " +
            (frame.source_file().empty() ?
                std::string() :
                frame.source_file() + ":" + std::to_string(frame.source_line()))
        );
        // clang-format on
    }
}

void print_exception(
    stl_types::Pair<std::string, stacktrace>&& exception,
    typing::logging::LogInterfaceBasic         print,
    typing::logging::StackWriterEx             stack_writer)
{
    detail::print(print, "exception encountered:");
    detail::print(print, " >> " + exception.first);

    print_frames(std::move(exception.second), print, stack_writer);
}

} // namespace platform::stacktrace
