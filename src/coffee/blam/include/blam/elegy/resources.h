#pragma once

#include "map.h"

#include <map>
#include <memory>
#include <vector>

namespace blam::elegy {

/*! Writes `address` into the resource definition at `offset` */
struct resource_fixup_t
{
    be<i32> offset;
    be<u32> address;
};

static_assert(sizeof(resource_fixup_t) == 0x8);

/*! A D3D object living in the definition, e.g. a vertex buffer */
struct definition_fixup_t
{
    be<u32> address;
    be<i32> type; /*!< 0 vertex buffer, 1 index buffer */
};

static_assert(sizeof(definition_fixup_t) == 0x8);

/*!
 * \brief Where a fixed-up resource address points: the high nibble picks
 * the buffer, the rest is an offset into it.
 */
enum class resource_space_t : u8
{
    definition = 2, /*!< The resource's slice of the info buffer */
    data       = 4, /*!< The resource's bytes in its primary page */
    secondary  = 6, /*!< ...and in its secondary page, e.g. a bitmap's top mip */
};

struct resource_type_t
{
    std::array<u8, 16> guid;
    std::array<u16, 4> unknown_0;
    string_id          name; /*!< e.g. render_geometry_api_resource_definition */
};

static_assert(sizeof(resource_type_t) == 0x1C);

struct resource_t
{
    tag_class                      parent_group;
    std::array<u32, 2>             runtime;
    datum_index                    parent;
    be<u16>                        salt;
    be<i8>                         type_index;
    u8                             flags;
    be<i32>                        info_offset;
    be<i32>                        info_size;
    be<i32>                        alt_info_offset;
    be<u16>                        resource_bits;
    be<i16>                        segment_index;
    be<u32>                        base_definition_address;
    tag_block<resource_fixup_t>   resource_fixups;
    tag_block<definition_fixup_t> definition_fixups;

    inline bool valid() const
    {
        return parent_group.valid() && segment_index.value() >= 0;
    }
};

static_assert(offsetof(resource_t, info_offset) == 0x14);
static_assert(offsetof(resource_t, segment_index) == 0x22);
static_assert(offsetof(resource_t, resource_fixups) == 0x28);
static_assert(sizeof(resource_t) == 0x40);

/*!
 * \brief cache_file_resource_gestalt ("zone"): the resource table and the
 * info buffer resource definitions are sliced out of.
 */
struct resource_gestalt_t
{
    u32                         unknown_0;
    tag_block<resource_type_t> types;
    std::array<u8, 0x48>        unknown_1;
    tag_block<resource_t>       resources;
    std::array<u8, 0xD8>        unknown_2;
    data_reference              info_buffer;
};

static_assert(offsetof(resource_gestalt_t, types) == 0x4);
static_assert(offsetof(resource_gestalt_t, resources) == 0x58);
static_assert(offsetof(resource_gestalt_t, info_buffer) == 0x13C);

/*! A cache file other maps pull pages from, e.g. maps\shared.map */
struct external_cache_t
{
    bl_path            path;
    std::array<u32, 2> unknown_0;

    /*! File name only, no directory */
    inline std::string_view file_name() const
    {
        auto full = path.str();
        auto at   = full.find_last_of('\\');
        return at == std::string_view::npos ? full : full.substr(at + 1);
    }
};

static_assert(sizeof(external_cache_t) == 0x108);

enum class page_codec_t : i8
{
    none    = -1,
    deflate = 0,
};

struct raw_page_t
{
    be<u16>                           salt;
    u8                                flags;
    be<page_codec_t>                  codec;
    block_index<external_cache_t>     cache; /*!< -1: this file */
    be<i16>                           unknown_0;
    be<i32>                           offset; /*!< From the resource section */
    be<i32>                           compressed_size;
    be<i32>                           size;
    be<u32>                           checksum;
    std::array<std::array<u8, 20>, 3> hashes;
    be<i16>                           asset_count;
    be<i16>                           unknown_1;
};

static_assert(offsetof(raw_page_t, offset) == 0x8);
static_assert(sizeof(raw_page_t) == 0x58);

struct raw_size_t
{
    be<i32>                     size;
    tag_block<std::array<u32, 2>> parts;
};

static_assert(sizeof(raw_size_t) == 0x10);

/*!
 * \brief A resource's bytes: primary page (required data) and secondary
 * page (optional, higher detail), each at an offset.
 */
struct raw_segment_t
{
    block_index<raw_page_t> primary_page;
    block_index<raw_page_t> secondary_page;
    be<i32>                 primary_offset;
    be<i32>                 secondary_offset;
    block_index<raw_size_t> primary_size;
    block_index<raw_size_t> secondary_size;
};

static_assert(sizeof(raw_segment_t) == 0x10);

/*! cache_file_resource_layout_table ("play") */
struct resource_layout_t
{
    std::array<u32, 3>          unknown_0;
    tag_block<external_cache_t> external_caches;
    tag_block<raw_page_t>       pages;
    tag_block<raw_size_t>       sizes;
    tag_block<raw_segment_t>    segments;
};

static_assert(offsetof(resource_layout_t, pages) == 0x18);
static_assert(offsetof(resource_layout_t, segments) == 0x30);

/*!
 * \brief A block inside a resource definition. The pointer is zero in the
 * file; a resource fixup at its position gives the address.
 */
template<typename T>
struct resource_block
{
    using value_type = T;

    be<i32> count;
    be<u32> pointer;
    u32     runtime;
};

static_assert(sizeof(resource_block<u8>) == 0xC);

/*! A D3D object wrapper; its definition comes in through a fixup */
template<typename T>
struct d3d_structure
{
    be<u32>            definition;
    std::array<u32, 2> runtime;
};

static_assert(sizeof(d3d_structure<u8>) == 0xC);

/*!
 * \brief One resource, resolved: its definition with the fixups to read
 * it by, and the page bytes it addresses. Keeps the pages alive.
 */
struct resource_view
{
    Span<byte_t const>           definition;
    Span<byte_t const>           data;
    Span<byte_t const>           secondary;
    std::map<u32, u32>           fixups;
    u32                          root{0};
    std::shared_ptr<void const>  keep;
    std::shared_ptr<void const>  keep_secondary;

    template<typename T>
    inline T const* root_as() const
    {
        return at<T>(root);
    }

    template<typename T>
    inline T const* at(u32 offset, u32 count = 1) const
    {
        if(offset > definition.size() ||
           count * sizeof(T) > definition.size() - offset)
            return nullptr;
        return reinterpret_cast<T const*>(definition.data() + offset);
    }

    inline u32 offset_of(void const* field) const
    {
        return static_cast<u32>(
            reinterpret_cast<byte_t const*>(field) - definition.data());
    }

    /*! The fixed-up address stored at `field`, which must be in the
     * definition */
    inline optional<std::pair<resource_space_t, u32>> address(
        void const* field) const
    {
        auto it = fixups.find(offset_of(field));
        if(it == fixups.end())
            return std::nullopt;
        return std::make_pair(
            static_cast<resource_space_t>(it->second >> 28),
            it->second & 0x0FFFFFFF);
    }

    template<typename T>
    inline Span<T const> block(resource_block<T> const& block) const
    {
        i32 count = block.count;
        if(count <= 0)
            return {};
        auto where = address(&block.pointer);
        if(!where || where->first != resource_space_t::definition)
            return {};
        auto const* items = at<T>(where->second, static_cast<u32>(count));
        if(!items)
            return {};
        return Span<T const>(items, static_cast<size_t>(count));
    }

    template<typename T>
    inline T const* resolve(d3d_structure<T> const& object) const
    {
        auto where = address(&object.definition);
        if(!where || where->first != resource_space_t::definition)
            return nullptr;
        return at<T>(where->second);
    }

    /*! The page bytes a data_reference in the definition points to */
    inline Span<byte_t const> bytes(data_reference const& ref) const
    {
        i32  size  = ref.size;
        auto where = address(&ref.pointer);
        if(size <= 0 || !where)
            return {};
        Span<byte_t const> page;
        if(where->first == resource_space_t::data)
            page = data;
        else if(where->first == resource_space_t::secondary)
            page = secondary;
        else
            return {};
        if(where->second > page.size() ||
           static_cast<u32>(size) > page.size() - where->second)
            return {};
        return page.subspan(where->second, static_cast<size_t>(size));
    }
};

enum class resource_error
{
    no_gestalt,
    no_resource,
    bad_segment,
    missing_cache_file,
    page_out_of_bounds,
    decompression_failed,
};

/*! Inflates a raw-deflate page into `out` */
bool inflate_page(Span<byte_t const> compressed, Span<byte_t> out);

/*!
 * \brief Resolves resources for one map: the zone/play tags, external
 * cache files attached by name, decompressed pages kept by index.
 */
struct resource_loader
{
    map_container const*      map{nullptr};
    resource_gestalt_t const* gestalt{nullptr};
    resource_layout_t const*  layout{nullptr};

    static optional<resource_loader> create(map_container const& map)
    {
        auto const* zone = map.find("zone");
        auto const* play = map.find("play");
        if(!zone || !play)
            return std::nullopt;
        resource_loader out;
        out.map     = &map;
        out.gestalt = map.tag_data<resource_gestalt_t>(*zone);
        out.layout  = map.tag_data<resource_layout_t>(*play);
        if(!out.gestalt || !out.layout)
            return std::nullopt;
        return out;
    }

    /*! Attach an external cache, e.g. shared.map, by its file name */
    inline void attach(std::string_view file_name, map_container const& cache)
    {
        m_caches.emplace_back(std::string(file_name), &cache);
    }

    inline Span<resource_t const> resources() const
    {
        return gestalt->resources.data(map->magic()).value_or(Span<resource_t const>());
    }

    inline std::string_view type_name(resource_t const& res) const
    {
        auto types = gestalt->types.data(map->magic()).value_or(Span<resource_type_t const>());
        i8   idx   = res.type_index;
        if(idx < 0 || static_cast<size_t>(idx) >= types.size())
            return {};
        return map->string(types[static_cast<size_t>(idx)].name);
    }

    inline resource_t const* resource(datum_index id) const
    {
        auto table = resources();
        if(!id.valid() || id.index() >= table.size())
            return nullptr;
        auto const& res = table[id.index()];
        if(res.salt.value() != id.salt() || !res.valid())
            return nullptr;
        return &res;
    }

    inline Span<external_cache_t const> external_caches() const
    {
        return layout->external_caches.data(map->magic()).value_or(Span<external_cache_t const>());
    }

    inline result<resource_view, resource_error> load(datum_index id)
    {
        using stl_types::failure;
        auto const* res = resource(id);
        if(!res)
            return failure(resource_error::no_resource);
        auto magic    = map->magic();
        auto segments = layout->segments.data(magic).value_or(Span<raw_segment_t const>());
        auto const* segment = block_index<raw_segment_t>{res->segment_index}
                                  .resolve(segments);
        if(!segment)
            return failure(resource_error::bad_segment);
        auto sizes = layout->sizes.data(magic).value_or(Span<raw_size_t const>());
        /* A segment's bytes in one page, trimmed to its size entry */
        auto part = [&](block_index<raw_page_t> index,
                        i32                     at,
                        block_index<raw_size_t> size,
                        Span<byte_t const>&     out,
                        std::shared_ptr<void const>& keep)
            -> optional<resource_error> {
            if(!index.valid())
                return std::nullopt;
            auto page = this->page(index);
            if(!page.has_value())
                return page.error();
            auto const& bytes = *page.value();
            if(at < 0 || static_cast<u32>(at) > bytes.size())
                return resource_error::page_out_of_bounds;
            out = Span<byte_t const>(bytes.data(), bytes.size()).subspan(at);
            if(auto const* entry = size.resolve(sizes);
               entry && entry->size.value() >= 0 &&
               static_cast<size_t>(entry->size.value()) < out.size())
                out = out.subspan(0, static_cast<size_t>(entry->size.value()));
            keep = page.value();
            return std::nullopt;
        };

        resource_view out;
        if(auto error = part(
               segment->primary_page,
               segment->primary_offset,
               segment->primary_size,
               out.data,
               out.keep))
            return failure(*error);
        auto info = gestalt->info_buffer.data(magic);
        i32  info_offset = res->info_offset, info_size = res->info_size;
        if(info_size > 0 && static_cast<u32>(info_offset) <= info.size() &&
           static_cast<u32>(info_size) <= info.size() - info_offset)
            out.definition = info.subspan(info_offset, info_size);
        /* Optional detail; the primary page is usable without it */
        part(
            segment->secondary_page,
            segment->secondary_offset,
            segment->secondary_size,
            out.secondary,
            out.keep_secondary);
        out.root = res->base_definition_address.value() & 0x0FFFFFFF;
        for(auto const& fix : res->resource_fixups.data(magic).value_or(Span<resource_fixup_t const>()))
            out.fixups[static_cast<u32>(fix.offset.value())] = fix.address;
        return out;
    }

    /*! A page decompressed, cached by index */
    inline result<std::shared_ptr<std::vector<byte_t> const>, resource_error>
    page(block_index<raw_page_t> index)
    {
        using stl_types::failure;
        auto pages = layout->pages.data(map->magic()).value_or(Span<raw_page_t const>());
        auto const* page = index.resolve(pages);
        if(!page)
            return failure(resource_error::bad_segment);
        if(auto it = m_pages.find(index.value()); it != m_pages.end())
            return it->second;

        map_container const* source = map;
        if(page->cache.valid())
        {
            auto const* cache = page->cache.resolve(external_caches());
            source            = cache ? attached(cache->file_name()) : nullptr;
            if(!source)
                return failure(resource_error::missing_cache_file);
        }
        u64 start = u64(source->resource_offset()) + u32(page->offset.value());
        u64 csize = static_cast<u32>(page->compressed_size.value());
        u64 usize = static_cast<u32>(page->size.value());
        if(start > source->data.size() || csize > source->data.size() - start)
            return failure(resource_error::page_out_of_bounds);
        auto compressed = source->data.subspan(start, csize);
        auto out        = std::make_shared<std::vector<byte_t>>();
        if(page->codec.value() == page_codec_t::none)
            out->assign(compressed.begin(), compressed.end());
        else
        {
            out->resize(usize);
            if(page->codec.value() != page_codec_t::deflate ||
               !inflate_page(compressed, Span<byte_t>(out->data(), out->size())))
                return failure(resource_error::decompression_failed);
        }
        m_pages[index.value()] = out;
        return std::shared_ptr<std::vector<byte_t> const>(out);
    }

    /*! Drop decompressed pages */
    inline void trim()
    {
        m_pages.clear();
    }

  private:
    inline map_container const* attached(std::string_view file_name) const
    {
        for(auto const& [name, cache] : m_caches)
            if(name == file_name)
                return cache;
        return nullptr;
    }

    std::vector<std::pair<std::string, map_container const*>> m_caches;
    std::map<i16, std::shared_ptr<std::vector<byte_t>>>       m_pages;
};

} // namespace blam::elegy
