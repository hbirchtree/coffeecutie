#pragma once

#include "cblam_structures.h"

#include <coffee/interfaces/cgraphics_pixops.h>
#include <peripherals/typing/enum/pixels/format_transform.h>

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
    tex_cube  = 0x2, /*!< Cubemap used for skybox*/
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

C_FLAGS(flags_t, u16);

struct header_t;

struct locator_block
{
    string_ref name;
    u32        size;
    u32        offset;

    inline reflexive_t<header_t> to_reflexive() const
    {
        return {1, offset, 0};
    }
};

struct bitmap_header_t
{
    u32 unknown;
    u32 name_block_off;

    u32 locators_offset;
    u32 locators_count;

    static inline bitmap_header_t const* from_data(semantic::Bytes const& data)
    {
        return C_RCAST<bitmap_header_t const*>(data.data);
    }

    inline cstring get_name(locator_block const& block) const
    {
        auto base_ptr = C_RCAST<byte_t const*>(this);

        return C_RCAST<cstring>(base_ptr + name_block_off + block.name.offset);
    }

    inline semantic::mem_chunk<locator_block const> locators() const
    {
        auto base_ptr = C_RCAST<byte_t const*>(this);

        return semantic::mem_chunk<locator_block const>::From(
            C_RCAST<locator_block const*>(base_ptr + locators_offset),
            locators_count);
    }

    inline magic_data_t block_magic(
        magic_data_t const& bitm, u32 bitm_idx) const
    {
        auto const& loc = locators()[bitm_idx];
        return magic_data_t(
            bitm.base_ptr + loc.offset, 0, bitm.max_size - loc.offset);
    }
    inline reflexive_t<header_t> get_block(u32 bitm_idx = 0) const
    {
        auto const& loc = locators()[bitm_idx];
        return loc.to_reflexive();
    }
};

struct padding_t;
struct image_t;

/*!
 * \brief A bitmap header for images
 */
struct header_t
{
    union
    {
        struct
        {
            //            u32    _something;
            u32    type;
            u32    format;
            u32    usage;
            u32    flags;
            scalar fade_factor;
            scalar sharpening;
            scalar bump_amplitude;
            u32    sprite_budget;
            u16    sprite_budget_count;
            u16    color_plate_w;
            u16    pixel_data;
            scalar blur_size;
            scalar alpha_bias;
            u16    mip_count;
            u32    sprite_usage;
            u16    sprite_spacing;
            u32    _pad[2];
        };
        i32 unknown[21];
    };

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
    u16        mipmaps;   /*!< Number of mipmaps*/
    u16        pixOffset; /*!< Pixel offset when in use*/
    u32        offset;    /*!< Data offset*/
    u32        size;      /*!< Data size in bytes*/
    u32        unknown[4];

    inline bool compressed() const
    {
        return enum_helpers::feval(flags & flags_t::compressed);
    }

    inline stl_types::Tup<BitFmt, PixCmp> to_fmt() const
    {
        switch(format)
        {
        case format_t::A8:
        case format_t::Y8:
        case format_t::P8:
        case format_t::AY8:
            return {BitFmt::UByte, PixCmp::R};

        case format_t::A8Y8:
            return {BitFmt::UByte, PixCmp::RG};
        case format_t::R5G6B5:
            return {BitFmt::UShort_565, PixCmp::RGB};
        case format_t::A1R5G5B5:
            return {BitFmt::UShort_1555R, PixCmp::RGBA};
        case format_t::A4R4G4B4:
            return {BitFmt::UShort_4444, PixCmp::RGBA};

        case format_t::A8R8G8B8:
            return {BitFmt::UByte, PixCmp::RGBA};
        case format_t::X8R8G8B8:
            return {BitFmt::UByte, PixCmp::RGBA};

        default:
            return {BitFmt::Undefined, PixCmp::None};
        }
    }

    inline PixFmt to_pixfmt() const
    {
        switch(format)
        {
        case format_t::A8:
        case format_t::Y8:
        case format_t::P8:
        case format_t::AY8:
            return PixFmt::R8;
        case format_t::R5G6B5:
            return PixFmt::RGB565;
        case format_t::A8Y8:
            return PixFmt::RG8;
        case format_t::A1R5G5B5:
            return PixFmt::RGB5A1;
        case format_t::A4R4G4B4:
            return PixFmt::RGBA4;
        case format_t::A8R8G8B8:
            return PixFmt::RGBA8;
        case format_t::X8R8G8B8:
            return PixFmt::RGBA8;
        case format_t::DXT1:
        case format_t::DXT2AND3:
        case format_t::DXT4AND5:
            return PixFmt::DXTn;
        }
    }

    inline stl_types::Tup<PixFmt, CompFlags> to_compressed_fmt() const
    {
        switch(format)
        {
        case format_t::DXT1:
            return {PixFmt::DXTn, CompFlags::S3TC_1};
        case format_t::DXT2AND3:
            return {PixFmt::DXTn, CompFlags::S3TC_3};
        case format_t::DXT4AND5:
            return {PixFmt::DXTn, CompFlags::S3TC_5};
        }
    }

    inline semantic::BytesConst data(
        magic_data_t const& magic, u16 mipmap = 0) const
    {
        using namespace typing::pixels::properties;

        if(mipmap != 0 && mipmap >= mipmaps)
            Throw(undefined_behavior("mipmap out of range"));

        if(mipmap != 0 && !enum_helpers::feval(flags, flags_t::pow2))
            Throw(undefined_behavior("non-pow2 texture cannot be mipmapped"));

        auto mipsize = isize;
        mipsize.w <<= mipmap;
        mipsize.h <<= mipmap;

        if(!compressed())
        {
            BitFmt data_fmt;
            PixCmp data_layout;
            std::tie(data_fmt, data_layout) = to_fmt();

            u32 size = get<pixel_size>(data_fmt, data_layout, 1) *
                       mipsize.convert<u32>().area();
            u32 mip_offset = 0;

            for(auto i : stl_types::Range<>(mipmap))
                mip_offset += (isize.area() >> (2 * i));

            return reflexive_t<u8>{size, offset + mip_offset, 0}.data(magic);
        } else
        {
            PixFmt    fmt;
            CompFlags flags;
            std::tie(fmt, flags) = to_compressed_fmt();
            auto comp_fmt        = typing::pixels::CompFmt(fmt, flags);

            u32 size = Coffee::GetPixCompressedSize(comp_fmt, mipsize);

            u32  mip_offset = 0;
            auto off_size   = isize;
            for(auto i : stl_types::Range<>(mipmap))
            {
                mip_offset += Coffee::GetPixCompressedSize(comp_fmt, off_size);
                off_size.w << 1;
                off_size.h << 1;
            }
            return reflexive_t<u8>{size, offset + mip_offset, 0}.data(magic);
        }
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

} // namespace bitm

} // namespace blam
