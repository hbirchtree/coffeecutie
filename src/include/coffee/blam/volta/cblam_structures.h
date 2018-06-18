#ifndef CBLAM_STRUCTURES
#define CBLAM_STRUCTURES

/* In the future, we might use this for packing-sensitive structs */
#define BL_STRUCT struct

#include <coffee/core/types/cdef/geometry.h>
#include <coffee/core/types/cdef/pixtypes.h>
#include <coffee/core/types/edef/graphicsenum.h>
#include <coffee/core/types/edef/pixenum.h>
#include <coffee/core/types/edef/resenum.h>
#include <coffee/core/types/map.h>

#include "blam_tag_classes.h"

namespace Coffee{
namespace Blam{

using bl_tag = char[4];
using bl_string = char[32];
using bl_header = char [4];
using bl_footer = char[4];

struct tagref_t
{
    bl_tag  tag;
    int32   string_offset;
    int32   unknown;
    int32   tagId;
};

/*!
 * \brief Blam, at least for Halo 1, uses int16 to store bitmap sizes
 */
using bl_size_t = _cbasic_size_2d<int16>;
/*!
 * \brief As with blam_size, int16 is standard size for Halo 1.
 */
using bl_point_t = _cbasic_point_2d<int16>;

/*!
 * \brief Function pointers for blam bitmap processing, raw function pointer is much faster than Function
 */
using BitmProcess = uint32(*)(uint32,uint16,byte_t);

using bl_rgba_t = CRGBA;

/*!
 * \brief Blam maptypes. Names being obvious, the UI type does not give a playable map.
 */
enum class maptype_t : int32
{
    singleplayer = 0, /*!< A single-player map, typically with cutscenes and AI*/
    multiplayer  = 1, /*!< A multi-player map, typically with up to 16 players*/
    ui           = 2, /*!< A UI map, used only in the main menu*/
};

/*!
 * \brief Possible Blam versions which we may encounter. The different formats work in different ways. For instance, Xbox stores bitmaps and other data in a single file while PC spreads it across "bitmaps.map" and "sounds.map". Both ways are necessary for parsing to happen correctly.
 */
enum class version_t : int32
{
    xbox   = 5, /*!< The 2001 version of Halo: Combat Evolved for Xbox*/
    pc     = 7, /*!< The 2004 version of Halo: Combat Evolved for PC*/
};

constexpr cstring index_item_type_mod2 = "2dom"; /*!< Tag class for models*/
constexpr cstring index_item_type_bitm = "mtib"; /*!< Tag class for bitmaps*/
constexpr cstring index_item_type_scnr = "rncs"; /*!< Tag class for scenarios*/
constexpr cstring index_item_type_sbsp = "psbs"; /*!< Tag class for structured BSP*/

constexpr cstring header_head = "deah"; /*!< Header of file header*/
constexpr cstring header_foot = "toof"; /*!< Footer of file header*/

constexpr int32 blam_num_map_names = 28; /*!< Number of recognizable map names*/
constexpr _cbasic_static_map<char[15],char[28],28>
blam_map_names = {
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
struct file_header_t
{
    bl_header id; /*!< Header value, should correspond with specific data*/
    version_t version; /*!< Version of Halo, determines the process*/
    int32   decomp_len; /*!< Decompressed length, for Xbox where format is compressed. PC is uncompressed*/
    int32   unknown1;
    int32   tagIndexOffset; /*!< Offset to tag index*/
    int32   tagIndexMetaLen; /*!< Length of tag index item metadata*/
    int32   reserved_1[2];
    bl_string name; /*!< Name identifier for map*/
    bl_string buildDate; /*!< Build date for the map file*/
    maptype_t mapType; /*!< Type of map, determines whether it is playable*/
    int32   unknown_4;
    int32   reserved_2[485];
    bl_footer footer; /*!< Footer value, should correspond with specific data*/
};

/*!
 * \brief A tag index contains information about the map, its resources and its layout. Bitmaps, sounds, scenarios and etc. are referred to by this index.
 */
struct tag_index_t
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


template<typename T>
struct reflexive_t;

/*!
 * \brief An item in the tag index, contains tag class (which identifies the type of item), tag ID (a numerical ID), an offset to a proper string, and an offset to its associated data.
 */
struct index_item_t
{
    union{
        bl_tag  tagclass[3]; /*!< Strings which identify its class*/
        tag_class_t tagclass_e[3]; /*!< enum-ified tagclass value */
    };
    int32   tagId; /*!< A number representing its ID, only used for enumeration*/
    uint32  string_offset; /*! Magic data offset to a full string for the item*/
    int32   offset; /*!< A byte offset to associated data*/
    int32   zeroes[2];

    template<
            typename T,
            typename std::enable_if<std::is_class<T>::value,
                                    bool>::type* = nullptr>
    reflexive_t<T> to_reflexive() const
    {
        return {sizeof(T), offset};
    }
};

/*!
 * \brief Points to a chunk of memory within the file
 */
template<typename T>
struct reflexive_t
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
    const T* data(c_cptr basePtr, u32 magic) const
    {
        if(count == 0 || basePtr == nullptr || zero != 0)
            return nullptr;
        const byte_t* b_basePtr = C_CAST<const byte_t*>(basePtr);
        return (const T*)(b_basePtr+offset-magic);
    }
};

/*!
 * \brief These are the various texture formats found in the Blam engine
 */
enum class bitm_format : uint16
{
    A8         = 0x00, /*!< 000A -> GL_RED + GL_UNSIGNED_BYTE*/
    Y8         = 0x01, /*!< LLL0 -> GL_RED + GL_UNSIGNED_BYTE*/
    AY8        = 0x02, /*!< LLLL -> GL_RED + GL_UNSIGNED_BYTE*/
    A8Y8       = 0x03, /*!< LLLA1 -> GL_RG + GL_UNSIGNED_BYTE*/
    R5G6B5     = 0x06, /*!< R5G6B5 -> GL_RGB + GL_UNSIGNED_BYTE_5_6_5*/
    A1R5G5B5   = 0x08, /*!< RGB5A1 -> GL_RGB + GL_UNSIGNED_SHORT_5_5_5_1*/
    A4R4G4B4   = 0x09, /*!< RGBA4 -> GL_RGB + GL_UNSIGNED_SHORT_4_4_4_4*/
    X8R8G8B8   = 0x0A, /*!< RGBX8 -> GL_RGBA + GL_UNSIGNED_BYTE*/
    A8R8G8B8   = 0x0B, /*!< RGBA8 -> GL_RGBA + GL_UNSIGNED_BYTE*/
    DXT1       = 0x0E, /*!< DXT1*/
    DXT2AND3   = 0x0F, /*!< DXT3*/
    DXT4AND5   = 0x10, /*!< DXT5*/
    P8         = 0x11, /*!< LLL01 (See A8)*/
};

/*!
 * \brief Texture types
 */
enum class bitm_type_t : uint16
{
    T2D   = 0x0, /*!< Typical 2D texture*/
    T3D   = 0x1, /*!< Volume texture*/
    TCube = 0x2, /*!< Cubemap used for skybox*/
};

/*!
 * \brief Texture flags
 */
enum class bitm_flags_t : uint16
{
    linear = 0x10,
};

struct bitm_image_t;

/*!
 * \brief A bitmap header for images
 */
struct bitm_header_t
{
    int32 unknown[22];
    int32 offset_first; /*!< Offset to the bitm_padding_t structure */
    int32 unknown23;
    int32 imageCount; /*!< Count of images described by this header*/
    int32 imageOffset; /*!< Data offset to bitm_image_t */
    int32 unknown25;

    reflexive_t<bitm_image_t> image_headers() const;
    reflexive_t<char> image_ptr(bitm_image_t const* img) const;
};

struct bitm_padding_t
{
    int32 unknown[16];
};

/*!
 * \brief A memory structure for Blam images containing all the necessary information to extract the data.
 */
struct bitm_image_t
{
    int32        id;         /*!< A character string*/
    bl_size_t    isize;      /*!< Size of image*/
    int16        depth;      /*!< Depth bits for image*/
    bitm_type_t  type;       /*!< Type of image*/
    bitm_format  format;     /*!< Format of image*/
    bitm_flags_t flags;      /*!< Flags present in image*/
    bl_point_t   reg_pnt;    /*!< I have no idea what this is.*/
    int16        mipmaps;    /*!< Number of mipmaps*/
    int16        pixOffset;  /*!< Pixel offset when in use*/
    int32        offset;     /*!< Data offset*/
    int32        size;       /*!< Data size in bytes*/
    int32        unknown[4];
};

/*!
 * \brief Data ready to be uploaded to the GL
 */
struct bitm_texture_t
{
    CSize3    resolution;  /*!< Size of texture*/
    c_cptr    data;        /*!< Pointer to described data*/
    int16     mipmaps;     /*!< Number of mipmaps, assumed to be r/2 per mipmap*/
    PixCmp    format;      /*!< Texture format, DXT or RGBA*/
    PixFmt    cformat;     /*!< Compression format, if applicable*/
    BitFormat dformat;     /*!< Data format of texture data*/
    TexType   type;        /*!< Texture type, 2D, 3D and cubes*/
    uint16    blocksize;   /*!< Block size of DXT* formats*/
};

FORCEDINLINE
reflexive_t<bitm_image_t> bitm_header_t::image_headers(
        ) const
{
    if(imageCount)
        return {sizeof(bitm_image_t), imageOffset, 0};
    else
        return {0, 0, 0};
}

FORCEDINLINE
reflexive_t<char> bitm_header_t::image_ptr(
        bitm_image_t const* img) const
{
    if(!img)
        return {};

    return {img->size, img->offset, 0};
}

C_FLAGS(bitm_flags_t, u16);

}
}

#endif
