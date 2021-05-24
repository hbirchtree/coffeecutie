#pragma once

#include <peripherals/libc/output_ops.h>
#include <peripherals/stl/functional_types.h>
#include <peripherals/stl/types.h>
#include <peripherals/typing/enum/debug/severity.h>

namespace typing {
namespace logging {

using LogInterfaceBasic = void (*)(
    libc::io::output_fd,
    stl_types::CString const&,
    semantic::debug::Severity,
    libc_types::u32, /* level */
    libc_types::u32  /* flags */
);
using LogInterfaceTagged = void (*)(
    libc::io::output_fd,
    libc_types::cstring,       /* tag */
    stl_types::CString const&, /* line to print */
    semantic::debug::Severity,
    libc_types::u32, /* level */
    libc_types::u32  /* flags */
);
using StackWriter = void (*)(
    stl_types::CString const&, /* frame name */
    stl_types::CString const&  /* instruction pointer (hex format) */
    );

using StackWriterEx = stl_types::Function<void(
    stl_types::String const&, /* frame name */
    stl_types::String const&, /* instruction pointer */
    stl_types::String const&, /* source file */
    libc_types::u64 /* source line */
    )>;

STATICINLINE void fprintf_logger(
    libc::io::output_fd       fd,
    stl_types::CString const& line,
    semantic::debug::Severity,
    libc_types::u32,
    libc_types::u32)
{
    fprintf(fd, "%s\n", line.c_str());
}

} // namespace logging
} // namespace typing
