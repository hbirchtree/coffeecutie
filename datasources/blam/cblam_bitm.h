#ifndef CBLAM_BITM
#define CBLAM_BITM

#include "cblam_map.h"
#include "coffee_types.h"
#include "coffee_macros.h"
#include <functional>

namespace Coffee{
namespace CBlam{

/*!
 * \brief Blam, at least for Halo 1, uses int16 to store bitmap sizes
 */
typedef _cbasic_size<int16> blam_size;
/*!
 * \brief As with blam_size, int16 is standard size for Halo 1.
 */
typedef _cbasic_point<int16> blam_point;

/*!
 * \brief Function pointers for blam bitmap processing, raw function pointer is much faster than std::function
 */
typedef uint32 (*BlamBitmProcess)(uint32,uint16,byte);

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
 * \brief A memory structure for Blam images containing all the necessary information to extract the data.
 */
struct blam_bitm_image
{
    int32   id; /*!< A character string*/
    blam_size isize; /*!< Size of image*/
    int16   depth; /*!< Depth bits for image*/
    int16   type; /*!< Type of image*/
    int16   format; /*!< Format of image*/
    int16   flags; /*!< Flags present in image*/
    blam_point reg_pnt;
    int16   mipmaps; /*!< Number of mipmaps*/
    int16   pixOffset; /*!< Pixel offset when in use*/
    int32   offset; /*!< Data offset*/
    int32   size; /*!< Data size in bytes*/
    int32   unknown[4];
};

/*!
 * \brief Stores mipmapped texture data, ready for upload to OpenGL
 */
struct blam_bitm_texture_data
{
    int16 mipmaps; /*!< Number of mipmaps*/
    const byte** data; /*!< Data buffers*/
    szptr* dsizes; /*!< Data buffer sizes*/
    blam_size* sizes; /*!< Mipmap sizes*/
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
 * \brief Calculate size of DXTC data
 * \param f
 * \param w
 * \param h
 * \return The size of described data
 */
extern szptr coffee_bitm_dxtc_getsize(int16 f, int16 w, int16 h);

/*!
 * \brief Decode Blam data to texture data (necessary for DXTC formats)
 * \param img
 * \param bitmfile
 * \return A structure containing all decoded data, including all mipmaps
 */
extern blam_bitm_texture_data coffee_bitm_decode_dxtc(
        const blam_bitm_image* img,
        const void* bitmfile);

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

/*!
 * \brief Convert RGBA struct to integer format containing the RGBA bytes
 * \param c
 * \return
 */
inline static uint32 blam_rgba_to_int(const blam_rgba &c)
{
    return (c.r << 24) | (c.g << 16) | (c.b << 8) | c.a;
}

/*!
 * \brief Decoder for the A8 format
 * \param d
 * \param s
 * \param b
 * \return
 */
inline static uint32 coffee_bitm_decode_m_a8(uint32 d, uint16 s,byte b)
{
    return b << 24;
}
/*!
 * \brief Decoder for the Y8 and P8 format
 * \param d
 * \param s
 * \param b
 * \return
 */
inline static uint32 coffee_bitm_decode_m_p8_y8(uint32 d, uint16 s,byte b)
{
    blam_rgba col;
    col.a = 0;
    col.r = col.g = col.b = b;
    return blam_rgba_to_int(col);
}
/*!
 * \brief Decoder for the AY8 format
 * \param d
 * \param s
 * \param b
 * \return
 */
inline static uint32 coffee_bitm_decode_m_ay8(uint32 d, uint16 s,byte b)
{
    blam_rgba col;
    col.r = col.g = col.b = col.a = b;
    return blam_rgba_to_int(col);
}
/*!
 * \brief Decoder for the A8Y8 format
 * \param d
 * \param s
 * \param b
 * \return
 */
inline static uint32 coffee_bitm_decode_m_a8y8(uint32 d, uint16 s,byte b)
{
    blam_rgba col;
    col.a = s & 0xFF;
    col.r = col.g = col.b = s >> 8;
    return blam_rgba_to_int(col);
}
/*!
 * \brief Decoder for the R5G6B5 format
 * \param d
 * \param s
 * \param b
 * \return
 */
inline static uint32 coffee_bitm_decode_m_r5g6b5(uint32 d, uint16 s,byte b)
{
    blam_rgba c;
    c.r = (((s >> 11) & 0x1F) * 0xFF) / 31;
    c.g = (((s >>  5) & 0x3F) * 0xFF) / 63;
    c.b = (((s      ) & 0x1F) * 0xFF) / 31;
    c.a = 255;
    return blam_rgba_to_int(c);
}
/*!
 * \brief Decoder for the A1R5G5B5 format
 * \param d
 * \param s
 * \param b
 * \return
 */
inline static uint32 coffee_bitm_decode_m_a1r5g5b5(uint32 d, uint16 s,byte b)
{
    blam_rgba c;
    c.a = (  s >> 15)         * 0xFF;
    c.r = (((s >> 10) & 0x1F) * 0xFF) / 31;
    c.g = (((s >>  5) & 0x3F) * 0xFF) / 31;
    c.b = (((s      ) & 0x1F) * 0xFF) / 31;
    return blam_rgba_to_int(c);
}
/*!
 * \brief Decoder for the A4R4G4B4 format
 * \param d
 * \param s
 * \param b
 * \return
 */
inline static uint32 coffee_bitm_decode_m_a4r4g4b4(uint32 d, uint16 s,byte b)
{
    blam_rgba c;
    c.a = (( s >> 12)         * 0xFF) / 15;
    c.r = (((s >>  8) & 0x0F) * 0xFF) / 15;
    c.g = (((s >>  4) & 0x0F) * 0xFF) / 15;
    c.b = (((s >>  0) & 0x0F) * 0xFF) / 15;
    return blam_rgba_to_int(c);
}
/*!
 * \brief Decoder for the ARGB8 format
 * \param d
 * \param s
 * \param b
 * \return
 */
inline static uint32 coffee_bitm_decode_m_a8r8g8b8(uint32 d, uint16 s,byte b)
{
    blam_rgba col;
    col.a = (d >> 24);
    col.r = (d >> 16) & 0xFF;
    col.g = (d >>  8) & 0xFF;
    col.b = (d      ) & 0xFF;
    return blam_rgba_to_int(col);
}
/*!
 * \brief Decoder for the XRGB8 format
 * \param d
 * \param s
 * \param b
 * \return
 */
inline static uint32 coffee_bitm_decode_m_x8r8g8b8(uint32 d, uint16 s,byte b)
{
    blam_rgba col;
    col.a = 0;
    col.r = (d >> 16) & 0xFF;
    col.g = (d >> 8) & 0xFF;
    col.b = (d) & 0xFF;
    return blam_rgba_to_int(col);
}

}
}

#endif
