#pragma once

#include <peripherals/concepts/data_codec.h>
#include <peripherals/libc/types.h>
#include <peripherals/semantic/chunk.h>

namespace zstd {

using namespace libc_types::size_literals;

using semantic::concepts::stream_writer;

struct options_t
{
    size_t chunk_size{16_kB};
    int    level{4};
};

struct stream_codec
{
    using options_t = ::zstd::options_t;
    using error_t   = int;

    static std::optional<error_t> compress(
        semantic::Span<const libc_types::u8> const& input,
        stream_writer&&                             receiver,
        options_t&&                                 options = {});

    static std::optional<error_t> decompress(
        semantic::Span<const libc_types::u8> const& input,
        stream_writer&&                             receiver,
        options_t&&                                 options = {});
};

struct codec
{
    using options_t = ::zstd::options_t;
    using error_t   = int;

    static std::optional<error_t> compress(
        semantic::Span<const libc_types::u8> const& input,
        std::vector<char>&                          output,
        options_t&&                                 options = options_t());

    static std::optional<error_t> decompress(
        semantic::Span<const libc_types::u8> const& input,
        std::vector<char>&                          output,
        options_t&&                                 options = {});
};

static_assert(semantic::concepts::is_stream_codec<stream_codec>);
static_assert(semantic::concepts::is_codec<codec>);

} // namespace zstd
