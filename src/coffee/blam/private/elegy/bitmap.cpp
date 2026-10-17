#include <blam/elegy/bitmap.h>

namespace blam::elegy {

std::string_view to_string(bitmap_format_t format)
{
    using enum bitmap_format_t;
    switch(format)
    {
    case a8:
        return "a8";
    case y8:
        return "y8";
    case ay8:
        return "ay8";
    case a8y8:
        return "a8y8";
    case r8:
        return "r8";
    case r5g6b5:
        return "r5g6b5";
    case a1r5g5b5:
        return "a1r5g5b5";
    case a4r4g4b4:
        return "a4r4g4b4";
    case x8r8g8b8:
        return "x8r8g8b8";
    case a8r8g8b8:
        return "a8r8g8b8";
    case dxt1:
        return "dxt1";
    case dxt3:
        return "dxt3";
    case dxt5:
        return "dxt5";
    case a4r4g4b4_font:
        return "a4r4g4b4_font";
    case software_rgbfp32:
        return "software_rgbfp32";
    case v8u8:
        return "v8u8";
    case g8b8:
        return "g8b8";
    case abgrfp32:
        return "abgrfp32";
    case abgrfp16:
        return "abgrfp16";
    case q8w8v8u8:
        return "q8w8v8u8";
    case a2r10g10b10:
        return "a2r10g10b10";
    case a16b16g16r16:
        return "a16b16g16r16";
    case v16u16:
        return "v16u16";
    case dxt3a:
        return "dxt3a";
    case dxt5a:
        return "dxt5a";
    case dxt3a_1111:
        return "dxt3a_1111";
    case dxn:
        return "dxn";
    case ctx1:
        return "ctx1";
    case dxt3a_alpha:
        return "dxt3a_alpha";
    case dxt3a_mono:
        return "dxt3a_mono";
    case dxt5a_alpha:
        return "dxt5a_alpha";
    case dxt5a_mono:
        return "dxt5a_mono";
    case dxn_mono_alpha:
        return "dxn_mono_alpha";
    }
    return "?";
}

format_layout_t layout_of(bitmap_format_t format)
{
    using enum bitmap_format_t;
    switch(format)
    {
    case a8:
    case y8:
    case ay8:
    case r8:
        return {1, 1, 1};
    case a8y8:
    case r5g6b5:
    case a1r5g5b5:
    case a4r4g4b4:
    case a4r4g4b4_font:
    case v8u8:
    case g8b8:
        return {1, 2, 2};
    case x8r8g8b8:
    case a8r8g8b8:
    case q8w8v8u8:
    case a2r10g10b10:
    case v16u16:
        return {1, 4, 4};
    case abgrfp16:
    case a16b16g16r16:
        return {1, 8, 2};
    case abgrfp32:
    case software_rgbfp32:
        return {1, 16, 4};
    case dxt1:
    case dxt3a:
    case dxt5a:
    case dxt3a_1111:
    case ctx1:
    case dxt3a_alpha:
    case dxt3a_mono:
    case dxt5a_alpha:
    case dxt5a_mono:
        return {4, 8, 2};
    case dxt3:
    case dxt5:
    case dxn:
    case dxn_mono_alpha:
        return {4, 16, 2};
    }
    return {};
}

void decode_ctx1(
    Span<byte_t const> blocks, u32 width, u32 height, std::vector<byte_t>& out)
{
    u32 bw = (width + 3) / 4, bh = (height + 3) / 4;
    out.assign(size_t(width) * height * 2, 0);
    for(u32 by = 0; by < bh; by++)
        for(u32 bx = 0; bx < bw; bx++)
        {
            size_t at = (size_t(by) * bw + bx) * 8;
            if(at + 8 > blocks.size())
                return;
            byte_t const* b = blocks.data() + at;
            std::array<std::array<byte_t, 2>, 4> palette;
            for(u32 c = 0; c < 2; c++)
            {
                u32 e0 = b[1 - c], e1 = b[3 - c];
                palette[0][c] = static_cast<byte_t>(e0);
                palette[1][c] = static_cast<byte_t>(e1);
                palette[2][c] = static_cast<byte_t>((2 * e0 + e1 + 1) / 3);
                palette[3][c] = static_cast<byte_t>((e0 + 2 * e1 + 1) / 3);
            }
            u32 indices = u32(b[4]) | u32(b[5]) << 8 | u32(b[6]) << 16 | u32(b[7]) << 24;
            for(u32 i = 0; i < 16; i++)
            {
                u32 x = bx * 4 + (i & 3), y = by * 4 + (i >> 2);
                if(x >= width || y >= height)
                    continue;
                auto const& texel = palette[(indices >> (i * 2)) & 3];
                std::memcpy(out.data() + (size_t(y) * width + x) * 2, texel.data(), 2);
            }
        }
}

std::array<byte_t, 4> dxt5_texel(Span<byte_t const> blocks, u32 width, u32 x, u32 y)
{
    std::array<byte_t, 4> out{};
    u32    bw = (width + 3) / 4;
    size_t at = (size_t(y / 4) * bw + x / 4) * 16;
    if(at + 16 > blocks.size())
        return out;
    byte_t const* b = blocks.data() + at;
    u32           i = (y % 4) * 4 + (x % 4);
    /* Alpha: BC4 */
    u32 a0 = b[0], a1 = b[1];
    u64 alpha_bits = 0;
    for(u32 k = 0; k < 6; k++)
        alpha_bits |= u64(b[2 + k]) << (8 * k);
    u32 ai = (alpha_bits >> (3 * i)) & 7;
    u32 alpha = ai == 0 ? a0
                : ai == 1 ? a1
                : a0 > a1 ? ((8 - ai) * a0 + (ai - 1) * a1) / 7
                : ai < 6  ? ((6 - ai) * a0 + (ai - 1) * a1) / 5
                : ai == 6 ? 0
                          : 255;
    /* Color: BC1, always four colors */
    auto rgb = [](u32 c) {
        return std::array<u32, 3>{
            ((c >> 11) & 31) * 255 / 31, ((c >> 5) & 63) * 255 / 63, (c & 31) * 255 / 31};
    };
    auto c0 = rgb(u32(b[8]) | u32(b[9]) << 8);
    auto c1 = rgb(u32(b[10]) | u32(b[11]) << 8);
    u32  ci = ((u32(b[12]) | u32(b[13]) << 8 | u32(b[14]) << 16 | u32(b[15]) << 24) >> (2 * i)) & 3;
    for(u32 c = 0; c < 3; c++)
        out[c] = static_cast<byte_t>(
            ci == 0   ? c0[c]
            : ci == 1 ? c1[c]
            : ci == 2 ? (2 * c0[c] + c1[c]) / 3
                      : (c0[c] + 2 * c1[c]) / 3);
    out[3] = static_cast<byte_t>(alpha);
    return out;
}

bool linearize(
    Span<byte_t const>  stored,
    bitmap_format_t     format,
    u32                 width,
    u32                 height,
    bitmap_xbox_flags_t flags,
    std::vector<byte_t>& out)
{
    auto layout = layout_of(format);
    if(!layout.valid() || width == 0 || height == 0)
        return false;
    u32 bw     = (width + layout.block_edge - 1) / layout.block_edge;
    u32 bh     = (height + layout.block_edge - 1) / layout.block_edge;
    u32 bpb    = layout.block_bytes;
    u32 pitch  = (bw + 31) & ~31u;
    bool tiled = enum_helpers::feval(flags, bitmap_xbox_flags_t::tiled);
    out.assign(size_t(bw) * bh * bpb, 0);
    for(u32 y = 0; y < bh; y++)
        for(u32 x = 0; x < bw; x++)
        {
            u64 from = u64(tiled ? tiled_offset(x, y, bw, bpb)
                                 : y * pitch + x) * bpb;
            if(from + bpb > stored.size())
                continue;
            std::memcpy(
                out.data() + (size_t(y) * bw + x) * bpb,
                stored.data() + from,
                bpb);
        }
    if(enum_helpers::feval(flags, bitmap_xbox_flags_t::byte_order) &&
       layout.swap_bytes > 1)
        for(size_t i = 0; i + layout.swap_bytes <= out.size();
            i += layout.swap_bytes)
            std::reverse(
                out.begin() + static_cast<std::ptrdiff_t>(i),
                out.begin() + static_cast<std::ptrdiff_t>(i + layout.swap_bytes));
    return true;
}

Span<byte_t const> top_level_interleaved(resource_view const& view, bitmap_image_t const& image)
{
    auto const* interop = view.root_as<d3d_structure<interleaved_interop_t>>();
    auto const* pair    = interop ? view.resolve(*interop) : nullptr;
    if(!pair || image.interleaved_texture < 0 || image.interleaved_texture > 1)
        return {};
    u32 faces  = image.type.value() == bitmap_type_t::cube_map ? 6
                 : image.type.value() == bitmap_type_t::array ? std::max<u32>(image.depth, 1)
                                                               : 1;
    u64 bytes  = u64(slice_stride(image.format.value(), static_cast<u32>(image.width.value()),
                                  static_cast<u32>(image.height.value()))) * faces;
    auto level = enum_helpers::feval(image.xbox_flags, bitmap_xbox_flags_t::high_res_offset)
                     ? view.bytes(pair->secondary)
                     : view.bytes(pair->primary);
    u64 at = bytes * static_cast<u64>(image.interleaved_texture);
    if(at + bytes > level.size())
        return {};
    return level.subspan(at, bytes);
}

} // namespace blam::elegy
