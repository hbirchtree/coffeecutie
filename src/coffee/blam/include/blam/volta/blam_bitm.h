#pragma once

#include "blam_magic_data.h"
#include "blam_reference.h"
#include "blam_structures.h"
#include "blam_tag_index.h"

#include <peripherals/typing/enum/pixels/components.h>
#include <peripherals/typing/enum/pixels/format.h>
#include <peripherals/typing/enum/pixels/format_transform.h>

#if defined(GLEAM_USE_CORE) || defined(GLEAM_USE_ES) || defined(GLEAM_DUMMY)
#include <glw/texture_formats.h>
#include <glw/texture_formats_desc.h>
#include <peripherals/stl/range.h>
#endif

namespace blam::bitm {

using typing::pix_components;

/*!
 * \brief These are the various texture formats found in the Blam engine
 */
enum class format_t : u16
{
    A8     = 0x00, /*!< 000A -> GL_RED + GL_UNSIGNED_BYTE*/
    Y8     = 0x01, /*!< LLL0 -> GL_RED + GL_UNSIGNED_BYTE*/
    AY8    = 0x02, /*!< LLLL -> GL_RED + GL_UNSIGNED_BYTE*/
    A8Y8   = 0x03, /*!< LLLA1 -> GL_RG + GL_UNSIGNED_BYTE*/
    R5G6B5 = 0x06, /*!< R5G6B5 -> GL_RGB + GL_UNSIGNED_BYTE_5_6_5*/
    A1RGB5 = 0x08, /*!< RGB5A1 -> GL_RGB + GL_UNSIGNED_SHORT_5_5_5_1*/
    ARGB4  = 0x09, /*!< RGBA4 -> GL_RGB + GL_UNSIGNED_SHORT_4_4_4_4*/
    XRGB8  = 0x0A, /*!< RGBX8 -> GL_RGBA + GL_UNSIGNED_BYTE*/
    ARGB8  = 0x0B, /*!< RGBA8 -> GL_RGBA + GL_UNSIGNED_BYTE*/
    BC1    = 0x0E, /*!< BC1/DXT1 */
    BC2    = 0x0F, /*!< BC2/DXT2/3*/
    BC3    = 0x10, /*!< BC3/DXT5*/
    P8     = 0x11, /*!< LLL01 (See A8), Halo 2 calls this P8-bump */

    /* Halo 2 additions; same numbering scheme, shared with blam::dimeter */
    P8_flat = 0x12,
    ARGBF32 = 0x13,
    RGBF32  = 0x14,
    RGBF16  = 0x15,
    V8U8    = 0x16, /*!< Signed RG8, used for bump maps */
    G8B8    = 0x17,

    /* Range in which platforms-specific transcoded formats live
     * Used for weaker platforms where we want to transcode for better perf
     */
    PLATFORM_SPECIFIC_MASK = 0xFF00,

    /* For GX, does not collide with the OpenGL ones */
    BC1_GX_TILED    = 0x100, /*!< GX's tiled DXT1 */
    RGB565_GX_TILED = 0x200, /*!< RGB565 tiled for GX */
    I8_GX_TILED     = 0x300, /*!< I8 tiled for GX */
    IA8_GX_TILED    = 0x400, /*!< IA8 tiled for GX*/

    /* For OpenGL ES targets */
    PVRTCV1_RGBA = 0x100, /*!< PVRTCv1 RGBA format */
    PVRTCV1_RGB  = 0x200, /*!< PVRTCv1 RGBA format */
    ETC1_RGB     = 0x300, /*!< ETC1 RGB format */
    ETC1_RGBA    = 0x400, /*!< ETC1 RGB + ETC1 RGB to produce RGBA */
    ETC2_RGB     = 0x500, /*!< ETC2 RGB8 format */
    ETC2_RGBA    = 0x600, /*!< ETC2 RGBA8 format */

    /* Desktop GL usually fulfills all the formats, so no need */
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
    shared    = 0x100,
};

C_FLAGS(flags_t, u16);

struct header_t;

struct padding_t;
struct image_t;
struct sequence_t;

enum class bitmap_type_t : u16
{
    d2,
    d3,
    cube,
    sprite,
    interface,
};

enum class bitmap_format_t : u16
{
    bc1,
    bc2,
    bc3,
    raw16,
    raw32,
    intensity,
};

enum class bitmap_usage_t : u16
{
    alpha_blend,
    default_,
    height_map,
    detail_map,
    light_map,
    vector_map,
};

enum class bitmap_usage_flags_t : u16
{
    none,
    diffusion_dithering     = 0x1,
    uncompressed_height_map = 0x2,
    uniform_sprite_sequence = 0x4,
    filthy_sprite_fix       = 0x8,
};

enum class sprite_budget_t : u16
{
    w32,
    w64,
    w128,
    w256,
    w512,
};

enum class sprite_usage_t : u16
{
    blend_add_sub_max,
    mul_min,
    double_mul,
};

/*!
 * \brief A bitmap header for images
 */
struct header_t
{
    bitmap_type_t   type;
    bitmap_format_t format;
    bitmap_usage_t  usage;

    f32 detail_fade_factor;
    f32 sharpen_amount;
    f32 bump_height;

    sprite_budget_t budget;
    u16             budget_count;

    u16 import_width;
    u16 import_height;

    i32 unknown[10];

    f32 blur_filter_size;
    f32 alpha_bias;
    u32 mipmap_count;

    sprite_usage_t sprite_usage;
    u16            sprite_spacing;

    reference<sequence_t> sequences;
    reference<image_t>    images;
};

static_assert(offsetof(header_t, images) == 96);
static_assert(sizeof(header_t) == 108);

struct sprite_t;

struct alignas(64) sequence_t
{
    bl_string_var<32> name;
    u16               first_bitmap;
    u16               bitmap_count;

    reference<sprite_t> sprites;
};

struct alignas(32) sprite_t
{
    u16   bitmap_index;
    f32   left;
    f32   right;
    f32   top;
    f32   bottom;
    Vecf2 registration_point;
};

static_assert(sizeof(sprite_t) == 32);

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

    inline bool shared() const
    {
        return enum_helpers::feval(flags & flags_t::shared);
    }

    typing::pixels::PixDesc to_fmt() const;

    /*!
     * \brief Bytes one face/layer occupies at a single mip level.
     */
    inline u32 layer_mip_bytes(u16 mip) const
    {
        auto mipsize = isize;
        mipsize.x >>= mip;
        mipsize.y >>= mip;
#if defined(GLEAM_USE_CORE) || defined(GLEAM_USE_ES) || defined(GLEAM_DUMMY)
        if(!compressed())
            return gl::tex::format_of(to_fmt()).data_size(mipsize);
        return gl::tex::format_of(to_fmt().c).data_size(mipsize);
#else
        auto w = (u32)(mipsize.x < 1 ? 1 : mipsize.x);
        auto h = (u32)(mipsize.y < 1 ? 1 : mipsize.y);
        switch(format)
        {
        case format_t::BC1:
        case format_t::BC1_GX_TILED:
            return ((w + 3u) / 4u) * ((h + 3u) / 4u) * 8u;
        case format_t::BC2:
        case format_t::BC3:
            return ((w + 3u) / 4u) * ((h + 3u) / 4u) * 16u;
        case format_t::A8:
        case format_t::Y8:
        case format_t::AY8:
        case format_t::I8_GX_TILED:
            return w * h;
        case format_t::A8Y8:
        case format_t::R5G6B5:
        case format_t::A1RGB5:
        case format_t::ARGB4:
        case format_t::RGB565_GX_TILED:
        case format_t::IA8_GX_TILED:
            return w * h * 2u;
        case format_t::ARGB8:
        case format_t::XRGB8:
            return w * h * 4u;
        default:
            return 0u;
        }
#endif
    }

    /*!
     * \brief Bytes one face's whole mip chain occupies.
     */
    inline u32 layer_chain_bytes() const
    {
        u32 out = 0;
        for(u16 i = 0; i < (mipmaps ? mipmaps : 1); i++)
            out += layer_mip_bytes(i);
        return out;
    }

    /*!
     * \brief Data for one cubemap face at one mip level.
     * \param face Cube face in API order (+X, -X, +Y, -Y, +Z, -Z).
     * \param mipmap
     */
    template<typename V>
    inline Span<const u8> cube_face(
        map_ptr const& magic, u32 face, u16 mipmap = 0) const
    {
        if(mipmap != 0 && mipmap >= mipmaps)
            Throw(undefined_behavior("mipmap out of range"));
        if(face >= 6)
            Throw(undefined_behavior("cube face out of range"));

        u32 const level = layer_mip_bytes(mipmap);
        u32       start = 0;

        if constexpr(std::is_same_v<V, xbox_version_t>)
        {
            u32 const stride = (layer_chain_bytes() + 127u) & ~127u;
            u32       within = 0;
            for(u16 i = 0; i < mipmap; i++)
                within += layer_mip_bytes(i);
            start = face * stride + within;
        } else
        {
            /* Swapping 1 and 2 is its own inverse, so this reads the same
             * either direction. */
            constexpr u32 stored[6] = {0, 2, 1, 3, 4, 5};
            u32 preceding           = 0;
            for(u16 i = 0; i < mipmap; i++)
                preceding += layer_mip_bytes(i) * 6u;
            start = preceding + stored[face] * level;
        }

        return reference<u8>{.count = level, .offset = offset + start}
            .data(magic)
            .value();
    }

    inline Span<const u8> data(map_ptr const& magic, u16 mipmap = 0) const
    {
#if defined(GLEAM_USE_CORE) || defined(GLEAM_USE_ES) || defined(GLEAM_DUMMY)
        using namespace typing::pixels::properties;

        if(mipmap != 0 && mipmap >= mipmaps)
            Throw(undefined_behavior("mipmap out of range"));

        auto mipsize = isize;
        mipsize.x >>= mipmap;
        mipsize.y >>= mipmap;

        if(!compressed())
        {
            auto const& format = gl::tex::format_of(to_fmt());

            u32 size = format.data_size(mipsize);

            /* Every layer of a level is stored before the next level starts,
             * so skipping a level means skipping all of its layers. */
            u32 const layers = type == type_t::tex_cube  ? 6u
                               : type == type_t::tex_3d  ? depth
                                                         : 1u;
            u32 mip_offset = 0;

            for(auto i : stl_types::range<>(mipmap))
            {
                auto imsize = isize;
                imsize.x >>= i;
                imsize.y >>= i;
                mip_offset += format.data_size(imsize) * layers;
            }

            size *= layers;

            return reference<u8>{.count = size, .offset = offset + mip_offset}
                .data(magic)
                .value();
        } else
        {
            auto fmt = to_fmt();

            u32 size = gl::tex::format_of(fmt.c).data_size(mipsize);

            u32 const layers = type == type_t::tex_cube  ? 6u
                               : type == type_t::tex_3d  ? depth
                                                         : 1u;
            u32 mip_offset = 0;
            for(auto i : stl_types::Range<>(mipmap))
            {
                auto off_size = isize;
                off_size.x >>= i;
                off_size.y >>= i;
                mip_offset +=
                    gl::tex::format_of(fmt.c).data_size(off_size) * layers;
            }

            size *= layers;

            return reference<u8>{.count = size, .offset = offset + mip_offset}
                .data(magic)
                .value();
        }
#elif defined(COFFEE_GEKKO)
        auto bytes_per_mip = [this](u16 mip) -> u32 {
            auto w = (u32)((isize.x >> mip) < 1 ? 1 : (isize.x >> mip));
            auto h = (u32)((isize.y >> mip) < 1 ? 1 : (isize.y >> mip));
            switch(format)
            {
            case format_t::BC1:
            case format_t::BC1_GX_TILED:
                return ((w + 3u) / 4u) * ((h + 3u) / 4u) * 8u;
            case format_t::BC2:
            case format_t::BC3:
                return ((w + 3u) / 4u) * ((h + 3u) / 4u) * 16u;
            case format_t::A8:
            case format_t::Y8:
            case format_t::AY8:
            case format_t::I8_GX_TILED:
                return w * h;
            case format_t::A8Y8:
            case format_t::R5G6B5:
            case format_t::A1RGB5:
            case format_t::ARGB4:
            case format_t::RGB565_GX_TILED:
            case format_t::IA8_GX_TILED:
                return w * h * 2u;
            case format_t::ARGB8:
            case format_t::XRGB8:
                return w * h * 4u;
            default:
                return 0u;
            }
        };

        if(mipmap != 0 && mipmap >= mipmaps)
            Throw(undefined_behavior("mipmap out of range"));

        u32 const layers = type == type_t::tex_cube ? 6u
                           : type == type_t::tex_3d
                               ? (u32)(depth > 0 ? depth : 1)
                               : 1u;
        u32 mip_offset = 0;
        for(u16 i = 0; i < mipmap; i++)
            mip_offset += bytes_per_mip(i) * layers;

        u32 sz = bytes_per_mip(mipmap) * layers;

        return reference<u8>{.count = sz, .offset = offset + mip_offset}
            .data(magic)
            .value();
#else
#error No impl for blam::bitm::image_t::data
#endif
    }
};

} // namespace blam::bitm
