#pragma once

#include <peripherals/semantic/handle.h>
#include <platforms/base/rdwrio.h>
#include <url/url.h>

#include <stdio.h>

namespace platform::file::libc {
using url::Url;

namespace detail {

FORCEDINLINE const char* access_to_native(RSCA access)
{
    using namespace enum_helpers;
    bool create   = feval(access, RSCA::NewFile);
    bool truncate = feval(access, RSCA::Discard);

    if(feval(access, RSCA::ReadWrite | RSCA::Append))
        return "ab+";
    else if(feval(access, RSCA::WriteOnly | RSCA::Append))
        return "ab";
    else if(feval(access, RSCA::ReadWrite))
        return (!create && !truncate) ? "rb+" : "wb+";
    else if(feval(access, RSCA::WriteOnly))
        return "wb";
    else if(feval(access, RSCA::ReadOnly))
        return "rb";
    Throw(undefined_behavior("no access specified"));
}

FORCEDINLINE void close_file(FILE* file)
{
    fclose(file);
}

} // namespace detail

using file_t = semantic::generic_handle_t<
    FILE*,
    semantic::handle_modes::auto_close,
    nullptr,
    detail::close_file>;

FORCEDINLINE result<file_t, int> open_file(
    Url const& file, RSCA access = RSCA::ReadOnly)
{
    auto filename = *file;
    auto mode     = detail::access_to_native(access);
    if(auto fd = fopen(filename.c_str(), mode); !fd)
        return failure(fd ? ferror(fd) : 0);
    else
        return success(fd);
}

FORCEDINLINE result<szptr, int> size(file_t const& file)
{
    if(fseek(file, 0, SEEK_END) != 0)
        return failure(ferror(file));

    if(auto size = ftell(file); size == -1)
        return failure(ferror(file));
    else
    {
        if(fseek(file, 0, SEEK_SET) != 0)
            return failure(ferror(file));
        return success(size);
    }
}

} // namespace platform::file::libc
