#pragma once

#include "../posix/fsio.h"
#include "detail.h"

namespace platform::file::android {

FORCEDINLINE result<file_info_t, posix::posix_error> file_info(
    detail::android_fd_t const& file)
{
    if(detail::is_asset(file))
    {
        using libc_types::u64;
        return file_info_t{
#if defined(COFFEE_ARCH_ARM64)
            .size = static_cast<szptr>(AAsset_getLength64(file.asset)),
#else
            .size = static_cast<szptr>(AAsset_getLength(file.asset)),
#endif
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
    auto info = file_info(fd.value());
    if(detail::is_asset(fd.value()))
        AAsset_close(fd.value().asset);
    return info;
}

template<typename T>
requires(std::is_same_v<T, Url> || std::is_same_v<T, detail::android_fd_t>)
    FORCEDINLINE result<mode_t, posix::posix_error> exists(T const& file)
{
    if(auto info = file_info(file); info.has_error())
        return posix::posix_error{info.error()};
    else
        return info.value().mode;
}

template<typename T>
requires(std::is_same_v<T, Url> || std::is_same_v<T, detail::android_fd_t>)
    FORCEDINLINE result<szptr, posix::posix_error> size(T const& file)
{
    if(auto info = file_info(file); info.has_error())
        return failure(posix::posix_error{info.error()});
    else
        return success(info.value().size);
}

FORCEDINLINE result<stl_types::Vector<file_entry_t>, posix::posix_error> list(
    Url const& dir)
{
    if(detail::is_asset(dir, RSCA::ReadOnly))
    {
        auto       dir_name = *dir;
        AAssetDir* dir      = AAssetManager_openDir(
            ::android::activity_manager().asset_manager(), dir_name.c_str());
        if(!dir)
            return posix::posix_error{ENOENT};
        const char*                     fname = AAssetDir_getNextFileName(dir);
        stl_types::Vector<file_entry_t> files;
        while(!fname)
        {
            files.push_back(file_entry_t{
                .mode = mode_t::file,
                .name = std::string(fname),
            });
            fname = AAssetDir_getNextFileName(dir);
        }
        AAssetDir_close(dir);
        return files;
    } else
        return posix::list(dir);
}

FORCEDINLINE Optional<posix::posix_error> truncate(Url const& file)
{
    if(detail::is_asset(file, RSCA::ReadOnly))
        return posix::posix_error{EACCES};
    else
        return posix::truncate(file);
}

FORCEDINLINE Optional<posix::posix_error> truncate(
    detail::android_fd_t const& file)
{
    if(detail::is_asset(file))
        return posix::posix_error{EACCES};
    else
        return posix::truncate(file.fd);
}

} // namespace platform::file::android
