#include <coffee/compression/lz4.h>

#include <coffee/core/base.h>
#include <lz4hc.h>

namespace lz4 {

const char* lz4_error_category::name() const noexcept
{
    return "lz4_error_code";
}

std::string lz4_error_category::message(int error) const
{
    switch(C_CAST<lz4_error>(error))
    {
    case lz4_error::exceeds_file_size_limit:
        return "exceeds_file_size_limit";
    case lz4_error::compression_failed:
        return "compression_failed";
    case lz4_error::not_enough_data:
        return "not_enough_data";
    case lz4_error::malformed_header:
        return "malformed_header";
    case lz4_error::decompression_mismatch_size:
        return "decompression_mismatch_size";
    case lz4_error::decompression_failed:
        return "decompression_failed";
    default:
        return "no_error";
    }
}

struct alignas(8) chunk_header
{
    static constexpr libc_types::u32 header_magic = 0x4444BEEF;

    libc_types::u32 magic     = header_magic;
    libc_types::u32 real_size = 0;
};

bool compressor::Compress(
    semantic::Bytes const&  uncompressed,
    semantic::Bytes*        target,
    compressor::opts const& opts,
    compressor::error_code& ec)
{
    if(uncompressed.size > std::numeric_limits<int>::max())
    {
        ec = lz4_error::exceeds_file_size_limit;
        return false;
    }

    semantic::Bytes state = semantic::Bytes::Alloc(
        opts.mode == compression_mode::high ? LZ4_sizeofStateHC()
                                            : LZ4_sizeofState());

    int approx_size =
        LZ4_compressBound(C_FCAST<libc_types::i32>(uncompressed.size));

    *target = semantic::Bytes::Alloc(
        C_FCAST<libc_types::u32>(approx_size) + sizeof(chunk_header));

    chunk_header& header = *C_RCAST<chunk_header*>(target->data);

    header.magic     = chunk_header::header_magic;
    header.real_size = C_FCAST<libc_types::u32>(uncompressed.size);

    auto contentChunk = target->at(sizeof(chunk_header));

    int result;

    switch(opts.mode)
    {
    case compression_mode::fast:
    case compression_mode::default_:
    {
        result = LZ4_compress_fast_extState(
            state.data,
            C_RCAST<const char*>(uncompressed.data),
            C_RCAST<char*>(contentChunk.data),
            C_FCAST<int>(uncompressed.size),
            C_FCAST<int>(contentChunk.size),
            opts.mode == compression_mode::default_ ? 0
                                                    : opts.fast_acceleration);
        break;
    }
    case compression_mode::high:
    {
        result = LZ4_compress_HC_extStateHC(
            state.data,
            C_RCAST<const char*>(uncompressed.data),
            C_RCAST<char*>(target->data + sizeof(chunk_header)),
            C_FCAST<int>(uncompressed.size),
            C_FCAST<int>(target->size - sizeof(chunk_header)),
            opts.high_compression);
        break;
    }
    }

    if(result > 0)
    {
        target->size = C_FCAST<libc_types::u32>(result) + sizeof(chunk_header);
        target->elements = target->size;
        return true;
    } else
    {
        ec = lz4_error::compression_failed;
        return false;
    }
}

bool compressor::Decompress(
    semantic::Bytes const& compressed,
    semantic::Bytes*       target,
    compressor::opts const&,
    compressor::error_code& ec)
{
    if(compressed.size < sizeof(chunk_header))
    {
        ec = lz4_error::not_enough_data;
        return false;
    }

    if(compressed.size > std::numeric_limits<int>::max())
    {
        ec = lz4_error::exceeds_file_size_limit;
        return false;
    }

    chunk_header& header = *C_RCAST<chunk_header*>(compressed.data);

    if(header.magic != chunk_header::header_magic)
    {
        ec = lz4_error::malformed_header;
        return false;
    }

    if(header.real_size > std::numeric_limits<int>::max())
    {
        ec = lz4_error::exceeds_file_size_limit;
        return false;
    }

    *target = semantic::Bytes::Alloc(header.real_size);

    auto contentChunk = compressed.at(sizeof(chunk_header));

    int result = LZ4_decompress_safe(
        C_RCAST<const char*>(compressed.data + sizeof(chunk_header)),
        C_RCAST<char*>(target->data),
        C_FCAST<int>(compressed.size - sizeof(chunk_header)),
        C_FCAST<int>(target->size));

    if(result > 0 && target->size != result)
    {
        ec = lz4_error::decompression_mismatch_size;
        return false;
    } else if(result > 0)
    {
        return true;
    } else
    {
        ec = lz4_error::decompression_failed;
        return false;
    }
}

} // namespace lz4
