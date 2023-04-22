#pragma once

#include "blam_base_types.h"
#include "blam_reflexive.h"
#include "blam_strings.h"
#include "blam_tag_ref.h"
#include "blam_versions.h"
#include "blam_vertex.h"

namespace blam {
namespace bitm {

struct bitmap_atlas_view;
struct header_t;

} // namespace bitm
namespace scn {
template<typename V>
requires is_game_version<V>
struct scenario;
} // namespace scn

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
        std::array<bl_tag, 3> tagclass; /*!< Strings which identify its class*/
        std::array<tag_class_t, 3> tagclass_e; /*!< enum-ified tagclass value */
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

    inline tag_class_t tag_class() const
    {
        return tagclass_e[0];
    }

    inline bool valid() const
    {
        return padding == 0;
    }

    template<typename T>
    inline result<T const*, error_msg> data(magic_data_t const& magic) const
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
            return "bitmap uses external storage, use blam::bitm::bitm_header_t"sv;
        }

        if(!valid())
            return "invalid tag"sv;

        return reflexive_t<T>{1, offset}.data(magic, single_value);
    }

    /*!
     * Special handling when opening bitmaps; they can either be external or
     *  internal, and this function simplifies that part.
     * The storage is indicated by the storage flag
     */
    result<std::pair<bitm::header_t const*, magic_data_t>, error_msg> image(
        magic_data_t const& magic, bitm::bitmap_atlas_view const& source) const;

    inline string_ref to_name() const
    {
        return name;
    }

    inline auto as_ref() const
    {
        tagref_t out = {
            .tag_class = tag_class(),
            .name      = name,
            .tag_id    = tag_id,
        };
        out.tag_class = tag_class();
        return out;
    }
};

template<typename V>
/*!
 * \brief A tag index contains information about the map, its resources and its
 * layout. Bitmaps, sounds, scenarios and etc. are referred to by this index.
 */
struct alignas(4) tag_index_t : stl_types::non_copy
{
    using vertex_array
        = reflexive_t<vert::mod2_vertex<vert::uncompressed>, xbox_t>;
    using index_array = reflexive_t<vert::idx_t, xbox_t>;

    i32          index_magic;    /*!< Number used to adjust indexes*/
    u32          base_tag;       /*!< Base tag, smallest tag id */
    u32          vertex_size;    /*!< Size of vertex data*/
    u32          tag_count;      /*!< Number of tags*/
    vertex_array vertex_objects; /*!< Number of vertex objects*/
    index_array  index_objects;  /*!< Number of index objects*/
    union
    {
        bl_tag xbox_tag_sentinel;
        u32    raw_model_data_size;
    };
    bl_tag pc_tag_sentinel; /*!< Says "tags" */

    inline magic_data_t get_magic(
        file_header_t const* base, u32 file_size) const
    {
        byte_t const* base_ptr   = C_RCAST<byte_t const*>(base);
        u32           index_size = base->version == version_t::xbox
                                       ? sizeof(tag_index_t) - sizeof(u32)
                                       : sizeof(tag_index_t);

        u32 max_size = std::max(file_size, base->decomp_len);
        return magic_data_t{
            {base_ptr, max_size},
            index_magic - (base->tag_index_offset + index_size)};
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
        return magic_data_t{{base.base_ptr, base.max_size}, 0};
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

    inline tag_t const* tag_at(const file_header_t* header, u32 tag_id) const
    {
        if(tag_id > base_tag + tag_count)
            return nullptr;
        else
            return &(tags(header)[(tag_id - base_tag)]);
    }

    inline optional<blam::scn::scenario<V> const*> scenario(
        file_header_t const* header, magic_data_t const& magic) const
    {
        tag_t const* tag = &(tags(header)[0]);

        if constexpr(std::is_same_v<V, custom_version_t>)
        {
            /* Some Custom Edition maps are terminally destroyed by modders
             * applying "protection", which means they fuck with the tags
             * system.
             * *sigh*
             * Let's just go easy on them.
             */
        } else if(!tag->matches(tag_class_t::scnr))
            return std::nullopt;

        auto data = tag->data<blam::scn::scenario<V>>(magic);

        if(!data.has_value())
            return std::nullopt;
        return data.value();
    }

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

static_assert(sizeof(tag_index_t<grbx_t>) == 40, "Invalid tag index size");
static_assert(sizeof(tag_index_t<xbox_t>) == 40, "Invalid tag index size");

} // namespace blam
