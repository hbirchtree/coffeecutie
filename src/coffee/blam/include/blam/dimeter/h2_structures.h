#pragma once

#include "../volta/blam_endian.h"
#include "../volta/blam_structures.h"

#include "h2_versions.h"

namespace blam::dimeter {

enum class cache_type_t : i32
{
    singleplayer         = 0,
    multiplayer          = 1,
    mainmenu             = 2,
    shared               = 3, /*!< shared.map, multiplayer raw data */
    single_player_shared = 4, /*!< single_player_shared.map */
};

/*!
 * \brief Handle-style ID used for tags; low 16 bits index into the tag table,
 * high 16 bits are a salt value.
 */
struct datum_index
{
    u32 raw;

    inline u16 index() const
    {
        return from_le(raw) & 0xFFFF;
    }

    inline u16 salt() const
    {
        return from_le(raw) >> 16;
    }

    inline bool valid() const
    {
        return raw != 0xFFFFFFFF;
    }
};

static_assert(sizeof(datum_index) == 4);

/*!
 * \brief Halo 2 string IDs index into the map's string table (resolved
 * through the string index table in the file header).
 */
struct string_id
{
    u32 raw;

    inline u32 index() const
    {
        return from_le(raw) & 0xFFFF;
    }
};

static_assert(sizeof(string_id) == 4);

/*!
 * \brief Which cache file a raw-data offset points into. Every map may pull
 * raw payloads (bitmap pixels, sound chunks) from the three global cache
 * files in addition to its own.
 */
enum class raw_file_t : u8
{
    local                = 0,
    mainmenu             = 1, /*!< mainmenu.map */
    shared               = 2, /*!< shared.map */
    single_player_shared = 3, /*!< single_player_shared.map */
};

/*!
 * \brief Raw-data pointer as stored in bitmap/sound structures: top two bits
 * select the file, the rest is an absolute offset from that file's start.
 */
struct raw_ref
{
    u32 raw;

    inline bool valid() const
    {
        return raw != 0xFFFFFFFF;
    }

    inline raw_file_t file() const
    {
        return static_cast<raw_file_t>(from_le(raw) >> 30);
    }

    inline u32 offset() const
    {
        return from_le(raw) & 0x3FFFFFFF;
    }
};

static_assert(sizeof(raw_ref) == 4);

/*!
 * \brief Pointer translation for the meta (tag data) area. Halo 2 stores
 * virtual addresses of the Xbox/Vista runtime; a pointer translates to a
 * file offset as meta_offset + (pointer - mask). On Xbox the mask is derived
 * from the tag index (see map_container), on Vista it is stored in the file
 * header. Equivalent to Assembly's MetaOffsetConverter.
 */
struct magic_ptr
{
    byte_t const* base{nullptr};
    u32           max_size{0};
    u32           meta_offset{0};
    u32           mask{0};

    inline optional<u32> to_offset(u32 pointer, u32 size = 0) const
    {
        /* Unsigned wrap-around takes care of pointers below the mask */
        u32 offset = meta_offset + (pointer - mask);
        if(offset >= max_size || size > max_size - offset)
            return std::nullopt;
        return offset;
    }

    template<typename T>
    inline T const* as(u32 pointer, u32 count = 1) const
    {
        auto offset = to_offset(pointer, count * sizeof(T));
        if(!base || !offset)
            return nullptr;
        return reinterpret_cast<T const*>(base + *offset);
    }
};

/*!
 * \brief Halo 2's equivalent of Halo 1's reflexive_t: a count plus a virtual
 * address into the meta area.
 */
template<typename T>
struct tag_block
{
    i32 count_raw;
    u32 pointer_raw;

    inline i32 count() const
    {
        return from_le(count_raw);
    }

    inline u32 pointer() const
    {
        return from_le(pointer_raw);
    }

    inline result<Span<T const>, error_msg> data(magic_ptr const& magic) const
    {
        i32 num = count();
        if(num == 0)
            return Span<T const>();
        if(num < 0)
            return "negative tag_block count"sv;
        auto const* items = magic.as<T>(pointer(), static_cast<u32>(num));
        if(!items)
            return "tag_block pointer out of bounds"sv;
        return Span<T const>(items, static_cast<size_t>(num));
    }
};

static_assert(sizeof(tag_block<u32>) == 8);

/*!
 * \brief Reference to another tag, resolved through the tag table by datum
 * index.
 */
struct tag_reference
{
    bl_tag      tag_class;
    datum_index tag_id;
};

static_assert(sizeof(tag_reference) == 8);

/*!
 * \brief Sized blob of data in the meta area.
 */
struct data_reference
{
    i32 size_raw;
    u32 pointer_raw;

    inline i32 size() const
    {
        return from_le(size_raw);
    }

    inline u32 pointer() const
    {
        return from_le(pointer_raw);
    }

    inline result<Span<const byte_t>, error_msg> data(
        magic_ptr const& magic) const
    {
        i32 num = size();
        if(num == 0)
            return Span<const byte_t>();
        if(num < 0)
            return "negative data_reference size"sv;
        auto const* bytes = magic.as<byte_t>(pointer(), static_cast<u32>(num));
        if(!bytes)
            return "data_reference pointer out of bounds"sv;
        return Span<const byte_t>(bytes, static_cast<size_t>(num));
    }
};

static_assert(sizeof(data_reference) == 8);

} // namespace blam::dimeter
