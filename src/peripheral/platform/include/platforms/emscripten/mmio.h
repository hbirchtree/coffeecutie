#pragma once

#include "../posix/mmio.h"
#include <future>
#include <string>

namespace platform::file::emscripten {

struct mem_mapping_ex_t : posix::mem_mapping_t
{
    semantic::mem_chunk<char> alloc;
    int                       handle{-1};
};

std::future<posix::mem_mapping_t> mmap_async(Url const& file);

/*!
 * \brief Marks URLs starting with prefix as offline-only: mmap_async() loads
 * them from the Emscripten Fetch IndexedDB cache (emscripten_filesystem ->
 * FILES) and fails on a miss instead of falling back to a network request.
 * Other URLs keep using the cache first and the network second.
 */
void add_offline_prefix(std::string prefix);

bool is_offline_url(std::string_view url);

} // namespace platform::file::emscripten
