#pragma once

#include <coffee/core/types/tdef/stltypes.h>
#include <coffee/core/types/cdef/memtypes.h>

namespace Coffee{

//#define COFFEE_USE_OLD_MESH_MAP

/*!
 * \brief A simple mesh format, made for static vertex data.
 * Attributes can store anything, really, from transformation
 *  matrices to vectors and scalars.
 *
 */
struct _cbasic_mesh
{
    enum AttributeTypes
    {
        Undefined,

        Indices,

        Position, TexCoord,
        Normal,
        Tangent, Bitangent,

        Color,
    };

    using attr_component = byte_t;
    struct attr
    {
        attr():
            data(),
            mem(),
            attr_index(0)
        {
        }
        attr(attr&& a)
        {
            data = std::move(a.data);
            mem = std::move(a.mem);
            attr_index = a.attr_index;
        }

        Vector<attr_component> data;
        Bytes mem;
        u32 attr_index;
    };

    using AttributeType_t = uint16;

    _cbasic_mesh(bool referenced):
        referenced_attributes(referenced)
    {
    }

    _cbasic_mesh():
        _cbasic_mesh(false)
    {
    }

    template<typename K, typename V>
    using maptype = MultiMap<K,V>;

    using map_container = maptype<AttributeType_t, attr>;
    using map_pair = Pair<AttributeType_t, attr>;

    map_container attributes;
    const bool referenced_attributes;

    using iterator = map_container::iterator;

    /* Convenience functions for the attributes */

    FORCEDINLINE
    szptr numAttributes()
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
    attr* attrGet(AttributeType_t t, szptr stride, u32 idx = 0)
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
            it ++;
        }
        return nullptr;
    }
    FORCEDINLINE
    attr const* attrGetConst(AttributeType_t t, u32 idx = 0) const
    {
        auto range = attributes.equal_range(t);
        if(range.first == range.second)
            return nullptr;
        auto it = range.first;
        while(it != range.second)
        {
            if(it->second.attr_index == idx)
                return &it->second;
            it ++;
        }
        return nullptr;
    }

    FORCEDINLINE
    szptr attrSize(AttributeType_t t, u32 idx = 0) const
    {
        auto ptr = attrGetConst(t, idx);
        if(ptr)
        {
            if(referenced_attributes)
                return ptr->mem.size;
            else
                return ptr->data.size();
        }
        return 0;
    }

    FORCEDINLINE
    szptr attrCount(AttributeType_t t,szptr stride, u32 idx = 0) const
    {
        return attrSize(t, idx) / stride;
    }
    FORCEDINLINE
    szptr indices(szptr stride, u32 idx = 0) const
    {
        return attrCount(Indices, stride, idx);
    }
    FORCEDINLINE
    szptr vertices(AttributeType_t t, szptr stride, u32 idx = 0) const
    {
        return attrCount(t, stride, idx);
    }

    static void secretAllocDestructor(Bytes& data)
    {
        CFree(data.data);
    }

    template<typename VT>
    FORCEDINLINE
    void addAttributeData(
            AttributeType_t a, VT const* ptr, szptr num, u32 idx = 0,
            bool alloc_mem = false)
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
            data_ptr->attr_index = idx;
            if(referenced_attributes)
            {
                auto& data = data_ptr->mem;
                data.size = num * sizeof(VT);
                if(alloc_mem)
                {
                    data.data = C_RCAST<byte_t*>(Alloc(data.size));
                    MemCpy(data.data, ptr, data.size);
                    Bytes::SetDestr(data, secretAllocDestructor);
                }
                else
                    data.data = C_RCAST<byte_t*>(C_CCAST<VT*>(ptr));
            }else{
                auto& data = data_ptr->data;
                data.insert(
                            data.end(),
                            C_RCAST<byte_t const*>(&ptr[0]),
                        C_RCAST<byte_t const*>(&ptr[num]));
            }
        }
    }
    template<typename VT>
    FORCEDINLINE
    VT* getAttributeData(AttributeType_t attr, u32 idx = 0)
    {
        auto ptr = attrGet(attr, idx);
        if(ptr)
        {
            if(referenced_attributes)
                return C_RCAST<VT*>(ptr->mem.data);
            else
                return C_RCAST<VT*>(ptr->data.data());
        }
        return nullptr;
    }
    template<typename VT>
    FORCEDINLINE
    VT const* getAttributeData(AttributeType_t attr, u32 idx = 0) const
    {
        auto ptr = attrGetConst(attr, idx);
        if(ptr)
        {
            if(referenced_attributes)
                return C_RCAST<VT const*>(ptr->mem.data);
            else
                return C_RCAST<VT const*>(ptr->data.data());
        }
        return nullptr;
    }

    FORCEDINLINE
    void clearAttributes()
    {
        attributes.clear();
    }
};

using Mesh = _cbasic_mesh;

}
