#ifndef CBLAM_BITM
#define CBLAM_BITM

#include "cblam_map.h"
#include "coffee_types.h"

namespace Coffee{
namespace CBlam{

/*!
 * \brief These are the various texture formats found in the Blam engine
 */
enum blam_bitm_formats
{
    blam_bitm_format_A8         = 0x00, /*!< Alpha map?*/
    blam_bitm_format_Y8         = 0x01, /*!< */
    blam_bitm_format_AY8        = 0x02, /*!< */
    blam_bitm_format_A8Y8       = 0x03, /*!< */
    blam_bitm_format_R5G6B5     = 0x06, /*!< */
    blam_bitm_format_A1R5G5B5   = 0x08, /*!< */
    blam_bitm_format_A4R4G4B4   = 0x09, /*!< */
    blam_bitm_format_X8R8G8B8   = 0x0A, /*!< */
    blam_bitm_format_A8R8G8B8   = 0x0B, /*!< Run-of-the-mill RGBA, still needs to be converted*/
    blam_bitm_format_DXT1       = 0x0E, /*!< S3TC format*/
    blam_bitm_format_DXT2AND3   = 0x0F, /*!< S3TC format*/
    blam_bitm_format_DXT4AND5   = 0x10, /*!< S3TC format*/
    blam_bitm_format_P8         = 0x11, /*!< */
};

/*!
 * \brief Texture types
 */
enum blam_bitm_types
{
    blam_bitm_type_2D   = 0x0, /*!< A typical 2D texture*/
    blam_bitm_type_3D   = 0x1, /*!< A volume texture?*/
    blam_bitm_type_cube = 0x2, /*!< A cubemap used for skybox*/
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
    int32   id; /*!< A character string*/
    _cbasic_size<int16> isize; /*!< Size of image*/
    int16   depth; /*!< Depth bits for image*/
    int16   type; /*!< Type of image*/
    int16   format; /*!< Format of image*/
    int16   flags; /*!< Flags present in image*/
    _cbasic_point<int16> reg_pnt;
    int16   mipmaps; /*!< Number of mipmaps*/
    int16   pixOffset; /*!< Pixel offset when in use*/
    int32   offset; /*!< Data offset*/
    int32   size; /*!< Data size in bytes*/
    int32   unknown[4];
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
 * \brief Decode A8R8G8B8 data into something readable by GL. The resulting data may be loaded directly into GL and rendered on a texture. The acceptable format to display this data is GL_RGBA8. To note, this allocates memory which will be used to store the texture data.
 * \param img Image which will be extracted
 * \param map Pointer to bitmap data or file header, depending on the situation. Halo PC usually stores textures in a "bitmaps.map" file, while some third-party content stores this same data in the map file, a-la Xbox does.
 * \return A pointer to texture data for GL to use
 */
extern uint32 *coffee_bitm_decode_a8r8g8b8(const blam_bitm_image* img,
        const void *map);

}
}

#endif
