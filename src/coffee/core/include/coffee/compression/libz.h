#pragma once

#if defined(COFFEE_BUILD_ZLIB)
#endif

#if defined(COFFEE_BUILD_WINDOWS_DEFLATE)
#include <peripherals/error/windows.h>
#endif

#if defined(COFFEE_BUILD_WINDOWS_DEFLATE)
namespace deflate {

struct Compressor : Compressor_def
{
    struct Opts
    {
        Opts()
            : chunk_size(1_MB)
            , level(5)
        {
        }

        szptr chunk_size;
        int   level;
    };

    static bool Compress(
        BytesConst const&   uncompressed,
        Bytes*              target,
        Opts const&         opts,
        deflate_error_code& ec);

    static bool Decompress(
        BytesConst const&   compressed,
        Bytes*              target,
        Opts const&         opts,
        deflate_error_code& ec);
};

using error_code = deflate_error_code;

} // namespace deflate
#endif
