#pragma once

#include "../posix/fsio.h"

namespace platform::file::emscripten {

std::optional<posix::posix_error> remove(Url const& file);

std::optional<posix::posix_error> truncate(posix::posix_fd_t const& file);

FORCEDINLINE std::optional<posix::posix_error> truncate(Url const& file)
{
    return posix::truncate(file);
}

} // namespace platform::file::emscripten
