#ifndef CBLAM_STRUCTURES
#define CBLAM_STRUCTURES

#include <coffee/core/CTypes>

namespace Coffee{
namespace CBlam{

typedef byte_t bl_tag[4];
typedef byte_t bl_string[32];
typedef byte_t bl_header[4];
typedef byte_t bl_footer[4];

/*!
 * \brief Blam maptypes. Names being obvious, the UI type does not give a playable map.
 */
enum class blam_maptype : int32
{
    singleplayer = 0, /*!< A single-player map, typically with cutscenes and AI*/
    multiplayer  = 1, /*!< A multi-player map, typically with up to 16 players*/
    ui           = 2, /*!< A UI map, used only in the main menu*/
};

/*!
 * \brief Possible Blam versions which we may encounter. The different formats work in different ways. For instance, Xbox stores bitmaps and other data in a single file while PC spreads it across "bitmaps.map" and "sounds.map". Both ways are necessary for parsing to happen correctly.
 */
enum class blam_version : int32
{
    xbox   = 5, /*!< The 2001 version of Halo: Combat Evolved for Xbox*/
    pc     = 7, /*!< The 2004 version of Halo: Combat Evolved for PC*/
};

constexpr cstring blam_index_item_type_mod2 = "2dom"; /*!< Tag class for models*/
constexpr cstring blam_index_item_type_bitm = "mtib"; /*!< Tag class for bitmaps*/
constexpr cstring blam_index_item_type_scnr = "rncs"; /*!< Tag class for scenarios*/
constexpr cstring blam_index_item_type_sbsp = "psbs"; /*!< Tag class for structured BSP*/

constexpr cstring blam_header_head = "deah"; /*!< Header of file header*/
constexpr cstring blam_header_foot = "toof"; /*!< Footer of file header*/

constexpr int32 blam_num_map_names = 28; /*!< Number of recognizable map names*/
constexpr _cbasic_static_map<bl_string,bl_string,28> blam_map_names = {
    //Single player maps
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

//Multi-player maps
{"beavercreek",    "Beaver Creek"},
{"bloodgulch",     "Blood Gulch"},
{"boardingaction", "Boarding Action"},
{"carousel",       "Derelict"},
{"chillout",       "Chill Out"},
{"deathisland",    "Death Island"},
{"damnation",      "Damnation"},
{"dangercanyon",   "Danger Canyon"},
{"gephyrophobia",  "Gephyrophobia"},
{"hangemhigh",     "Hang 'em High"},
{"icefields",      "Ice Fields"},
{"longest",        "Longest"},
{"prisoner",       "Prisoner"},
{"putput",         "Chiron TL34"},
{"ratrace",        "Rat Race"},
{"sidewinder",     "Sidewinder"},
{"timberland",     "Timberland"},
{"wizard",         "Wizard"},
}; /*!< A mapping of map names which this library can recognize. These are the stock maps.*/

/*!
 * \brief A file header located from the start of a map file
 */
struct blam_file_header
{
    bl_header id; /*!< Header value, should correspond with specific data*/
    blam_version version; /*!< Version of Halo, determines the process*/
    int32   decomp_len; /*!< Decompressed length, for Xbox where format is compressed. PC is uncompressed*/
    int32   unknown1;
    int32   tagIndexOffset; /*!< Offset to tag index*/
    int32   tagIndexMetaLen; /*!< Length of tag index item metadata*/
    int32   reserved_1[2];
    bl_string name; /*!< Name identifier for map*/
    bl_string buildDate; /*!< Build date for the map file*/
    blam_maptype mapType; /*!< Type of map, determines whether it is playable*/
    int32   unknown_4;
    int32   reserved_2[485];
    bl_footer footer; /*!< Footer value, should correspond with specific data*/
};

/*!
 * \brief A tag index contains information about the map, its resources and its layout. Bitmaps, sounds, scenarios and etc. are referred to by this index.
 */
struct blam_tag_index
{
    int32   index_magic; /*!< A magic number used to configure the pointers to data*/
    int32   baseTag; /*!< Base tag from which all tag IDs start from for this index*/
    int32   vertexSize; /*!< Size of vertex data*/
    int32   tagCount; /*!< Number of tags starting from the base tag*/
    int32   vertexObjectCount; /*!< Number of vertex objects*/
    uint32  vertexOffset; /*!< Offset to vertex data*/
    int32   indicesObjectCount; /*!< Number of index objects*/
    uint32  indicesOffset; /*!< Offset to index objects*/
    int32   modelRawDataSize; /*!< Raw model data size*/
    int32   tagStart; /*!< ???*/
};

/*!
 * \brief An item in the tag index, contains tag class (which identifies the type of item), tag ID (a numerical ID), an offset to a proper string, and an offset to its associated data.
 */
struct blam_index_item
{
    bl_tag  tagclass[3]; /*!< Strings which identify its class*/
    int32   tagId; /*!< A number representing its ID, only used for enumeration*/
    uint32  string_offset; /*! Magic data offset to a full string for the item*/
    int32   offset; /*!< A byte offset to associated data*/
    int32   zeroes[2];
};

/*!
 * \brief Points to a chunk of memory within the file
 */
template<typename T>
struct blam_reflexive
{
    int32 count; /*!< Size of data*/
    int32 offset; /*!< Offset to data within file (this will only refer to data within the map file)*/
    int32 zero;

    /*!
     * \brief Made for simplicity when working with reflexive data. Instead of several functions performing the same task, this template allows to access the data much more conveniently.
     * \param basePtr Base pointer of map in most cases
     * \param magic Magic number from tag index
     * \return A valid pointer if the reflexive is deemed valid (if the variable zero is indeed zero)
     */
    const T* data(const void* basePtr, szptr magic) const
    {
        if(zero != 0)
            return nullptr;
        const byte_t* b_basePtr = (const byte_t*)basePtr;
        return (const T*)(b_basePtr+offset-magic);
    }
};

struct blam_tagref
{
    bl_tag  tag;
    int32   string_offset;
    int32   unknown;
    int32   tagId;
};

/*!
 * \brief Blam, at least for Halo 1, uses int16 to store bitmap sizes
 */
typedef _cbasic_size_2d<int16> blam_size;
/*!
 * \brief As with blam_size, int16 is standard size for Halo 1.
 */
typedef _cbasic_point<int16> blam_point;

/*!
 * \brief Function pointers for blam bitmap processing, raw function pointer is much faster than std::function
 */
typedef uint32 (*BlamBitmProcess)(uint32,uint16,byte_t);

typedef CRGBA blam_rgba;

}
}

#endif
