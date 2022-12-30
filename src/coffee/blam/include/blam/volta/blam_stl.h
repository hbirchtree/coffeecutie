#pragma once

#include "cblam_map.h"
#include "cblam_scenario.h"
#include "cblam_structures.h"

#include <algorithm>
#include <coffee/compression/libz.h>
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

        auto            header = file_header_t::from_data(map, ver).value();
        semantic::Bytes decompressed;

        if(header->version == version_t::xbox)
        {
            using namespace Coffee::Compression;
            using Coffee::Zlib;

            auto compressed_segment = *map.at(
                sizeof(file_header_t),
                (map.size - sizeof(file_header_t) - header->trailing_space));

            /* Time to decompress! */
            decompressed = semantic::Bytes::Alloc(header->decomp_len);

            semantic::chunk_ops::MemCpy(
                semantic::BytesConst::ofBytes(*header),
                *decompressed.at(0, sizeof(file_header_t)));

            auto decompressed_region = *decompressed.at(sizeof(file_header_t));

            zlib_error_code ec;
            Zlib::Decompress(
                compressed_segment.view,
                &decompressed_region,
                Zlib::Opts(10_MB, 5, decompressed_region.size),
                ec);
            if(ec)
                return map_load_error::decompression_error;

            header = file_header_t::from_data(decompressed, ver).value();
        }

        if(!header)
            return map_load_error::not_a_map;

        auto const* tags_index = &tag_index_t<Ver>::from_header(header);

        return map_container{
            .map                = header,
            .tags               = tags_index,
            .magic              = tags_index->get_magic(header),
            .decompressed_store = std::move(decompressed),
        };
    }

    file_header_t const*    map;
    tag_index_t<Ver> const* tags;
    magic_data_t            magic;

    semantic::Bytes decompressed_store;

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
    tag_index_t<Ver> const& m_idx;
    file_header_t const*    m_file;
    magic_data_t            m_magic;

  public:
    class index_iterator
        : public stl_types::
              Iterator<stl_types::ForwardIteratorTag, tag_t const*>
    {
        friend class tag_index_view;

        i32                 i;
        tag_index_view*     idx;
        magic_data_t const& magic;

        auto const* deref()
        {
            return &(idx->tags()->tags(
                C_RCAST<file_header_t const*>(magic.base_ptr))[i]);
        }

        auto const* deref() const
        {
            return &(idx->tags()->tags(
                C_RCAST<file_header_t const*>(magic.base_ptr))[i]);
        }

        index_iterator(tag_index_view& idx, i32 i, magic_data_t const& magic) :
            i(i), idx(&idx), magic(magic)
        {
        }

      public:
        index_iterator& operator=(const index_iterator& other)
        {
            if(other == *this)
                return *this;

            if(&other.idx != &this->idx)
                this->idx = other.idx;
            this->i = other.i;

            return *this;
        }

        bool operator!=(index_iterator const& other) const
        {
            return i != other.i;
        }

        bool operator==(index_iterator const& other) const
        {
            return i == other.i;
        }

        index_iterator& operator++()
        {
            i++;
            return *this;
        }

        index_iterator operator++(int)
        {
            auto cpy = *this;
            i++;
            return cpy;
        }

        index_iterator operator+(i32 add)
        {
            auto cpy = *this;
            cpy.i += add;
            return cpy;
        }

        index_iterator& operator+=(i32 add)
        {
            i += add;
            return *this;
        }

        auto const* operator*()
        {
            return deref();
        }

        auto const* operator*() const
        {
            return deref();
        }

        operator std::string_view() const
        {
            return idx->tags()
                ->tags(C_RCAST<file_header_t const*>(magic.base_ptr))[i]
                .to_name()
                .to_string(magic);
        }
    };

    using iterator = index_iterator;

    tag_index_view(map_container<Ver> const& map) :
        m_idx(*map.tags), m_file(map.map), m_magic(map.magic)
    {
    }

    file_header_t const* file() const
    {
        return m_file;
    }
    auto const* tags() const
    {
        return &m_idx;
    }

    iterator begin()
    {
        return index_iterator(*this, 0, m_magic);
    }
    iterator end()
    {
        return index_iterator(*this, m_idx.tag_count, m_magic);
    }

    template<tag_class_t... Class>
    iterator find(tagref_typed_t<Class...> const& tag)
    {
        /* TODO: Add tag validation */

        auto tag_unwrapped = C_RCAST<tagref_t const&>(tag);
        return find(tag_unwrapped);
    }

    iterator find(tagref_t const& tag)
    {
        if(tag.unknown != 0)
            Throw(undefined_behavior("invalid tagref_t"));

        return find(tag.tag_id);
    }

    iterator find(u32 tag_id)
    {
        /* Tag index has sequential tag IDs, allowing binary search */
        i32  size = m_idx.tag_count;
        auto it   = begin();

        while(size > 1)
        {
            if(size % 2 == 0)
                size /= 2;
            else
                size = size / 2 + 1;

            auto mid     = it + size;
            auto mid_tag = (*mid)->tag_id;
            auto odd_tag = (*it)->tag_id;

            if(mid_tag == tag_id)
                return mid;

            if(tag_id > mid_tag)
                it = it + size;
        }

        return end();
    }
};

} // namespace blam
