#pragma once

#include "standard.h"

#if defined(COFFEE_BUILD_WINDOWS_DEFLATE)
#include <peripherals/error/windows.h>
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

#endif
#if defined(COFFEE_BUILD_WINDOWS_DEFLATE)

using deflate_error_code = platform::win32::error_code;

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
namespace zlib {

struct Compressor : Compressor_def
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
} // namespace zlib
#endif

#if defined(COFFEE_BUILD_WINDOWS_DEFLATE)
namespace deflate {

struct Compressor : Compressor_def
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

} // namespace deflate

#else

using error_code = Coffee::error_code;

#endif
} // namespace Compression

#if defined(COFFEE_BUILD_ZLIB)

using Zlib = Compression::zlib::Compressor;

#elif defined(COFFEE_BUILD_WINDOWS_DEFLATE)

using Zlib = Compression::deflate::Compressor;

#else

using Zlib = Compression::Compressor_def;

#endif

} // namespace Coffee
