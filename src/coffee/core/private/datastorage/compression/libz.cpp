#include <coffee/core/base.h>

#include <coffee/compression/libz.h>
#include <coffee/core/CProfiling>
#include <platforms/profiling.h>

#if defined(COFFEE_BUILD_ZLIB)

#include <zlib.h>

namespace Coffee {
namespace Compression {

const char* zlib_error_category::name() const noexcept
{
    return "zlib_error_category";
}

#define ZLIB_ERROR_MSG(code) \
    case code:               \
        return C_STR(code);

std::string zlib_error_category::message(int error_code) const
{
    switch(error_code)
    {
        ZLIB_ERROR_MSG(Z_STREAM_END);
        ZLIB_ERROR_MSG(Z_NEED_DICT);
        ZLIB_ERROR_MSG(Z_STREAM_ERROR);
        ZLIB_ERROR_MSG(Z_DATA_ERROR);
        ZLIB_ERROR_MSG(Z_MEM_ERROR);
        ZLIB_ERROR_MSG(Z_BUF_ERROR);
        ZLIB_ERROR_MSG(Z_VERSION_ERROR);

    case Z_ERRNO:
        return std::string("Z_ERRNO: ") + strerror(errno);

    default:
        return "No error";
    }
}


} // namespace Compression
} // namespace Coffee

#endif
#if defined(COFFEE_BUILD_WINDOWS_DEFLATE)

#include <compressapi.h>
#include <peripherals/platform/windows.h>

namespace Coffee {
namespace Compression {
namespace deflate {

bool Compressor::Compress(
    BytesConst const&   uncompressed,
    Bytes*              target,
    Opts const&         opts,
    deflate_error_code& ec)
{
    COMPRESSOR_HANDLE cHnd = nullptr;

    auto succ = ::CreateCompressor(COMPRESS_ALGORITHM_LZMS, nullptr, &cHnd);

    if(!succ)
    {
        ec = GetLastError();
        return false;
    }

    SIZE_T compSize = 0;

    succ = ::Compress(
        cHnd, uncompressed.data, uncompressed.size, nullptr, 0, &compSize);

    if(compSize == 0 || !succ)
    {
        ec = GetLastError();
        return false;
    }

    *target = Bytes::Alloc(compSize);

    succ = ::Compress(
        cHnd,
        uncompressed.data,
        uncompressed.size,
        target->data,
        target->size,
        &compSize);

    ::CloseCompressor(cHnd);

    return true;
}

bool Compressor::Decompress(
    BytesConst const&   compressed,
    Bytes*              target,
    Opts const&         opts,
    deflate_error_code& ec)
{
    DECOMPRESSOR_HANDLE cHnd = nullptr;

    auto succ = ::CreateDecompressor(COMPRESS_ALGORITHM_LZMS, nullptr, &cHnd);

    if(!succ)
    {
        ec = GetLastError();
        return false;
    }

    SIZE_T compSize = 0;

    succ = ::Decompress(
        cHnd, compressed.data, compressed.size, nullptr, 0, &compSize);

    if(compSize == 0 || !succ)
    {
        ec = GetLastError();
        return false;
    }

    *target = Bytes::Alloc(compSize);

    succ = ::Decompress(
        cHnd,
        compressed.data,
        compressed.size,
        target->data,
        target->size,
        &compSize);

    ::CloseDecompressor(cHnd);

    return true;
}

} // namespace deflate
} // namespace Compression
} // namespace Coffee

#endif
