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
#endif
