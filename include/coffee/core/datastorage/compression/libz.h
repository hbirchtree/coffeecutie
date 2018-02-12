#pragma once

#include "standard.h"

#if defined(COFFEE_BUILD_ZLIB) || defined(COFFEE_BUILD_WINDOWS_DEFLATE)

namespace Coffee{
namespace Compression{

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
