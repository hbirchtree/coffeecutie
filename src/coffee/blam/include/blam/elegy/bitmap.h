#pragma once

#include "resources.h"

#include <algorithm>
#include <vector>

namespace blam::elegy {

enum class bitmap_type_t : u16
{
    texture_2d = 0,
    texture_3d = 1,
    cube_map   = 2,
    array      = 3,
};

enum class bitmap_format_t : u16
{
    a8              = 0x00,
    y8              = 0x01,
    ay8             = 0x02,
    a8y8            = 0x03,
    r8              = 0x04,
    r5g6b5          = 0x06,
    a1r5g5b5        = 0x08,
    a4r4g4b4        = 0x09,
    x8r8g8b8        = 0x0A,
    a8r8g8b8        = 0x0B,
    dxt1            = 0x0E,
    dxt3            = 0x0F,
    dxt5            = 0x10,
    a4r4g4b4_font   = 0x11,
    software_rgbfp32 = 0x14,
    v8u8            = 0x16,
    g8b8            = 0x17,
    abgrfp32        = 0x18,
    abgrfp16        = 0x19,
    q8w8v8u8        = 0x1A,
    a2r10g10b10     = 0x1B,
    a16b16g16r16    = 0x1C,
    v16u16          = 0x1D,
    dxt3a           = 0x1E, /*!< 4-bit explicit single channel */
    dxt5a           = 0x1F, /*!< BC4 */
    dxt3a_1111      = 0x20,
    dxn             = 0x21, /*!< BC5, x then y */
    ctx1            = 0x22, /*!< 360 only: shared-index two-channel, 4bpp */
    dxt3a_alpha     = 0x23,
    dxt3a_mono      = 0x24,
    dxt5a_alpha     = 0x25,
    dxt5a_mono      = 0x26,
    dxn_mono_alpha  = 0x27,
};

constexpr std::string_view to_string(bitmap_format_t format)
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

/*!
 * \brief Storage of a format: block edge in texels (1 or 4), bytes per
 * block, and the word size the 360 byte order swaps within.
 */
struct format_layout_t
{
    u8 block_edge{0};
    u8 block_bytes{0};
    u8 swap_bytes{0};

    inline bool valid() const
    {
        return block_bytes != 0;
    }
};

constexpr format_layout_t layout_of(bitmap_format_t format)
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

/*! Xbox 360 storage flags */
enum class bitmap_xbox_flags_t : u8
{
    none                = 0x0,
    delete_from_cache   = 0x1,
    pitch               = 0x2,
    byte_order          = 0x4, /*!< Words big-endian, see format_layout_t */
    tiled               = 0x8,
    created_correctly   = 0x10,
    high_res_offset     = 0x20, /*!< Top mip is in the secondary page */
    interleaved         = 0x40,
};
C_FLAGS(bitmap_xbox_flags_t, u8);

enum class bitmap_flags_t : u16
{
    none          = 0x0,
    power_of_two  = 0x1,
    compressed    = 0x2,
    swap_axes     = 0x4,
    swizzled      = 0x8,
    linear        = 0x10,
    v16u16        = 0x20,
    mip_map_debug = 0x40,
    prefer_stutter = 0x80,
};
C_FLAGS(bitmap_flags_t, u16);

/*! One image of a bitmap tag; pixels are in the matching hardware texture */
struct bitmap_image_t
{
    bl_tag                  signature;
    be<i16>                 width;
    be<i16>                 height;
    i8                      depth;
    bitmap_xbox_flags_t     xbox_flags;
    be<bitmap_type_t>       type;
    be<bitmap_format_t>     format;
    be<bitmap_flags_t>      flags;
    std::array<be<i16>, 2>  registration_point;
    i8                      mipmap_count; /*!< Not counting the top level */
    u8                      curve;
    i8                      interleaved_interop;
    i8                      interleaved_texture;
    be<i32>                 pixels_offset;
    be<i32>                 pixels_size;
    be<i32>                 high_res_pixels_offset;
    be<i32>                 high_res_pixels_size;
    be<i32>                 hardware_format;
    u32                     unknown_0;
};

static_assert(offsetof(bitmap_image_t, format) == 0xC);
static_assert(offsetof(bitmap_image_t, pixels_offset) == 0x18);
static_assert(sizeof(bitmap_image_t) == 0x30);

struct bitmap_sprite_t
{
    be<i16>                bitmap;
    u16                    unknown_0;
    u32                    unknown_1;
    be<f32>                left;
    be<f32>                right;
    be<f32>                top;
    be<f32>                bottom;
    be<Vecf2>              registration_point;
};

static_assert(sizeof(bitmap_sprite_t) == 0x20);

struct bitmap_sequence_t
{
    bl_string                  name;
    be<i16>                    first_bitmap;
    be<i16>                    bitmap_count;
    std::array<u32, 4>         unknown_0;
    tag_block<bitmap_sprite_t> sprites;
};

static_assert(sizeof(bitmap_sequence_t) == 0x40);

struct hardware_texture_t
{
    datum_index resource;
    u32         unknown_0;
};

static_assert(sizeof(hardware_texture_t) == 0x8);

/*! bitmap ("bitm") */
struct bitmap_t
{
    be<u32>                               usage;
    be<u16>                               flags;
    be<i16>                               sprite_spacing;
    be<f32>                               bump_height;
    be<f32>                               fade_factor;
    u8                                    curve_mode;
    i8                                    max_mipmap_level;
    be<u16>                               force_format;
    tag_block<std::array<u8, 0x28>>       usage_override;
    tag_block<bitmap_sequence_t>          manual_sequences;
    data_reference                        source_data;
    data_reference                        processed_pixels;
    tag_block<bitmap_sequence_t>          sequences;
    tag_block<bitmap_image_t>             images;
    data_reference                        processed_pixels_2;
    std::array<u32, 3>                    unknown_0;
    tag_block<hardware_texture_t>         hardware_textures;
    tag_block<hardware_texture_t>         interleaved_textures;
};

static_assert(offsetof(bitmap_t, sequences) == 0x54);
static_assert(offsetof(bitmap_t, images) == 0x60);
static_assert(offsetof(bitmap_t, hardware_textures) == 0x8C);
static_assert(sizeof(bitmap_t) == 0xA4);

/*! The texture as the 360 GPU sees it */
struct texture_definition_t
{
    be<u16>           width;
    be<u16>           height;
    u8                depth;
    u8                mipmap_count; /*!< Counting the top level */
    u8                type;
    u8                high_resolution;
    be<u32>           d3d_format;
};

static_assert(sizeof(texture_definition_t) == 0xC);

/*!
 * \brief bitmap_texture_interop_resource: the top mip lives in the
 * secondary data when high_resolution is set, the rest in the primary.
 */
struct texture_interop_t
{
    data_reference       primary;
    data_reference       secondary;
    texture_definition_t texture;
};

static_assert(sizeof(texture_interop_t) == 0x34);

/*!
 * \brief Block offset of (x, y) in a 360 tiled surface, in blocks
 * (XGAddress2DTiledOffset). Tiled surfaces lay blocks out in 32x32 block
 * macro tiles; width is in blocks.
 */
constexpr u32 tiled_offset(u32 x, u32 y, u32 width, u32 block_bytes)
{
    u32 aligned = (width + 31) & ~31u;
    u32 log_bpb =
        (block_bytes >> 2) + ((block_bytes >> 1) >> (block_bytes >> 2));
    u32 macro = ((x >> 5) + (y >> 5) * (aligned >> 5)) << (log_bpb + 7);
    u32 micro = ((x & 7) + ((y & 6) << 2)) << log_bpb;
    u32 offset = macro + ((micro & ~15u) << 1) + (micro & 15) +
                 ((y & 8) << (3 + log_bpb)) + ((y & 1) << 4);
    return (((offset & ~511u) << 3) + ((offset & 448) << 2) + (offset & 63) +
            ((y & 16) << 7) + (((((y & 8) >> 2) + (x >> 3)) & 3) << 6)) >>
           log_bpb;
}

/*!
 * \brief A texture level as the 360 stored it, turned into the linear,
 * little-endian block order PC formats expect. Tiled data may stop at the
 * last block in use, and linear rows are 32 blocks apart; blocks outside
 * the data come out zero.
 */
inline bool linearize(
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

/*!
 * \brief Bytes from one slice of an array (or face of a cube) to the next:
 * the surface padded to 32x32 blocks, then to 4 KiB.
 */
constexpr u32 slice_stride(bitmap_format_t format, u32 width, u32 height)
{
    auto layout = layout_of(format);
    if(!layout.valid())
        return 0;
    u32 bw = (width + layout.block_edge - 1) / layout.block_edge;
    u32 bh = (height + layout.block_edge - 1) / layout.block_edge;
    u32 bytes = ((bw + 31) & ~31u) * ((bh + 31) & ~31u) * layout.block_bytes;
    return (bytes + 4095) & ~4095u;
}

/*!
 * \brief CTX1 blocks, linearized, to (x, y) 8-bit texels. A block is two
 * endpoints stored y first, as DXN, and 2-bit indices shared by both
 * channels; the indices pick endpoint 0, 1, or 1/3 and 2/3 between them.
 */
inline void decode_ctx1(
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

/*!
 * \brief DXN keeps 3Dc's channel order, y in the first half of a block;
 * swapping the halves gives BC5 with x in red.
 */
inline void dxn_to_bc5(std::vector<byte_t>& blocks)
{
    for(auto it = blocks.begin(); blocks.end() - it >= 16; it += 16)
        std::swap_ranges(it, it + 8, it + 8);
}

/*!
 * \brief The top level of an image's first hardware texture: secondary
 * data when the high resolution level was split off, else the start of the
 * primary data.
 */
inline Span<byte_t const> top_level(
    resource_view const& view, bitmap_image_t const& image)
{
    auto const* interop = view.root_as<d3d_structure<texture_interop_t>>();
    auto const* texture = interop ? view.resolve(*interop) : nullptr;
    if(!texture)
        return {};
    if(enum_helpers::feval(
           image.xbox_flags, bitmap_xbox_flags_t::high_res_offset))
        if(auto bytes = view.bytes(texture->secondary); !bytes.empty())
            return bytes;
    return view.bytes(texture->primary);
}

} // namespace blam::elegy
