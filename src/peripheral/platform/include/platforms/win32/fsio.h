#pragma once

#include "../posix/detail.h"
#include "../posix/fsio.h"
#include "detail.h"
#include <peripherals/error/result.h>
#include <peripherals/identify/compiler/function_inlining.h>
#include <url/url.h>

namespace platform::file::win32 {

FORCEDINLINE result<file_info_t, posix::posix_error> file_info(
    detail::win32_fd_t const& file)
{
    if(detail::is_resource(file))
    {
        return file_info_t{
            .size = detail::resource_length(file),
            .mode = mode_t::file,
        };
    } else
        return posix::file_info(file.fd);
}

FORCEDINLINE result<file_info_t, posix::posix_error> file_info(Url const& file)
{
    auto fd = open_file(file, RSCA::ReadOnly);
    if(fd.has_error())
        return fd.error();
    return file_info(fd.value());
}

template<typename T>
requires(std::is_same_v<T, Url> || std::is_same_v<T, detail::win32_fd_t>)
FORCEDINLINE result<mode_t, posix::posix_error> exists(T const& file)
{
    if(auto info = file_info(file); info.has_error())
        return posix::posix_error{info.error()};
    else
        return info.value().mode;
}

template<typename T>
requires(std::is_same_v<T, Url> || std::is_same_v<T, detail::win32_fd_t>)
FORCEDINLINE result<szptr, posix::posix_error> size(T const& file)
{
    if(auto info = file_info(file); info.has_error())
        return failure(posix::posix_error{info.error()});
    else
        return success(info.value().size);
}

} // namespace platform::file::win32

namespace platform::file::win32::path {

FORCEDINLINE result<Url, posix::posix_error> executable()
{
    return url::constructors::MkUrl(".");
}

FORCEDINLINE result<Url, posix::posix_error> app_dir()
{
    return url::constructors::MkUrl(".");
}

} // namespace platform::file::win32::path
