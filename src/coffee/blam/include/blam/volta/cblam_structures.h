#pragma once

#include <coffee/core/libc_types.h>
#include <coffee/core/types/map.h>
#include <coffee/core/types/pixel_components.h>
#include <coffee/core/types/pixel_format.h>
#include <coffee/core/types/point.h>
#include <coffee/core/types/rgba.h>
#include <coffee/core/types/size.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/stl/functional_types.h>
#include <peripherals/typing/enum/graphics/texture_types.h>
#include <platforms/types/vector_types.h>

#include "blam_tag_classes.h"

namespace blam {

using namespace libc_types;
using stl_types::Array;
using typing::PixCmp;
using typing::geometry::point_2d;
using typing::geometry::size_2d;
using typing::geometry::size_3d;
using typing::pixels::BitFmt;
using typing::pixels::CompFlags;
using typing::pixels::PixFmt;
using typing::vector_types::Quatf;
using typing::vector_types::Vecf2;
using typing::vector_types::Vecf3;
using typing::vector_types::Vecf4;
using TexType = typing::graphics::TexType;

template<size_t Size>
struct bl_string_var
{
    static constexpr size_t size = Size;

    char data[Size];

    template<
        typename Dummy                                   = void,
        typename std::enable_if<Size == 4, Dummy>::type* = nullptr>
    inline stl_types::CString str() const
    {
        if(data[0] == 0)
            return {};

        auto out = stl_types::CString(data, Size);
#if __cplusplus >= 201703
        std::reverse(out.begin(), out.end());
#else
        auto it  = out.begin();
        auto end = out.end();
        while((it != end) && (it != --end))
            std::iter_swap(it++, end);
#endif
        return out;
    }

    template<
        typename Dummy                                   = void,
        typename std::enable_if<Size != 4, Dummy>::type* = nullptr>
    inline stl_types::CString str() const
    {
        auto end = libc::str::find(data, '\0');
        return stl_types::CString(data, end);
    }

    inline operator cstring() const
    {
        return data;
    }

    template<
        typename Dummy                                   = void,
        typename std::enable_if<Size == 4, Dummy>::type* = nullptr>
    explicit operator u32() const
    {
        u32 const* idata = C_RCAST<u32 const*>(data);
        return *idata;
    }
};

using bl_tag    = bl_string_var<4>;
using bl_string = bl_string_var<32>;
using bl_header = char[4];
using bl_footer = char[4];

template<size_t Size>
struct unicode_var
{
    using wide_string = std::basic_string<u16>;

    u16 data[Size];

    inline wide_string str() const
    {
        constexpr u16 terminator = 0;

        auto out = wide_string(data, Size);
        auto end = out.find(terminator);
        if(end != wide_string::npos)
            out.resize(end);
        return out;
    }

    inline stl_types::CString flat_str() const
    {
        auto from = str();
        return stl_types::CString(from.begin(), from.end());
    }
};

/*!
 * \brief Blam, at least for Halo 1, uses int16 to store bitmap sizes
 */
using bl_size_t = size_2d<i16>;
/*!
 * \brief As with blam_size, int16 is standard size for Halo 1.
 */
using bl_point_t = point_2d<i16>;

/*!
 * \brief The blam_bounding_box struct
 */
struct bounding_box
{
    bounding_box() : min(0), max(0)
    {
    }

    Vecf3 min;
    Vecf3 max;
};

/*!
 * \brief Function pointers for blam bitmap processing, raw function pointer is
 * much faster than Function
 */
using BitmProcess = u32 (*)(u32, uint16, byte_t);

using bl_rgba_t = typing::pixels::rgba_t;

/*!
 * \brief Blam maptypes. Names being obvious, the UI type does not give a
 * playable map.
 */
enum class maptype_t : int32
{
    singleplayer =
        0,           /*!< A single-player map, typically with cutscenes and AI*/
    multiplayer = 1, /*!< A multi-player map, typically with up to 16 players*/
    ui          = 2, /*!< A UI map, used only in the main menu*/
};

enum class game_difficulty_t : u16
{
    insane = 0,

};

/*!
 * \brief Possible Blam versions which we may encounter. The different formats
 * work in different ways. For instance, Xbox stores bitmaps and other data in a
 * single file while PC spreads it across "bitmaps.map" and "sounds.map". Both
 * ways are necessary for parsing to happen correctly.
 */
enum class version_t : int32
{
    xbox = 5, /*!< The 2001 version of Halo: Combat Evolved for Xbox*/
    pc   = 7, /*!< The 2004 version of Halo: Combat Evolved for PC*/

    custom_edition = 609,
};

struct xbox_version_t
{
};
struct pc_version_t
{
};
struct custom_version_t
{
};

constexpr xbox_version_t   xbox_version;
constexpr pc_version_t     pc_version;
constexpr custom_version_t custom_version;

namespace vert {

struct compressed
{
};
struct uncompressed
{
};

template<
    typename RType = uncompressed,
    bool           = std::is_same<RType, uncompressed>::value>
struct alignas(4) vertex
{
    Vecf3 position;
    Vecf3 normal;
    Vecf3 binorm;
    Vecf3 tangent;
    Vecf2 texcoord;

  private:
    constexpr void size_check()
    {
        static_assert(sizeof(vertex<uncompressed>) == 56, "Invalid size");
    }
};

template<typename RType>
struct alignas(4) vertex<RType, false>
{
    // Compressed Xbox variant
    Vecf3               position;
    typing::pixels::f11 normal;  /*!< PixFmt::R11G11B10F */
    typing::pixels::f11 binorm;  /*!< Same as binormal */
    typing::pixels::f11 tangent; /*!< Same as normal */
    Vecf2               texcoord;

  private:
    constexpr void size_check()
    {
        static_assert(sizeof(vertex<compressed>) == 32, "Invalid size");
    }
};

template<
    typename RType = uncompressed,
    bool           = std::is_same<RType, uncompressed>::value>
struct alignas(4) mod2_vertex
{
    Vecf3 position;
    Vecf3 normal;
    Vecf3 binorm;
    Vecf3 tangent;
    Vecf2 texcoord;
    Vecf3 _garbo;

  private:
    constexpr void size_check()
    {
        static_assert(sizeof(mod2_vertex<uncompressed>) == 68, "Invalid size");
    }
};

template<typename RType>
struct alignas(4) mod2_vertex<RType, false>
{
    // Compressed Xbox variant
    Vecf3 position;
    u32   normal;   /*!< PixFmt::R11G11B10F */
    u32   binormal; /*!< Same as binormal */
    u32   tangent;  /*!< Same as normal */

    typing::vectors::tvector<i16, 2> texcoord;

    u16 unknown;
    u16 weight;

  private:
    constexpr void size_check()
    {
        static_assert(sizeof(mod2_vertex<compressed>) == 32, "Invalid size");
    }
};

using idx_t = u16;
using face  = Array<idx_t, 3>;

template<
    typename RType = uncompressed,
    bool           = std::is_same<RType, uncompressed>::value>
struct light_vertex
{
    Vecf3 normal;
    Vecf2 texcoord;
};

template<typename RType>
struct light_vertex<RType, false>
{
    // Compressed Xbox variant
    u32                              normal;   /*!< PixFmt::R11G11B10F */
    typing::vectors::tvector<i16, 2> texcoord; /*!< PixFmt::R16, Normalized */
};

} // namespace vert

constexpr cstring header_head = "daeh"; /*!< Header of file header*/
constexpr cstring header_foot = "toof"; /*!< Footer of file header*/

constexpr i32 blam_num_map_names = 28; /*!< Number of recognizable map names*/
constexpr stl_types::Array<stl_types::Pair<cstring, cstring>, 28> map_names = {{
    // Single player maps
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

    // Multi-player maps
    {"beavercreek", "Beaver Creek"},
    {"bloodgulch", "Blood Gulch"},
    {"boardingaction", "Boarding Action"},
    {"carousel", "Derelict"},
    {"chillout", "Chill Out"},
    {"deathisland", "Death Island"},
    {"damnation", "Damnation"},
    {"dangercanyon", "Danger Canyon"},
    {"gephyrophobia", "Gephyrophobia"},
    {"hangemhigh", "Hang 'em High"},
    {"icefields", "Ice Fields"},
    {"longest", "Longest"},
    {"prisoner", "Prisoner"},
    {"putput", "Chiron TL34"},
    {"ratrace", "Rat Race"},
    {"sidewinder", "Sidewinder"},
    {"timberland", "Timberland"},
    {"wizard", "Wizard"},
}}; /*!< A mapping of map names which this library can recognize. These are
      the stock maps.*/

struct map_load_error : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

struct reflexive_error : std::runtime_error
{
    using std::runtime_error::runtime_error;
};

/*!
 * \brief A file header located from the start of a map file
 */
struct alignas(4) file_header_t : stl_types::non_copy
{
    bl_header id;      /*!< Header value, should correspond with specific data*/
    version_t version; /*!< Version of Halo, determines the process*/
    u32       decomp_len; /*!< Decompressed length */
    u32 trailing_space;   /*!< Specifies size of empty space at end of file */
    u32 tag_index_offset; /*!< Offset to tag index*/
    i32 tag_index_len;    /*!< Length of tag index item metadata*/
    u32 reserved_1[2];
    bl_string name;      /*!< Name identifier for map*/
    bl_string buildDate; /*!< Build date for the map file*/
    maptype_t map_type;  /*!< Type of map, determines whether it is playable*/
    i32       unknown_4;
    i32       reserved_2[485];
    bl_footer footer; /*!< Footer value, should correspond with specific data*/

    /*!
     * \brief Get the proper, full name of a map
     * \param map Map from which we identify a string
     * \return A null-terminated string with a map name or empty (empty if map
     * name is not recognized)
     */
    inline cstring full_mapname() const
    {
        for(auto name : map_names)
        {
            if(libc::str::cmp(C_OCAST<const char*>(this->name), name.first))
                return name.second;
        }
        return name;
    }
    /*!
     * \brief Validate and get the file header from a base pointer, mostly
     * validation
     * \param data Pointer to data which will be validated
     * \param expected Expected version of Halo to be parsed. If version does
     *    not match, fail.
     * \return Returns a casted pointer on success, nullptr on failure
     */
    static file_header_t const* from_data(
        semantic::Bytes const& data, pc_version_t);

    static file_header_t const* from_data(
        semantic::Bytes const& data, custom_version_t);

    static file_header_t const* from_data(
        semantic::Bytes const& data, xbox_version_t);

    inline bool is_xbox() const
    {
        return version == version_t::xbox;
    }
};

struct magic_data_t
{
    magic_data_t() : magic_offset(0)
    {
        base_ptr = nullptr;
    }
    magic_data_t(byte_t const* base_ptr, u32 magic, u32 max_size) :
        magic_offset(magic), max_size(max_size)
    {
        this->base_ptr = base_ptr;
    }
    magic_data_t(semantic::Bytes const& data) :
        magic_offset(0), max_size(data.size)
    {
        base_ptr = data.data;
    }

    inline magic_data_t& operator=(semantic::Bytes const& data)
    {
        base_ptr     = data.data;
        magic_offset = 0;
        max_size     = data.size;

        return *this;
    }

    inline version_t map_version() const
    {
        return header_ptr->version;
    }

    union
    {
        file_header_t const* header_ptr;
        byte_t const*        base_ptr;
    };
    u32 magic_offset;
    u32 max_size;
};

struct xbox_variant
{
};
struct pc_variant
{
};

/*!
 * \brief Points to a chunk of memory within the file
 */
template<
    typename T,
    typename RType = pc_variant,
    bool           = std::is_same<RType, pc_variant>::value>
struct reflexive_t
{
};

template<typename T, typename RType>
struct alignas(4) reflexive_t<T, RType, true>
{
    using data_type = T;

    u32 count;  /*!< Number of elements */
    u32 offset; /*!< Offset to data within file (this will only refer to data
                   within the map file)*/
    i32 zero;

    /*!
     * \brief Made for simplicity when working with reflexive data. Instead of
     * several functions performing the same task, this template allows to
     * access the data much more conveniently. \param basePtr Base pointer of
     * map in most cases \param magic Magic number from tag index \return A
     * valid pointer if the reflexive is deemed valid (if the variable zero is
     * indeed zero)
     */
    semantic::mem_chunk<T const> data(magic_data_t const& magic) const
    {
        using Output = semantic::mem_chunk<T const>;

        if(zero != 0)
            Throw(reflexive_error("invalid reflexive_t"));

        if(count == 0)
            return Output();

        if((offset - magic.magic_offset) > magic.max_size)
            Throw(reflexive_error("reflexive pointer out of bounds"));

        return Output::From(
            C_RCAST<T const*>(magic.base_ptr + offset - magic.magic_offset),
            count);
    }

    template<typename T2>
    inline reflexive_t<T2> as() const
    {
        auto cpy   = reflexive_t<T2>();
        cpy.count  = count;
        cpy.offset = offset;
        cpy.zero   = 0;
        return cpy;
    }
};

template<typename T, typename RType>
struct alignas(4) reflexive_t<T, RType, false>
{
    using data_type = T;

    u32 count;
    u32 offset;

    semantic::mem_chunk<T const> data(magic_data_t const& magic) const
    {
        using Output = semantic::mem_chunk<T const>;

        if(count == 0)
            return Output();

        if((offset - magic.magic_offset) > magic.max_size)
            Throw(reflexive_error("reflexive pointer out of bounds"));

        return Output::From(
            C_RCAST<T const*>(magic.base_ptr + offset - magic.magic_offset),
            count);
    }
};

struct alignas(4) string_ref
{
    u32 offset;

    cstring to_string(magic_data_t const& magic) const
    {
        if(offset == 0)
            return "[empty string]";

        return C_RCAST<cstring>(magic.base_ptr + offset - magic.magic_offset);
    }
};

template<tag_class_t... Tags>
struct alignas(4) tagref_typed_t
{
    union
    {
        bl_tag      tag;
        tag_class_t tag_class;
    };
    string_ref name;
    i32        unknown;
    u32        tag_id;

    inline bool valid() const
    {
        return tag_id != std::numeric_limits<u32>::max();
    }

    inline string_ref to_name() const
    {
        return name;
    }

    inline bool matches(tag_class_t cls) const
    {
        return tag_class == cls;
    }

    inline tagref_typed_t<> to_plain() const
    {
        return *C_RCAST<tagref_typed_t<> const*>(this);
    }
};

using tagref_t = tagref_typed_t<>;

struct tag_t;

/*!
 * \brief A tag index contains information about the map, its resources and its
 * layout. Bitmaps, sounds, scenarios and etc. are referred to by this index.
 */
struct alignas(4) tag_index_t : stl_types::non_copy
{
    using vertex_array =
        reflexive_t<vert::mod2_vertex<vert::uncompressed>, xbox_variant>;
    using index_array = reflexive_t<vert::idx_t, xbox_variant>;

    i32          index_magic;    /*!< Number used to adjust indexes*/
    u32          base_tag;       /*!< Base tag, smallest tag id */
    u32          vertex_size;    /*!< Size of vertex data*/
    u32          tagCount;       /*!< Number of tags*/
    vertex_array vertex_objects; /*!< Number of vertex objects*/
    index_array  index_objects;  /*!< Number of index objects*/
    union
    {
        bl_tag xbox_tag_sentinel;
        u32    raw_model_data_size;
    };
    bl_tag pc_tag_sentinel; /*!< Says "tags" */

    inline magic_data_t get_magic(file_header_t const* base) const
    {
        byte_t const* base_ptr   = C_RCAST<byte_t const*>(base);
        u32           index_size = base->version == version_t::xbox
                             ? sizeof(tag_index_t) - sizeof(u32)
                             : sizeof(tag_index_t);

        return magic_data_t{base_ptr,
                            index_magic - (base->tag_index_offset + index_size),
                            base->decomp_len};
    }

    inline vertex_array vertex_base() const
    {
        return vertex_objects;
    }

    inline index_array index_base() const
    {
        index_array base_reflexive = index_objects;
        base_reflexive.offset += vertex_objects.offset;
        return base_reflexive;
    }

    inline magic_data_t vertex_magic(magic_data_t const& base) const
    {
        return magic_data_t{base.base_ptr, 0, base.max_size};
    }

    /*!
     * \brief Get a pointer to the start of the tag index items, traversed as a
     * simple array \param file File header from which we get the pointer
     * \return A pointer to the first tag index item
     */
    inline tag_t const* tags(file_header_t const* header) const
    {
        szptr offset = header->version == version_t::xbox
                           ? sizeof(tag_index_t) - sizeof(u32)
                           : sizeof(tag_index_t);

        return C_RCAST<tag_t const*>(C_RCAST<byte_t const*>(this) + offset);
    }

    inline tag_t const* tag_at(const file_header_t* header, u32 tag_id) const;

    inline tag_t const& scenario(file_header_t const* header) const;

    /*!
     * \brief Get the tag index pointer out of the file header
     * \param file File header from which we want a tag index
     * \return A reference to the tag index
     */
    STATICINLINE tag_index_t const& from_header(file_header_t const* base)
    {
        byte_t const* base_ptr = C_RCAST<byte_t const*>(base);
        return *C_RCAST<tag_index_t const*>(&base_ptr[base->tag_index_offset]);
    }
};

enum class image_storage_t : u32
{
    internal = 0,
    external = 1
};

/*!
 * \brief An item in the tag index, contains tag class (which identifies the
 * type of item), tag ID (a numerical ID), an offset to a proper string, and an
 * offset to its associated data.
 */
struct alignas(4) tag_t
{
    union
    {
        Array<bl_tag, 3>      tagclass; /*!< Strings which identify its class*/
        Array<tag_class_t, 3> tagclass_e; /*!< enum-ified tagclass value */
    };
    u32        tag_id;
    string_ref name;
    u32        offset; /*!< A byte offset to associated data*/
    union
    {
        u32             unknown;
        image_storage_t storage; /*!< Only applies to custom_edition */
    };
    i32 padding;

    inline bool matches(tag_class_t other) const
    {
        for(auto tag : tagclass_e)
            if(tag == other)
                return true;
        return false;
    }

    inline bool valid() const
    {
        return padding == 0;
    }

    template<typename T>
    inline reflexive_t<T> to_reflexive() const
    {
        if(storage == image_storage_t::external && matches(tag_class_t::bitm))
        {
            /* If you're here, that means you hit one of Custom Edition's
             *  externally stored bitmaps, but used the map magic
             *  to dereference it.
             * These are located entirely in the bimaps.map file.
             *
             * To start you must plug the .offset
             *  into blam::bitm::bitmap_header_t's get_block() function
             *  and dereference the reflexive_t<> with block_magic()
             *  from the bitmap.
             *
             * Also remember to use the Custom Edition bitmaps.map.
             */
            Throw(reflexive_error(
                "bitmap uses external storage, use blam::bitm::bitm_header_t"));
        }

        if(!valid())
            Throw(reflexive_error("invalid tag"));

        return {1, offset};
    }

    inline string_ref to_name() const
    {
        return name;
    }
};

tag_t const* tag_index_t::tag_at(file_header_t const* header, u32 tag_id) const
{
    if(tag_id > base_tag + tagCount)
        return nullptr;
    else
        return &(tags(header)[(tag_id - base_tag)]);
}

tag_t const& tag_index_t::scenario(file_header_t const* header) const
{
    tag_t const* tag = &(tags(header)[0]);

    if(!tag->matches(tag_class_t::scnr))
        Throw(undefined_behavior("failed to locate scenario header"));

    return *tag;
}

struct string_segment_ref
{
    struct string_ref
    {
        const char* data;
        u32         offset;

        inline cstring str() const
        {
            return data;
        }

        inline bool valid() const
        {
            return data[0] != 0 && data[1] != 0 && data[2] != 0;
        }
    };

    semantic::mem_chunk<const char> data;
    u32                             num_strings;

    inline string_ref at(u32 offset = 0) const
    {
        auto out = data.at(offset);

        if(!out.size)
            Throw(undefined_behavior("string out of bounds"));
        return {out.data, offset};
    }

    inline string_ref indexed(u32 i = 0) const
    {
        const char* loc = data.data;

        u32 s_i = 0;
        while(s_i != i)
        {
            loc = C_RCAST<const char*>(
                ::memchr(loc, '\0', std::numeric_limits<u16>::max()));
            loc++;
            s_i++;
        }

        return {C_RCAST<const char*>(loc), C_FCAST<u32>(loc - data.data)};
    }

    inline u32 get_index(bl_string const& str) const
    {
        Array<char, bl_string::size + 1> search;
        search.fill(0);

        u32 num_chars = 2;
        for(size_t i = 0; i < bl_string::size; i++)
        {
            if(str.data[i] == 0)
                break;
            search[1 + i] = str.data[i];
            num_chars++;
        }

        const char* ptr = C_RCAST<const char*>(
            ::memmem(data.data, data.size, search.data(), num_chars));

        if(!ptr)
            return 0;

        ptr++;

        return ptr - data.data;
    }
};

struct unicode_reflexive
{
    using wide_string = std::basic_string<u16>;

    reflexive_t<u16> data;

    inline wide_string str(magic_data_t const& magic) const
    {
        constexpr u16 terminator = 0;

        auto seg = data.data(magic);
        auto out = wide_string(seg.data, seg.elements);
        out.resize(out.find(terminator));
        return out;
    }
};

struct mod2_shader
{
    tagref_t ref;
    u32      unknown;
};

struct shader_desc
{
    tagref_t ref;
    u32      unknown[4];
};

struct alignas(4) shader_base /* aka shdr */
{
    enum class radiosity_flags : u32
    {
        simple_params   = 0x1,
        ignore_normals  = 0x2,
        transparent_lit = 0x4,
    };
    enum class radiosity_lod : u32
    {
        high,
        medium,
        low,
        turd,
    };
    enum class physics_material : u32
    {
        dirt,
        sand,
        stone,
        snow,
        wood,
        metal_hollow,
        metal_thin,
        metal_thicc,
        rubber,
        glass,
        force_field,
        grunt,
        hunter_armor,
        hunter_skin,
        elite,
        jackal,
        jackal_energy_shield,
        engineer_skin,
        engineer_force_field,
        flood_combat_form,
        flood_carrier_form,
        cyborg_armor,
        cyborg_energy_shield,
        human_armor,
        human_skin,
        sentinel,
        monitor,
        plastic,
        water,
        leaves,
        elite_energy_shield,
        ice,
        hunter_shield,
    };
    enum class detail_map_function : u32
    {
        biased_multiply,
        multiply,
        biased_add,
    };
    enum class animation_function : u16
    {
        none,
    };
    enum class animation_src : u16
    {
        none,
        A_out,
        B_out,
        C_out,
        D_out,
    };
    enum class reflection_map_type : u32
    {
        none,
        bumped_cube,
        flat_cube,
        bumped_radiosity,
    };

    struct detail_map
    {
        scalar                            scale;
        tagref_typed_t<tag_class_t::bitm> map;
    };

    struct view_props
    {
        scalar brightness;
        Vecf3  tint_color;
    };

    radiosity_flags  flags;
    radiosity_lod    detail_level;
    Vecf3            emission;
    Vecf3            tint;
    physics_material physics;
};

struct texture_scrolling_animation
{
    struct texture_anim_properties
    {
        shader_base::animation_src      source;
        shader_base::animation_function function;

        scalar period;
        scalar phase;
        scalar repeats;
    };

    texture_anim_properties u;
    texture_anim_properties v;

    texture_anim_properties rot; /* rotation-animation */

    Vecf2 rotation_center;
};

struct alignas(4) shader_chicago : shader_base /* aka schi */
{
    enum class chicago_flags : u16
    {
        none                          = 0x0,
        alpha_testing                 = 0x1,
        decal                         = 0x2,
        two_sided                     = 0x4,
        first_map_screenspace         = 0x8,
        draw_before_water             = 0x10,
        ignore_effect                 = 0x20,
        scale_first_map_with_distance = 0x40,
        numeric                       = 0x80
    };
    enum class map_type : u16
    {
        map_2d,
        map_cubemap,
        object_centered_cubemap,
        viewer_centered_cubemap,
    };
    enum class framebuffer_fade_mode : u16
    {
        none,
        perpendicular,
        parallel,
    };
    enum class framebuffer_fade_src : u16
    {
        none,
        A_out,
        B_out,
        C_out,
        D_out,
    };
    enum class framebuffer_blending : u16
    {
        alpha_blend,
        multiply,
        double_multiply,
        add,
        subtract,
        component_min,
        component_max,
        alpha_multiply_add,
    };
    enum class extra_flags : u32
    {
        none                    = 0x0,
        dont_fade_active_camo   = 0x1,
        numeric_countdown_timer = 0x2,
    };
    enum class map_flags : u16
    {
        none            = 0x0,
        unfiltered      = 0x1,
        alpha_replicate = 0x2,
        u_clamp         = 0x4,
        v_clamp         = 0x8,
    };

    struct lens_flares_t
    {
        scalar                            spacing;
        tagref_typed_t<tag_class_t::lens> lens_flare;
    };
    struct map_t
    {
        u32 padding_1[11];

        map_flags flags;
        u16       color_function;
        u16       alpha_function;
        u16       padding_2;

        u32 padding_3[8];

        struct
        {
            Vecf2  uv_scale;
            Vecf2  uv_offset;
            scalar rotation;
            scalar mip_bias;

            tagref_typed_t<tag_class_t::bitm> map;
        } map;

        u32 padding_4[8];

        texture_scrolling_animation anim_2d;

        u32 padding_5[2];
    };

    u8                    numeric_counter_limit;
    chicago_flags         flags;
    map_type              first_map_type;
    framebuffer_blending  blend_function;
    framebuffer_fade_mode fade_mode;
    framebuffer_fade_src  fade_src;

    u32 padding_1;

    lens_flares_t lens_flares;

    reflexive_t<tagref_typed_t<tag_class_t::shdr>> layers;

    reflexive_t<map_t> maps;

    extra_flags ex_flags;
};

struct reflection_properties
{
    shader_base::reflection_map_type  type;
    shader_base::view_props           perpendicular;
    shader_base::view_props           parallel;
    tagref_typed_t<tag_class_t::bitm> map;
    shader_base::detail_map           bump_map;
};

struct alignas(4) shader_chicago_extended : shader_base /* aka scex */
{
    u8                            counter_limit;
    u8                            padding_1;
    shader_chicago::chicago_flags flags;
    shader_chicago::map_type      first_map_type;

    shader_chicago::framebuffer_blending  blend_mode;
    shader_chicago::framebuffer_fade_mode fade_mode;
    shader_chicago::framebuffer_fade_src  fade_src;

    u32 padding_2;

    shader_chicago::lens_flares_t lens_flares;

    u32 padding_3[3];

    reflexive_t<tagref_typed_t<tag_class_t::shdr>> layers;

    reflexive_t<shader_chicago::map_t> maps_4stage;
    reflexive_t<shader_chicago::map_t> maps_2stage;
    shader_chicago::extra_flags        extra_flags;
};

struct alignas(4) shader_glass : shader_base /* aka sgla */
{
    enum class glass_flags : u32
    {
        none                      = 0x0,
        alpha_test                = 0x1,
        decal                     = 0x2,
        two_sided                 = 0x4,
        bump_map_is_specular_mask = 0x8,
    };

    glass_flags flags;

    u32 padding_[11];

    struct
    {
        Vecf3      color;
        detail_map map;

        u32 padding_[5];
    } background_tint;

    struct
    {
        reflection_map_type               type;
        view_props                        perpendicular;
        view_props                        parallel;
        tagref_typed_t<tag_class_t::bitm> map;
        detail_map                        bump_map;

        u32 padding_[33];
    } reflection;

    struct
    {
        detail_map map;
        detail_map detail;

        u32 padding_[8];
    } diffuse;

    struct
    {
        detail_map map;
        detail_map detail;
    } specular;
};

struct alignas(64) shader_meter : shader_base /* aka smet, TODO */
{
    enum class meter_flags : u32
    {
        none                    = 0x0,
        decal                   = 0x1,
        two_sided               = 0x2,
        flash_color_is_negative = 0x4,
        tint_mode_2             = 0x8,
        unfiltered              = 0x10,
    };

    meter_flags flags;

    u32 padding_1[9];

    tagref_typed_t<tag_class_t::bitm> map;

    u32 padding_2[8];

    struct alignas(32)
    {
        Vecf3  gradient_min;
        Vecf3  gradient_max;
        Vecf3  background;
        Vecf3  flash;
        Vecf3  tint;
        scalar transparency;
        scalar background_transparency;
    } colors;

    struct alignas(32)
    {
        animation_src brightness;
        animation_src flash;
        animation_src value;
        animation_src gradient;
        animation_src flash_extension;
    } ext_func_src;
};

struct alignas(4) shader_water : shader_base /* aka swat */
{
    enum class water_flags : u32
    {
        none                                = 0x0,
        base_map_alpha_modulates_reflect    = 0x1,
        base_map_color_modulates_background = 0x2,
        atmospheric_fod                     = 0x4,
        draw_before_fog                     = 0x8,
    };

    u32         unknown_;
    water_flags flags;
    u32         padding_[8];

    tagref_typed_t<tag_class_t::bitm> base;

    u32 padding_2[4];

    view_props perpendicular;
    view_props parallel;

    u32 padding_3[4];

    tagref_typed_t<tag_class_t::bitm> reflection_map;

    struct ripple_t /* TODO: Find the correct layout of this structure */
    {
        scalar contribution;
        scalar anim_angle;
        scalar anim_velocity;
        Vecf2  map_offset;
        u32    map_repeats;
        u32    map_index;
    };

    u32 padding_4[4];

    struct
    {
        scalar anim_angle;
        scalar anim_velocity;
        scalar scale;

        tagref_typed_t<tag_class_t::bitm> maps;

        u32    mipmap_levels;
        scalar fade_factor;
        scalar mipmap_bias;

        u32 padding_[16];

        reflexive_t<ripple_t> ripples;
    } ripple;
};

struct alignas(4) shader_env : shader_base /* aka senv */
{
    enum class env_shader_type : u32
    {
        unknown,

        normal = 1,
        blended,
        blended_base_specular,
    };
    enum class diffuse_flags : u32
    {
        none                = 0x0,
        rescale_detail_maps = 0x1,
        rescale_bump_map    = 0x2,
    };

    env_shader_type shader_type;

    u32 padding_;

    struct
    {
        scalar                            spacing;
        tagref_typed_t<tag_class_t::lens> lens_flare;

        u32 unknown_2[17];
    } lens_flare;

    struct
    {
        diffuse_flags                     flags;
        tagref_typed_t<tag_class_t::bitm> base;

        u32 padding_[6];

        detail_map_function detail_function;
        detail_map          primary;
        detail_map          secondary;

        u32 padding_2[6];

        detail_map_function micro_function;
        detail_map          micro;
        Vecf3               material_color;

        u32 padding_1[3];
    } diffuse;

    detail_map bump;

    scalar unknown_1[2];
    u32    padding_2[4];

    struct
    {
        animation_function u_anim;
        scalar             u_period;
        scalar             u_scale;

        animation_function v_anim;
        scalar             v_period;
        scalar             v_scale;

        u32 padding_[2];
    } scrolling;

    /* EVERYTHING BELOW IS NOT WORKING */

    enum class illum_flags : u32
    {
        none       = 0x0,
        unfiltered = 0x1,
    };
    struct illumination_props
    {
        Vecf3              on_color;
        Vecf3              off_color;
        animation_function anim;
        scalar             period;
        scalar             phase;

        u32 padding_[26];
    };

    struct
    {
        illum_flags flags;

        illumination_props primary;
        illumination_props secondary;
        illumination_props plasma;

        u32 padding_1[2];

        detail_map map;
    } self_illum;

    enum class specular_flags : u32
    {
        none              = 0x0,
        overbright        = 0x1,
        extra_shiny       = 0x2,
        lightmap_specular = 0x4,
    };

    union
    {
        struct
        {
            specular_flags flags;
            scalar         brightness;
            Vecf3          perpendicular_color;
            Vecf3          parallel_color;

        } specular;

        u32    unknown_3[42];
        scalar unknown_4[42];
    };

    enum class reflection_flags : u32
    {
        none           = 0x0,
        dynamic_mirror = 0x1,
    };

    reflection_properties reflection;
};

struct alignas(4) shader_model : shader_base /* aka soso */
{
    enum class model_flags : u32
    {
        none                      = 0x0,
        detail_after_reflection   = 0x1,
        two_sided                 = 0x2,
        no_alpha_test             = 0x4,
        alpha_blend_decal         = 0x8,
        true_atmospheric_fog      = 0x10,
        disable_two_sided_culling = 0x20,
    };
    enum class color_src : u32
    {
        A_out,
        B_out,
        C_out,
        D_out,
    };
    enum class self_illum_flags : u32
    {
        none            = 0x0,
        no_random_phase = 0x1,
    };

    model_flags flags;
    scalar      translucency;

    struct
    {
        color_src src;
    } change_color;

    u32 padding_1[14];

    struct
    {
        self_illum_flags   flags;
        color_src          color_source;
        animation_function anim_func;
        scalar             anim_period;
        Vecf3              anim_color_lower_bound;
        Vecf3              anim_color_upper_bound;
    } self_illum;

    u32 padding_2[3];

    struct
    {
        Vecf2 scale;

        tagref_typed_t<tag_class_t::bitm> base;
        u32                               padding_1[2];
        tagref_typed_t<tag_class_t::bitm> multipurpose;
        u32                               padding_2[2];

        struct
        {
            u16                               function;
            u16                               mask;
            scalar                            scale;
            tagref_typed_t<tag_class_t::bipd> map;
            scalar                            v_scale;
        } detail;
    } maps;

    texture_scrolling_animation anim_2d;
};

struct alignas(4) shader_plasma : shader_base /* TODO */
{
    u32 padding_1[2];

    struct
    {
        animation_src source;
        scalar        exponent;
    } intensity;

    struct
    {
        animation_src source;
        scalar        amount;
        scalar        exponent;
    } offset;

    u32 padding_2[9];

    struct
    {
        scalar        perpendicular_brightness;
        Vecf3         perpendicular_tint;
        scalar        parallel_brightness;
        Vecf3         parellel_tint;
        animation_src tint_src;
    } color;

    u32 padding_3[2 + 3 * 4];

    struct noise_map
    {
        scalar     anim_period;
        Vecf3      anim_dir;
        detail_map noise;
    };

    noise_map primary_noise;

    noise_map secondary_noise;
};

C_FLAGS(shader_model::model_flags, u32)
C_FLAGS(shader_chicago::chicago_flags, u32)

struct multiplayer_scenario
{
    struct map_string
    {
        reflexive_t<byte_t> unknown_1;
        u32                 unknown_2;
        u32                 unknown_3;
        unicode_reflexive   unknown_4;

        u16 data[32];
    };

    struct level_ref
    {
        tagref_typed_t<tag_class_t::bitm> thumbnail;
        tagref_typed_t<tag_class_t::ustr> name;

        bl_string_var<36> map_name;
    };

    reflexive_t<level_ref> unknown_data;
};

} // namespace blam
