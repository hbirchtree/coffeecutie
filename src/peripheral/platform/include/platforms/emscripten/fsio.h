#pragma once

#include "../posix/fsio.h"

namespace platform::file::emscripten {

std::optional<posix::posix_error> remove(Url const& file);

std::optional<posix::posix_error> truncate(posix::posix_fd_t const& file);

FORCEDINLINE std::optional<posix::posix_error> truncate(Url const& file)
{
    return posix::truncate(file);
}

/*!
 * \brief Creates a directory on the persistent (OPFS) backend, so its
 * files survive the page. The backend is created on first call, which
 * must not be on the main thread's event loop: a short-lived thread does
 * it. False without OPFS, or if the directory could not be made
 */
bool persistent_mkdir(const char* path);

} // namespace platform::file::emscripten
