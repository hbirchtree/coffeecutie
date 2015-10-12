#ifndef CBLAM_BITM
#define CBLAM_BITM

#include "cblam_map.h"
#include "coffee_types.h"
#include <functional>

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

struct blam_rgba
{
    uint32 r,g,b,a;
};

/*!
 * \brief Function pointers for blam bitmap processing, raw function pointer is much faster than std::function
 */
typedef uint32 (BlamBitmProcess)(uint32,byte);

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
 * \brief Decoder which can be told how to decode each bit.
 * \param img
 * \param map
 * \param process This specifies how to process each pixel, from a uint32 to another uint32
 * \return
 */
extern uint32* coffee_bitm_decode_micro(
        const blam_bitm_image* img,
        const void* map,
        BlamBitmProcess process);

inline static uint32 blam_rgba_to_int(const blam_rgba &c)
{
    return (c.r << 24) | (c.g << 16) | (c.b << 8) | c.a;
}

inline static uint32 coffee_bitm_decode_m_a8r8g8b8(uint32 d,byte b)
{
    blam_rgba col;
    col.a = (d >> 24);
    col.r = (d >> 16) & 0xff;
    col.g = (d >> 8) & 0xff;
    col.b = (d) & 0xff;

    return blam_rgba_to_int(col);
}
inline static uint32 coffee_bitm_decode_m_a8r8g8b8(uint32 d,byte b)
{
    blam_rgba col;
    col.a = 0;
    col.r = col.g = col.b = d;

    return blam_rgba_to_int(col);
}
inline static uint32 coffee_bitm_decode_m_x8r8g8b8(uint32 d,byte b)
{
    blam_rgba col;
    col.a = 0;
    col.r = col.g = col.b = d;

    return blam_rgba_to_int(col);
}
inline static uint32 coffee_bitm_decode_m_a8(uint32 d,byte b)
{
    return d << 24;
}

}
}

#endif
