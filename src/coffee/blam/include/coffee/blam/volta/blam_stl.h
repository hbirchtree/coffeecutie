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

struct map_container
{
    template<typename T>
    map_container(semantic::Bytes map, T ver)
    {
        this->map = file_header_t::from_data(map, ver);

        if(this->map->version == version_t::xbox)
        {
            using namespace Coffee::Compression;
            using Coffee::Zlib;

            auto compressed_segment = map.at(
                sizeof(file_header_t),
                (map.size - sizeof(file_header_t) - this->map->trailing_space));

            /* Time to decompress! */
            decompressed_store = semantic::Bytes::Alloc(this->map->decomp_len);

            semantic::chunk_ops::MemCpy(
                semantic::Bytes::From(*this->map),
                decompressed_store.at(0, sizeof(file_header_t)));

            auto decompressed_region =
                decompressed_store.at(sizeof(file_header_t));

            zlib_error_code ec;
            Zlib::Decompress(
                compressed_segment,
                &decompressed_region,
                Zlib::Opts(10_MB),
                ec);
            C_ERROR_CHECK(ec);

            this->map = file_header_t::from_data(decompressed_store, ver);
        }

        if(!this->map)
            Throw(undefined_behavior("not a valid map"));

        static_assert(sizeof(tag_index_t) == 40, "Invalid tag index size");

        tags  = &tag_index_t::from_header(this->map);
        magic = tags->get_magic(this->map);
    }

    bool is_valid()
    {
        return this->map;
    }

    file_header_t const* map;
    tag_index_t const*   tags;
    magic_data_t         magic;

    semantic::Bytes decompressed_store;

    operator cstring()
    {
        return map->full_mapname();
    }

    template<typename T>
    inline cstring get_name(T const* ref) const
    {
        return ref->to_name().to_string(magic);
    }
};

class tag_index_view
{
    tag_index_t const&   m_idx;
    file_header_t const* m_file;
    magic_data_t         m_magic;

  public:
    class index_iterator
        : public stl_types::
              Iterator<stl_types::ForwardIteratorTag, tag_t const*>
    {
        friend class tag_index_view;

        i32                 i;
        tag_index_view*     idx;
        magic_data_t const& magic;

        tag_t const* deref()
        {
            return &(idx->tags()->tags(
                C_RCAST<file_header_t const*>(magic.base_ptr))[i]);
        }

        tag_t const* deref() const
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

        tag_t const* operator*()
        {
            return deref();
        }

        tag_t const* operator*() const
        {
            return deref();
        }

        operator cstring()
        {
            return idx->tags()
                ->tags(C_RCAST<file_header_t const*>(magic.base_ptr))[i]
                .to_name()
                .to_string(magic);
        }
    };

    using iterator = index_iterator;

    tag_index_view(map_container& map) :
        m_idx(*map.tags), m_file(map.map), m_magic(map.magic)
    {
    }

    file_header_t const* file() const
    {
        return m_file;
    }
    tag_index_t const* tags() const
    {
        return &m_idx;
    }

    iterator begin()
    {
        return index_iterator(*this, 0, m_magic);
    }
    iterator end()
    {
        return index_iterator(*this, m_idx.tagCount, m_magic);
    }

    iterator find(tagref_t const& tag)
    {
        return find(tag.tag_id);
    }

    iterator find(u32 tag_id)
    {
        /* Tag index has sequential tag IDs, allowing binary search */
        i32  size = m_idx.tagCount;
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
