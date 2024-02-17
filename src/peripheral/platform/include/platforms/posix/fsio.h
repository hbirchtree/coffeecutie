#pragma once

#include "detail.h"

#include <libgen.h>

#if defined(COFFEE_MACOS)
#include <libproc.h>
#endif
#if defined(COFFEE_MINGW64)
#include <filesystem>
#endif

namespace platform::file::posix {

FORCEDINLINE result<file_info_t, posix_error> file_info(posix_fd_t const& file)
{
    struct stat file_ = {};
    if(fstat(file, &file_) != 0)
    {
        return detail::posix_failure();
    }
    using namespace permission_t;
    return success(file_info_t{
        .size = C_CAST<szptr>(file_.st_size),
        .mode = detail::mode_from_native(file_.st_mode),
        .perms =
            {
                .owner = (file_.st_mode & S_IRUSR ? read : none) |
                         (file_.st_mode & S_IWUSR ? write : none) |
                         (file_.st_mode & S_IXUSR ? execute : none),
                .group = (file_.st_mode & S_IRGRP ? read : none) |
                         (file_.st_mode & S_IWGRP ? write : none) |
                         (file_.st_mode & S_IXGRP ? execute : none),
                .other = (file_.st_mode & S_IROTH ? read : none) |
                         (file_.st_mode & S_IWOTH ? write : none) |
                         (file_.st_mode & S_IXOTH ? execute : none),
            },
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
requires(std::is_same_v<T, Url> || std::is_same_v<T, posix_fd_t>)
//
FORCEDINLINE result<mode_t, posix_error> exists(T const& file)
{
    if(auto info = file_info(file); info.has_error())
        return failure(info.error());
    else
        return success(info.value().mode);
}

template<typename T>
requires(std::is_same_v<T, Url> || std::is_same_v<T, posix_fd_t>)
//
FORCEDINLINE result<szptr, posix_error> size(T const& file)
{
    if(auto info = file_info(file); info.has_error())
        return failure(info.error());
    else
        return success(info.value().size);
}

FORCEDINLINE result<std::vector<file_entry_t>, posix_error> list(Url const& dir)
{
    auto resolved  = *dir;
    auto directory = opendir(resolved.c_str());
    if(!directory)
        return detail::posix_failure();
    std::vector<file_entry_t> result;
    while(auto direntry = readdir(directory))
    {
        if(auto name = std::string(direntry->d_name);
           name == "." || name == "..")
            continue;
        result.push_back(file_entry_t {
#if defined(COFFEE_MINGW64)
            .mode = mode_t::file,
#else
            .mode = detail::dirmode_from_native(direntry->d_type),
#endif
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

FORCEDINLINE std::optional<posix_error> create_directory(
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
#if defined(COFFEE_MINGW64)
            if(auto status = mkdir(current_path.c_str()); status != 0)
#else
            if(auto status = mkdir(current_path.c_str(), perm); status != 0)
#endif
                return detail::posix_failure().error();
            it = std::find(it, resolved.end(), '/');
        }
    }
#if defined(COFFEE_MINGW64)
    if(auto status = mkdir(resolved.c_str()); status != 0)
#else
    if(auto status = mkdir(resolved.c_str(), perm); status != 0)
#endif
        return detail::posix_failure().error();
    return std::nullopt;
}

FORCEDINLINE std::optional<posix_error> create(
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

FORCEDINLINE std::optional<posix_error> remove(Url const& file)
{
    auto resolved = *file;
    if(auto status = ::remove(resolved.c_str()); status != 0)
        return detail::posix_failure().error();
    return std::nullopt;
}

FORCEDINLINE std::optional<posix_error> truncate(Url const& file)
{
    auto resolved = *file;
    if(auto status = ::truncate(resolved.c_str(), 0); status != 0)
        return detail::posix_failure().error();
    return std::nullopt;
}

FORCEDINLINE std::optional<posix_error> truncate(posix_fd_t const& file)
{
    if(auto status = ::ftruncate(file, 0); status != 0)
        return detail::posix_failure().error();
    return std::nullopt;
}

FORCEDINLINE std::optional<posix_error> link(
    Url const& source, Url const& destination, create_params_t const& = {})
{
#if defined(COFFEE_MINGW64)
    return EPERM;
#else
    auto src_resolved = *source;
    auto dst_resolved = *destination;
    if(auto status = symlink(src_resolved.c_str(), dst_resolved.c_str());
       status != 0)
        return detail::posix_failure().error();
    return std::nullopt;
#endif
}

namespace path {

FORCEDINLINE std::optional<posix_error> change_dir(Url const& path)
{
    auto newdir = *path;
    if(auto res = chdir(newdir.c_str()); res != 0)
        return detail::posix_failure().error();
    return std::nullopt;
}

FORCEDINLINE result<Url, posix_error> canon(Url const& path)
{
#if defined(COFFEE_MINGW64)
    auto            rel = std::filesystem::path(*path);
    std::error_code ec;
    auto            abs = std::filesystem::absolute(rel, ec);
    if(ec)
        return posix::posix_error{ENOENT};
    return url::constructors::MkUrl(abs.string());
#else
    if(auto result = realpath(C_OCAST<const char*>(path), nullptr); !result)
        return detail::posix_failure();
    else
    {
        std::string output(result);
        ::free(result);
        return success(url::constructors::MkUrl(output));
    }
#endif
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
#if defined(COFFEE_MINGW64)
    return path;
#else
    auto        unresolved = path.internUrl.c_str();
    struct stat link_info  = {};
    if(auto status = lstat(unresolved, &link_info); status == 0)
        return detail::posix_failure();
    std::string output(link_info.st_size, 0);
    if(auto size = readlink(unresolved, output.data(), output.size()); size < 0)
        return detail::posix_failure();
    else
        return success(url::constructors::MkUrl(output));
#endif
}

#if !defined(COFFEE_MINGW64)
FORCEDINLINE result<Url, posix_error> executable()
{
    using namespace url::constructors;
#if defined(COFFEE_LINUX) || defined(COFFEE_ANDROID)
    return path::canon(MkSysUrl("/proc/self/exe"));
#elif defined(COFFEE_IOS)
    return path::canon(MkUrl("App"));
#elif defined(COFFEE_MACOS)
    std::string path(PATH_MAX, '\0');
    auto        res =
        proc_pidpath(getpid(), path.data(), static_cast<int>(path.size()));
    return MkSysUrl("");
#elif defined(COFFEE_EMSCRIPTEN)
    return MkSysUrl("/app");
#else
    static_assert(false, "Unimplemented executable()");
#endif
}

FORCEDINLINE result<Url, posix_error> app_dir()
{
    if(auto exec = executable(); exec.has_error())
        return failure(exec.error());
    else
        return path::dir(exec.value());
}
#endif

FORCEDINLINE result<Url, posix_error> current_dir()
{
    using url::constructors::MkSysUrl;

#if defined(COFFEE_LINUX)
    if(auto res = get_current_dir_name(); !res)
#else
    if(auto res = getcwd(nullptr, 0); !res)
#endif
        return detail::posix_failure();
    else
    {
        auto out = MkSysUrl(res);
        ::free(res);
        return success(out);
    }
}

FORCEDINLINE result<mode_t, posix_error> exists(Url const& path)
{
#if defined(COFFEE_LINUX)
    /* O_PATH is only on Linux */
    if(auto dir = open(path.internUrl.c_str(), O_DIRECTORY | O_PATH); dir != -1)
    {
        close(dir);
        return success(mode_t::directory);
    }
#else
    struct stat dirstat
    {
    };

    if(auto res = stat(path.internUrl.c_str(), &dirstat);
       res == 0 && S_ISDIR(dirstat.st_mode))
        return mode_t::directory;
#endif
    else
        return detail::posix_failure();
}

} // namespace path
} // namespace platform::file::posix
