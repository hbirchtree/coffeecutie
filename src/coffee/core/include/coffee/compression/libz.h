#pragma once

#include "standard.h"

#if defined(COFFEE_BUILD_WINDOWS_DEFLATE)
#include <coffee/core/plat/plat_windows_errors.h>
#endif

namespace Coffee {
namespace Compression {

#if defined(COFFEE_BUILD_ZLIB)

struct zlib_error_category : error_category
{
    virtual const char* name() const noexcept override;
    virtual std::string message(int) const override;
};

using zlib_error_code = domain_error_code<int, zlib_error_category>;

#elif defined(COFFEE_BUILD_WINDOWS_DEFLATE)

using deflate_error_code = Win32::win32_error_code;

#endif

/*!
 * \brief Compress and uncompress data using a system-provided codec
 *
 * On Unix/Linux, this is libz
 *
 * On Windows, this is the built-in Win32 compression
 *
 * Data compressed on Windows is not compatible with Unix/Linux
 */
#if defined(COFFEE_BUILD_ZLIB)
struct ZlibCompressor : Compressor_def
{
    struct Opts
    {
        Opts(szptr chunk_size = 1_MB, int level = 5, szptr prealloc_size = 0) :
            chunk_size(chunk_size), level(level), prealloc_size(prealloc_size)
        {
        }

        szptr chunk_size;
        int   level;
        szptr prealloc_size;
    };

    static bool Compress(
        Bytes const&     uncompressed,
        Bytes*           target,
        Opts const&      opts,
        zlib_error_code& ec);

    static bool Decompress(
        Bytes const&     compressed,
        Bytes*           target,
        Opts const&      opts,
        zlib_error_code& ec);
};

using error_code = zlib_error_code;

#elif defined(COFFEE_BUILD_WINDOWS_DEFLATE)
struct DeflateCompressor : Compressor_def
{
    struct Opts
    {
        Opts() : chunk_size(1_MB), level(5)
        {
        }

        szptr chunk_size;
        int   level;
    };

    static bool Compress(
        Bytes const&        uncompressed,
        Bytes*              target,
        Opts const&         opts,
        deflate_error_code& ec);

    static bool Decompress(
        Bytes const&        compressed,
        Bytes*              target,
        Opts const&         opts,
        deflate_error_code& ec);
};

using error_code = deflate_error_code;

#else

using error_code = Coffee::error_code;

#endif
} // namespace Compression

#if defined(COFFEE_BUILD_ZLIB)

using Zlib = Compression::ZlibCompressor;

#elif defined(COFFEE_BUILD_WINDOWS_DEFLATE)

using Zlib = Compression::DeflateCompressor;

#else

using Zlib = Compression::Compressor_def;

#endif

} // namespace Coffee
