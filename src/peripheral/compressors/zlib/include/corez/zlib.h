#pragma once

#include <peripherals/concepts/data_codec.h>
#include <peripherals/libc/types.h>
#include <peripherals/semantic/chunk.h>

namespace zlib {

using namespace libc_types::size_literals;

using libc_types::szptr;
using semantic::concepts::stream_writer;

struct options_t
{
    szptr                chunk_size{1_MB};
    int                  level{5};
};

struct stream_codec
{
    using options_t = ::zlib::options_t;
    using error_t   = std::pair<int, std::string_view>;

    static std::optional<error_t> compress(
        semantic::Span<const libc_types::u8> const& input,
        stream_writer&&                   receiver,
        options_t&&                       options = options_t());

    static std::optional<error_t> decompress(
        semantic::Span<const libc_types::u8> const& input,
        stream_writer&&                   receiver,
        options_t&&                       options = {});
};

using codec = semantic::codec_stream_adapter<stream_codec>;

static_assert(semantic::concepts::is_stream_codec<stream_codec>);
static_assert(semantic::concepts::is_codec<codec>);

} // namespace zlib
