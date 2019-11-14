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
    default:
        return "no_error";
    }
}

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

    *target = semantic::Bytes::Alloc(approx_size);

    int result;

    switch(opts.mode)
    {
    case compression_mode::fast:
    case compression_mode::default_:
    {
        result = LZ4_compress_fast_extState(
            state.data,
            C_RCAST<const char*>(uncompressed.data),
            C_RCAST<char*>(target->data),
            C_FCAST<int>(uncompressed.size),
            C_FCAST<int>(target->size),
            opts.mode == compression_mode::default_ ? 0
                                                    : opts.fast_acceleration);
        break;
    }
    case compression_mode::high:
    {
        result = LZ4_compress_HC_extStateHC(
            state.data,
            C_RCAST<const char*>(uncompressed.data),
            C_RCAST<char*>(target->data),
            C_FCAST<int>(uncompressed.size),
            C_FCAST<int>(target->size),
            opts.high_compression);
        break;
    }
    }

    if(result > 0)
    {
        target->size     = C_FCAST<libc_types::u32>(result);
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
    return false;
}

} // namespace lz4
