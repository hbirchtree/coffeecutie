#pragma once

#include <peripherals/libc/output_ops.h>
#include <peripherals/stl/functional_types.h>
#include <peripherals/stl/types.h>
#include <peripherals/typing/enum/debug/severity.h>

namespace typing {
namespace logging {

using LogInterfaceBasic = void (*)(
    libc::io::output_fd,
    std::string_view,
    semantic::debug::Severity,
    libc_types::u32, /* level */
    libc_types::u32  /* flags */
);
using LogInterfaceTagged = void (*)(
    libc::io::output_fd,
    std::string_view, /* tag */
    std::string_view, /* line to print */
    semantic::debug::Severity,
    libc_types::u32, /* level */
    libc_types::u32  /* flags */
);
using StackWriter = void (*)(
    std::string const&, /* frame name */
    std::string const&  /* instruction pointer (hex format) */
);

using StackWriterEx = std::function<void(
    std::string_view, /* frame name */
    std::string_view, /* instruction pointer */
    std::string_view, /* source file */
    libc_types::u64   /* source line */
    )>;

STATICINLINE void fprintf_logger(
    libc::io::output_fd fd,
    std::string_view    line,
    semantic::debug::Severity,
    libc_types::u32,
    libc_types::u32)
{
    fprintf(fd, "%.*s\n", static_cast<int>(line.size()), line.data());
}

} // namespace logging
} // namespace typing
