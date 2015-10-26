#ifndef CBLAM_STRUCTURES
#define CBLAM_STRUCTURES

#include <coffee/core/coffee_types.h>

namespace Coffee{
namespace CBlam{
/*!
 * \brief Blam maptypes. Names being obvious, the UI type does not give a playable map.
 */
enum blam_maptype
{
    blam_maptype_singleplayer  = 0, /*!< A single-player map, typically with cutscenes and AI*/
    blam_maptype_multiplayer   = 1, /*!< A multi-player map, typically with up to 16 players*/
    blam_maptype_ui            = 2, /*!< A UI map, used only in the main menu*/
};

/*!
 * \brief Possible Blam versions which we may encounter. The different formats work in different ways. For instance, Xbox stores bitmaps and other data in a single file while PC spreads it across "bitmaps.map" and "sounds.map". Both ways are necessary for parsing to happen correctly.
 */
enum blam_versions
{
    blam_version_xbox   = 5, /*!< The 2001 version of Halo: Combat Evolved for Xbox*/
    blam_version_pc     = 7, /*!< The 2004 version of Halo: Combat Evolved for PC*/
};

constexpr cstring blam_index_item_type_mod2 = "2dom"; /*!< Tag class for models*/
constexpr cstring blam_index_item_type_bitm = "mtib"; /*!< Tag class for bitmaps*/
constexpr cstring blam_index_item_type_scnr = "rncs"; /*!< Tag class for scenarios*/

constexpr cstring blam_header_head = "deah"; /*!< Header of file header*/
constexpr cstring blam_header_foot = "toof"; /*!< Footer of file header*/

constexpr int32 blam_num_map_names = 28; /*!< Number of recognizable map names*/
constexpr struct mapnames { byte inname[32]; byte outname[32];} blam_map_names[28] = {
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
    int32   id; /*!< Header value, should correspond with specific data*/
    int32   version; /*!< Version of Halo, determines the process*/
    int32   decomp_len; /*!< Decompressed length, for Xbox where format is compressed. PC is uncompressed*/
    int32   unknown1;
    int32   tagIndexOffset; /*!< Offset to tag index*/
    int32   tagIndexMetaLen; /*!< Length of tag index item metadata*/
    int32   reserved_1[2];
    byte    name[32]; /*!< Name identifier for map*/
    byte    buildDate[32]; /*!< Build date for the map file*/
    int32   mapType; /*!< Type of map, determines whether it is playable*/
    int32   unknown_4;
    int32   reserved_2[485];
    int32   footer; /*!< Footer value, should correspond with specific data*/
};

/*!
 * \brief A tag index contains information about the map, its resources and its layout. Bitmaps, sounds, scenarios and etc. are referred to by this index.
 */
struct blam_tag_index
{
    int32   index_magic; /*! A magic number used to configure the pointers to data*/
    int32   baseTag; /*! Base tag from which all tag IDs start from for this index*/
    int32   vertexSize; /*! Size of vertex data*/
    int32   tagCount; /*! Number of tags starting from the base tag*/
    int32   vertexObjectCount; /*! Number of vertex objects*/
    uint32  vertexOffset; /*! Offset to vertex data*/
    int32   indicesObjectCount; /*! Number of index objects*/
    uint32  indicesOffset; /*! Offset to index objects*/
    int32   modelRawDataSize; /*! Raw model data size*/
    int32   tagStart; /*! ???*/
};

/*!
 * \brief An item in the tag index, contains tag class (which identifies the type of item), tag ID (a numerical ID), an offset to a proper string, and an offset to its associated data.
 */
struct blam_index_item
{
    byte    tagclass[3][4]; /*! Strings which identify its class*/
    int32   tagId; /*! A number representing its ID, only used for enumeration*/
    uint32  stringOffset; /*! Magic data offset to a full string for the item*/
    int32   offset; /*! A byte offset to associated data*/
    int32   zeroes[2];
};

/*!
 * \brief Points to a chunk of memory within the file, mostly with Xbox version
 */
struct blam_reflexive
{
    int32 count; /*! Size of data*/
    int32 offset; /*! Offset to data within file (this will only refer to data within the map file)*/
    int32 zero;
};

/*!
 * \brief Not yet implemented
 */
struct blam_tagref
{
    byte    tag[4];
    int32   namePtr;
    int32   unknown;
    int32   tagId;
};

}
}

#endif
