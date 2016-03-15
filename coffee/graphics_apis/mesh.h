#pragma once

#include <coffee/core/types/vector_types.h>
#include <coffee/core/CFiles>
#include <coffee/core/coffee.h>

namespace Coffee{

template<typename IT>
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

    using AttributeType_t = uint8;

    _cbasic_mesh()
    {
    }

    Map<AttributeType_t,Vector<byte_t>> attributes;

    FORCEDINLINE
    bool indexed()
    {
	return attributes.find(Indices)!=attributes.end();
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

using Mesh = _cbasic_mesh<uint32>;

template<typename T>
void InsertBytes(Vector<byte_t>& buf, T* s, szptr num)
{
    byte_t* start = (byte_t*)&(s[0]);
    byte_t* end = (byte_t*)&(s[num]);
    buf.insert(buf.end(),start,end);
}

template<typename IT>
FORCEDINLINE void SerializeMesh(_cbasic_mesh<IT> const& mesh,
				cstring fn)
{
    typedef struct grouping
    {
	szptr content_size;
	int64 content_offset;
	byte_t id;

	uint8 pad_1;
	uint16 pad_2;
	uint32 pad_3;
    } attr_grouping;

    const constexpr szptr build_str_len = 40;

    CResources::Resource rsc(fn);

    Vector<byte_t> storage;

    sbyte_t magic[10] = "CSMSH";
    sbyte_t build_str[build_str_len];

    MemClear(build_str,build_str_len);
    StrCpy(build_str,CoffeeBuildString);

    InsertBytes(storage,magic,sizeof(magic));
    InsertBytes(storage,build_str,build_str_len);

    Vector<attr_grouping> groups;

    for(std::pair<Mesh::AttributeType_t,Vector<byte_t>> const& v : mesh.attributes)
    {
	attr_grouping g;
	g.id = v.first;
	g.content_size = v.second.size()*sizeof(v.second[0]);
	g.content_offset = 0;
	groups.push_back(g);
    }

    /* Insert number of attribute groups */
    {
	uint32 num = groups.size();
	InsertBytes(storage,&num,1);
    }

    szptr base_offset = groups.size()*sizeof(groups[0])+storage.size();

    for(szptr i=0;i<groups.size();i++)
    {

	attr_grouping const* prev = nullptr;
	attr_grouping* curr = &groups[i];
	if(i>0)
	    prev = &groups[i-1];

	curr->content_offset =
		base_offset +
		((prev) ? prev->content_offset
		+prev->content_size : 0);
    }

    InsertBytes(storage,(byte_t*)groups.data(),groups.size()*sizeof(groups[0]));

    for(attr_grouping const& g : groups)
    {
	Vector<byte_t> const& data =
		mesh.attributes.find(g.id)->second;
	InsertBytes(storage,data.data(),data.size()*sizeof(data[0]));
    }

    rsc.size = storage.size();
    rsc.data = storage.data();

    CResources::FileCommit(rsc);
}

}
