#include <coffee/core/datastorage/compression/libz.h>

#if defined(COFFEE_BUILD_ZLIB)

#include <zlib.h>

namespace Coffee{
namespace Compression{

bool LibZCompressor::Compress(
        Bytes const& uncompressed, Bytes* target, Opts const& opts)
{
    Vector<byte_t> compress_store;
    compress_store.resize(opts.chunk_size);

    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    strm.avail_out = opts.chunk_size;
    strm.next_out = compress_store.data();

    strm.avail_in = uncompressed.size;
    strm.next_in = uncompressed.data;

    int ret = deflateInit(&strm, opts.level);

    if(ret != Z_OK)
        return false;

    while(strm.avail_in > 0)
    {
        auto comp_size = compress_store.size();
        ret = deflate(&strm, Z_NO_FLUSH);

        if(ret != Z_OK)
            return false;

        if(strm.avail_out == 0 && strm.avail_in > 0)
        {
            compress_store.resize(comp_size + opts.chunk_size);
            strm.avail_out = opts.chunk_size;
            strm.next_out = &compress_store[comp_size];
        }
    }

    do
    {
        ret = deflate(&strm, Z_FINISH);

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

    ret = deflateEnd(&strm);

    if(ret != Z_OK)
        return false;

    szptr out_size = compress_store.size() - strm.avail_out;
    target->data = C_FCAST<byte_t*>(Alloc(out_size));
    target->size = out_size;

    MemCpy(target->data, compress_store.data(), target->size);

    return true;
}

bool LibZCompressor::Decompress(
        Bytes const& compressed, Bytes* target, Opts const& opts)
{
    return true;
}

}
}
#endif
