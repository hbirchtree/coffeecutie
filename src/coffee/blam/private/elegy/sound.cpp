#include <blam/elegy/sound.h>

namespace blam::elegy {

std::vector<byte_t> permutation_data(
    sound_permutation_t const& permutation,
    sound_gestalt_t const&     gestalt,
    tag_magic const&           magic,
    resource_view const&       view)
{
    auto chunks = gestalt.chunks.data(magic).value_or(Span<sound_chunk_t const>());
    i32  first  = permutation.first_chunk;
    i16  count  = permutation.chunk_count;
    std::vector<byte_t> out;
    if(first < 0 || count < 0 ||
       static_cast<size_t>(first) + static_cast<size_t>(count) > chunks.size())
        return out;
    u64 primary = view.data.size();
    for(auto const& chunk : chunks.subspan(static_cast<size_t>(first), static_cast<size_t>(count)))
    {
        u64 at = chunk.offset, size = chunk.size();
        /* Chunks never straddle the two pages */
        Span<byte_t const> source = at < primary ? view.data : view.secondary;
        u64 local = at < primary ? at : at - primary;
        if(local > source.size() || size > source.size() - local)
            return {};
        out.insert(out.end(), source.begin() + local, source.begin() + local + size);
    }
    return out;
}

xma2_format_t xma2_format_t::from(
    sound_codec_t const& codec, u32 samples, u32 bytes)
{
    u32 channels = codec.channels();
    u32 rate     = codec.rate();
    auto le32    = [](u32 v) { return to_le(v); };
    auto le16    = [](u16 v) { return to_le(v); };
    xma2_format_t out;
    out.format_tag        = le16(0x166);
    out.channels          = le16(static_cast<u16>(channels));
    out.samples_per_sec   = le32(rate);
    out.avg_bytes_per_sec = le32(rate * channels * 2);
    out.block_align       = le16(2048);
    out.bits_per_sample   = le16(16);
    out.extra_size        = le16(34);
    out.num_streams       = le16(static_cast<u16>((channels + 1) / 2));
    out.channel_mask      = le32(
        channels == 1 ? 0x4 : channels == 2 ? 0x3 : channels == 4 ? 0x33 : 0x3F);
    out.samples_encoded = le32(samples);
    out.bytes_per_block = le32(bytes);
    out.play_length     = le32(samples);
    out.block_count     = le16(1);
    return out;
}

} // namespace blam::elegy
