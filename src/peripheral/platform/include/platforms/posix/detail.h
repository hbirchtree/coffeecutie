#pragma once

#include <peripherals/base.h>
#include <peripherals/enum/helpers.h>
#include <peripherals/error/posix.h>
#include <peripherals/error/result.h>
#include <peripherals/libc/memory_ops.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/semantic/enum/rsca.h>
#include <peripherals/semantic/handle.h>
#include <platforms/base/rdwrio.h>
#include <url/url.h>

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#if !defined(COFFEE_MINGW64)
#include <sys/mman.h>
#endif

#include "posix_error.h"

namespace platform::file::emscripten {

void track_file_open(int fd, semantic::RSCA access);
void notify_file_close(int fd);

} // namespace platform::file::emscripten

namespace platform::file::posix {

namespace detail {
using namespace enum_helpers;

FORCEDINLINE int openmode(RSCA access)
{
    int out = (feval(access, RSCA::ReadWrite) ? O_RDWR
               : (access & (RSCA::ReadOnly | RSCA::Executable)) != RSCA::None
                   ? O_RDONLY
               : feval(access, RSCA::WriteOnly) ? O_WRONLY
                                                : 0) |
              (feval(access, RSCA::Discard) ? O_TRUNC : 0) |
              (feval(access, RSCA::Append) ? O_APPEND : 0) |
              (feval(access, RSCA::NewFile) ? O_CREAT : 0)
#if defined(O_TMPFILE)
              | (feval(access, RSCA::TempFile) ? O_TMPFILE : 0)
#endif
        ;
    return out;
}

#if !defined(COFFEE_MINGW64)
FORCEDINLINE int protection(RSCA access)
{
    return (feval(access, RSCA::ReadOnly) ? PROT_READ : 0) |
           (feval(access, RSCA::WriteOnly) ? PROT_WRITE : 0) |
           (feval(access, RSCA::Executable) ? PROT_EXEC : 0);
}

FORCEDINLINE int mapping(RSCA access)
{
    return (feval(access, RSCA::Persistent) ? MAP_SHARED : MAP_PRIVATE)
#if defined(MAP_HUGETLB)
           | (feval(access, RSCA::HugeFile) ? MAP_HUGETLB : 0)
#endif
#if defined(MAP_LOCKED)
           | (feval(access, RSCA::ExclusiveLocking) ? MAP_LOCKED : 0)
#endif
        ;
}
#endif

FORCEDINLINE mode_t mode_from_native(::mode_t mode)
{
    // clang-format off
    if(S_ISDIR(mode)) return mode_t::directory;
    if(S_ISREG(mode)) return mode_t::file;
    if(S_ISCHR(mode)) return mode_t::character;
    if(S_ISBLK(mode)) return mode_t::block;
    if(S_ISFIFO(mode)) return mode_t::fifo;
#if !defined(COFFEE_MINGW64)
    if(S_ISLNK(mode)) return mode_t::link;
    if(S_ISSOCK(mode)) return mode_t::socket;
#endif
    return mode_t::none;
    // clang-format on
}

#if !defined(COFFEE_MINGW64)
FORCEDINLINE mode_t dirmode_from_native(int mode)
{
    switch(mode)
    {
    case DT_REG:
        return mode_t::file;
    case DT_DIR:
        return mode_t::directory;
    case DT_BLK:
        return mode_t::block;
    case DT_CHR:
        return mode_t::character;
    case DT_FIFO:
        return mode_t::fifo;
    case DT_LNK:
        return mode_t::link;
    case DT_SOCK:
        return mode_t::socket;
    default:
        return mode_t::none;
    }
}
#endif

STATICINLINE constexpr libc_types::u32 permission_to_native(
    permission_t::permission_t p)
{
    using namespace permission_t;
    // clang-format off
    return
           ((p & read)    ? 04 : 0) |
           ((p & write)   ? 02 : 0) |
           ((p & execute) ? 01 : 0);
    // clang-format on
}

FORCEDINLINE int permissions_to_native(permissions_t const& perms)
{
    using namespace permission_t;
    static_assert(
        0700 == (permission_to_native(read | write | execute) << 6),
        "permission combination wrong");

    return permission_to_native(perms.owner) << 6 |
           permission_to_native(perms.group) << 3 |
           permission_to_native(perms.other);
}

FORCEDINLINE auto align_offset(szptr offset)
{
    const szptr alignment_mask = ~C_CAST<szptr>(libc::mem::page_size());
    return C_CAST<ptroff>(offset & alignment_mask);
}

FORCEDINLINE void close_fd(int fd)
{
    if(fd <= 0)
        return;
    if(close(fd) != 0)
    {
        Throw(common::posix::posix_runtime_error(common::posix::get_error()));
    }
#if defined(COFFEE_EMSCRIPTEN)
    emscripten::notify_file_close(fd);
#endif
}

FORCEDINLINE auto posix_failure()
{
    return failure(common::posix::get_error());
}

#if defined(COFFEE_LINUX)
constexpr auto lseek_ = ::lseek64;
constexpr auto mmap_  = ::mmap64;
#elif !defined(COFFEE_MINGW64)
constexpr auto lseek_ = ::lseek;
constexpr auto mmap_  = ::mmap;
#elif defined(COFFEE_MINGW64)
constexpr auto lseek_ = ::lseek64;
#endif

} // namespace detail

using posix_error = int;
using posix_fd_t  = semantic::generic_handle_t<
    int,
    semantic::handle_modes::auto_close,
    -1,
    detail::close_fd>;

FORCEDINLINE result<posix_fd_t, posix_error> open_file(
    Url const&    file,
    RSCA          access = RSCA::ReadOnly,
    permissions_t perms  = default_permissions)
{
    auto       resolved = *file;
    posix_fd_t fd(open(
        resolved.c_str(),
        detail::openmode(access),
        detail::permissions_to_native(perms)));
    if(fd < 0)
    {
        return failure(common::posix::get_error());
    }
#if defined(COFFEE_EMSCRIPTEN)
    emscripten::track_file_open(fd, file.flags & RSCA::StorageMask);
#endif
    return success(std::move(fd));
}

using platform::common::posix::error_message;

} // namespace platform::file::posix
