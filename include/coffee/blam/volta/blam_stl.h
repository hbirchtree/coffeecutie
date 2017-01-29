#pragma once

#include <algorithm>
#include <functional>
#include <iterator>
#include "cblam_structures.h"
#include "cblam_map.h"

namespace Coffee{
namespace Blam{

struct map_container
{
    map_container(c_ptr map, version_t ver)
    {
        this->map = file_header_get(map, ver);
        if(!this->map)
            throw std::runtime_error("Failed to load map; invalid data");

        tags = tag_index_get(this->map);
    }

    file_header_t const* map;
    tag_index_t tags;

    operator cstring()
    {
        return file_header_full_mapname(map);
    }

    cstring get_name(index_item_t const* idx)
    {
        return index_item_get_string(idx, map, &tags);
    }
};

class tag_index_view
{
    tag_index_t& m_idx;
    file_header_t const* m_file;
    index_item_t const* m_root;

public:

    class index_iterator : public std::iterator<std::forward_iterator_tag, index_item_t const*>
    {
        friend class tag_index_view;

        int32 i;
        tag_index_view* idx;

        index_item_t const* deref()
        {
            if(!idx->m_root)
                idx->m_root = tag_index_get_items(idx->m_file);
            return &idx->m_root[i];
        }

        index_iterator(tag_index_view& idx, int32 i):
            i(i),
            idx(&idx)
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

        bool operator !=(index_iterator const& other) const
        {
            return i != other.i;
        }

        bool operator ==(index_iterator const& other) const
        {
            return !(*this != other);
        }

        index_iterator& operator++()
        {
            i++;
            return *this;
        }

        index_item_t const* operator*()
        {
            return deref();
        }

        operator cstring()
        {
            return index_item_get_string(*(*this), idx->m_file, &idx->m_idx);
        }
    };

    using iterator = index_iterator;

    tag_index_view(map_container& map):
        m_idx(map.tags),
        m_file(map.map),
        m_root(nullptr)
    {
    }

    iterator begin()
    {
        return index_iterator(*this, 0);
    }
    iterator end()
    {
        return index_iterator(*this, m_idx.tagCount - 1);
    }
};

}
}
