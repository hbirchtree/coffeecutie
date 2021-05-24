#pragma once

#include "detail.h"

#include <libgen.h>

namespace platform::file::posix {

FORCEDINLINE result<file_info_t, posix_error> file_info(posix_fd_t const& file)
{
    struct stat file_ = {};
    if(fstat(file, &file_) != 0)
    {
        return detail::posix_failure();
    }

    return success(file_info_t{
        .size = C_CAST<szptr>(file_.st_size),
        .mode = detail::mode_from_native(file_.st_mode),
    });
}

FORCEDINLINE result<file_info_t, posix_error> file_info(Url const& file)
{
    if(auto fd = open_file(file); fd.has_value())
    {
        if(auto info = file_info(fd.value()); info.has_value())
            return success(info.value());
        else
            return failure(info.error());
    } else
        return failure(fd.error());
}

template<typename T>
FORCEDINLINE result<mode_t, posix_error> exists(T const& file)
{
    if(auto info = file_info(file); info.has_error())
        return failure(info.error());
    else
        return success(info.value().mode);
}

template<typename T>
FORCEDINLINE result<szptr, posix_error> size(T const& file)
{
    if(auto info = file_info(file); info.has_error())
        return failure(info.error());
    else
        return success(info.value().size);
}

FORCEDINLINE result<stl_types::Vector<file_entry_t>, posix_error> list(
    Url const& dir)
{
    auto resolved  = *dir;
    auto directory = opendir(resolved.c_str());
    if(!directory)
        return detail::posix_failure();
    stl_types::Vector<file_entry_t> result;
    while(auto direntry = readdir(directory))
    {
        if(auto name = stl_types::String(direntry->d_name);
           name == "." || name == "..")
            continue;
        result.push_back(file_entry_t{
            .mode = detail::mode_from_native(direntry->d_type),
            .name = direntry->d_name,
        });
    }
    if(closedir(directory) != 0)
        return detail::posix_failure();
    return result;
}

namespace detail {

STATICINLINE result<posix_fd_t, posix_error> create_file(
    Url const& file, create_params_t const& params)
{
    auto       resolved = *file;
    posix_fd_t fd(
        ::creat(resolved.c_str(), detail::permissions_to_native(params.perms)));
    if(fd < 0)
        return detail::posix_failure();
    return success(std::move(fd));
}

} // namespace detail

FORCEDINLINE Optional<posix_error> create_directory(
    Url const& file, create_params_t const& params)
{
    auto resolved = *file;
    auto perm     = detail::permissions_to_native(params.perms);
    perm |= S_IXUSR | S_IXGRP | S_IXOTH;
    if(params.recursive)
    {
        auto it = resolved.begin();
        while(it != resolved.end())
        {
            auto current_path =
                resolved.substr(0, C_FCAST<szptr>(it - resolved.begin()));
            if(current_path == resolved || current_path.empty())
                break;
            if(auto status = mkdir(current_path.c_str(), perm); status != 0)
                return detail::posix_failure().error();
            it = std::find(it, resolved.end(), '/');
        }
    }
    if(auto status = mkdir(resolved.c_str(), perm); status != 0)
        return detail::posix_failure().error();
    return std::nullopt;
}

FORCEDINLINE Optional<posix_error> create(
    Url const& file, create_params_t const& params)
{
    auto resolved = *file;
    switch(params.mode)
    {
    case mode_t::file:
        if(auto result = detail::create_file(file, params); result.has_error())
            return result.error();
        break;
    case mode_t::directory:
        if(auto error = create_directory(file, params))
            return error.value();
        break;
    default:
        return posix_error(EINVAL);
    }
    return std::nullopt;
}

FORCEDINLINE Optional<posix_error> remove(Url const& file)
{
    auto resolved = *file;
    if(auto status = ::remove(resolved.c_str()); status != 0)
        return detail::posix_failure().error();
    return std::nullopt;
}

FORCEDINLINE Optional<posix_error> truncate(Url const& file)
{
    auto resolved = *file;
    if(auto status = ::truncate(resolved.c_str(), 0); status != 0)
        return detail::posix_failure().error();
    return std::nullopt;
}

FORCEDINLINE Optional<posix_error> truncate(posix_fd_t const& file)
{
    if(auto status = ::ftruncate(file, 0); status != 0)
        return detail::posix_failure().error();
    return std::nullopt;
}

FORCEDINLINE Optional<posix_error> link(
    Url const&             source,
    Url const&             destination,
    create_params_t const& params = {})
{
    auto src_resolved = *source;
    auto dst_resolved = *destination;
    if(auto status = symlink(src_resolved.c_str(), dst_resolved.c_str());
       status != 0)
        return detail::posix_failure().error();
    return std::nullopt;
}

namespace path {

FORCEDINLINE Optional<posix_error> change_dir(Url const& path)
{
    auto newdir = *path;
    if(auto res = chdir(newdir.c_str()); res != 0)
        return detail::posix_failure().error();
    return std::nullopt;
}

FORCEDINLINE result<Url, posix_error> canon(Url const& path)
{
    if(auto result = realpath(C_OCAST<const char*>(path), nullptr); !result)
        return detail::posix_failure();
    else
    {
        stl_types::String output(result);
        ::free(result);
        return success(url::constructors::MkUrl(output));
    }
}
FORCEDINLINE result<Url, posix_error> dir(Url const& path)
{
    auto resolved = *path;
    if(auto result = ::dirname(&resolved[0]); !result)
        return detail::posix_failure();
    else
        return success(url::constructors::MkUrl(result));
}
FORCEDINLINE result<Url, posix_error> base(Url const& path)
{
    auto resolved = *path;
    if(auto result = ::basename(&resolved[0]); !result)
        return detail::posix_failure();
    else
        return success(url::constructors::MkUrl(result));
}
FORCEDINLINE result<Url, posix_error> dereference(Url const& path)
{
    auto unresolved = path.internUrl.c_str();
    char probe_storage;
    if(auto size = readlink(unresolved, &probe_storage, 1); size < 0)
        return detail::posix_failure();
    else
    {
        stl_types::String output;
        output.resize(C_FCAST<unsigned>(size + 1), '\0');
        if(auto sz = readlink(unresolved, &output[0], C_FCAST<unsigned>(size));
           sz < 0)
            return detail::posix_failure();
        return success(url::constructors::MkUrl(output));
    }
}

FORCEDINLINE result<Url, posix_error> executable()
{
#if defined(COFFEE_LINUX)
    using url::constructors::MkSysUrl;

    return path::canon(MkSysUrl("/proc/self/exe"));
#elif defined(COFFEE_IOS)
    return url::constructors::MkUrl("App");
#elif defined(COFFEE_MACOS)
#error Unimplemented function
#else
    return url::constructors::MkInvalidUrl();
#endif
}

FORCEDINLINE result<Url, posix_error> app_dir()
{
    if(auto exec = executable(); exec.has_error())
        return failure(exec.error());
    else
        return path::dir(exec.value());
}

FORCEDINLINE result<Url, posix_error> current_dir()
{
    using url::constructors::MkSysUrl;

    if(auto res = get_current_dir_name(); !res)
        return detail::posix_failure();
    else
    {
        auto out = MkSysUrl(res);
        ::free(res);
        return success(out);
    }
}

} // namespace path
} // namespace platform::file::posix
