#pragma once

#include "tag_index.h"

#include <peripherals/semantic/chunk.h>

#include <algorithm>
#include <cstring>
#include <functional>
#include <memory>
#include <vector>

namespace blam::dimeter {

/*!
 * \brief Bytes of a raw block, alive for as long as the slice is held.
 * A view into a mapped file has nothing to keep; a slice read on demand
 * holds its buffer.
 */
struct raw_slice
{
    Span<const byte_t>          bytes;
    std::shared_ptr<void const> keep;

    inline byte_t const* data() const
    {
        return bytes.data();
    }
    inline size_t size() const
    {
        return bytes.size();
    }
    inline byte_t const* begin() const
    {
        return bytes.data();
    }
    inline byte_t const* end() const
    {
        return bytes.data() + bytes.size();
    }
    inline operator Span<const byte_t>() const
    {
        return bytes;
    }
};

/*! A cache file the raw region is read out of */
struct raw_source
{
    virtual ~raw_source() = default;

    virtual u64 size() const = 0;
    /*! [offset, offset+size), or nothing if out of range or unreadable */
    virtual optional<raw_slice> slice(u64 offset, u64 size) = 0;
};

/*! A file that is all in memory: slices are views */
struct span_source final : raw_source
{
    Span<const byte_t>          bytes;
    std::shared_ptr<void const> keep;

    span_source(Span<const byte_t> bytes, std::shared_ptr<void const> keep = {})
        : bytes(bytes)
        , keep(std::move(keep))
    {
    }

    u64 size() const override
    {
        return bytes.size();
    }
    optional<raw_slice> slice(u64 offset, u64 size) override
    {
        if(offset > bytes.size() || size > bytes.size() - offset)
            return std::nullopt;
        return raw_slice{bytes.subspan(offset, size), keep};
    }
};

/*!
 * \brief A file read on demand through `read`, in chunks kept most recent
 * first. A slice inside one chunk aliases it; one across chunks is copied.
 */
struct pread_source final : raw_source
{
    using reader_t = std::function<bool(u64 offset, Span<byte_t> dst)>;

    pread_source(
        u64      size,
        reader_t read,
        u32      chunk_size  = 4u << 20,
        u32      chunk_count = 16)
        : m_size(size)
        , m_read(std::move(read))
        , m_chunk_size(chunk_size)
        , m_chunk_count(chunk_count)
    {
    }

    u64 size() const override
    {
        return m_size;
    }

    optional<raw_slice> slice(u64 offset, u64 size) override
    {
        if(offset > m_size || size > m_size - offset)
            return std::nullopt;
        if(size == 0)
            return raw_slice{};
        u64 first = offset / m_chunk_size, last = (offset + size - 1) / m_chunk_size;
        if(first == last)
        {
            auto chunk = fetch(first);
            if(!chunk)
                return std::nullopt;
            u64 at = offset - first * m_chunk_size;
            if(at + size > chunk->size())
                return std::nullopt;
            return raw_slice{
                Span<const byte_t>(chunk->data() + at, size), chunk};
        }
        auto out = std::make_shared<std::vector<byte_t>>(size);
        u64  done = 0;
        for(u64 i = first; i <= last; i++)
        {
            auto chunk = fetch(i);
            if(!chunk)
                return std::nullopt;
            u64 at = i == first ? offset - first * m_chunk_size : 0;
            if(at > chunk->size())
                return std::nullopt;
            u64 take = std::min<u64>(chunk->size() - at, size - done);
            std::memcpy(out->data() + done, chunk->data() + at, take);
            done += take;
        }
        if(done != size)
            return std::nullopt;
        return raw_slice{Span<const byte_t>(out->data(), size), out};
    }

    void trim()
    {
        m_chunks = {};
    }

    /*! Bytes gone through `read` so far, for accounting */
    u64 bytes_read() const
    {
        return m_bytes_read;
    }

  private:
    using chunk_ptr = std::shared_ptr<std::vector<byte_t>>;
    struct chunk_t
    {
        u64       index;
        chunk_ptr data;
        u64       stamp;
    };

    chunk_ptr fetch(u64 index)
    {
        for(auto& c : m_chunks)
            if(c.index == index)
            {
                c.stamp = ++m_stamp;
                return c.data;
            }
        u64 offset = index * m_chunk_size;
        u64 size   = std::min<u64>(m_chunk_size, m_size - offset);
        auto data  = std::make_shared<std::vector<byte_t>>(size);
        if(!m_read(offset, Span<byte_t>(data->data(), size)))
            return {};
        m_bytes_read += size;
        if(m_chunks.size() >= m_chunk_count)
        {
            auto oldest = std::min_element(
                m_chunks.begin(), m_chunks.end(), [](auto& a, auto& b) {
                    return a.stamp < b.stamp;
                });
            *oldest = chunk_t{index, data, ++m_stamp};
        } else
            m_chunks.push_back(chunk_t{index, data, ++m_stamp});
        return data;
    }

    u64                  m_size;
    reader_t             m_read;
    u32                  m_chunk_size;
    u32                  m_chunk_count;
    std::vector<chunk_t> m_chunks;
    u64                  m_stamp{0};
    u64                  m_bytes_read{0};
};

/*!
 * \brief Resolves raw_refs across the cache file set. Slots follow
 * raw_file_t; missing side files just fail resolution for refs into them.
 */
struct raw_pool
{
    std::array<std::shared_ptr<raw_source>, 4> files;
    /*! Vista: the side map whose tags this map borrows; their "local" raw
     * data is in that file */
    raw_file_t shared_file{raw_file_t::local};

    inline void attach(raw_file_t slot, std::shared_ptr<raw_source> source)
    {
        files[static_cast<u8>(slot)] = std::move(source);
    }
    /*! A file already in memory */
    inline void attach(
        raw_file_t                  slot,
        Span<const byte_t>          bytes,
        std::shared_ptr<void const> keep = {})
    {
        attach(slot, std::make_shared<span_source>(bytes, std::move(keep)));
    }
    inline raw_source* file(raw_file_t slot) const
    {
        return files[static_cast<u8>(slot)].get();
    }
    inline u64 file_size(raw_file_t slot) const
    {
        auto* f = file(slot);
        return f ? f->size() : 0;
    }

    inline optional<raw_slice> resolve(raw_ref ref, u32 size) const
    {
        if(!ref.valid())
            return std::nullopt;
        auto* file = this->file(ref.file());
        if(!file)
            return std::nullopt;
        return file->slice(ref.offset(), size);
    }

    /*! A reference held by `owner`, pointed at the file it really lives
     * in: a tag borrowed from the shared map keeps its raw data there */
    template<heap_t Heap>
    inline raw_ref rebase(
        magic_ptr_base<Heap> const& magic, void const* owner, raw_ref ref) const
    {
        auto const* at = reinterpret_cast<byte_t const*>(owner);
        if(ref.valid() && ref.file() == raw_file_t::local &&
           shared_file != raw_file_t::local && magic.shared.base &&
           at >= magic.shared.base &&
           at < magic.shared.base + magic.shared.max_size)
            ref.raw = to_le(
                (static_cast<u32>(shared_file) << 30) |
                (ref.offset() & 0x3FFFFFFF));
        return ref;
    }

    template<heap_t Heap>
    inline optional<raw_slice> resolve(
        magic_ptr_base<Heap> const& magic,
        void const*                 owner,
        raw_ref                     ref,
        u32                         size) const
    {
        return resolve(rebase(magic, owner, ref), size);
    }
};

/*!
 * \brief Read-only view of a Halo 2 cache file. Wires together the file
 * header, the tag index and the pointer masks needed to resolve virtual
 * addresses, mirroring how Assembly's SecondGenCacheFile derives them:
 *
 * - Xbox: the index area mask is group_table_pointer - sizeof(tag_index_t)
 *   (the group table directly follows the index header), and the meta mask is
 *   first_tag.pointer - tag_table_size (tag data directly follows the index
 *   stream in the file).
 * - Vista: index pointers are already offsets relative to the index header,
 *   and the meta mask is stored in the file header.
 */
template<typename V>
requires is_game_version<V>
struct map_container
{
    using header_type = file_header_t<V>;
    using result_type = result<map_container, map_load_error>;

    header_type const* header{nullptr};
    tag_index_t const* index{nullptr};
    u32                index_mask{0};
    u32                meta_mask{0};

    Span<const byte_t> data;

    /*! Vista: the map every tag this one lists as null is borrowed from
     * (shared.map or single_player_shared.map); same table numbering */
    map_container const* shared{nullptr};

    static result_type from_bytes(semantic::BytesConst const& map, V)
    {
        if(map.size < sizeof(header_type))
            return stl_types::failure(map_load_error::map_file_too_small);

        auto const* header = reinterpret_cast<header_type const*>(map.data);
        if(!header->valid())
            return stl_types::failure(map_load_error::not_a_map);

        u32 meta_offset = from_le(header->meta_offset);
        if(meta_offset > map.size - sizeof(tag_index_t))
            return stl_types::failure(map_load_error::not_a_map);
        auto const* index =
            reinterpret_cast<tag_index_t const*>(map.data + meta_offset);
        /* Vista maps ship compressed; the header survives but the index magic
         * won't be found in that case */
        if(!index->valid())
            return stl_types::failure(map_load_error::not_a_map);

        /* Same version value either way, so tell the layouts apart by the
         * group table pointer: a virtual address on Xbox, a small
         * header-relative offset on Vista */
        u32  group_ptr = from_le(index->group_table_pointer);
        bool is_vista  = group_ptr < from_le(header->meta_size);
        if(is_vista != std::is_same_v<V, vista_version_t>)
            return stl_types::failure(
                map_load_error::incompatible_map_version_expected_halo2);

        map_container out{
            .header = header,
            .index  = index,
            .data   = Span<const byte_t>(map.data, map.size),
        };

        if constexpr(std::is_same_v<V, xbox_version_t>)
        {
            out.index_mask = group_ptr - sizeof(tag_index_t);
            auto tags      = out.tags();
            if(tags.empty())
                return stl_types::failure(map_load_error::not_a_map);
            out.meta_mask = tags[0].pointer() - from_le(header->tag_table_size);
        } else
        {
            out.index_mask = 0;
            out.meta_mask  = from_le(header->meta_offset_mask);
        }

        return out;
    }

    /*! Resolves pointers within the tag index (group + tag tables) */
    inline dimeter::index_magic index_magic() const
    {
        return dimeter::index_magic{
            .base        = data.data(),
            .max_size    = static_cast<u32>(data.size()),
            .meta_offset = from_le(header->meta_offset),
            .mask        = index_mask,
        };
    }

    /*! Resolves pointers within tag data (tag_t::data, tag_block), the
     * shared map's heap included */
    inline tag_magic magic() const
    {
        tag_magic out{
            .base        = data.data(),
            .max_size    = static_cast<u32>(data.size()),
            .meta_offset = from_le(header->meta_offset),
            .mask        = meta_mask,
        };
        if(shared)
            out.shared = {
                .base        = shared->data.data(),
                .max_size    = static_cast<u32>(shared->data.size()),
                .meta_offset = from_le(shared->header->meta_offset),
                .mask        = shared->meta_mask,
            };
        return out;
    }

    inline Span<group_t const> groups() const
    {
        i32 count = from_le(index->group_count);
        if(count <= 0)
            return {};
        auto        magic  = index_magic();
        auto const* groups = magic.template as<group_t>(
            from_le(index->group_table_pointer), static_cast<u32>(count));
        if(!groups)
            return {};
        return Span<group_t const>(groups, static_cast<size_t>(count));
    }

    inline Span<tag_t const> tags() const
    {
        i32 count = from_le(index->tag_count);
        if(count <= 0)
            return {};
        auto        magic = index_magic();
        auto const* tags  = magic.template as<tag_t>(
            from_le(index->tag_table_pointer), static_cast<u32>(count));
        if(!tags)
            return {};
        return Span<tag_t const>(tags, static_cast<size_t>(count));
    }

    inline tag_t const* tag_at(datum_index id) const
    {
        auto table = tags();
        u16  idx   = id.index();
        if(!id.valid() || idx >= table.size())
            return nullptr;
        if(!table[idx].valid() && shared)
            if(auto const* borrowed = shared->tag_at(id);
               borrowed && borrowed->valid())
                return borrowed;
        return &table[idx];
    }

    inline tag_t const* scenario() const
    {
        return tag_at(index->scenario_id);
    }

    inline tag_t const* globals() const
    {
        return tag_at(index->globals_id);
    }

    /*!
     * Tag names are stored in the header's file table, one entry per tag
     * table slot, looked up by position.
     */
    inline std::string_view name_of(tag_t const& tag) const
    {
        auto table = tags();
        if(shared &&
           (&tag < table.data() || &tag >= table.data() + table.size()))
            return shared->name_of(tag);
        if(table.empty() || &tag < table.data())
            return {};
        auto position = static_cast<u32>(&tag - table.data());
        return table_string(
            position,
            from_le(header->file_table_count),
            from_le(header->file_index_offset),
            from_le(header->file_table_offset),
            from_le(header->file_table_size));
    }

    /*! Resolve a string_id through the header's string table */
    inline std::string_view string(string_id id) const
    {
        return table_string(
            id.index(),
            from_le(header->string_count),
            from_le(header->string_index_offset),
            from_le(header->string_table_offset),
            from_le(header->string_table_size));
    }

    inline std::string_view name() const
    {
        return header->name.str();
    }

    inline std::string_view scenario_name() const
    {
        return header->scenario.str();
    }

    inline std::string_view build() const
    {
        return header->build.str();
    }

  private:
    /* Both the tag name table and the string_id table are an array of u32
     * offsets plus a packed buffer of NUL-terminated strings, addressed by
     * absolute file offsets from the header */
    inline std::string_view table_string(
        u32 position,
        u32 count,
        u32 index_offset,
        u32 buffer_offset,
        u32 buffer_size) const
    {
        u32 map_size = static_cast<u32>(data.size());
        if(position >= count)
            return {};
        if(index_offset > map_size || count > (map_size - index_offset) / 4)
            return {};
        if(buffer_offset > map_size || buffer_size > map_size - buffer_offset)
            return {};
        auto const* offsets =
            reinterpret_cast<u32 const*>(data.data() + index_offset);
        u32 string_offset = from_le(offsets[position]);
        if(string_offset >= buffer_size)
            return {};
        auto const* start = reinterpret_cast<char const*>(
            data.data() + buffer_offset + string_offset);
        auto max_len = static_cast<size_t>(buffer_size - string_offset);
        return std::string_view(start, ::strnlen(start, max_len));
    }
};

/*! The side map a Vista map borrows tags from, when the pool has it and
 * the tag tables line up; the caller keeps it alive and sets map.shared */
template<typename V>
inline std::optional<map_container<V>> load_shared(
    map_container<V> const& map, raw_pool& pool)
{
    if constexpr(!std::is_same_v<V, vista_version_t>)
        return std::nullopt;
    else
    {
        for(auto slot : {raw_file_t::shared, raw_file_t::single_player_shared})
        {
            auto* file = pool.file(slot);
            if(!file || !file->size())
                continue;
            /* A side map read on demand would be pulled in whole here */
            auto bytes = file->slice(0, file->size());
            if(!bytes)
                continue;
            auto side = map_container<V>::from_bytes(
                semantic::BytesConst::ofBytes(bytes->data(), bytes->size()), V{});
            if(!side.has_value() || from_le(side.value().index->tag_count) !=
                                        from_le(map.index->tag_count))
                continue;
            pool.shared_file = slot;
            return side.value();
        }
        return std::nullopt;
    }
}

} // namespace blam::dimeter
