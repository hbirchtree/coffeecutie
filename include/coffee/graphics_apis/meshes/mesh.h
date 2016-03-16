#pragma once

#include <coffee/core/types/vector_types.h>

namespace Coffee{

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
    };

    using AttributeType_t = uint16;

    _cbasic_mesh()
    {
    }

    Map<AttributeType_t,Vector<byte_t>> attributes;

    /* Convenience functions for the attributes */

    FORCEDINLINE
    szptr numAttributes()
    {
        return attributes.size();
    }
    FORCEDINLINE
    bool indexed()
    {
	return attributes.find(Indices)!=attributes.end();
    }
    FORCEDINLINE
    szptr indices(szptr stride)
    {
        auto it = attributes.find(Indices);
        if(it!=attributes.end())
            return it->second.size()/stride;
        else
            return 0;
    }
    FORCEDINLINE
    szptr vertices(AttributeType_t t,szptr stride)
    {
        auto it = attributes.find(t);
        if(it!=attributes.end())
            return it->second.size()/stride;
        else
            return 0;

    }

    template<typename VT>
    FORCEDINLINE
    void addAttributeData(AttributeType_t attr, VT const* ptr, szptr num)
    {
	Vector<byte_t>& data = attributes[attr];
	data.insert(data.end(),(byte_t*)&ptr[0],(byte_t*)&ptr[num]);
    }
    template<typename VT>
    FORCEDINLINE
    VT* getAttributeData(AttributeType_t attr)
    {
	if(attributes.find(attr)==attributes.end())
	    return nullptr;
	return (VT*)&attributes[attr][0];
    }
    template<typename VT>
    FORCEDINLINE
    VT const* getAttributeData(AttributeType_t attr) const
    {
	if(attributes.find(attr)==attributes.end())
	    return nullptr;
	return (VT const*)attributes.find(attr)->second.data();
    }

    FORCEDINLINE
    void clearAttributes()
    {
	attributes.clear();
    }
};

using Mesh = _cbasic_mesh;

}
