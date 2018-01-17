#pragma once

#include "cassimpimporters.h"
#include "assimp_data.h"

#include <assimp/scene.h>
#include <assimp/vector3.h>
#include <assimp/vector2.h>
#include <assimp/color4.h>

#include <coffee/graphics/common/SMesh>
#include <coffee/interfaces/cgraphics_api.h>

namespace Coffee{
namespace ASSIMP{

struct MeshLoader
{
    using M = Mesh;

    using AttrType = M::AttributeTypes;

    struct Attr
    {
        Attr(AttrType t, u32 ch = 0):
            type(t),
            channel(ch)
        {
        }

        AttrType type;
        u32 channel;
    };

    struct DrawInfo
    {
        DrawInfo():
            element_type(TypeEnum::UInt)
        {
        }

        TypeEnum element_type;
    };

    struct ChainedBytes
    {
        Vector<Bytes> refs;

        FORCEDINLINE bool cpy(void* target, szptr size)
        {
            szptr ptr = 0;

            /* First, do a dry run to check that the copy will succeed */
            for(auto const& data : refs)
            {
                if(ptr + data.size > size)
                    return false;
                ptr += data.size;
            }

            ptr = 0;

            byte_t* targetP = C_RCAST<byte_t*>(target);

            for(auto const& data : refs)
            {
                MemCpy(&targetP[ptr], data.data, data.size);
                ptr += data.size;
            }
        }
    };

//    template<typename GFX>
    struct BufferDescription
    {
        using GFX = RHI::NullAPI;

        typename GFX::D_CALL call;
        Vector<typename GFX::V_ATTR> attributes;
        Vector<typename GFX::D_DATA> draws;

        ChainedBytes vertexData;
        ChainedBytes elementData;
    };

    template<typename API>
    bool ExtractDescriptors(
            ASSIMP::AssimpPtr& scene,
            Vector<Attr>const& attributes,
            DrawInfo const& draw,
            BufferDescription<API>& buffers
            )
    {
        i32 meshCount = 0;

        if(!ASSIMP::GetMeshCount(scene, meshCount))
            return false;

        /* Helper class for calculating attribute sizes */
        Vector<Mesh> meshes;
        meshes.resize(C_FCAST<szptr>(meshCount));

        buffers.attributes.resize(attributes.size());
        buffers.draws.resize(C_FCAST<szptr>(meshCount));

        szptr mesh_buffer_size = 0,
                element_buffer_size = 0,
                vertex_size = 0;

        for(auto i : Range<i32>(meshCount))
        {
            auto& mesh = meshes[i];
            auto& di = buffers.draws[i];

            ASSIMP::GetMeshData(scene, i, mesh);

            di.m_eltype = TypeEnum::UInt;
            di.m_elems = mesh.attrSize(M::Indices) / sizeof(u32);
            di.m_verts = mesh.attrSize(M::Position) / sizeof(Vecf3);
            di.m_voff = vertex_size / sizeof(Vecf3);

            for(auto j : Range<i32>(attributes.size()))
            {
                /* Calculate attribute offsets */
                for(i32 k=j+1;k<attributes.size();k++)
                    buffers.attributes[k].m_boffset +=
                            mesh.attrSize(attributes[j].type,
                                          attributes[j].channel);

                /* Calculate total vertex buffer size for all attributes */
                mesh_buffer_size += mesh.attrSize(attributes[j].type,
                                                  attributes[j].channel);
            }
            /* Calculate element buffer size */
            element_buffer_size += mesh.attrSize(M::Indices);
        }

        /* Initialize some vertex attribute information */
        for(auto i : Range<i32>(attributes.size()))
        {
            auto& vd = buffers.attributes[i];
            vd.m_idx = i; /* The user will have to define this or bind it */
            vd.m_type = TypeEnum::Scalar;
            switch(attributes[i].type)
            {
            case AttrType::Position:
            case AttrType::Color:
            case AttrType::Normal:
            case AttrType::Tangent:
            case AttrType::Bitangent:
                vd.m_size = 3;
                break;
            case AttrType::TexCoord:
                vd.m_size = 2;
                break;
            }
        }

        return true;
    }
};

}
}
