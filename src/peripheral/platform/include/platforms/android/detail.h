#pragma once

#include "../posix/detail.h"

#include <coffee/android/android_main.h>
#include <peripherals/error/result.h>

namespace platform::file::android {
using libc_types::szptr;

namespace detail {

using stl_types::failure;
using stl_types::result;
using stl_types::success;

struct android_fd_t
{
    AAsset*           asset;
    posix::posix_fd_t fd;
};

FORCEDINLINE bool is_asset(Url const& file, RSCA access)
{
    return (access & RSCA::AccessMask) == RSCA::ReadOnly &&
           (file.flags & RSCA::AssetFile) == RSCA::AssetFile;
}

FORCEDINLINE bool is_asset(android_fd_t const& fd)
{
    return fd.asset;
}

FORCEDINLINE auto* open_asset(Url const& file)
{
    return AAssetManager_open(
        ::android::activity_manager().asset_manager(),
        file.internUrl.c_str(),
        AASSET_MODE_BUFFER);
}

FORCEDINLINE std::optional<posix::posix_error> close_file(android_fd_t&& file)
{
    if(file.asset)
        AAsset_close(file.asset);
    else
        posix::detail::close_fd(file.fd);
    return std::nullopt;
}
} // namespace detail

FORCEDINLINE result<detail::android_fd_t, posix::posix_error> open_file(
    Url const& file, RSCA access, permissions_t perms = default_permissions)
{
    if(detail::is_asset(file, access))
    {
        if(auto asset = detail::open_asset(file))
            return detail::android_fd_t{
                .asset = asset,
            };
    }

    if(auto res = posix::open_file(file, access, std::move(perms));
       res.has_value())
        return detail::android_fd_t{
            .fd = std::move(res.value()),
        };
    else
        return res.error();
}

} // namespace platform::file::android
