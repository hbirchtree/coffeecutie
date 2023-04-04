#include <corezstd/zstd.h>

#include <zstd.h>
#include <zstd_errors.h>

namespace zstd {

struct resource_free
{
    resource_free(std::function<void()>&& f) : m_free(std::move(f))
    {
    }
    ~resource_free()
    {
        m_free();
    }
    std::function<void()> m_free;
};

std::optional<stream_codec::error_t> stream_codec::compress(
    const semantic::Span<const libc_types::u8>& input,
    semantic::concepts::stream_writer&&         receiver,
    options_t&&                                 options)
{
    auto stream = ZSTD_createCStream();
    auto _      = resource_free([stream] { ZSTD_freeCStream(stream); });

    /* Params for compression */
    ZSTD_CCtx_reset(stream, ZSTD_reset_session_and_parameters);
    ZSTD_CCtx_refCDict(stream, nullptr);
    ZSTD_CCtx_setParameter(stream, ZSTD_c_compressionLevel, options.level);
    ZSTD_CCtx_setPledgedSrcSize(stream, input.size());

    std::vector<char> chunk;
    chunk.resize(options.chunk_size);

    ZSTD_outBuffer out = {
        .dst  = chunk.data(),
        .size = chunk.size(),
        .pos  = 0,
    };
    ZSTD_inBuffer in = {
        .src  = input.data(),
        .size = input.size(),
        .pos  = 0,
    };

    const auto emit_chunk = [&chunk, &out, &receiver] {
        receiver(
            semantic::mem_chunk<char>::ofBytes(chunk.data(), out.pos).view);
        out.pos = 0;
    };

    size_t r{0};
    do
    {
        r = ZSTD_compressStream2(stream, &out, &in, ZSTD_e_continue);
        if(ZSTD_isError(r))
            break;
        if((out.size - out.pos) < 128)
            emit_chunk();
    } while(r > 0);

    do
    {
        r = ZSTD_compressStream2(stream, &out, &in, ZSTD_e_end);
        if((out.size - out.pos) < 128)
            emit_chunk();
    } while(!ZSTD_isError(r));

    return std::nullopt;
}

std::optional<stream_codec::error_t> stream_codec::decompress(
    const semantic::Span<const libc_types::u8>& input,
    semantic::concepts::stream_writer&&         receiver,
    options_t&&                                 options)
{
    return std::nullopt;
}

std::optional<codec::error_t> codec::compress(
    const semantic::Span<const libc_types::u8>& input,
    std::vector<char>&                  output,
    options_t&&                                 options)
{
    output.resize(ZSTD_compressBound(input.size_bytes()));

    auto written = ZSTD_compress(
        output.data(),
        output.size(),
        input.data(),
        input.size_bytes(),
        options.level);

    /* TODO: Find way of collecting error message */
    if(ZSTD_isError(written))
        return std::make_optional(1);

    output.resize(written);

    return std::nullopt;
}

std::optional<codec::error_t> codec::decompress(
    const semantic::Span<const libc_types::u8>& input,
    std::vector<char>&                  output,
    options_t&&                                 options)
{
    auto to_decompress
        = ZSTD_getFrameContentSize(input.data(), input.size_bytes());
    if(ZSTD_isError(to_decompress))
        return std::make_optional(1);
    to_decompress += to_decompress >> 3;
    output.resize(to_decompress);

    auto written = ZSTD_decompress(
        output.data(), output.size(), input.data(), input.size_bytes());

    /* TODO: Find way of collecting error message */
    if(ZSTD_isError(written))
        return std::make_optional(1);

    output.resize(written);

    return std::nullopt;
}

} // namespace zstd
