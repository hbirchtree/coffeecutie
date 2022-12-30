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

namespace zlib {

using Opts = Compressor::Opts;

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
    semantic::Span<const libc_types::u8> const& input,
    semantic::mem_chunk<libc_types::u8>*        output,
    Opts const&                                 opts,
    zlib_error_code&                            ret)
{
    DProfContext _("zlib::Compression routine");

    bool direct_output = opts.prealloc_size > 0;

    if(direct_output && output->size < opts.prealloc_size)
        direct_output = false;

    Vector<byte_t> compress_store;
    compress_store.resize(direct_output ? opts.prealloc_size : opts.chunk_size);

    byte_t* write_ptr = direct_output ? output->data : compress_store.data();

    z_stream strm;
    strm.zalloc = nullptr;
    strm.zfree  = nullptr;
    strm.opaque = nullptr;

    strm.avail_out = C_FCAST<u32>(
        direct_output ? opts.prealloc_size : compress_store.size());
    strm.next_out = write_ptr;

    strm.avail_in = C_FCAST<u32>(input.size());
    strm.next_in  = C_CCAST<unsigned char*>(input.data());

    ret = Z_OK;

    if(InitD != nullptr)
        ret = deflateInit(&strm, opts.level);
    else
        ret = inflateInit(&strm);

    if(ret != Z_OK)
        return false;

    while(strm.avail_in > 0)
    {
        ret = Proc(&strm, Z_NO_FLUSH);

        if(ret != Z_OK && ret != Z_STREAM_END)
            return false;

        if(strm.avail_out == 0 && strm.avail_in > 0)
        {
            if(direct_output)
                Throw(undefined_behavior("Failed direct output"));

            auto comp_size = compress_store.size();
            compress_store.resize(comp_size + opts.chunk_size);
            write_ptr      = compress_store.data();
            strm.avail_out = C_FCAST<u32>(opts.chunk_size);
            strm.next_out  = &write_ptr[comp_size];
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
                if(direct_output)
                    Throw(undefined_behavior("Failed direct output"));

                auto comp_size = compress_store.size();
                compress_store.resize(comp_size + opts.chunk_size);
                write_ptr      = compress_store.data();
                strm.avail_out = C_FCAST<u32>(opts.chunk_size);
                strm.next_out  = &write_ptr[comp_size];
            }
        } while(ret != Z_STREAM_END);
    }

    ret = End(&strm);

    if(ret != Z_OK)
        return false;

    szptr out_size = direct_output ? 0 : compress_store.size() - strm.avail_out;
    compress_store.resize(out_size);

    if(!direct_output)
    {
        DProfContext _("zlib::Copy to output");
        MemCpy(
            *Bytes::ofContainer(compress_store).at(0, output->size), *output);
    }

    return true;
}

bool Compressor::Compress(
    semantic::Span<const libc_types::u8> const& uncompressed,
    semantic::mem_chunk<libc_types::u8>*        target,
    Opts const&                                 opts,
    zlib_error_code&                            ec)
{
    return compression_routine<
        nullptr,
        ::deflateInit_,
        ::deflate,
        ::deflateEnd>(uncompressed, target, opts, ec);
}

bool Compressor::Decompress(
    semantic::Span<const libc_types::u8> const& compressed,
    semantic::mem_chunk<libc_types::u8>*        target,
    Opts const&                                 opts,
    zlib_error_code&                            ec)
{
    return compression_routine<
        ::inflateInit_,
        nullptr,
        ::inflate,
        ::inflateEnd>(compressed, target, opts, ec);
}

} // namespace zlib
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
