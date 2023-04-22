#pragma once

#include "../posix/rdwrio.h"

#include "detail.h"

namespace platform::file::android {

FORCEDINLINE result<mem_chunk<char>, posix::posix_error> read(
    detail::android_fd_t const& file, read_params_t const& params = {})
{
    if(detail::is_asset(file))
    {
        auto out = mem_chunk<char>::withSize(params.size);
        if(auto res = AAsset_read(file.asset, out.data, out.size); res < 0)
            /* TODO: Translate the error */
            return failure(posix::posix_error{EACCES});
        else if(res == 0)
            return failure(posix::posix_error{EOF});
        else
            return success(std::move(out));
    } else
    {
        return posix::read(file.fd, params);
    }
}

FORCEDINLINE std::optional<posix::posix_error> write(
    detail::android_fd_t const&  file,
    mem_chunk<const char> const& data,
    write_params_t const&        params = {})
{
    if(detail::is_asset(file))
        return posix::posix_error{EACCES};
    else
        return posix::write(file.fd, data, params);
}

} // namespace platform::file::android
