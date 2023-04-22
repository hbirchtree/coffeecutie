#pragma once

#include "blam_magic_data.h"
#include "blam_reflexive.h"
#include "blam_strings.h"
#include "blam_structures.h"
#include "blam_tag_index.h"

#include <peripherals/typing/enum/pixels/components.h>
#include <peripherals/typing/enum/pixels/format.h>
#include <peripherals/typing/enum/pixels/format_transform.h>

namespace blam::bitm {

using typing::PixCmp;

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
    P8     = 0x11, /*!< LLL01 (See A8)*/
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

struct locator_block
{
    string_ref name;
    u32        size;
    u32        offset;

    inline auto to_reflexive() const
    {
        return reflexive_t<header_t>{1, {offset}};
    }
};

struct bitmap_atlas_t
{
    u32 unknown;
    u32 name_block_off;

    u32 locators_offset;
    u32 locators_count;

    static inline bitmap_atlas_t const* from_data(
        semantic::Span<const byte_t> const& data)
    {
        return C_RCAST<bitmap_atlas_t const*>(data.data());
    }

    inline std::string_view get_name(locator_block const& block) const
    {
        auto base_ptr = C_RCAST<byte_t const*>(this);

        return C_RCAST<const char*>(
            base_ptr + name_block_off + block.name.offset);
    }

    inline semantic::Span<locator_block const> locators() const
    {
        auto base_ptr = C_RCAST<byte_t const*>(this);

        return semantic::Span<locator_block const>(
            C_RCAST<locator_block const*>(base_ptr + locators_offset),
            locators_count);
    }

    inline magic_data_t block_magic(
        magic_data_t const& bitm, u32 bitm_idx) const
    {
        auto const& loc = locators()[bitm_idx];
        return magic_data_t(
            {bitm.base_ptr + loc.offset, bitm.max_size - loc.offset}, 0);
    }
    inline reflexive_t<header_t> get_block(u32 bitm_idx = 0) const
    {
        auto const& loc = locators()[bitm_idx];
        return loc.to_reflexive();
    }
};

struct bitmap_atlas_view
{
    magic_data_t          bitmap_magic;
    bitmap_atlas_t const* header;

    static inline bitmap_atlas_view from_data(
        semantic::Span<const byte_t> const& data)
    {
        return {
            .bitmap_magic = magic_data_t(data),
            .header       = bitmap_atlas_t::from_data(data),
        };
    }
};

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

    reflexive_t<sequence_t> sequences;
    reflexive_t<image_t>    images;
};

static_assert(offsetof(header_t, images) == 96);
static_assert(sizeof(header_t) == 108);

struct sprite_t;

struct alignas(64) sequence_t
{
    bl_string_var<32> name;
    u16               first_bitmap;
    u16               bitmap_count;

    reflexive_t<sprite_t> sprites;
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

    std::tuple<BitFmt, PixCmp> to_fmt() const;

    PixFmt to_pixfmt() const;

    std::tuple<PixFmt, CompFlags> to_compressed_fmt() const;

    Span<const u8> data(magic_data_t const& magic, u16 mipmap = 0) const;
};

} // namespace blam::bitm
