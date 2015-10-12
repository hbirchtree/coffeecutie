#ifndef CBLAM_MAP
#define CBLAM_MAP

#include "coffee_types.h"

namespace Coffee{

/*!
 * \brief Primarily created to compare tag classes, it compares part of memory
 * \param target Typically a non-null-terminated string
 * \param cmp Typically what is compared to
 * \param len The length of the comparison
 * \return
 */
extern bool coffee_cmp_memarea(cstring target, cstring cmp, szptr len);

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
    blam_version_h2     = 8, /*!< The Halo 2 map version*/
};

static cstring const blam_index_item_type_mod2 = "2dom"; /*!< Tag class for models*/
static cstring const blam_index_item_type_bitm = "mtib"; /*!< Tag class for bitmaps*/
static cstring const blam_index_item_type_scnr = "rncs"; /*!< Tag class for scenarios*/

static cstring const blam_header_head = "deah"; /*!< Character string for header of file header, used to determine validity of data*/
static cstring const blam_header_foot = "toof"; /*!< Character string for footer of file header, used to determine validity of data*/

const static int32 blam_num_map_names = 28; /*!< Number of recognizable map names*/
const static struct mapnames { byte inname[32]; byte outname[32];} blam_map_names[28] = {
//Single player maps
{"a10","Pillar of Autumn"},
{"a30", "Halo"},
{"a50", "Truth and Reconciliation"},
{"b30","Silent Cartographer"},
{"b40","Assault on the Control Room"},
{"c10","343 Guilty Spark"},
{"c20","The Library"},
{"c40","Two Betrayals"},
{"d20","Keyes"},
{"d40", "The Maw"},

//Multi-player maps
{"beavercreek","Beaver Creek"},
{"bloodgulch","Blood Gulch"},
{"boardingaction","Boarding Action"},
{"carousel","Derelict"},
{"chillout","Chill Out"},
{"deathisland","Death Island"},
{"damnation","Damnation"},
{"dangercanyon","Danger Canyon"},
{"gephyrophobia","Gephyrophobia"},
{"hangemhigh","Hang 'em High"},
{"icefields","Ice Fields"},
{"longest","Longest"},
{"prisoner","Prisoner"},
{"putput","Chiron TL34"},
{"ratrace","Rat Race"},
{"sidewinder","Sidewinder"},
{"timberland","Timberland"},
{"wizard","Wizard"},
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
 * \brief A mockup of the Halo 2 file header format, only speculation out of hex editor
 */
struct blam_file_header_h2
{
    byte    header[4];
    int32   version; /*!< Will be 8 for all Halo 2, Xbox and PC */
    int32   decomp_len;
    int32   unknown4;
    int32   tagCount2;
    byte    reserved1[280];
    byte    buildDate[32];
    int32   mapType;
    int32   tagCount;
    int32   unknown5;
    byte    unknown1[76];
    byte    name[32]; /*!< Short-hand name for map*/
    int32   unknown6;

    byte    scenario[64]; /*!< Scenario to load on start */

    byte    reserved2[196];
    byte    unknown2[48];
    byte    reserved3[1280];
    byte    footer[4];
};

/*!
 * \brief A mockup of the Halo 3 file header format, only speculation out of hex editor
 */
struct blam_file_header_h3
{
    byte    header[4];
    byte    unknown1[280];
    byte    buildDate[32];
    byte    unknown2[80];
    byte    name[32];
    int32   unknown3;
    byte    scenario[64];
    byte    unknown4[11788];
    byte    footer[4];
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

/*!
 * \brief Get the proper, full name of a map
 * \param map Map from which we identify a string
 * \return A null-terminated string with a map name or empty (empty if map name is not recognized)
 */
extern cstring blam_file_header_full_mapname(const blam_file_header* map);

/*!
 * \brief Validate and get the file header from a base pointer, mostly validation
 * \param baseptr Pointer to data which will be validated
 * \param expectedVersion Expected version of Halo to be parsed. If version does not match, fail.
 * \return Returns a casted pointer on success, nullptr on failure
 */
extern blam_file_header* blam_file_header_get(void* baseptr, int32 expectedVersion);

/*!
 * \brief Sets index magic for a tag index and modifies it with the correct magic number.
 * \param tagindex Tag index to update
 * \param tagIndexOffset Tag index offset from map, used to calculate magic
 */
extern void blam_tag_index_magic(
        blam_tag_index* tagindex,
        int32 tagIndexOffset);

/*!
 * \brief Get the tag index pointer out of the file header
 * \param file File header from which we want a tag index
 * \return A pointer to the tag index
 */
extern const blam_tag_index* blam_tag_index_ptr(const blam_file_header* file);

/*!
 * \brief Get a pointer to the start of the tag index items, traversed as a simple array
 * \param file File header from which we get the pointer
 * \return A pointer to the first tag index item
 */
extern const blam_index_item* blam_tag_meta_offset(const blam_file_header* file);

/*!
 * \brief Creates a copy of the tag index and sets up its magic index
 * \param file File header from which we extract a tag index
 * \return A tag index object copied from the file
 */
extern blam_tag_index blam_tag_index_get(const blam_file_header* file);

/*!
 * \brief Used to decode magic index pointers
 * \param base Pointer to the file header
 * \param magic Magic number from tag index
 * \param offset Data offset which will be transformed
 * \return A pointer to the described data
 */
extern const void* blam_magic_ptr(const void* base, int32 magic, int32 offset);

/*!
 * \brief Acquire the name string for a tag
 * \param idx Index item for which a string will be given
 * \param map Map file pointer
 * \param tagindex Index item's tag index for the index magic
 * \return A null-terminated string with the index item's name
 */
extern cstring blam_index_item_get_string(
        const blam_index_item* idx,
        const blam_file_header* map,
        const blam_tag_index* tagindex);

}
}

#endif //CBLAM_MAP
