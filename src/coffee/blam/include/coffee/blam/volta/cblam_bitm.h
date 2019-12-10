#pragma once

#include "cblam_structures.h"

namespace blam {
namespace bitm {
/*!
 * \brief These are the various texture formats found in the Blam engine
 */
enum class format_t : u16
{
    A8       = 0x00, /*!< 000A -> GL_RED + GL_UNSIGNED_BYTE*/
    Y8       = 0x01, /*!< LLL0 -> GL_RED + GL_UNSIGNED_BYTE*/
    AY8      = 0x02, /*!< LLLL -> GL_RED + GL_UNSIGNED_BYTE*/
    A8Y8     = 0x03, /*!< LLLA1 -> GL_RG + GL_UNSIGNED_BYTE*/
    R5G6B5   = 0x06, /*!< R5G6B5 -> GL_RGB + GL_UNSIGNED_BYTE_5_6_5*/
    A1R5G5B5 = 0x08, /*!< RGB5A1 -> GL_RGB + GL_UNSIGNED_SHORT_5_5_5_1*/
    A4R4G4B4 = 0x09, /*!< RGBA4 -> GL_RGB + GL_UNSIGNED_SHORT_4_4_4_4*/
    X8R8G8B8 = 0x0A, /*!< RGBX8 -> GL_RGBA + GL_UNSIGNED_BYTE*/
    A8R8G8B8 = 0x0B, /*!< RGBA8 -> GL_RGBA + GL_UNSIGNED_BYTE*/
    DXT1     = 0x0E, /*!< DXT1*/
    DXT2AND3 = 0x0F, /*!< DXT3*/
    DXT4AND5 = 0x10, /*!< DXT5*/
    P8       = 0x11, /*!< LLL01 (See A8)*/
};

/*!
 * \brief Texture types
 */
enum class type_t : u16
{
    tex_2d    = 0x0, /*!< Typical 2D texture*/
    tex_3d    = 0x1, /*!< Volume texture*/
    tex_cude  = 0x2, /*!< Cubemap used for skybox*/
    tex_white = 0x3,
};

/*!
 * \brief Texture flags
 */
enum class flags_t : u16
{
    pow2       = 0x1,
    compressed = 0x2,
    palettized = 0x4,
    swizzled   = 0x8,
    linear     = 0x10,
    v16u16     = 0x20,

    unknown_1 = 0x80,
    unknown_2 = 0x100,
};

struct padding_t;
struct image_t;

/*!
 * \brief A bitmap header for images
 */
struct header_t
{
    i32                    unknown[21];
    reflexive_t<padding_t> padding_data;
    reflexive_t<image_t>   images;
};

struct padding_t
{
    i32 unknown[16];
};

/*!
 * \brief A memory structure for Blam images containing all the necessary
 * information to extract the data.
 */
struct image_t
{
    using img_data = semantic::mem_chunk<const char>;

    bl_tag     sentinel;  /*!< A character string*/
    bl_size_t  isize;     /*!< Size of image*/
    i16        depth;     /*!< Depth bits for image*/
    type_t     type;      /*!< Type of image*/
    format_t   format;    /*!< Format of image*/
    flags_t    flags;     /*!< Flags present in image*/
    bl_point_t reg_pnt;   /*!< I have no idea what this is.*/
    i16        mipmaps;   /*!< Number of mipmaps*/
    u16        pixOffset; /*!< Pixel offset when in use*/
    u32        offset;    /*!< Data offset*/
    u32        size;      /*!< Data size in bytes*/
    i32        unknown[4];

    inline semantic::BytesConst data(magic_data_t const& magic) const
    {
        reflexive_t<u8> img_data = {size, offset, 0};
        return img_data.data(magic);
    }
};

/*!
 * \brief Data ready to be uploaded to the GL, not part of Blam!
 */
struct texture_t
{
    size_3d<i32> resolution; /*!< Size of texture*/
    c_cptr       data;       /*!< Pointer to described data*/
    i16          mipmaps; /*!< Number of mipmaps, assumed to be r/2 per mipmap*/
    PixCmp       format;  /*!< Texture format, DXT or RGBA*/
    PixFmt       cformat; /*!< Compression format, if applicable*/
    BitFmt       dformat; /*!< Data format of texture data*/
    TexType      type;    /*!< Texture type, 2D, 3D and cubes*/
    uint16       blocksize; /*!< Block size of DXT* formats*/
};

C_FLAGS(flags_t, u16);

} // namespace bitm

} // namespace blam
