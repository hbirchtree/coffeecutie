#pragma once

#include "../volta/blam_endian.h"
#include "../volta/blam_structures.h"

#include <array>
#include <cstring>
#include <optional>
#include <vector>

namespace blam::dimeter::font {

/*!
 * Halo 2's bitmap fonts, the files in the disc's fonts/ directory
 * (font_table.txt maps the UI's font slots to them). A zeroed 0x200-byte
 * prefix, a header, the kerning pairs, then a glyph index per codepoint
 * at 0x400 and the glyph table at 0x40400; each glyph's pixels are
 * run-length coded 4-bit ARGB (decoder after Lord Zedd's FontPackager).
 */

constexpr u32 header_offset      = 0x200;
constexpr u32 codepoint_offset   = 0x400;
constexpr u32 glyph_table_offset = 0x40400;
constexpr u32 no_glyph           = 0x1FE;

struct alignas(4) header_t
{
    u32 version; /*!< 0xF0000001 */
    i16 ascending_height;
    i16 descending_height;
    i32 leading;
    u32 glyph_count;
    u32 max_compressed_size;
    u32 unknown_0;
    u32 compressed_size; /*!< Sum over the glyphs */
    u32 pixel_count;     /*!< Sum of width x height */
    u32 kerning_count;

    inline bool valid() const
    {
        return from_le(version) == 0xF0000001u;
    }
};

static_assert(sizeof(header_t) == 0x24);

struct alignas(2) kerning_t
{
    u8  first, second;
    i16 adjust;
};

static_assert(sizeof(kerning_t) == 4);

struct alignas(4) glyph_t
{
    u16 advance;
    u16 compressed_size;
    u16 width;
    u16 height;
    i16 origin_x;
    i16 origin_y; /*!< Rows above the baseline */
    u32 data_offset;
};

static_assert(sizeof(glyph_t) == 16);

/*! A decoded glyph: width x height RGBA8, white with the coverage in alpha
 * for the usual glyph, tinted where the font says so */
struct image_t
{
    u32             width{0}, height{0};
    std::vector<u8> rgba;
};

struct font_t
{
    Span<const byte_t> file;
    header_t const*    header{nullptr};
    Span<kerning_t const> kerning;
    Span<glyph_t const>   glyphs;

    static std::optional<font_t> from_bytes(Span<const byte_t> file)
    {
        if(file.size() < glyph_table_offset)
            return std::nullopt;
        font_t out;
        out.file   = file;
        out.header = reinterpret_cast<header_t const*>(file.data() + header_offset);
        if(!out.header->valid())
            return std::nullopt;
        u32 kerns  = from_le(out.header->kerning_count);
        u32 glyphs = from_le(out.header->glyph_count);
        if(header_offset + sizeof(header_t) + kerns * sizeof(kerning_t) >
               codepoint_offset ||
           glyph_table_offset + glyphs * sizeof(glyph_t) > file.size())
            return std::nullopt;
        out.kerning = Span<kerning_t const>(
            reinterpret_cast<kerning_t const*>(
                file.data() + header_offset + sizeof(header_t)),
            kerns);
        out.glyphs = Span<glyph_t const>(
            reinterpret_cast<glyph_t const*>(file.data() + glyph_table_offset),
            glyphs);
        return out;
    }

    inline i16 ascent() const
    {
        return from_le(header->ascending_height);
    }

    inline i16 descent() const
    {
        return from_le(header->descending_height);
    }

    inline i32 line_height() const
    {
        return ascent() + descent() + from_le(header->leading);
    }

    /*! The glyph for a codepoint, null if the font lacks it */
    inline glyph_t const* glyph(char32_t codepoint) const
    {
        if(codepoint > 0xFFFF)
            return nullptr;
        u32 idx;
        std::memcpy(&idx, file.data() + codepoint_offset + codepoint * 4, 4);
        idx = from_le(idx);
        if(idx == no_glyph || idx >= glyphs.size())
            return nullptr;
        return &glyphs[idx];
    }

    inline i16 kerning_between(char32_t a, char32_t b) const
    {
        if(a > 0xFF || b > 0xFF)
            return 0;
        for(auto const& k : kerning)
            if(k.first == a && k.second == b)
                return from_le(k.adjust);
        return 0;
    }

    /*! Decodes a glyph's pixels; false if the data runs out */
    bool decode(glyph_t const& g, image_t& out) const
    {
        u32 w = from_le(g.width), h = from_le(g.height);
        u32 at = from_le(g.data_offset), size = from_le(g.compressed_size);
        if(at + size > file.size())
            return false;
        out.width  = w;
        out.height = h;
        out.rgba.assign(static_cast<size_t>(w) * h * 4, 0);
        u8 const*  data  = file.data() + at;
        size_t     pixel = 0;
        size_t     total = static_cast<size_t>(w) * h;
        u16        base  = 0xFFF; /* rgb, 4 bits each */
        auto       put   = [&](u32 count, u16 argb) {
            for(u32 i = 0; i < count && pixel < total; i++, pixel++)
            {
                u8* p = out.rgba.data() + pixel * 4;
                p[0] = static_cast<u8>(((argb >> 8) & 0xF) * 17);
                p[1] = static_cast<u8>(((argb >> 4) & 0xF) * 17);
                p[2] = static_cast<u8>((argb & 0xF) * 17);
                p[3] = static_cast<u8>(((argb >> 12) & 0xF) * 17);
            }
        };
        for(u32 i = 0; i < size && pixel < total; i++)
        {
            u8 code  = data[i];
            u8 flags = code >> 6;
            u8 value = code & 0x3F;
            if(flags < 2)
            {
                if(code == 0)
                {
                    /* Literal ARGB pixel, which also sets the colour */
                    if(i + 2 >= size)
                        return false;
                    u16 argb = static_cast<u16>((data[i + 1] << 8) | data[i + 2]);
                    i += 2;
                    put(1, argb);
                    base = argb & 0xFFF;
                } else
                    /* A run of the colour: transparent, or opaque */
                    put(value, static_cast<u16>(base | (flags ? 0xF000 : 0)));
                continue;
            }
            /* Two 3-bit alphas, or one plus a short run */
            u16 hi = (code >> 3) & 7;
            hi     = static_cast<u16>((hi << 1) | (hi & 1));
            put(1, static_cast<u16>((hi << 12) | base));
            u16 lo = code & 7;
            if(flags != 2)
            {
                lo = static_cast<u16>((lo << 1) | (lo & 1));
                put(1, static_cast<u16>((lo << 12) | base));
            } else if(lo != 0)
            {
                u16 argb  = (lo & 4) ? base : static_cast<u16>(base | 0xF000);
                u32 count = static_cast<u32>(std::abs(5 - static_cast<int>(lo & 3)));
                put(count, argb);
            }
        }
        return true;
    }
};

/*! The UI's font slots, as font_table.txt lists them (one file name per
 * line, in slot order) */
inline std::vector<std::string> parse_font_table(Span<const byte_t> text)
{
    std::vector<std::string> out;
    std::string              line;
    for(auto c : text)
    {
        if(c == '\n' || c == '\r')
        {
            if(!line.empty())
                out.push_back(line);
            line.clear();
        } else
            line.push_back(static_cast<char>(c));
    }
    if(!line.empty())
        out.push_back(line);
    return out;
}

} // namespace blam::dimeter::font
