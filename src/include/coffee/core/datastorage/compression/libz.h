#pragma once

#include "standard.h"

#if defined(COFFEE_BUILD_ZLIB) || defined(COFFEE_BUILD_WINDOWS_DEFLATE)

namespace Coffee{
namespace Compression{

/*!
 * \brief Compress and uncompress data using a system-provided codec
 *
 * On Unix/Linux, this is libz
 *
 * On Windows, this is the built-in Win32 compression
 *
 * Data compressed on Windows is not compatible with Unix/Linux
 */
struct LibZCompressor : Compressor_def
{
    struct Opts
    {
        Opts():
            chunk_size(1_MB),
            level(5)
        {
        }

        szptr chunk_size;
        int level;
    };

    static bool Compress(
            Bytes const& uncompressed, Bytes* target, Opts const& opts);

    static bool Decompress(
            Bytes const& compressed, Bytes* target, Opts const& opts);
};

}

using Zlib = Compression::LibZCompressor;

}

#endif
