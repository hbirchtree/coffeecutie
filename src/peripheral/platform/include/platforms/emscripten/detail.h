#pragma once

#include "../posix/detail.h"

namespace platform::file::emscripten::detail {

struct emscripten_fd_t
{
    semantic::RSCA    storage_class{RSCA::None};
    std::string       db;
    std::string       file_name;
    posix::posix_fd_t fd;

    operator posix::posix_fd_t&()
    {
        return fd;
    }

    operator posix::posix_fd_t const&() const
    {
        return fd;
    }
};

FORCEDINLINE std::pair<std::string, std::string> split_filename(Url const& file)
{
    auto fname = *file;
    auto split = fname.find(":");
    if(split == std::string::npos)
        return {};
    auto db_name = fname.substr(0, split);
    fname        = fname.substr(split + 1);
    return {db_name, fname};
}

} // namespace platform::file::emscripten::detail

namespace platform::file::emscripten {

FORCEDINLINE result<detail::emscripten_fd_t, posix::posix_error> open_file(
    Url const& file, RSCA access, permissions_t perms = default_permissions)
{
    if(enum_helpers::feval(file.flags, RSCA::ConfigFile))
    {
        auto [db, fname] = detail::split_filename(file);
        return detail::emscripten_fd_t{
            .storage_class = RSCA::ConfigFile,
            .db            = db,
            .file_name     = fname,
        };
    } else if(auto fd = posix::open_file(file, access, perms); fd.has_value())
        return detail::emscripten_fd_t{
            .fd = std::move(fd.value()),
        };
    else
        return fd.error();
}

FORCEDINLINE std::optional<posix::posix_error> close_file(
    detail::emscripten_fd_t&& file)
{
    if(file.storage_class != RSCA::None)
    {
        file = {};
        return std::nullopt;
    } else
        return std::nullopt;
}

} // namespace platform::file::emscripten
