#pragma once

#include "../posix/mmio.h"
#include <future>

namespace platform::file::emscripten {

struct mem_mapping_ex_t : posix::mem_mapping_t
{
    semantic::mem_chunk<char> alloc;
    int handle{-1};
};

std::future<posix::mem_mapping_t> mmap_async(Url const& file);

} // namespace platform::file::emscripten
