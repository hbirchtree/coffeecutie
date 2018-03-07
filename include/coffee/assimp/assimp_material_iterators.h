#pragma once

#include <assimp/material.h>
#include <assimp/scene.h>
#include "cassimpimporters.h"
#include "assimp_data.h"
#include <coffee/core/VirtualFS>
#include <coffee/core/types/edef/pixenum.h>

namespace Coffee{
namespace ASSIMP{

struct MaterialParser
{
    struct MaterialCollection;

    using Vech2 = _cbasic_vec2<i16>;

    enum PropertyClass
    {
        Diffuse = 1,
    };

    struct Property
    {
        szptr data_offset;
        u32 property;
    };

    struct Material
    {
        szptr name_offset; /*!< Byte offset to name in string store */

        cstring name(MaterialCollection const& col) const
        {
            byte_t const* basePtr = C_RCAST<byte_t const*>(&col);
            szptr total_offset = col.string_segment + name_offset;
            return C_RCAST<cstring>(&basePtr[total_offset]);
        }
    };

    struct MatTex
    {
        szptr path_offset; /*!< Byte offset to path in string store */
        Vecf2 offset;
        Vecf2 scale;
        WrapPolicy wrap;
        scalar blend;
        u32 uvIdx;

        cstring path(MaterialCollection const& col) const
        {
            byte_t const* basePtr = C_RCAST<byte_t const*>(&col);
            szptr total_offset = col.string_segment + path_offset;
            return C_RCAST<cstring>(&basePtr[total_offset]);
        }
    };

    struct MaterialCollection
    {
        szptr num_materials;
        szptr data_segment;
        szptr string_segment;
        u32 num_properties;
        u32 _pad;

        struct MaterialContext
        {
            Material const& material;
        };

        Material const& material(szptr i = 0) const
        {
            if(i < num_materials)
                return C_RCAST<Material const*>(&this[1])[i];
            else
                throw std::out_of_range("invalid access");
        }
    };

    struct MaterialSerializer
    {
        MaterialCollection header;
        Vector<byte_t> materials;
        Vector<byte_t> data;
        Vector<CString> string_store;
    };

    static
    szptr PropertySize(PropertyClass c)
    {
        switch(c)
        {
        case Diffuse:
            return sizeof(Vecf3);
        default:
            return 0;
        }
    }

    static
    bool ExtractDescriptors(
            ASSIMP::AssimpPtr& scene,
            MaterialSerializer& materialsData
            )
    {
        auto materials = scene->scene->mMaterials;

        for(auto i : Range<>(scene->scene->mNumMaterials))
        {
            auto const& material = *materials[i];

            aiString name;
            material.Get(AI_MATKEY_NAME, name);
            aiString t_path;
            u32 t_uvIdx;
            ai_real t_blend;
            aiTextureMapping t_map;
            aiTextureMapMode t_mapMode;
            aiTextureOp t_op;
            material.GetTexture(aiTextureType_DIFFUSE, 0, &t_path,
                                &t_map, &t_uvIdx, &t_blend,
                                &t_op, &t_mapMode);

            materialsData.string_store.push_back(name.C_Str());
        }

        auto& header = materialsData.header;

        header.data_segment = sizeof(MaterialCollection)
                + materialsData.materials.size();
        header.string_segment = header.data_segment
                + materialsData.data.size();

        return true;
    }
};

}
}
