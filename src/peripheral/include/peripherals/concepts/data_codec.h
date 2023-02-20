#pragma once

#include <peripherals/semantic/chunk.h>

namespace semantic::concepts {

/* Full file codec, does not return until all is (de)compressed */

using stream_writer = std::function<void(semantic::Span<char>&&)>;

template<typename T, typename InType = libc_types::u8>
concept is_compressor = std::is_same_v<
    typename declreturntype(T::compress),
    std::optional<typename T::error_t>> && requires(T)
{
    {T::compress(
        std::declval<semantic::Span<const InType> const&>(),
        std::declval<semantic::mem_chunk<char>&>(),
        std::declval<typename T::options_t&&>())};
};

template<typename T, typename OutType = char>
concept is_decompressor = std::is_same_v<
    typename declreturntype(T::decompress),
    std::optional<typename T::error_t>> && requires(T)
{
    {T::decompress(
        std::declval<semantic::Span<const libc_types::u8> const&>(),
        std::declval<semantic::mem_chunk<OutType>&>(),
        std::declval<typename T::options_t&&>())};
};

template<typename T>
concept is_codec = is_compressor<T> && is_decompressor<T>;

/* Chunk codec, returns (de)compressed data chunk-wislibc_types::u8e */

template<typename T, typename InType = libc_types::u8>
concept is_stream_compressor = std::is_same_v<
    typename declreturntype(T::compress),
    std::optional<typename T::error_t>> && requires(T)
{
    {T::compress(
        std::declval<semantic::Span<const InType> const&>(),
        std::declval<stream_writer>(),
        std::declval<typename T::options_t&&>())};
};

template<typename T>
concept is_stream_decompressor = std::is_same_v<
    typename declreturntype(T::decompress),
    std::optional<typename T::error_t>> && requires(T)
{
    {T::decompress(
        std::declval<semantic::Span<const libc_types::u8> const&>(),
        std::declval<stream_writer>(),
        std::declval<typename T::options_t&&>())};
};

template<typename T>
concept is_stream_codec = is_stream_compressor<T> && is_stream_decompressor<T>;

} // namespace semantic::concepts

namespace semantic {

template<
    concepts::is_stream_codec Codec,
    typename InType  = libc_types::u8,
    typename OutType = char>
struct codec_stream_adapter
{
    using options_t = typename Codec::options_t;
    using error_t   = typename Codec::error_t;

    static std::optional<error_t> compress(
        semantic::Span<const InType> const& input,
        semantic::mem_chunk<char>&          output,
        options_t&&                         options = options_t())
    {
        auto out_it = std::back_inserter(output.allocation);

        return Codec::compress(
            input,
            [&out_it](semantic::Span<char>&& data) {
                std::copy(data.begin(), data.end(), out_it);
            },
            std::move(options));
    }

    static std::optional<error_t> decompress(
        semantic::Span<const libc_types::u8> const& input,
        semantic::mem_chunk<OutType>&               output,
        options_t&&                                 options = {})
    {
        if(output.ownership != Ownership::Owned)
            Throw(std::runtime_error("mem_chunk must be owned"));
        std::vector<OutType>& out = output.allocation;

        auto caster = [&out](semantic::Span<char>&& data) {
            if(data.size_bytes() % sizeof(OutType) == 0)
                Throw(undefined_behavior("partial object received"));
            auto casted = semantic::mem_chunk<OutType>::ofContainer(data).view;
            std::copy(casted.begin(), casted.end(), std::back_inserter(out));
        };
        auto err = Codec::decompress(input, caster, std::move(options));
        output.updatePointers(semantic::Ownership::Owned);
        return err;
    }
};

} // namespace semantic
