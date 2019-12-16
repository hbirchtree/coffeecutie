#pragma once

#include <coffee/core/libc_types.h>
#include <coffee/core/types/map.h>
#include <coffee/core/types/pixel_components.h>
#include <coffee/core/types/pixel_format.h>
#include <coffee/core/types/point.h>
#include <coffee/core/types/rgba.h>
#include <coffee/core/types/size.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/typing/enum/graphics/texture_types.h>
#include <platforms/types/vector_types.h>

#include "blam_tag_classes.h"

namespace blam {

using namespace libc_types;
using stl_types::Array;
using typing::PixCmp;
using typing::geometry::point_2d;
using typing::geometry::size_2d;
using typing::geometry::size_3d;
using typing::pixels::BitFmt;
using typing::pixels::CompFlags;
using typing::pixels::PixFmt;
using typing::vector_types::Quatf;
using typing::vector_types::Vecf2;
using typing::vector_types::Vecf3;
using typing::vector_types::Vecf4;
using TexType = typing::graphics::TexType;

template<size_t Size>
struct bl_string_var
{
    char data[Size];

    template<
        typename Dummy                                   = void,
        typename std::enable_if<Size == 4, Dummy>::type* = nullptr>
    inline stl_types::CString str() const
    {
        if(data[0] == 0)
            return {};

        auto out = stl_types::CString(data, Size);
        std::reverse(out.begin(), out.end());
        return out;
    }

    template<
        typename Dummy                                   = void,
        typename std::enable_if<Size != 4, Dummy>::type* = nullptr>
    inline stl_types::CString str() const
    {
        auto end = libc::str::find(data, '\0');
        return stl_types::CString(data, end);
    }

    inline operator cstring() const
    {
        return data;
    }

    template<
        typename Dummy                                   = void,
        typename std::enable_if<Size == 4, Dummy>::type* = nullptr>
    explicit operator u32() const
    {
        u32 const* idata = C_RCAST<u32 const*>(data);
        return *idata;
    }
};

using bl_tag    = bl_string_var<4>;
using bl_string = bl_string_var<32>;
using bl_header = char[4];
using bl_footer = char[4];

template<size_t Size>
struct unicode_var
{
    using wide_string = std::basic_string<u16>;

    u16 data[Size];

    inline wide_string str() const
    {
        constexpr u16 terminator = 0;

        auto out = wide_string(data, Size);
        auto end = out.find(terminator);
        if(end != wide_string::npos)
            out.resize(end);
        return out;
    }

    inline stl_types::CString flat_str() const
    {
        auto from = str();
        return stl_types::CString(from.begin(), from.end());
    }
};

/*!
 * \brief Blam, at least for Halo 1, uses int16 to store bitmap sizes
 */
using bl_size_t = size_2d<i16>;
/*!
 * \brief As with blam_size, int16 is standard size for Halo 1.
 */
using bl_point_t = point_2d<i16>;

/*!
 * \brief Function pointers for blam bitmap processing, raw function pointer is
 * much faster than Function
 */
using BitmProcess = u32 (*)(u32, uint16, byte_t);

using bl_rgba_t = typing::pixels::rgba_t;

/*!
 * \brief Blam maptypes. Names being obvious, the UI type does not give a
 * playable map.
 */
enum class maptype_t : int32
{
    singleplayer =
        0,           /*!< A single-player map, typically with cutscenes and AI*/
    multiplayer = 1, /*!< A multi-player map, typically with up to 16 players*/
    ui          = 2, /*!< A UI map, used only in the main menu*/
};

/*!
 * \brief Possible Blam versions which we may encounter. The different formats
 * work in different ways. For instance, Xbox stores bitmaps and other data in a
 * single file while PC spreads it across "bitmaps.map" and "sounds.map". Both
 * ways are necessary for parsing to happen correctly.
 */
enum class version_t : int32
{
    xbox = 5, /*!< The 2001 version of Halo: Combat Evolved for Xbox*/
    pc   = 7, /*!< The 2004 version of Halo: Combat Evolved for PC*/

    custom_edition = 609,
};

struct xbox_version_t
{
};
struct pc_version_t
{
};
struct custom_version_t
{
};

constexpr xbox_version_t   xbox_version;
constexpr pc_version_t     pc_version;
constexpr custom_version_t custom_version;

namespace vert {

struct compressed
{
};
struct uncompressed
{
};

template<
    typename RType = uncompressed,
    bool           = std::is_same<RType, uncompressed>::value>
struct alignas(4) vertex
{
    Vecf3 position;
    Vecf3 normal;
    Vecf3 binorm;
    Vecf3 tangent;
    Vecf2 texcoord;
    Vecf3 _garbo;

  private:
    constexpr void size_check()
    {
        static_assert(sizeof(vertex<uncompressed>) == 68, "Invalid size");
    }
};

template<typename RType>
struct vertex<RType, false>
{
    // Compressed Xbox variant
    Vecf3 position;
    u32   normal;   /*!< PixFmt::R11G11B10F */
    u32   binormal; /*!< Same as binormal */
    u32   tangent;  /*!< Same as normal */
    Vecf2 texcoord;

  private:
    constexpr void size_check()
    {
        static_assert(sizeof(vertex<compressed>) == 32, "Invalid size");
    }
};

using face = Array<u16, 3>;

template<
    typename RType = uncompressed,
    bool           = std::is_same<RType, uncompressed>::value>
struct light_vertex
{
    Vecf3 normal;
    Vecf2 texcoord;
};

template<typename RType>
struct light_vertex<RType, false>
{
    // Compressed Xbox variant
    u32 normal;   /*!< PixFmt::R11G11B10F */
    u16 texcoord; /*!< PixFmt::R16, Normalized */
};

} // namespace vert

constexpr cstring header_head = "daeh"; /*!< Header of file header*/
constexpr cstring header_foot = "toof"; /*!< Footer of file header*/

constexpr i32 blam_num_map_names = 28; /*!< Number of recognizable map names*/
constexpr stl_types::Array<stl_types::Pair<cstring, cstring>, 28> map_names = {{
    // Single player maps
    {"a10", "Pillar of Autumn"},
    {"a30", "Halo"},
    {"a50", "Truth and Reconciliation"},
    {"b30", "Silent Cartographer"},
    {"b40", "Assault on the Control Room"},
    {"c10", "343 Guilty Spark"},
    {"c20", "The Library"},
    {"c40", "Two Betrayals"},
    {"d20", "Keyes"},
    {"d40", "The Maw"},

    // Multi-player maps
    {"beavercreek", "Beaver Creek"},
    {"bloodgulch", "Blood Gulch"},
    {"boardingaction", "Boarding Action"},
    {"carousel", "Derelict"},
    {"chillout", "Chill Out"},
    {"deathisland", "Death Island"},
    {"damnation", "Damnation"},
    {"dangercanyon", "Danger Canyon"},
    {"gephyrophobia", "Gephyrophobia"},
    {"hangemhigh", "Hang 'em High"},
    {"icefields", "Ice Fields"},
    {"longest", "Longest"},
    {"prisoner", "Prisoner"},
    {"putput", "Chiron TL34"},
    {"ratrace", "Rat Race"},
    {"sidewinder", "Sidewinder"},
    {"timberland", "Timberland"},
    {"wizard", "Wizard"},
}}; /*!< A mapping of map names which this library can recognize. These are
      the stock maps.*/

/*!
 * \brief A file header located from the start of a map file
 */
struct alignas(4) file_header_t : stl_types::non_copy
{
    bl_header id;      /*!< Header value, should correspond with specific data*/
    version_t version; /*!< Version of Halo, determines the process*/
    u32       decomp_len; /*!< Decompressed length */
    u32 trailing_space;   /*!< Specifies size of empty space at end of file */
    u32 tag_index_offset; /*!< Offset to tag index*/
    i32 tag_index_len;    /*!< Length of tag index item metadata*/
    u32 reserved_1[2];
    bl_string name;      /*!< Name identifier for map*/
    bl_string buildDate; /*!< Build date for the map file*/
    maptype_t mapType;   /*!< Type of map, determines whether it is playable*/
    i32       unknown_4;
    i32       reserved_2[485];
    bl_footer footer; /*!< Footer value, should correspond with specific data*/

    /*!
     * \brief Get the proper, full name of a map
     * \param map Map from which we identify a string
     * \return A null-terminated string with a map name or empty (empty if map
     * name is not recognized)
     */
    inline cstring full_mapname() const
    {
        for(auto name : map_names)
        {
            if(libc::str::cmp(C_OCAST<const char*>(this->name), name.first))
                return name.second;
        }
        return name;
    }
    /*!
     * \brief Validate and get the file header from a base pointer, mostly
     * validation
     * \param data Pointer to data which will be validated
     * \param expected Expected version of Halo to be parsed. If version does
     *    not match, fail.
     * \return Returns a casted pointer on success, nullptr on failure
     */
    static file_header_t const* from_data(
        semantic::Bytes const& data, pc_version_t);

    static file_header_t const* from_data(
        semantic::Bytes const& data, custom_version_t);

    static file_header_t const* from_data(
        semantic::Bytes const& data, xbox_version_t);

    inline bool is_xbox() const
    {
        return version == version_t::xbox;
    }
};

struct magic_data_t
{
    magic_data_t() : magic_offset(0)
    {
        base_ptr = nullptr;
    }
    magic_data_t(byte_t const* base_ptr, u32 magic, u32 max_size) :
        magic_offset(magic), max_size(max_size)
    {
        this->base_ptr = base_ptr;
    }

    magic_data_t& operator=(semantic::Bytes const& data)
    {
        base_ptr     = data.data;
        magic_offset = 0;
        max_size     = data.size;

        return *this;
    }

    union
    {
        file_header_t const* header_ptr;
        byte_t const*        base_ptr;
    };
    u32 magic_offset;
    u32 max_size;
};

struct xbox_variant
{
};
struct pc_variant
{
};

/*!
 * \brief Points to a chunk of memory within the file
 */
template<
    typename T,
    typename RType = pc_variant,
    bool           = std::is_same<RType, pc_variant>::value>
struct reflexive_t
{
};

template<typename T, typename RType>
struct alignas(4) reflexive_t<T, RType, true>
{
    u32 count;  /*!< Number of elements */
    u32 offset; /*!< Offset to data within file (this will only refer to data
                   within the map file)*/
    i32 zero;

    /*!
     * \brief Made for simplicity when working with reflexive data. Instead of
     * several functions performing the same task, this template allows to
     * access the data much more conveniently. \param basePtr Base pointer of
     * map in most cases \param magic Magic number from tag index \return A
     * valid pointer if the reflexive is deemed valid (if the variable zero is
     * indeed zero)
     */
    semantic::mem_chunk<T const> data(magic_data_t const& magic) const
    {
        using Output = semantic::mem_chunk<T const>;

        if(zero != 0)
            Throw(undefined_behavior("Invalid reflexive_t"));

        if(count == 0)
            return Output();

        if((offset - magic.magic_offset) > magic.max_size)
            Throw(undefined_behavior("reflexive pointer out of bounds"));

        return Output::From(
            C_RCAST<T const*>(magic.base_ptr + offset - magic.magic_offset),
            count);
    }
};

template<typename T, typename RType>
struct alignas(4) reflexive_t<T, RType, false>
{
    u32 count;
    u32 offset;

    semantic::mem_chunk<T const> data(magic_data_t const& magic) const
    {
        using Output = semantic::mem_chunk<T const>;

        if(count == 0)
            return Output();

        if((offset - magic.magic_offset) > magic.max_size)
            Throw(undefined_behavior("reflexive pointer out of bounds"));

        return Output::From(
            C_RCAST<T const*>(magic.base_ptr + offset - magic.magic_offset),
            count);
    }
};

struct alignas(4) string_ref
{
    u32 offset;

    cstring to_string(magic_data_t const& magic) const
    {
        if(offset == 0)
            return "[empty string]";

        return C_RCAST<cstring>(magic.base_ptr + offset - magic.magic_offset);
    }
};

struct alignas(4) tagref_t
{
    union
    {
        bl_tag      tag;
        tag_class_t tag_class;
    };
    string_ref name;
    i32        unknown;
    u32        tag_id;

    inline bool valid() const
    {
        return tag_id != std::numeric_limits<u32>::max();
    }

    inline string_ref to_name() const
    {
        return name;
    }

    inline bool matches(tag_class_t cls) const
    {
        return tag_class == cls;
    }
};

struct tag_t;

/*!
 * \brief A tag index contains information about the map, its resources and its
 * layout. Bitmaps, sounds, scenarios and etc. are referred to by this index.
 */
struct alignas(4) tag_index_t : stl_types::non_copy
{
    using vertex_array =
        reflexive_t<vert::vertex<vert::uncompressed>, xbox_variant>;
    using index_array = reflexive_t<vert::face, xbox_variant>;

    i32          index_magic;    /*!< Number used to adjust indexes*/
    u32          base_tag;       /*!< Base tag, smallest tag id */
    u32          vertex_size;    /*!< Size of vertex data*/
    u32          tagCount;       /*!< Number of tags*/
    vertex_array vertex_objects; /*!< Number of vertex objects*/
    index_array  index_objects;  /*!< Number of index objects*/
    union
    {
        bl_tag xbox_tag_sentinel;
        u32    raw_model_data_size;
    };
    bl_tag pc_tag_sentinel; /*!< Says "tags" */

    inline magic_data_t get_magic(file_header_t const* base) const
    {
        byte_t const* base_ptr   = C_RCAST<byte_t const*>(base);
        u32           index_size = base->version == version_t::xbox
                             ? sizeof(tag_index_t) - sizeof(u32)
                             : sizeof(tag_index_t);

        return magic_data_t{base_ptr,
                            index_magic - (base->tag_index_offset + index_size),
                            base->decomp_len};
    }

    inline vertex_array vertex_base() const
    {
        return vertex_objects;
    }

    inline index_array index_base() const
    {
        index_array base_reflexive = index_objects;
        base_reflexive.offset += vertex_objects.offset;
        return base_reflexive;
    }

    inline magic_data_t vertex_magic(magic_data_t const& base) const
    {
        return magic_data_t{base.base_ptr, 0, base.max_size};
    }

    /*!
     * \brief Get a pointer to the start of the tag index items, traversed as a
     * simple array \param file File header from which we get the pointer
     * \return A pointer to the first tag index item
     */
    inline tag_t const* tags(file_header_t const* header) const
    {
        szptr offset = header->version == version_t::xbox
                           ? sizeof(tag_index_t) - sizeof(u32)
                           : sizeof(tag_index_t);

        return C_RCAST<tag_t const*>(C_RCAST<byte_t const*>(this) + offset);
    }

    inline tag_t const* tag_at(const file_header_t* header, u32 tag_id) const;

    inline tag_t const& scenario(file_header_t const* header) const;

    /*!
     * \brief Get the tag index pointer out of the file header
     * \param file File header from which we want a tag index
     * \return A reference to the tag index
     */
    STATICINLINE tag_index_t const& from_header(file_header_t const* base)
    {
        byte_t const* base_ptr = C_RCAST<byte_t const*>(base);
        return *C_RCAST<tag_index_t const*>(&base_ptr[base->tag_index_offset]);
    }
};

/*!
 * \brief An item in the tag index, contains tag class (which identifies the
 * type of item), tag ID (a numerical ID), an offset to a proper string, and an
 * offset to its associated data.
 */
struct alignas(4) tag_t
{
    union
    {
        Array<bl_tag, 3>      tagclass; /*!< Strings which identify its class*/
        Array<tag_class_t, 3> tagclass_e; /*!< enum-ified tagclass value */
    };
    u32        tag_id;
    string_ref name;
    u32        offset; /*!< A byte offset to associated data*/
    u32        unknown;
    i32        padding;

    inline bool matches(tag_class_t other) const
    {
        for(auto tag : tagclass_e)
            if(tag == other)
                return true;
        return false;
    }

    template<typename T>
    reflexive_t<T> to_reflexive() const
    {
        if(padding != 0)
            Throw(undefined_behavior("invalid tag"));

        return {1, offset};
    }

    string_ref to_name() const
    {
        return name;
    }
};

tag_t const* tag_index_t::tag_at(file_header_t const* header, u32 tag_id) const
{
    if(tag_id > base_tag + tagCount)
        return nullptr;
    else
        return &(tags(header)[(tag_id - base_tag)]);
}

tag_t const& tag_index_t::scenario(file_header_t const* header) const
{
    tag_t const* tag = &(tags(header)[0]);

    if(!tag->matches(tag_class_t::scnr))
        Throw(undefined_behavior("Failed to locate scenario header"));

    return *tag;
}

struct shader_desc
{
    tagref_t ref;
    u32      padding[4];
};

struct alignas(4) shader_chicago
{
    struct map_t
    {
        u32      flags;
        u16      pad_;
        u32      pad_1[10];
        u32      color_func;
        u32      alpha_func;
        u32      pad_2[8];
        scalar   map[4];
        scalar   mip_bias;
        tagref_t map5;
        u32      pad_3[10];
    };

    u32 counter_limit;
    u32 flags;
    u32 map_type;

    struct
    {
        u32 blend;
        u32 fade;
        u32 fade_source;
        u32 pad[7];
    } fb;

    scalar   lens_flare_spacing;
    tagref_t lens_flare;

    reflexive_t<tagref_t> layers;
    reflexive_t<map_t>    maps;

    u32 extra_flags;
    u32 pad[2];
};

} // namespace blam
