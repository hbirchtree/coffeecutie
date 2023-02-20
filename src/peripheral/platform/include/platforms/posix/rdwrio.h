#pragma once

#include "detail.h"
#include "fsio.h"

#include <url/url.h>

#include <sys/stat.h>

namespace platform::file::posix {
using url::Url;

namespace detail {

struct scoped_offset
{
    scoped_offset(posix_fd_t const& file, ptroff offset) : file(file)
    {
        detail::lseek_(file, offset, SEEK_SET);
    }
    ~scoped_offset()
    {
        detail::lseek_(file, 0, SEEK_SET);
    }
    posix_fd_t const& file;
};

} // namespace detail

FORCEDINLINE result<mem_chunk<char>, posix_error> read(
    posix_fd_t const& file, read_params_t const& params = {})
{
    auto [size, offset] = params;

    if(size == 0)
    {
        if(auto info = file_info(file); info.has_value())
            size = info.value().size;
        else
            return failure(info.error());
    }

    auto  output = mem_chunk<char>::withSize(size);
    auto& view   = output.view;

    detail::scoped_offset _(file, offset);

    constexpr int read_max = std::numeric_limits<libc_types::i32>::max();
    ssize_t       chunk = 0, i = 0, ssize = static_cast<ssize_t>(size);
    while(i < ssize)
    {
        chunk = std::min<ssize_t>(read_max, ssize - i);
        if(auto read_count = ::read(file, view.data() + i, chunk);
           read_count < 0)
            return failure(common::posix::get_error());
        else if(read_count < chunk)
        {
            /* Special case when file size is unknown */
            output.resize(read_count);
            break;
        } else
            i += C_CAST<szptr>(read_count);
    }

    return success(std::move(output));
}

FORCEDINLINE Optional<posix_error> write(
    posix_fd_t const&                 file,
    semantic::Span<const char> const& data,
    write_params_t const&             params = {})
{
    auto size = data.size();

    detail::scoped_offset _(file, params.offset);

    constexpr int write_max = std::numeric_limits<libc_types::i32>::max();
    szptr         i         = 0;
    szptr         chunk     = 0;
    while(i < size)
    {
        chunk = std::min<szptr>(write_max, size - i);
        if(auto write_count = ::write(file, data.data() + i, chunk);
           write_count < 0)
        {
            return std::make_optional(common::posix::get_error());
        } else
            i += C_CAST<szptr>(write_count);
    }

    return std::nullopt;
}

} // namespace platform::file::posix
