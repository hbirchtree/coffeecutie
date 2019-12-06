#pragma once

#include "cblam_structures.h"

namespace blam {

/*!
 * \brief These are the various texture formats found in the Blam engine
 */
enum class bitm_format : uint16
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
    i32 unknown[22];
    i32 offset_first; /*!< Offset to the bitm_padding_t structure */
    i32 unknown23;
    i32 imageCount;  /*!< Count of images described by this header*/
    i32 imageOffset; /*!< Data offset to bitm_image_t */
    i32 unknown25;

    reflexive_t<bitm_image_t> image_headers() const;
    reflexive_t<char>         image_ptr(bitm_image_t const* img) const;
};

struct bitm_padding_t
{
    i32 unknown[16];
};

/*!
 * \brief A memory structure for Blam images containing all the necessary
 * information to extract the data.
 */
struct bitm_image_t
{
    i32          id;        /*!< A character string*/
    bl_size_t    isize;     /*!< Size of image*/
    i16          depth;     /*!< Depth bits for image*/
    bitm_type_t  type;      /*!< Type of image*/
    bitm_format  format;    /*!< Format of image*/
    bitm_flags_t flags;     /*!< Flags present in image*/
    bl_point_t   reg_pnt;   /*!< I have no idea what this is.*/
    i16          mipmaps;   /*!< Number of mipmaps*/
    i16          pixOffset; /*!< Pixel offset when in use*/
    i32          offset;    /*!< Data offset*/
    i32          size;      /*!< Data size in bytes*/
    i32          unknown[4];
};

/*!
 * \brief Data ready to be uploaded to the GL, not part of Blam!
 */
struct bitm_texture_t
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

FORCEDINLINE
reflexive_t<bitm_image_t> bitm_header_t::image_headers() const
{
    if(imageCount)
        return {sizeof(bitm_image_t), imageOffset, 0};
    else
        return {0, 0, 0};
}

FORCEDINLINE
reflexive_t<char> bitm_header_t::image_ptr(bitm_image_t const* img) const
{
    if(!img)
        return {};

    return {img->size, img->offset, 0};
}

C_FLAGS(bitm_flags_t, u16);

/*!
 * \brief To acquire a bitmap from a tag item
 * \param item Item which texture should be fetched
 * \param map File header from which we search
 * \param magic Magic tag number
 * \param numImages Number of images are written to this
 * \return
 */
extern const bitm_image_t* bitm_get(
    index_item_t const*  item,
    file_header_t const* map,
    i32                  magic,
    i32*                 numImages);

/*!
 * \brief Transforms the image structure to a GL-ready structure which may be
 * uploaded directly, doing any possible decoding on the GPU. The data is read
 * directly from the source file without copying. (Unless the GL does this)
 * \param img
 * \param bitmfile
 * \return A struct containing data readily digested for GL
 */
extern bitm_texture_t bitm_get_texture(
    bitm_image_t const* img, c_cptr bitmfile);

} // namespace blam
