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

    enum PropertyClass : u16
    {
        /* Base types, may be texture type depending on Texture flag */
        PDiffuse  = 0x1,
        /*!<
         * opacity is baked into A component here
         */
        PAmbient  = 0x2,
        PSpecular = 0x8,
        PNormal   = 0x10,
        PEmissive = 0x20,

        PSurface  = 0x80,
        /*!<
         * R = reflectivity
         * G = shininess
         * A = shininess strength
         */

        /* Subtypes for textures */
        PHeight   = 0x1000,
        PLightMap = 0x2000,

        /* Marks it as a texture */
        PTexture  = 0x4000,
    };

    struct MaterialMesh
    {
        u64 material;
        u64 mesh;
    };

    struct Property
    {
        u64 data_offset;
        u32 material;
        PropertyClass property;
        u16 _pad;

        template<typename T>
        T const* data(MaterialCollection const& col) const
        {
            byte_t const* basePtr = C_RCAST<byte_t const*>(&col);

            return C_RCAST<T const*>(basePtr
                                     + col.data_segment
                                     + data_offset);
        }
    };

    struct Material : non_copy
    {
        u64 name_offset; /*!< Byte offset to name in string store */

        szptr index(MaterialCollection const& col) const
        {
            for(auto i : Range<>(col.num_materials))
                if(this == &col.material(i))
                    return i;
            return C_CAST<szptr>(-1);
        }

        cstring name(MaterialCollection const& col) const
        {
            byte_t const* basePtr = C_RCAST<byte_t const*>(&col);
            szptr total_offset = col.string_segment + name_offset;
            return C_RCAST<cstring>(&basePtr[total_offset]);
        }

        Property const* property(
                MaterialCollection const& col,
                u32 property,
                u32 idx = 0
                ) const
        {
            byte_t const* basePtr = C_RCAST<byte_t const*>(&col)
                    + col.property_segment;

            auto properties = C_RCAST<Property const*>(basePtr);
            auto numProperties = col.data_segment - col.property_segment;
            numProperties /= sizeof(Property);

            auto selfIdx = this->index(col);

            if(selfIdx > col.num_materials)
                return nullptr;

            for(auto i : Range<>(numProperties))
                if(properties[i].material == selfIdx
                        && properties[i].property == property)
                {
                    if(idx-- > 0)
                        continue;

                    return &properties[i];
                }

            return nullptr;
        }
    };

    struct MatTex
    {
        u64 path_offset; /*!< Byte offset to path in string store */

        u32 uvIdx;
        scalar blend;

        Vecf2 offset;
        Vecf2 scale;

        WrapPolicy wrap;

        byte_t _pad[7];

        cstring path(MaterialCollection const& col) const
        {
            byte_t const* basePtr = C_RCAST<byte_t const*>(&col);
            szptr total_offset = col.string_segment + path_offset;
            return C_RCAST<cstring>(&basePtr[total_offset]);
        }
    };

    struct MaterialCollection : non_copy
    {
        u64 num_materials;
        u64 material_mesh_segment;
        u64 property_segment;
        u64 data_segment;
        u64 string_segment;

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

        szptr indexOf(Material const* mat) const
        {
            for(auto i : Range<>(num_materials))
                if(&material(i) == mat)
                    return i;

            return C_CAST<szptr>(-1);
        }

        Material const* meshMaterial(szptr meshIdx = 0) const
        {
            szptr num_meshes = (property_segment - material_mesh_segment);
            num_meshes /= sizeof(MaterialMesh);

            auto basePtr = C_RCAST<byte_t const*>(this);
            basePtr += material_mesh_segment;

            auto meshes = C_RCAST<MaterialMesh const*>(basePtr);

            for(auto i : Range<>(num_meshes))
                if(meshes[i].mesh == meshIdx)
                    return &material(meshes[i].material);

            return nullptr;
        }
    };

    struct MaterialSerializer
    {
        MaterialSerializer():
            header()
        {
        }

        MaterialCollection header;
        Vector<byte_t> data;
    };

    static
    szptr PropertySize(PropertyClass c)
    {
        switch(c)
        {
        case PDiffuse:
            return sizeof(Vecf3);
        default:
            return 0;
        }
    }

    struct AssimpProperty
    {
        cstring v1;
        u32 v2;
        u32 v3;
    };

    STATICINLINE
    PropertyClass TranslateTexType(aiTextureType type);

    template<typename T> STATICINLINE
    AssimpProperty TranslatePropertyType(PropertyClass type);

    static
    void ProcessTextureType(
            aiMaterial const& material,
            Vector<byte_t>& textureMaterials,
            Vector<CString>& stringStore,
            Vector<Property>& properties,
            szptr& stringStoreSize,
            u32 materialIdx,
            Vector<CString> const& baseDirs,
            aiTextureType texType = aiTextureType_DIFFUSE
            )
    {
        aiString t_path;
        u32 t_uvIdx;
        float t_blend;
        aiTextureMapping t_map;
        aiTextureMapMode t_mapMode;
        aiTextureOp t_op;

        for(auto i : Range<>(material.GetTextureCount(texType)))
        {
            material.GetTexture(texType, C_FCAST<u32>(i), &t_path,
                                &t_map, &t_uvIdx, &t_blend,
                                &t_op, &t_mapMode);


            MatTex texData = {
                stringStoreSize,

                t_uvIdx,
                t_blend,

                {},
                {},

                WrapPolicy::Clamp,
                {0},
            };

            Bytes texDataRef = Bytes::Create(texData);

            szptr dataStoreSize = textureMaterials.size();

            textureMaterials.insert(
                        textureMaterials.end(),
                        texDataRef.begin(), texDataRef.end()
                        );

            properties.push_back({
                        dataStoreSize,
                        materialIdx,
                        TranslateTexType(texType),
                        0
                        });

            CString path = t_path.C_Str();

            for(auto const& bdir : baseDirs)
                path = Search::CStrReplace(path, bdir, "");

            stringStore.push_back(path);
            stringStoreSize += path.size() + 1;
        }
    }

    template<typename T>
    static
    void ProcessProperty(
            aiMaterial const& material,
            Vector<byte_t>& storage,
            Vector<Property>& properties,
            PropertyClass c, u32 materialIdx
            )
    {
        Bytes colorBytes = {};

        T color;

        switch(c)
        {
        case PSurface:
        {
            material.Get(AI_MATKEY_REFLECTIVITY, color.r);
            material.Get(AI_MATKEY_SHININESS, color.g);
            material.Get(AI_MATKEY_SHININESS_STRENGTH, color.b);

            break;
        }
        default:
        {
            auto aiProp = TranslatePropertyType<T>(c);

            if(!aiProp.v1)
                return;

            material.Get(aiProp.v1, aiProp.v2, aiProp.v3, color);

            break;
        }
        }

        properties.push_back({
                                 storage.size(),
                                 materialIdx,
                                 c,
                                 0
                             });

        colorBytes = Bytes::Create(color);
        storage.insert(storage.end(),
                       colorBytes.begin(),
                       colorBytes.end());
    }

    template<szptr NumTexTypes>
    static
    bool ExtractDescriptors(
            ASSIMP::AssimpPtr& scene,
            MaterialSerializer& materialsData,
            Array<aiTextureType, NumTexTypes> const& textureTypes,
            Vector<PropertyClass> const& materialProps,
            Vector<CString> const& baseDirs = {}
            )
    {
        szptr stringStoreSize = 0;

        auto materials = scene->scene->mMaterials;

        Vector<Property> properties;
        Vector<Material> materialStore;
        Vector<CString> string_store;
        Vector<MaterialMesh> materialMeshConnection;

        for(auto i : Range<>(scene->scene->mNumMeshes))
        {
            auto const& mesh = *scene->scene->mMeshes[i];
            materialMeshConnection.push_back(
            {
                            mesh.mMaterialIndex,
                            i
                        });
        }

        for(auto i : Range<>(scene->scene->mNumMaterials))
        {
            auto const& material = *materials[i];

            aiString name;
            material.Get(AI_MATKEY_NAME, name);

            for(aiTextureType texType : textureTypes)
            {
                ProcessTextureType(
                            material, materialsData.data,
                            string_store,
                            properties,
                            stringStoreSize,
                            C_FCAST<u32>(i),
                            baseDirs,
                            texType
                            );
            }

            for(auto prop : materialProps)
            {
                ProcessProperty<aiColor3D>(
                            material, materialsData.data,
                            properties, prop, C_FCAST<u32>(i));
            }

            materialStore.emplace_back();
            materialStore.back().name_offset = stringStoreSize;

            string_store.push_back(name.C_Str());
            stringStoreSize += name.length + 1;
        }


        auto& data = materialsData.data;
        szptr dataSegmentSize = data.size();

        Bytes materialRef = Bytes::CreateFrom(materialStore);
        Bytes meshRef = Bytes::CreateFrom(materialMeshConnection);
        Bytes propertyRef = Bytes::CreateFrom(properties);
        Bytes headerRef = Bytes::Create(materialsData.header);

        data.insert(data.begin(),
                    propertyRef.begin(), propertyRef.end());
        data.insert(data.begin(),
                    meshRef.begin(), meshRef.end());
        data.insert(data.begin(),
                    materialRef.begin(), materialRef.end());

        for(auto const& str : string_store)
        {
            Bytes strData = Bytes::CreateString(str.c_str());
            data.insert(data.end(), strData.begin(), strData.end());
            data.insert(data.end(), 0);
        }

        string_store.clear();

        auto& header = materialsData.header;

        header.num_materials = materialStore.size();

        header.material_mesh_segment = sizeof(MaterialCollection)
                + sizeof(Material) * materialStore.size();
        header.property_segment = header.material_mesh_segment
                + sizeof(MaterialMesh) * materialMeshConnection.size();
        header.data_segment = header.property_segment
                + sizeof(Property) * properties.size();
        header.string_segment = header.data_segment
                + dataSegmentSize;

        data.insert(data.begin(),
                    headerRef.begin(), headerRef.end());

//        MaterialCollection* coll = C_RCAST<MaterialCollection*>(
//                    data.data());

//        for(auto i : Range<>(coll->num_materials))
//        {
//            auto const& m = coll->material(i);

//            auto diffTex = m.property(*coll, PTexture|PDiffuse);

//            cDebug("Material: {0}: {1} {2} {3}", i,
//                   m.name(*coll),
//                   *m.property(*coll, PDiffuse)->data<Vecf3>(*coll),
//                   (diffTex)
//                   ? diffTex->data<MatTex>(*coll)->path(*coll)
//                   : ""
//                     );
//        }

        return true;
    }
};

C_FLAGS(MaterialParser::PropertyClass, u16);

FORCEDINLINE
MaterialParser::PropertyClass MaterialParser::TranslateTexType(
        aiTextureType type)
{
    switch(type)
    {
    case aiTextureType_OPACITY:
    case aiTextureType_DIFFUSE:
        return PTexture|PDiffuse;
    case aiTextureType_AMBIENT:
        return PTexture|PAmbient;
    case aiTextureType_SPECULAR:
        return PTexture|PSpecular;
    case aiTextureType_EMISSIVE:
        return PTexture|PEmissive;

    case aiTextureType_NORMALS:
        return PTexture|PNormal;

    case aiTextureType_LIGHTMAP:
        return PTexture|PLightMap;
    case aiTextureType_DISPLACEMENT:
    case aiTextureType_HEIGHT:
        return PTexture|PHeight;

    case aiTextureType_REFLECTION:
    case aiTextureType_SHININESS:
        return PTexture|PSurface;

    default:
        return PTexture;
    }
}

template<>
FORCEDINLINE
MaterialParser::AssimpProperty
MaterialParser::TranslatePropertyType<aiColor3D>(
        MaterialParser::PropertyClass type
        )
{
    switch(type)
    {
    case PDiffuse:
        return {AI_MATKEY_COLOR_DIFFUSE};
    case PAmbient:
        return {AI_MATKEY_COLOR_AMBIENT};
    case PSpecular:
        return {AI_MATKEY_COLOR_SPECULAR};
    case PEmissive:
        return {AI_MATKEY_COLOR_SPECULAR};
    default:
        return {nullptr, 0, 0};
    }
}

}
}
