#pragma once

#include "../posix/mmio.h"
#include <cstdint>
#include <future>
#include <string>

namespace platform::file::emscripten {

struct mem_mapping_ex_t : posix::mem_mapping_t
{
    semantic::mem_chunk<char> alloc;
    int                       handle{-1};
};

std::future<posix::mem_mapping_t> mmap_async(Url const& file);

/* A fetched file is owned by Fetch, not an mmap */
std::optional<posix::posix_error> unmap(posix::mem_mapping_t&& mapping);

struct range_probe_t
{
    int           status{0}; /*!< 206 = ranges served, 200 = whole file only */
    std::uint64_t size{0};
    /*! On 200, the whole file the server sent, owned by Fetch like
     * mmap_async's */
    std::optional<posix::mem_mapping_t> whole;
};

/*! Size of a URL by a one-byte Range request. Synchronous: not on the main
 * thread */
range_probe_t fetch_probe(std::string const& url);

/*! Fills dst from offset with one Range request. Synchronous: not on the
 * main thread. Fails if the server answers without a partial content */
bool fetch_range(
    std::string const& url, std::uint64_t offset, gsl::span<char> dst);

/*!
 * \brief Marks URLs starting with prefix as offline-only: mmap_async() loads
 * them from the Emscripten Fetch IndexedDB cache (emscripten_filesystem ->
 * FILES) and fails on a miss instead of falling back to a network request.
 * Other URLs keep using the cache first and the network second.
 */
void add_offline_prefix(std::string prefix);

bool is_offline_url(std::string_view url);

} // namespace platform::file::emscripten
