#pragma once

#include <coffee/core/types/chunk.h>
#include <peripherals/libc/types.h>

namespace Coffee {
/*!
 * \brief Wrappers for compression libraries
 */
namespace Compression {

struct Compressor_def
{
    struct Opts
    {
    };

    static bool Compress(Bytes const&, Bytes*, Opts const&)
    {
        return false;
    }
    static bool Decompress(Bytes const&, Bytes*, Opts const&)
    {
        return false;
    }
};

} // namespace Compression
} // namespace Coffee

namespace compression {

using compressor_base = Coffee::Compression::Compressor_def;

enum class codec : libc_types::u32
{
    none,

    /* zlib/deflate codecs */
    deflate,
    deflate_ms,

    /* external codecs */
    lz4,
    zstd,
};

} // namespace compression
