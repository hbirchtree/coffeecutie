#pragma once

#include <coffee/core/types/tdef/stltypes.h>

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
        u32 attr_index;
        Vector<attr_component> data;
        struct {
            c_cptr ptr;
            szptr size;
        } mem;
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

    template<typename VT>
    FORCEDINLINE
    void addAttributeData(AttributeType_t a, VT const* ptr, szptr num, u32 idx = 0)
    {
        auto data_ptr = attrGet(a, idx);
        if(!data_ptr)
        {
            attributes.insert({a, {}});
            data_ptr = attrGet(a, idx);
        }
        if(data_ptr)
        {
            data_ptr->attr_index = idx;
            if(referenced_attributes)
            {
                auto& data = data_ptr->mem;
                data.ptr = ptr;
                data.size = num * sizeof(VT);
            }else{
                auto& data = data_ptr->data;
                data.insert(data.end(), (byte_t*)&ptr[0], (byte_t*)&ptr[num]);
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
                return C_FCAST<VT*>(ptr->mem.ptr);
            else
                return C_FCAST<VT*>(ptr->data.data());
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
                return C_FCAST<VT const*>(ptr->mem.ptr);
            else
                return C_FCAST<VT const*>(ptr->data.data());
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
