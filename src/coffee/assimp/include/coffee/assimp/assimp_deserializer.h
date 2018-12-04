#pragma once

#include <coffee/core/types/chunk.h>
#include <coffee/core/url.h>
#include <coffee/interfaces/cgraphics_api_basic.h>
#include <coffee/interfaces/file_resolver.h>
#include <peripherals/stl/type_safety.h>
#include <peripherals/stl/types.h>

#include "assimp_iterators.h"
#include "assimp_material_iterators.h"

namespace Coffee {
namespace ASSIMP {

template<
    typename API,

    typename implements<RHI::GraphicsAPI_Base, API>::type* = nullptr

    >
struct SerializedData
{
    using SNL = MeshLoader::SerialNodeList;
    using MTL = MaterialParser::MaterialCollection;

    Vector<Bytes> store;

    SerialArray<typename API::V_ATTR> attributes;
    SerialArray<typename API::D_DATA> draws;
    typename API::D_CALL              draw_call;
    SNL::SerialHeader const*          nodes;
    MTL const*                        materials;
    Bytes const*                      vertices;
    Bytes const*                      elements;
};

using ResolveResource = Function<Bytes(Url const&, CString const&)>;

template<typename API>
static bool LoadModel(
    ResolverPair<BytesResolver> const& base, SerializedData<API>& output)
{
    using SerialHeader = typename SerializedData<API>::SNL::SerialHeader;

    enum FileIndices
    {
        FI_Vertices,
        FI_Elements,
        FI_Draws,
        FI_Attributes,
        FI_DCall,
        FI_Graph,
        FI_Materials,
    };

    Array<cstring, 7> extensions = {{"vertices",
                                     "elements",
                                     "draws",
                                     "attributes",
                                     "dcall",
                                     "graph",
                                     "materials"}};

    DProfContext _("ASSIMP::LoadModel");

    for(auto ext : extensions)
    {
        if(!ext)
            continue;

        DProfContext _("ASSIMP::Resolving model file");

        output.store.push_back(
            std::move(base.resolver.extResolver(base.url, ext)));

        /* If file resolution fails, escape! */
        if(!output.store.back().data)
            return false;
    }

    {
        DProfContext _("ASSIMP::Setting up model pointers");
        output.draws =
            SerialArray<typename API::D_DATA>(output.store.at(FI_Draws));
        output.attributes =
            SerialArray<typename API::V_ATTR>(output.store.at(FI_Attributes));
        output.draw_call = *C_RCAST<typename API::D_CALL const*>(
            output.store.at(FI_DCall).data);
        output.nodes =
            C_RCAST<SerialHeader const*>(output.store.at(FI_Graph).data);
        output.materials = C_RCAST<typename SerializedData<API>::MTL const*>(
            output.store.at(FI_Materials).data);
        output.vertices = &output.store.at(FI_Vertices);
        output.elements = &output.store.at(FI_Elements);
    }

    return true;
}

} // namespace ASSIMP
} // namespace Coffee
