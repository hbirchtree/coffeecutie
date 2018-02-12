#include <coffee/core/datastorage/compression/libz.h>

#if defined(COFFEE_BUILD_ZLIB)

#include <zlib.h>

namespace Coffee{
namespace Compression{

using Opts = LibZCompressor::Opts;

using inflate_init_fun = int(*)(z_streamp, const char*, int);
using deflate_init_fun = int(*)(z_streamp, int, const char*, int);

using process_fun = int(*)(z_streamp, int);

using end_fun = int(*)(z_streamp);

template<inflate_init_fun InitI = nullptr,
         deflate_init_fun InitD = nullptr,
         process_fun Proc = nullptr,
         end_fun End = nullptr>
bool compression_routine(Bytes const& input, Bytes* output, Opts const& opts)
{
    Vector<byte_t> compress_store;
    compress_store.resize(opts.chunk_size);

    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    strm.avail_out = opts.chunk_size;
    strm.next_out = compress_store.data();

    strm.avail_in = input.size;
    strm.next_in = input.data;

    int ret = Z_OK;

    if(InitD == nullptr)
        ret = InitD(&strm, opts.level, ZLIB_VERSION,
                    C_FCAST<int>(sizeof(z_stream)));
    else
        ret = InitI(&strm, ZLIB_VERSION,
                    C_FCAST<int>(sizeof(z_stream)));

    if(ret != Z_OK)
        return false;

    while(strm.avail_in > 0)
    {
        auto comp_size = compress_store.size();
        ret = Proc(&strm, Z_NO_FLUSH);

        if(ret != Z_OK)
            return false;

        if(strm.avail_out == 0 && strm.avail_in > 0)
        {
            compress_store.resize(comp_size + opts.chunk_size);
            strm.avail_out = opts.chunk_size;
            strm.next_out = &compress_store[comp_size];
        }
    }

    if(InitD == nullptr)
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
                strm.avail_out = opts.chunk_size;
                strm.next_out = &compress_store[comp_size];
            }
        } while(ret != Z_STREAM_END);
    }

    ret = End(&strm);

    if(ret != Z_OK)
        return false;

    szptr out_size = compress_store.size() - strm.avail_out;

    output->data = C_FCAST<byte_t*>(Alloc(out_size));
    output->size = out_size;
    Bytes::SetDestr(*output, [](Bytes& d)
    {
        CFree(d.data);
    });

    MemCpy(output->data, compress_store.data(), output->size);

    return true;
}

bool LibZCompressor::Compress(
        Bytes const& uncompressed, Bytes* target, Opts const& opts)
{
    return compression_routine<
            nullptr, deflateInit_, deflate, deflateEnd>
            (uncompressed, target, opts);
}

bool LibZCompressor::Decompress(
        Bytes const& compressed, Bytes* target, Opts const& opts)
{
    return compression_routine<
            inflateInit_, nullptr, inflate, inflateEnd>
            (compressed, target, opts);
}

}
}
#elif defined(COFFEE_BUILD_WINDOWS_DEFLATE)

#include <coffee/core/plat/plat_windows.h>
#include <compressapi.h>
#include <coffee/core/CDebug>

namespace Coffee{
namespace Compression{

bool LibZCompressor::Compress(
        Bytes const& uncompressed, Bytes* target, Opts const& opts)
{
    COMPRESSOR_HANDLE cHnd = nullptr;

    auto succ = ::CreateCompressor(
            COMPRESS_ALGORITHM_LZMS,
            nullptr, &cHnd);

    if(!succ)
    {
        cWarning("LibZCompressor::Failed to create"
                 " compressor: {0}",
                 win_strerror(GetLastError()));
        return false;
    }

    SIZE_T compSize = 0;

    succ = ::Compress(
        cHnd,
        uncompressed.data, uncompressed.size,
        nullptr, 0, &compSize);

    if(compSize == 0 || !succ)
    {
        cWarning("LibZCompressor::Failed to estimate"
                 " compressed size: {0}",
                 win_strerror(GetLastError()));
    }

    target->data = C_RCAST<byte_t*>(Alloc(compSize));
    target->size = compSize;
    Bytes::SetDestr(*target, [](Bytes& b)
    {
        CFree(b.data);
    });

    succ = ::Compress(
            cHnd,
            uncompressed.data, uncompressed.size,
            target->data, target->size,
            &compSize);

    ::CloseCompressor(cHnd);

    return true;
}

bool LibZCompressor::Decompress(
        Bytes const& compressed, Bytes* target, Opts const& opts)
{
    DECOMPRESSOR_HANDLE cHnd = nullptr;

    auto succ = ::CreateDecompressor(
            COMPRESS_ALGORITHM_LZMS,
            nullptr, &cHnd);

    if(!succ)
    {
        cWarning("LibZCompressor::Failed to create decompressor: {0}",
                 win_strerror(GetLastError()));
        return false;
    }

    SIZE_T compSize = 0;

    succ = ::Decompress(
            cHnd,
            compressed.data, compressed.size,
            nullptr, 0,
            &compSize);

    if(compSize == 0 || !succ)
    {
        cWarning("LibZCompressor::Failed to estimate"
                 " decompressed size: {0}",
                 win_strerror(GetLastError()));
        return false;
    }

    target->data = C_RCAST<byte_t*>(Alloc(compSize));
    target->size = compSize;
    Bytes::SetDestr(*target, [](Bytes& b)
    {
        CFree(b.data);
    });

    succ = ::Decompress(
            cHnd,
            compressed.data, compressed.size,
            target->data, target->size,
            &compSize);

    ::CloseDecompressor(cHnd);

    return true;
}

}
}

#endif
