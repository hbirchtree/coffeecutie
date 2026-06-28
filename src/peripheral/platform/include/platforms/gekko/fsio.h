#pragma once

#include "../base/rdwrio.h"
#include "../libc/fsio.h"
#include "../libc/rdwrio.h"

#include <peripherals/error/result.h>
#include <url/url.h>

#include <dirent.h>
#include <optional>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// Minimal Gamecube/Wii (newlib) filesystem backend. Backed by C stdio and the
// POSIX subset newlib actually provides (stat/opendir/mkdir/unlink). There is
// no realpath/symlink/mmap/proc here, so those are simplified or stubbed.

namespace platform::file::gekko {

using url::Url;
using gekko_error = int;

namespace detail {

FORCEDINLINE mode_t mode_from_native(unsigned int m)
{
    if(S_ISDIR(m))
        return mode_t::directory;
    if(S_ISREG(m))
        return mode_t::file;
    if(S_ISCHR(m))
        return mode_t::character;
    if(S_ISBLK(m))
        return mode_t::block;
    if(S_ISFIFO(m))
        return mode_t::fifo;
    return mode_t::file;
}

FORCEDINLINE mode_t dirmode_from_native(unsigned char t)
{
    switch(t)
    {
    case DT_DIR:
        return mode_t::directory;
    case DT_REG:
        return mode_t::file;
    default:
        return mode_t::file;
    }
}

} // namespace detail

FORCEDINLINE result<file_info_t, gekko_error> file_info(Url const& file)
{
    auto        resolved = *file;
    struct stat st       = {};
    if(::stat(resolved.c_str(), &st) != 0)
        return failure(errno);
    using namespace permission_t;
    return success(
        file_info_t{
            .size  = C_CAST<libc_types::szptr>(st.st_size),
            .mode  = detail::mode_from_native(st.st_mode),
            .perms = {
                .owner = (st.st_mode & S_IRUSR ? read : none) |
                         (st.st_mode & S_IWUSR ? write : none) |
                         (st.st_mode & S_IXUSR ? execute : none),
                .group = (st.st_mode & S_IRGRP ? read : none) |
                         (st.st_mode & S_IWGRP ? write : none) |
                         (st.st_mode & S_IXGRP ? execute : none),
                .other = (st.st_mode & S_IROTH ? read : none) |
                         (st.st_mode & S_IWOTH ? write : none) |
                         (st.st_mode & S_IXOTH ? execute : none),
            }});
}

FORCEDINLINE result<mode_t, gekko_error> exists(Url const& file)
{
    if(auto info = file_info(file); info.has_error())
        return failure(info.error());
    else
        return success(info.value().mode);
}

FORCEDINLINE result<libc_types::szptr, gekko_error> size(Url const& file)
{
    if(auto info = file_info(file); info.has_error())
        return failure(info.error());
    else
        return success(info.value().size);
}

FORCEDINLINE result<std::vector<file_entry_t>, gekko_error> list(Url const& dir)
{
    auto resolved  = *dir;
    auto directory = ::opendir(resolved.c_str());
    if(!directory)
        return failure(errno);
    std::vector<file_entry_t> entries;
    while(auto entry = ::readdir(directory))
    {
        if(auto name = std::string(entry->d_name); name == "." || name == "..")
            continue;
        entries.push_back(
            file_entry_t{
                .mode = detail::dirmode_from_native(entry->d_type),
                .name = entry->d_name,
            });
    }
    ::closedir(directory);
    return entries;
}

FORCEDINLINE std::optional<gekko_error> create_directory(
    Url const& file, create_params_t const& params)
{
    return std::nullopt;

    // auto resolved = *file;
    // if(params.recursive)
    // {
    //     for(std::string::size_type i = resolved.find('/', 1);
    //         i != std::string::npos;
    //         i = resolved.find('/', i + 1))
    //     {
    //         auto part = resolved.substr(0, i);
    //         if(::mkdir(part.c_str(), 0777) != 0 && errno != EEXIST)
    //             return errno;
    //     }
    // }
    // if(::mkdir(resolved.c_str(), 0777) != 0 && errno != EEXIST)
    //     return errno;
    // return std::nullopt;
}

FORCEDINLINE std::optional<gekko_error> create(
    Url const& file, create_params_t const& params)
{
    return std::nullopt;
    // if(params.mode == mode_t::directory)
    //     return create_directory(file, params);
    // auto resolved = *file;
    // if(auto fd = ::fopen(resolved.c_str(), "ab"); fd)
    // {
    //     ::fclose(fd);
    //     return std::nullopt;
    // }
    // return errno;
}

FORCEDINLINE std::optional<gekko_error> remove(Url const& file)
{
    return std::nullopt;
    // auto resolved = *file;
    // if(::unlink(resolved.c_str()) == 0)
    //     return std::nullopt;
    // if(::rmdir(resolved.c_str()) == 0)
    //     return std::nullopt;
    // return errno;
}

FORCEDINLINE std::optional<gekko_error> truncate(Url const&, libc_types::szptr)
{
    // No meaningful file truncation on this platform; treat as success.
    return std::nullopt;
}

FORCEDINLINE std::optional<std::string> error_message(gekko_error code)
{
    if(auto* msg = ::strerror(code))
        return std::string(msg);
    return std::nullopt;
}

namespace path {

using url::constructors::MkSysUrl;

FORCEDINLINE result<Url, gekko_error> canon(Url const& path)
{
    return success(path);
}

FORCEDINLINE result<Url, gekko_error> dereference(Url const& path)
{
    return success(path);
}

FORCEDINLINE result<Url, gekko_error> dir(Url const& path)
{
    auto resolved = *path;
    if(auto pos = resolved.find_last_of('/'); pos != std::string::npos)
        return success(MkSysUrl(resolved.substr(0, pos)));
    return success(MkSysUrl("."));
}

FORCEDINLINE result<Url, gekko_error> base(Url const& path)
{
    auto resolved = *path;
    if(auto pos = resolved.find_last_of('/'); pos != std::string::npos)
        return success(MkSysUrl(resolved.substr(pos + 1)));
    return success(MkSysUrl(resolved));
}

FORCEDINLINE std::optional<gekko_error> change_dir(Url const&)
{
    // No working directory concept worth honouring here.
    return std::nullopt;
}

FORCEDINLINE result<Url, gekko_error> current_dir()
{
    char buf[256] = {};
    if(::getcwd(buf, sizeof(buf)))
        return success(MkSysUrl(buf));
    return success(MkSysUrl("/"));
}

FORCEDINLINE result<Url, gekko_error> executable()
{
    return success(MkSysUrl("/"));
}

FORCEDINLINE result<Url, gekko_error> app_dir()
{
    return success(MkSysUrl("/"));
}

} // namespace path
} // namespace platform::file::gekko
