#pragma once

#include "mesh.h"
#include "mesh_headers.h"

#include <coffee/core/coffee.h>
#include <coffee/core/CFiles>

namespace Coffee{
namespace SMSH{

template<typename T>
void InsertBytes(Vector<byte_t>& buf, T* s, szptr num)
{
    byte_t* start = (byte_t*)&(s[0]);
    byte_t* end = (byte_t*)&(s[num]);
    buf.insert(buf.end(),start,end);
}

FORCEDINLINE
void SerializeMesh(_cbasic_mesh& mesh,
                                cstring fn)
{

    const constexpr szptr build_str_len = 40;

    CResources::Resource rsc(fn);

    Vector<byte_t> storage;

    {
        sbyte_t magic[10] = "CSMSH";
        sbyte_t build_str[build_str_len];

        MemClear(build_str,build_str_len);
        MemCpy(build_str,CoffeeBuildString,build_str_len);

        InsertBytes(storage,magic,sizeof(magic));
        InsertBytes(storage,build_str,build_str_len);
    }

    Vector<attr_grouping_t> groups;

    /* Retrieve all attributes, get their sizes */
    for(auto const& v : mesh.attributes)
    {
        attr_grouping_t g;
        g.id = v.first;
        g.content_size = v.second.data.size()*sizeof(v.second.data[0]);
        g.content_offset = 0;
        groups.push_back(g);
    }

    /* Insert number of attribute groups */
    {
        uint32 num = groups.size();
        InsertBytes(storage,&num,1);
    }

    szptr base_offset = groups.size()*sizeof(groups[0])+storage.size();

    /* Make another pass to add offsets */
    for(szptr i=0;i<groups.size();i++)
    {

        attr_grouping_t const* prev = nullptr;
        attr_grouping_t* curr = &groups[i];
        if(i>0)
            prev = &groups[i-1];

        curr->content_offset =
                base_offset +
                ((prev) ? prev->content_offset
                +prev->content_size : 0);
    }

    /* Insert group data into storage, this is only metadata */
    InsertBytes(storage,(byte_t*)groups.data(),groups.size()*sizeof(groups[0]));

    /* Finally, insert raw data (attribute data) */
    for(attr_grouping_t const& g : groups)
    {
        Vector<byte_t> const& data =
                mesh.attributes.find(g.id)->second.data;
        InsertBytes(storage,data.data(),data.size()*sizeof(data[0]));
    }

    rsc.size = storage.size();
    rsc.data = storage.data();

    CResources::FileCommit(rsc);
}

FORCEDINLINE
bool DeserializeMesh(CResources::Resource const& source, _cbasic_mesh* dest)
{
    smsh_header_t const* header = (smsh_header_t const*)source.data;
    smsh_groups_t const* groups = (smsh_groups_t const*)&header[1];

    if(!StrCmp(header->magic,magic_word))
        return false;

    byte_t const* base = (byte_t const*)source.data;

    for(uint32 i=0;i<header->num_groups;i++)
    {
        attr_grouping_t const& meta = groups->groups[i];
        byte_t const* data = base+meta.content_offset;
        dest->addAttributeData(meta.id,data,meta.content_size);
    }

    return dest->attributes.size()>0;
}

}
}
