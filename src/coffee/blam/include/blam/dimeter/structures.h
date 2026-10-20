#pragma once

#include "../common/endian.h"
#include "../common/magic.h"
#include "../common/reference.h"
#include "../common/tag_class.h"
#include "../volta/blam_structures.h"

#include "versions.h"

#include <peripherals/enum/helpers.h>

#include <cstring>
#include <string_view>

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

/*! A tag class as a cache file stores it: the fourcc byte-reversed */
using tag_class = basic_tag_class<std::endian::little>;

/*!
 * \brief A tag's datum index on its own; the classes say which tags the
 * field may name, none admits any.
 */
template<tag_class_t... Classes>
struct tag_id
{
    datum_index id;

    inline u16 index() const
    {
        return id.index();
    }

    inline bool valid() const
    {
        return id.valid();
    }
};

static_assert(sizeof(tag_id<>) == 4);

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
 * \brief Pointer translation for one heap. Halo 2 stores virtual addresses of
 * the Xbox/Vista runtime; on Xbox the virtual base is derived from the tag
 * index (see map_container), on Vista it is stored in the file header; BSP
 * regions carry theirs in the scenario. Equivalent to Assembly's
 * MetaOffsetConverter.
 */
template<heap_t Heap>
struct magic_ptr_base : address_range
{
    static constexpr heap_t heap = Heap;

    /*! A second heap pointers may land in: Vista maps borrow tags from
     * shared.map, whose heap sits at its own virtual base */
    address_range shared{};

    /*! The bytes a heap pointer names, in whichever heap holds them */
    inline u8 const* resolve(u32 pointer, u32 size = 0) const
    {
        /* With two heaps a pointer below this heap's base belongs to the
         * other one; the wrap-around only serves single-heap maps */
        bool const below = shared.base && pointer < virtual_base;
        if(auto const* own = address_range::resolve(pointer, size); own && !below)
            return own;
        return shared.resolve(pointer, size);
    }

    template<typename T>
    inline T const* as(u32 pointer, u32 count = 1) const
    {
        return reinterpret_cast<T const*>(resolve(pointer, count * sizeof(T)));
    }
};

using index_magic = magic_ptr_base<heap_t::index>;
using tag_magic   = magic_ptr_base<heap_t::tag>;
using bsp_magic   = magic_ptr_base<heap_t::bsp>;

/*!
 * \brief Halo 2's equivalent of Halo 1's reflexive_t: a count plus a virtual
 * address into the heap the block lives in.
 */
template<typename T, heap_t Heap = heap_t::tag>
using tag_block = reference<T, xbox_version_t, atlas_type_t::map_file, Heap>;

static_assert(sizeof(tag_block<u32>) == 8);

/*! A block inside a structure BSP region, resolved with that region's
 * magic (volta's bsp_ptr) */
template<typename T>
using bsp_block = tag_block<T, heap_t::bsp>;

static_assert(resolves_with<bsp_block<u32>, bsp_magic>);
static_assert(!resolves_with<bsp_block<u32>, tag_magic>);
static_assert(!resolves_with<tag_block<u32>, bsp_magic>);

/*!
 * \brief Index into a tag block of T as the tags store it: a signed slot
 * (16-bit unless the tag packs it smaller), -1 for none. Resolves itself
 * against the block it names.
 */
template<typename T, typename I = i16>
struct block_index
{
    using value_type = T;
    using raw_type   = I;

    I raw;

    inline I value() const
    {
        return from_le(raw);
    }

    inline bool valid() const
    {
        return value() >= 0;
    }

    inline explicit operator bool() const
    {
        return valid();
    }

    /*! The element named, null when unset or out of range */
    inline T const* in(Span<T const> items) const
    {
        I i = value();
        return i >= 0 && static_cast<size_t>(i) < items.size() ? &items[i]
                                                               : nullptr;
    }

    template<heap_t Heap>
    inline T const* in(
        tag_block<T, Heap> const&   block,
        magic_ptr_base<Heap> const& magic) const
    {
        auto items = block.data(magic);
        return items.has_error() ? nullptr : in(items.value());
    }
};

static_assert(sizeof(block_index<u32>) == 2);

/*!
 * \brief Reference to another tag, resolved through the tag table by datum
 * index. The classes say which tags the field may name (volta's
 * tagref_typed_t); none admits any, and a typed reference converts to the
 * untyped one.
 */
template<tag_class_t... Classes>
struct alignas(4) tag_reference
{
    dimeter::tag_class tag_class;
    datum_index      tag_id;

    inline tag_class_t class_id() const
    {
        return tag_class.id();
    }

    inline bool valid() const
    {
        return tag_id.valid() && class_id() != tag_class_t::none;
    }

    inline bool matches(tag_class_t cls) const
    {
        return class_id() == cls;
    }

    /*! Whether the tag named is of a class the field admits */
    inline bool admitted() const
    {
        if constexpr(sizeof...(Classes) == 0)
            return true;
        else
            return ((class_id() == Classes) || ...);
    }

    inline tag_reference<> const& untyped() const
    {
        return *reinterpret_cast<tag_reference<> const*>(this);
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-conversion"

    template<typename Dummy = void>
    requires(sizeof...(Classes) > 0)
    inline operator tag_reference<> const&() const
    {
        return *reinterpret_cast<tag_reference<> const*>(this);
    }

#pragma GCC diagnostic pop
};

static_assert(sizeof(tag_reference<>) == 8);

/*! Any placeable object: what palettes and environment objects point at */
using object_reference = tag_reference<
    tag_class_t::bipd,
    tag_class_t::vehi,
    tag_class_t::weap,
    tag_class_t::eqip,
    tag_class_t::garb,
    tag_class_t::proj,
    tag_class_t::scen,
    tag_class_t::mach,
    tag_class_t::ctrl,
    tag_class_t::lifi,
    tag_class_t::ssce,
    tag_class_t::bloc,
    tag_class_t::crea>;

/*!
 * \brief Sized blob of data in the meta area.
 */
template<heap_t Heap = heap_t::tag>
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

    inline result<Span<const u8>, error_msg> data(
        magic_ptr_base<Heap> const& magic) const
    {
        i32 num = size();
        if(num == 0)
            return Span<const u8>();
        if(num < 0)
            return stl_types::failure("negative data_reference size"sv);
        auto const* bytes =
            magic.template as<u8>(pointer(), static_cast<u32>(num));
        if(!bytes)
            return stl_types::failure("data_reference pointer out of bounds"sv);
        return Span<const u8>(bytes, static_cast<size_t>(num));
    }
};

static_assert(sizeof(data_reference<>) == 8);

} // namespace blam::dimeter
