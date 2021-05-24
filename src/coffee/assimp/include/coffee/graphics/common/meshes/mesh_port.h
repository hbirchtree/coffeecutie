#pragma once

#include "mesh.h"
#include "mesh_headers.h"

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
bool DeserializeMesh(Resource const& source, memory_mesh* dest)
{
    return dest->attributes.size() > 0;
}

}
}
