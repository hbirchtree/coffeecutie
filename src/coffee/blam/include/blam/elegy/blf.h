#pragma once

#include "structures.h"

#include <optional>
#include <string>
#include <vector>

namespace blam::elegy {

/*! A chunk of a BLF file (images/*.blf, info/*.mapinfo); its size counts
 * this header */
struct blf_chunk_header_t
{
    bl_header tag;
    be<u32>   size;
    be<u16>   major_version;
    be<u16>   minor_version;
};

static_assert(sizeof(blf_chunk_header_t) == 0xC);

struct blf_chunk_t
{
    blf_chunk_header_t const* header;
    Span<u8 const>            body;

    inline std::string_view tag() const
    {
        return std::string_view(header->tag.data(), header->tag.size());
    }
};

/*! The chunks from _blf through _eof, stopping at one that does not fit */
inline std::vector<blf_chunk_t> blf_chunks(Span<u8 const> file)
{
    std::vector<blf_chunk_t> out;
    size_t                   offset = 0;
    while(offset + sizeof(blf_chunk_header_t) <= file.size())
    {
        auto const* header = reinterpret_cast<blf_chunk_header_t const*>(file.data() + offset);
        size_t const size  = header->size.value();
        if(size < sizeof(blf_chunk_header_t) || offset + size > file.size())
            break;
        out.push_back({header, file.subspan(offset + sizeof(blf_chunk_header_t), size - sizeof(blf_chunk_header_t))});
        offset += size;
        if(out.back().tag() == "_eof")
            break;
    }
    return out;
}

enum class blf_image_type_t : u32
{
    jpeg = 0,
    png  = 1,
};

/*! mapi: a map's menu image, stored as the file it was made from */
struct blf_map_image_t
{
    be<u32> type; /*!< blf_image_type_t */
    be<u32> size;
};

static_assert(sizeof(blf_map_image_t) == 0x8);

/*! A mapi chunk's image type and file bytes */
inline std::optional<std::pair<blf_image_type_t, Span<u8 const>>> map_image(blf_chunk_t const& chunk)
{
    if(chunk.tag() != "mapi" || chunk.body.size() < sizeof(blf_map_image_t))
        return std::nullopt;
    auto const* image = reinterpret_cast<blf_map_image_t const*>(chunk.body.data());
    size_t const size = image->size.value();
    if(sizeof(blf_map_image_t) + size > chunk.body.size())
        return std::nullopt;
    return std::make_pair(
        static_cast<blf_image_type_t>(image->type.value()), chunk.body.subspan(sizeof(blf_map_image_t), size));
}

/*! levl 3.1: a map's menu entry; strings are UTF-16, one per language */
struct blf_map_info_t
{
    static constexpr size_t languages = 12;

    be<u32>                                         map_id;
    be<u32>                                         flags; /* TODO: flags */
    std::array<std::array<be<u16>, 32>, languages>  names;
    std::array<std::array<be<u16>, 128>, languages> descriptions;
    bl_string_var<256>                              image_name; /*!< images/<this>.blf */
    bl_string_var<256>                              map_name;   /*!< <this>.map */
    /* Per mode settings and more follow */
};

static_assert(offsetof(blf_map_info_t, descriptions) == 0x308);
static_assert(offsetof(blf_map_info_t, image_name) == 0xF08);
static_assert(offsetof(blf_map_info_t, map_name) == 0x1008);

/*! A levl chunk's map info */
inline blf_map_info_t const* map_info(blf_chunk_t const& chunk)
{
    if(chunk.tag() != "levl" || chunk.body.size() < sizeof(blf_map_info_t))
        return nullptr;
    return reinterpret_cast<blf_map_info_t const*>(chunk.body.data());
}

/*! A NUL-terminated UTF-16 string as UTF-8 */
template<size_t N>
inline std::string utf8(std::array<be<u16>, N> const& text)
{
    std::string out;
    for(size_t i = 0; i < N; i++)
    {
        u32 c = text[i].value();
        if(c == 0)
            break;
        if(c >= 0xD800 && c < 0xDC00 && i + 1 < N)
        {
            u32 const low = text[i + 1].value();
            if(low >= 0xDC00 && low < 0xE000)
                c = 0x10000 + ((c - 0xD800) << 10) + (low - 0xDC00), i++;
        }
        if(c < 0x80)
            out += static_cast<char>(c);
        else if(c < 0x800)
            out += static_cast<char>(0xC0 | (c >> 6)), out += static_cast<char>(0x80 | (c & 0x3F));
        else if(c < 0x10000)
            out += static_cast<char>(0xE0 | (c >> 12)), out += static_cast<char>(0x80 | ((c >> 6) & 0x3F)),
                out += static_cast<char>(0x80 | (c & 0x3F));
        else
            out += static_cast<char>(0xF0 | (c >> 18)), out += static_cast<char>(0x80 | ((c >> 12) & 0x3F)),
                out += static_cast<char>(0x80 | ((c >> 6) & 0x3F)), out += static_cast<char>(0x80 | (c & 0x3F));
    }
    return out;
}

} // namespace blam::elegy
