#include <corez/zlib.h>

#include <zlib.h>

namespace zlib {

using inflate_init_fun = int (*)(z_streamp, const char*, int);
using deflate_init_fun = int (*)(z_streamp, int, const char*, int);
using process_fun      = int (*)(z_streamp, int);
using end_fun          = int (*)(z_streamp);

using libc_types::u32;
using libc_types::u8;

using namespace libc_types::size_literals;
using namespace std::string_view_literals;

inline std::string_view error_to_string(int error)
{
#define ZLIB_ERROR_MSG(code) \
    case code:               \
        return #code##sv;
    switch(error)
    {
        ZLIB_ERROR_MSG(Z_STREAM_END);
        ZLIB_ERROR_MSG(Z_NEED_DICT);
        ZLIB_ERROR_MSG(Z_STREAM_ERROR);
        ZLIB_ERROR_MSG(Z_DATA_ERROR);
        ZLIB_ERROR_MSG(Z_MEM_ERROR);
        ZLIB_ERROR_MSG(Z_BUF_ERROR);
        ZLIB_ERROR_MSG(Z_VERSION_ERROR);
    default:
        return "UNKNOWN_ERROR"sv;
    }
#undef ZLIB_ERROR_MSG
}

template<
    inflate_init_fun InitI = nullptr,
    deflate_init_fun InitD = nullptr,
    process_fun      Proc  = nullptr,
    end_fun          End   = nullptr>
std::optional<codec::error_t> compression_routine(
    semantic::Span<const u8> const& input,
    stream_writer&&                 output,
    options_t&&                     opts)
{
    /* Set the minimum to 64k for now, might want to make it smaller? */
    u32 chunk_size = std::min<szptr>(64u * 1024u, opts.chunk_size);

    z_stream strm = {};

    std::vector<unsigned char> chunk;

    strm.avail_out = chunk.size();
    strm.next_out  = chunk.data();

    strm.avail_in = input.size();
    strm.next_in  = const_cast<unsigned char*>(input.data());

    int ret = Z_OK;

    if constexpr(InitD != nullptr)
        ret = deflateInit(&strm, opts.level);
    else
        ret = inflateInit(&strm);

    if(ret != Z_OK)
        return std::make_pair(ret, error_to_string(ret));

    const auto emit_chunk = [&chunk, &strm, &output, chunk_size] {
        if(chunk_size - strm.avail_out == 0)
            return;
        output(semantic::mem_chunk<char>::ofBytes(
                   chunk.data(), chunk_size - strm.avail_out)
                   .view);
    };

    while(strm.avail_in > 0 && ret != Z_STREAM_END)
    {
        ret = Proc(&strm, Z_NO_FLUSH);

        if(ret != Z_OK && ret != Z_STREAM_END)
            return std::make_pair(ret, error_to_string(ret));

        if(strm.avail_out == 0 && strm.avail_in > 0)
        {
            emit_chunk();
            strm.avail_out = chunk.size();
            strm.next_out  = chunk.data();
        } else if(strm.avail_in == 0)
        {
            emit_chunk();
            break;
        }
    }

    ret = End(&strm);

    if(ret != Z_OK)
        return std::make_pair(ret, error_to_string(ret));

    return std::nullopt;
}

using semantic::mem_chunk;

std::optional<stream_codec::error_t> stream_codec::compress(
    const semantic::Span<const libc_types::u8>& input,
    semantic::concepts::stream_writer&&         receiver,
    options_t&&                                 options)
{
    return compression_routine<nullptr, deflateInit_, deflate, deflateEnd>(
        mem_chunk<const libc_types::u8>::ofContainer(input).view,
        std::move(receiver),
        std::move(options));
}

std::optional<stream_codec::error_t> stream_codec::decompress(
    const semantic::Span<const libc_types::u8>& input,
    semantic::concepts::stream_writer&&         receiver,
    options_t&&                                 options)
{
    return compression_routine<inflateInit_, nullptr, inflate, inflateEnd>(
        mem_chunk<const libc_types::u8>::ofContainer(input).view,
        std::move(receiver),
        std::move(options));
}

// bool Compressor::Compress(
//     semantic::Span<const libc_types::u8> const& uncompressed,
//     semantic::mem_chunk<libc_types::u8>*        target,
//     Opts const&                                 opts,
//     zlib_error_code&                            ec)
//{
//     return compression_routine<
//         nullptr,
//         ::deflateInit_,
//         ::deflate,
//         ::deflateEnd>(uncompressed, target, opts, ec);
// }

// bool Compressor::Decompress(
//     semantic::Span<const libc_types::u8> const& compressed,
//     semantic::mem_chunk<libc_types::u8>*        target,
//     Opts const&                                 opts,
//     zlib_error_code&                            ec)
//{
//     return compression_routine<
//         ::inflateInit_,
//         nullptr,
//         ::inflate,
//         ::inflateEnd>(compressed, target, opts, ec);
// }

} // namespace zlib
