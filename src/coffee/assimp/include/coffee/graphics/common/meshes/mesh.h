#pragma once

#include <coffee/core/types/chunk.h>
#include <peripherals/libc/types.h>

namespace Coffee {

//#define COFFEE_USE_OLD_MESH_MAP

/*!
 * \brief A simple mesh format, made for static vertex data.
 * Attributes can store anything, really, from transformation
 *  matrices to vectors and scalars.
 *
 */
struct memory_mesh
{
    enum AttributeTypes
    {
        Undefined,

        Indices,

        Position,
        TexCoord,
        Normal,
        Tangent,
        Bitangent,

        Color,
    };

    using attr_component = libc_types::byte_t;
    struct attr
    {
        attr() : attr_index(0)
        {
        }
        attr(attr&& a)
        {
            storage    = std::move(a.storage);
            view       = std::move(a.view);
            attr_index = a.attr_index;
        }

        Bytes              storage;
        Span<const byte_t> view;
        libc_types::u32    attr_index;
    };

    using AttributeType_t = libc_types::u16;

    memory_mesh(bool referenced) : referenced_attributes(referenced)
    {
    }

    memory_mesh() : memory_mesh(false)
    {
    }

    template<typename K, typename V>
    using maptype = stl_types::MultiMap<K, V>;

    using map_container = maptype<AttributeType_t, attr>;
    using map_pair      = typename map_container::value_type;

    map_container attributes;
    const bool    referenced_attributes;

    using iterator = map_container::iterator;

    /* Convenience functions for the attributes */

    FORCEDINLINE
    libc_types::szptr numAttributes()
    {
        return attributes.size();
    }
    FORCEDINLINE
    bool indexed()
    {
        auto range = attributes.equal_range(Indices);
        if(range.first == attributes.end())
            return false;
        return true;
    }
    FORCEDINLINE
    attr* attrGet(
        AttributeType_t t, libc_types::szptr stride, libc_types::u32 idx = 0)
    {
        (void)stride;

        auto range = attributes.equal_range(t);
        if(range.first == range.second)
            return nullptr;
        auto it = range.first;
        while(it != range.second)
        {
            if(it->second.attr_index == idx)
                return &it->second;
            it++;
        }
        return nullptr;
    }
    FORCEDINLINE
    attr const* attrGetConst(AttributeType_t t, libc_types::u32 idx = 0) const
    {
        auto range = attributes.equal_range(t);
        if(range.first == range.second)
            return nullptr;
        auto it = range.first;
        while(it != range.second)
        {
            if(it->second.attr_index == idx)
                return &it->second;
            it++;
        }
        return nullptr;
    }

    FORCEDINLINE
    libc_types::szptr attrSize(AttributeType_t t, libc_types::u32 idx = 0) const
    {
        auto ptr = attrGetConst(t, idx);
        if(ptr)
        {
            return ptr->view.size();
        }
        return 0;
    }

    FORCEDINLINE
    libc_types::szptr attrCount(
        AttributeType_t   t,
        libc_types::szptr stride,
        libc_types::u32   idx = 0) const
    {
        return attrSize(t, idx) / stride;
    }
    FORCEDINLINE
    libc_types::szptr indices(
        libc_types::szptr stride, libc_types::u32 idx = 0) const
    {
        return attrCount(Indices, stride, idx);
    }
    FORCEDINLINE
    libc_types::szptr vertices(
        AttributeType_t   t,
        libc_types::szptr stride,
        libc_types::u32   idx = 0) const
    {
        return attrCount(t, stride, idx);
    }

    template<typename VT>
    FORCEDINLINE void addAttributeData(
        AttributeType_t   a,
        VT const*         ptr,
        libc_types::szptr num,
        libc_types::u32   idx       = 0,
        bool              alloc_mem = false)
    {
        auto data_ptr = attrGet(a, idx);
        if(!data_ptr)
        {
            /* Bytes is a move-only structure, cannot copy that stuff */
            attributes.emplace(a, attr());
            data_ptr = attrGet(a, idx);
        }
        if(data_ptr)
        {
            auto& view = data_ptr->view;
            auto& data = data_ptr->storage;

            data_ptr->attr_index = idx;
            if(referenced_attributes && !alloc_mem)
            {
                view = BytesConst::ofBytes(ptr, num);
            } else
            {
                data = Bytes::withSize(data.size);
                MemCpy(BytesConst::ofBytes(ptr, num), data);
                view = data;
            }
        }
    }
    template<typename VT>
    FORCEDINLINE VT* getAttributeData(
        AttributeType_t attr, libc_types::u32 idx = 0)
    {
        auto ptr = attrGet(attr, idx);
        if(ptr)
        {
            return C_RCAST<VT const*>(ptr->view.data());
        }
        return nullptr;
    }
    template<typename VT>
    FORCEDINLINE VT const* getAttributeData(
        AttributeType_t attr, libc_types::u32 idx = 0) const
    {
        auto ptr = attrGetConst(attr, idx);
        if(ptr)
        {
            return C_RCAST<VT const*>(ptr->view.data());
        }
        return nullptr;
    }

    FORCEDINLINE
    void clearAttributes()
    {
        attributes.clear();
    }
};

using Mesh = memory_mesh;

} // namespace Coffee
