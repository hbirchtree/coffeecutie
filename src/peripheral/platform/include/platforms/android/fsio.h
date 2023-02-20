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
        return file_info_t
        {
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

FORCEDINLINE result<std::vector<file_entry_t>, posix::posix_error> list(
    Url const& dir)
{
    posix::posix_error earlier_error{0};

    std::vector<file_entry_t> files;
    if(auto result = posix::list(dir); result.has_value())
    {
        auto external_files = result.value();
        files.insert(
            files.begin(), external_files.begin(), external_files.end());
    } else
        earlier_error = result.error();

    auto       dir_name = *dir;
    AAssetDir* adir      = AAssetManager_openDir(
        ::android::activity_manager().asset_manager(), dir_name.c_str());
    if(!adir)
    {
        if(files.empty())
            return posix::posix_error{ENOENT};
        else if(earlier_error != 0)
            return earlier_error;
        else
            return files;
    }
    const char* fname = AAssetDir_getNextFileName(adir);
    while(fname)
    {
        files.push_back(file_entry_t{
            .mode = mode_t::file,
            .name = std::string(fname),
        });
        fname = AAssetDir_getNextFileName(adir);
    }
    AAssetDir_close(adir);
    return files;
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
