#pragma once

#include "../posix/detail.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <minwindef.h>
#undef far
#undef near

namespace platform::file::win32::detail {

std::string resource_hash(Url const& file);

struct win32_fd_t
{
    HRSRC             resource{};
    posix::posix_fd_t fd;

    operator posix::posix_fd_t const&() const
    {
        return fd;
    }
};

FORCEDINLINE bool is_resource(Url const& file, RSCA access)
{
    return (access & RSCA::AccessMask) == RSCA::ReadOnly &&
           (file.flags & RSCA::AssetFile) == RSCA::AssetFile;
}

FORCEDINLINE bool is_resource(win32_fd_t const& fd)
{
    return fd.resource;
}

szptr                          resource_length(win32_fd_t const& fd);
std::optional<gsl::span<char>> resource_data(win32_fd_t const& fd);

FORCEDINLINE std::optional<posix::posix_error> close_file(win32_fd_t&& file)
{
    if(!file.resource)
        posix::detail::close_fd(file.fd);
    return std::nullopt;
}

} // namespace platform::file::win32::detail

namespace platform::file::win32 {

result<detail::win32_fd_t, posix::posix_error> open_file(
    Url const& file, RSCA access, permissions_t perms = default_permissions);

}
