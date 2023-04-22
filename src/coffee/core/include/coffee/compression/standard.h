#pragma once

#include <peripherals/libc/types.h>

namespace compression {

enum class codec : libc_types::u32
{
    none,

    /* zlib/deflate codecs */
    deflate,
    deflate_ms,

    /* external codecs */
    zstd,
};

} // namespace compression
