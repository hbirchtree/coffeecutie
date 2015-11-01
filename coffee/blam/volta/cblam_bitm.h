#ifndef CBLAM_BITM
#define CBLAM_BITM

#include <coffee/core/coffee_types.h>
#include <coffee/core/coffee_macros.h>
#include <functional>
#include "cblam_map.h"

namespace Coffee{
namespace CBlam{

/*!
 * \brief These are the various texture formats found in the Blam engine
 */
enum blam_bitm_formats
{
    blam_bitm_format_A8         = 0x00, /*!< Alpha only*/
    blam_bitm_format_Y8         = 0x01, /*!< Luminance*/
    blam_bitm_format_AY8        = 0x02, /*!< Same bits all over*/
    blam_bitm_format_A8Y8       = 0x03, /*!< Alpha+luminance*/
    blam_bitm_format_R5G6B5     = 0x06, /*!< Short RGB format*/
    blam_bitm_format_A1R5G5B5   = 0x08, /*!< RGB + alpha-bit*/
    blam_bitm_format_A4R4G4B4   = 0x09, /*!< Short RGBA*/
    blam_bitm_format_X8R8G8B8   = 0x0A, /*!< RGB + extra bits*/
    blam_bitm_format_A8R8G8B8   = 0x0B, /*!< RGBA*/
    blam_bitm_format_DXT1       = 0x0E, /*!< S3TC format*/
    blam_bitm_format_DXT2AND3   = 0x0F, /*!< S3TC format*/
    blam_bitm_format_DXT4AND5   = 0x10, /*!< S3TC format*/
    blam_bitm_format_P8         = 0x11, /*!< Same as Y8, with a different name*/
};

/*!
 * \brief Converted texture formats we yield, DXT formats are loaded like any other S3TC data, requiring no definition of data store, only upload.
 */
enum blam_bitm_texture_format
{
    blam_bitm_tex_RGBA,

    blam_bitm_tex_DXT1,
    blam_bitm_tex_DXT3,
    blam_bitm_tex_DXT5,
};

/*!
 * \brief Texture types
 */
enum blam_bitm_types
{
    blam_bitm_type_2D   = 0x0, /*!< Typical 2D texture*/
    blam_bitm_type_3D   = 0x1, /*!< Volume texture*/
    blam_bitm_type_cube = 0x2, /*!< Cubemap used for skybox*/
};

/*!
 * \brief Texture flags
 */
enum blam_bitm_flags
{
    blam_bitm_flag_linear = 0x10,
};

/*!
 * \brief A bitmap header for images
 */
struct blam_bitm_header
{
    int32 unknown[22];
    int32 offset_first; /*!< Offset to the first header*/
    int32 unknown23;
    int32 imageCount; /*!< Count of images described by this header*/
    int32 imageOffset; /*!< Data offset to bitmap*/
    int32 unknown25;
};

struct blam_bitm_padding
{
    int32 unknown[16];
};

/*!
 * \brief A memory structure for Blam images containing all the necessary information to extract the data.
 */
struct blam_bitm_image
{
    int32       id;         /*!< A character string*/
    blam_size   isize;      /*!< Size of image*/
    int16       depth;      /*!< Depth bits for image*/
    int16       type;       /*!< Type of image*/
    int16       format;     /*!< Format of image*/
    int16       flags;      /*!< Flags present in image*/
    blam_point  reg_pnt;    /*!< I have no idea what this is.*/
    int16       mipmaps;    /*!< Number of mipmaps*/
    int16       pixOffset;  /*!< Pixel offset when in use*/
    int32       offset;     /*!< Data offset*/
    int32       size;       /*!< Data size in bytes*/
    int32       unknown[4];
};

/*!
 * \brief Data ready to be uploaded to the GL
 */
struct blam_bitm_texture_def
{
    int16       mipmaps     = 0;        /*!< Number of mipmaps, assumed to be r/2 per mipmap*/
    int16       type        = 0;        /*!< Texture type, 2D, 3D and cubes*/
    int16       format      = 0;        /*!< Texture format, DXT or RGBA*/
    blam_size   resolution;             /*!< Size of texture*/
    void*       data        = nullptr;  /*!< Pointer to described data*/
};

/*!
 * \brief To acquire a bitmap from a tag item
 * \param item Item which texture should be fetched
 * \param map File header from which we search
 * \param magic Magic tag number
 * \param numImages Number of images are written to this
 * \return
 */
extern const blam_bitm_image* coffee_bitm_get(
        const blam_index_item* item,
        const blam_file_header* map,
        int32 magic,
        int32 *numImages);

/*!
 * \brief Decode Blam bitmap into RGBA8 format
 * \param img
 * \param map Pointer to "bitmaps.map" data or the map data, depending on version of map format
 * \return A pointer to data which may be loaded into an OpenGL RGBA8 texture
 */
extern uint32* coffee_bitm_decode_image(
        const blam_bitm_image* img,
        const void* map);

/*!
 * \brief Debug function for dumping bitmap to file
 * \param img
 * \param bitmfile
 * \param filename
 */
extern void coffee_bitm_dump(
        const blam_bitm_image* img,
        const void* bitmfile,
        cstring filename);

/*!
 * \brief Decode a Blam bitmap and return GL-friendly data directly
 * \param img
 * \param bitmfile
 * \return A struct containing data readily digested for GL
 */
extern blam_bitm_texture_def coffee_bitm_get_texture(
        const blam_bitm_image* img,
        const void* bitmfile);

/*!
 * \brief Decoder which can be told how to decode each bit.
 * \param img
 * \param map
 * \param process This specifies how to process each pixel, from either of the inputs (uint32,uint16,byte) to uint32
 * \return
 */
extern uint32* coffee_bitm_decode_micro(
        const blam_bitm_image* img,
        const void* map,
        BlamBitmProcess process);

}
}

#endif
