#pragma once

/* In the future, we might use this for packing-sensitive structs */
#define BL_STRUCT struct

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
using typing::pixels::PixFmt;
using typing::vector_types::Vecf3;
using TexType = typing::graphics::TexType;

template<size_t Size>
struct bl_string_var
{
    char data[Size];

    stl_types::CString str() const
    {
        return stl_types::CString(data, Size);
    }

    operator cstring() const
    {
        return data;
    }
};
using bl_tag    = bl_string_var<4>;
using bl_string = bl_string_var<32>;
using bl_header = char[4];
using bl_footer = char[4];

struct tagref_t
{
    union
    {
        bl_tag      tag;
        tag_class_t tag_class;
    };
    u32 string_offset;
    i32 unknown;
    u32 tag_id;
};

struct magic_data_t
{
    c_cptr base_ptr;
    i32    magic_offset;
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
};

constexpr cstring header_head = "deah"; /*!< Header of file header*/
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
struct file_header_t
{
    bl_header id;      /*!< Header value, should correspond with specific data*/
    version_t version; /*!< Version of Halo, determines the process*/
    i32       decomp_len; /*!< Decompressed length, for Xbox where format is
                             compressed. PC is uncompressed*/
    i32       unknown1;
    i32       tagIndexOffset;  /*!< Offset to tag index*/
    i32       tagIndexMetaLen; /*!< Length of tag index item metadata*/
    i32       reserved_1[2];
    bl_string name;      /*!< Name identifier for map*/
    bl_string buildDate; /*!< Build date for the map file*/
    maptype_t mapType;   /*!< Type of map, determines whether it is playable*/
    i32       unknown_4;
    i32       reserved_2[485];
    bl_footer footer; /*!< Footer value, should correspond with specific data*/
};

/*!
 * \brief A tag index contains information about the map, its resources and its
 * layout. Bitmaps, sounds, scenarios and etc. are referred to by this index.
 */
struct tag_index_t
{
    i32 index_magic; /*!< A magic number used to configure the pointers to
                        data*/
    i32 baseTag;     /*!< Base tag from which all tag IDs start from for this
                        index*/
    i32 vertexSize;  /*!< Size of vertex data*/
    i32 tagCount;    /*!< Number of tags starting from the base tag*/
    i32 vertexObjectCount;  /*!< Number of vertex objects*/
    u32 vertexOffset;       /*!< Offset to vertex data*/
    i32 indicesObjectCount; /*!< Number of index objects*/
    u32 indicesOffset;      /*!< Offset to index objects*/
    i32 modelRawDataSize;   /*!< Raw model data size*/
    i32 tagStart;           /*!< ???*/
};

struct xbox_reflexive
{
};
struct pc_reflexive
{
};

/*!
 * \brief Points to a chunk of memory within the file
 */
template<
    typename T,
    typename RType = pc_reflexive,
    bool           = std::is_same<RType, pc_reflexive>::value>
struct reflexive_t
{
    i32 count;  /*!< Number of elements */
    i32 offset; /*!< Offset to data within file (this will only refer to data
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

        if(count == 0 || zero != 0)
            return Output();

        byte_t const* base_ptr = C_CAST<const byte_t*>(magic.base_ptr);
        return Output::From(
            C_RCAST<T const*>(base_ptr + offset - magic.magic_offset), count);
    }
};

template<typename T, typename RType>
struct reflexive_t<T, RType, false>
{
    i32 count;
    i32 offset;
};

/*!
 * \brief An item in the tag index, contains tag class (which identifies the
 * type of item), tag ID (a numerical ID), an offset to a proper string, and an
 * offset to its associated data.
 */
struct index_item_t
{
    union
    {
        Array<bl_tag, 3>      tagclass; /*!< Strings which identify its class*/
        Array<tag_class_t, 3> tagclass_e; /*!< enum-ified tagclass value */
    };
    u32 tag_id; /*!< A number representing its ID, only used for enumeration*/
    u32 string_offset; /*! Magic data offset to a full string for the item*/
    i32 offset;        /*!< A byte offset to associated data*/
    i32 zeroes[2];

    inline bool matches(tag_class_t other) const
    {
        for(auto tag : tagclass_e)
            if(tag == other)
                return true;
        return false;
    }

    template<
        typename T,
        typename std::enable_if<std::is_class<T>::value, bool>::type* = nullptr>
    reflexive_t<T> to_reflexive() const
    {
        return {1, offset};
    }
};

} // namespace blam
