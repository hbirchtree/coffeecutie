#pragma once

#include "blam_scenario.h"
#include "blam_structures.h"

#include <algorithm>
#include <corez/zlib.h>
#include <functional>
#include <iterator>
#include <peripherals/semantic/chunk.h>

namespace blam {

template<typename Ver>
struct map_container
{
    STATICINLINE result<map_container, map_load_error> from_bytes(
        semantic::BytesConst const& map, Ver ver)
    {
        if(map.size < sizeof(file_header_t))
            return map_load_error::map_file_too_small;

        file_header_t const* header
            = file_header_t::from_data(map, ver).value();

        if(header->version != version_t::xbox)
        {
            if(!header)
                return map_load_error::not_a_map;
            auto const* tags_index = &tag_index_t<Ver>::from_header(header);
            return map_container{
                .map   = header,
                .tags  = tags_index,
                .magic = tags_index->get_magic(header, map.size),
                .store = map,
            };
        }

        semantic::mem_chunk<char> decompressed;
        using namespace libc_types::size_literals;
        using zlib = ::zlib::codec;

        auto compressed_segment = *map.at(
            sizeof(file_header_t),
            (map.size - sizeof(file_header_t) - header->trailing_space));

        /* Time to decompress! */
        decompressed
            = semantic::mem_chunk<char>::withSize(sizeof(file_header_t));
        header->copy_to(decompressed.as<file_header_t>()[0]);
        auto map_data = std::move(decompressed.allocation);
        map_data.reserve(header->decomp_len);

        auto err = zlib::decompress(
            compressed_segment.view,
            map_data,
            zlib::options_t{.chunk_size = 10_MB});
        if(err)
            return map_load_error::decompression_error;
        decompressed = semantic::mem_chunk<char>::ofContainer(map_data);
        header       = file_header_t::from_data(decompressed, ver).value();

        if(!header)
            return map_load_error::not_a_map;

        auto const* tags_index = &tag_index_t<Ver>::from_header(header);

        return map_container{
            .map          = header,
            .tags         = tags_index,
            .magic        = tags_index->get_magic(header, map.size),
            .store        = decompressed,
            .decompressed = std::move(map_data),
        };
    }

    file_header_t const*    map{nullptr};
    tag_index_t<Ver> const* tags{nullptr};
    magic_data_t            magic;

    semantic::mem_chunk<const char> store;
    std::vector<char>               decompressed{};

    inline std::string_view name() const
    {
        return map->full_mapname();
    }

    operator std::string_view() const
    {
        return name();
    }

    template<typename T>
    inline std::string_view get_name(T const* ref) const
    {
        return ref->to_name().to_string(magic);
    }
};

template<typename Ver>
class tag_index_view
{
    tag_index_t<Ver> const* m_idx;
    file_header_t const*    m_file;
    magic_data_t            m_magic;

  public:
    using span_type = semantic::Span<const tag_t>;
    using iterator  = span_type::iterator;
    //    class index_iterator
    //    {
    //        using size_type  = i32;
    //        using value_type = const tag_t*;

    //        friend class tag_index_view;

    //        i32                   i;
    //        tag_index_view const* idx;
    //        magic_data_t const*   magic;

    //        auto const* deref()
    //        {
    //            return &(idx->tags()->tags(
    //                C_RCAST<file_header_t const*>(magic->base_ptr))[i]);
    //        }

    //        auto const* deref() const
    //        {
    //            return &(idx->tags()->tags(
    //                C_RCAST<file_header_t const*>(magic->base_ptr))[i]);
    //        }

    //        index_iterator(
    //            tag_index_view const& idx, i32 i, magic_data_t const& magic) :
    //            i(i),
    //            idx(&idx), magic(&magic)
    //        {
    //        }

    //      public:
    //        index_iterator& operator=(const index_iterator& other) = default;

    //        bool operator!=(index_iterator const& other) const
    //        {
    //            return i != other.i;
    //        }

    //        bool operator==(index_iterator const& other) const
    //        {
    //            return i == other.i;
    //        }

    //        index_iterator& operator++()
    //        {
    //            i++;
    //            return *this;
    //        }

    //        index_iterator operator++(int)
    //        {
    //            auto cpy = *this;
    //            i++;
    //            return cpy;
    //        }

    //        index_iterator operator+(i32 add)
    //        {
    //            auto cpy = *this;
    //            cpy.i += add;
    //            return cpy;
    //        }

    //        index_iterator& operator+=(i32 add)
    //        {
    //            i += add;
    //            return *this;
    //        }

    //        auto const* operator*()
    //        {
    //            return deref();
    //        }

    //        auto const* operator*() const
    //        {
    //            return deref();
    //        }

    //        operator std::string_view() const
    //        {
    //            return idx->tags()
    //                ->tags(C_RCAST<file_header_t const*>(magic->base_ptr))[i]
    //                .to_name()
    //                .to_string(magic);
    //        }
    //    };

    //    using iterator = index_iterator;

    tag_index_view() : m_idx(nullptr), m_file(nullptr)
    {
    }

    tag_index_view(map_container<Ver> const& map) :
        m_idx(map.tags), m_file(map.map), m_magic(map.magic)
    {
    }

    file_header_t const* file() const
    {
        return m_file;
    }
    tag_t const* tags() const
    {
        return m_idx;
    }
    span_type span() const
    {
        if(!m_idx)
            return span_type();
        return span_type(m_idx->tags(m_file), m_idx->tag_count);
    }

    iterator begin() const
    {
        return span().begin();
    }
    iterator end() const
    {
        return span().end();
    }

    template<tag_class_t... Class>
    iterator find(tagref_typed_t<Class...> const& tag) const
    {
        /* TODO: Add tag validation */

        auto tag_unwrapped = *C_RCAST<tagref_t const*>(&tag);
        return find(tag_unwrapped);
    }

    iterator find(tagref_t const& tag) const
    {
        if(tag.unknown != 0)
            Throw(undefined_behavior("invalid tagref_t"));

        return find(tag.tag_id);
    }

    iterator find(u32 tag_id) const
    {
        /* Tag index has sequential tag IDs, allowing binary search */
        i32  size    = m_idx->tag_count;
        auto end_tag = (*(--end())).tag_id;
        auto it      = begin();

        if(tag_id > end_tag)
            return end();

        while(size > 1)
        {
            if(size % 2 == 0)
                size /= 2;
            else
                size = size / 2 + 1;
            auto remaining = std::distance(it, end()) - 1;
            size           = std::min<i32>(size, remaining);

            auto mid     = it + size;
            auto mid_tag = (*mid).tag_id;

            if(mid_tag == tag_id)
                return mid;

            if(tag_id > mid_tag)
                it = it + size;
            if(it == end())
                break;
        }

        return end();
    }
};

} // namespace blam
