#include <coffee/core/base.h>

#include <coffee/core/CProfiling>
#include <coffee/core/datastorage/compression/libz.h>
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

using Opts = ZlibCompressor::Opts;

using inflate_init_fun = int (*)(z_streamp, const char*, int);
using deflate_init_fun = int (*)(z_streamp, int, const char*, int);

using process_fun = int (*)(z_streamp, int);

using end_fun = int (*)(z_streamp);

template<
    inflate_init_fun InitI = nullptr,
    deflate_init_fun InitD = nullptr,
    process_fun      Proc  = nullptr,
    end_fun          End   = nullptr>
bool compression_routine(
    Bytes const& input, Bytes* output, Opts const& opts, zlib_error_code& ret)
{
    DProfContext _("zlib::Compression routine");

    Vector<byte_t> compress_store;
    compress_store.resize(opts.chunk_size);

    z_stream strm;
    strm.zalloc = nullptr;
    strm.zfree  = nullptr;
    strm.opaque = nullptr;

    strm.avail_out = C_FCAST<u32>(opts.chunk_size);
    strm.next_out  = compress_store.data();

    strm.avail_in = C_FCAST<u32>(input.size);
    strm.next_in  = input.data;

    ret = Z_OK;

    if(InitD != nullptr)
        ret = deflateInit(&strm, opts.level);
    else
        ret = inflateInit(&strm);

    if(ret != Z_OK)
        return false;

    while(strm.avail_in > 0)
    {
        auto comp_size = compress_store.size();
        ret            = Proc(&strm, Z_NO_FLUSH);

        if(ret != Z_OK && ret != Z_STREAM_END)
            return false;

        if(strm.avail_out == 0 && strm.avail_in > 0)
        {
            compress_store.resize(comp_size + opts.chunk_size);
            strm.avail_out = C_FCAST<u32>(opts.chunk_size);
            strm.next_out  = &compress_store[comp_size];
        }
    }

    if(InitD != nullptr)
    {
        do
        {
            ret = Proc(&strm, Z_FINISH);

            if(ret != Z_STREAM_END && ret != Z_OK)
                return false;

            if(ret != Z_STREAM_END && strm.avail_out == 0)
            {
                auto comp_size = compress_store.size();
                compress_store.resize(comp_size + opts.chunk_size);
                strm.avail_out = C_FCAST<u32>(opts.chunk_size);
                strm.next_out  = &compress_store[comp_size];
            }
        } while(ret != Z_STREAM_END);
    }

    ret = End(&strm);

    if(ret != Z_OK)
        return false;

    szptr out_size = compress_store.size() - strm.avail_out;

    (*output) = Bytes::Alloc(out_size);

    MemCpy(Bytes::CreateFrom(compress_store).at(0, output->size), *output);

    return true;
}

bool ZlibCompressor::Compress(
    Bytes const&     uncompressed,
    Bytes*           target,
    Opts const&      opts,
    zlib_error_code& ec)
{
    return compression_routine<nullptr, deflateInit_, deflate, deflateEnd>(
        uncompressed, target, opts, ec);
}

bool ZlibCompressor::Decompress(
    Bytes const&     compressed,
    Bytes*           target,
    Opts const&      opts,
    zlib_error_code& ec)
{
    return compression_routine<inflateInit_, nullptr, inflate, inflateEnd>(
        compressed, target, opts, ec);
}

} // namespace Compression
} // namespace Coffee
#elif defined(COFFEE_BUILD_WINDOWS_DEFLATE)

#include <coffee/core/plat/plat_windows.h>
#include <compressapi.h>

#include <coffee/core/CDebug>

namespace Coffee {
namespace Compression {

bool DeflateCompressor::Compress(
    Bytes const&        uncompressed,
    Bytes*              target,
    Opts const&         opts,
    deflate_error_code& ec)
{
    COMPRESSOR_HANDLE cHnd = nullptr;

    auto succ = ::CreateCompressor(COMPRESS_ALGORITHM_LZMS, nullptr, &cHnd);

    if(!succ)
    {
        ec = GetLastError();

        cWarning(
            "LibZCompressor::Failed to create"
            " compressor: {0}",
            ec.message());
        return false;
    }

    SIZE_T compSize = 0;

    succ = ::Compress(
        cHnd, uncompressed.data, uncompressed.size, nullptr, 0, &compSize);

    if(compSize == 0 || !succ)
    {
        ec = GetLastError();

        cWarning(
            "LibZCompressor::Failed to estimate"
            " compressed size: {0}",
            ec.message());
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

bool DeflateCompressor::Decompress(
    Bytes const&        compressed,
    Bytes*              target,
    Opts const&         opts,
    deflate_error_code& ec)
{
    DECOMPRESSOR_HANDLE cHnd = nullptr;

    auto succ = ::CreateDecompressor(COMPRESS_ALGORITHM_LZMS, nullptr, &cHnd);

    if(!succ)
    {
        ec = GetLastError();

        cWarning(
            "LibZCompressor::Failed to create decompressor: {0}", ec.message());
        return false;
    }

    SIZE_T compSize = 0;

    succ = ::Decompress(
        cHnd, compressed.data, compressed.size, nullptr, 0, &compSize);

    if(compSize == 0 || !succ)
    {
        cWarning(
            "LibZCompressor::Failed to estimate"
            " decompressed size: {0}",
            ec.message());
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

} // namespace Compression
} // namespace Coffee

#endif
