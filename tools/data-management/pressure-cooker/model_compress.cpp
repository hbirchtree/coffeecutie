#include <coffee/core/coffee_mem_macros.h>
#include <coffee/core/CFiles>
#include <coffee/core/CDebug>
#include <coffee/core/types/tdef/stltypes.h>

#include <coffee/interfaces/cgraphics_api.h>
#include <coffee/interfaces/content_pipeline.h>
#include <coffee/core/terminal/terminal_cursor.h>

#include <coffee/assimp/cassimpimporters.h>
#include <coffee/assimp/assimp_iterators.h>
#include <coffee/assimp/assimp_material_iterators.h>

#define PRESSURE_LIB "PressurizeModels::"

using namespace CoffeePipeline;
using namespace Coffee;

static Array<cstring, 2> assimpExtensions =
{
    "FBX", "DAE"
};

bool supported(CString const& otherExt)
{
    for(auto ext : assimpExtensions)
        if(ext && StrICmp(ext, otherExt.c_str()))
            return true;
    return false;
}

struct AssimpProcessor : FileProcessor
{
    virtual void process(Vector<VirtFS::VirtDesc> &files,
                         TerminalCursor& cursor);
    
    virtual void receiveAssetPath(CString const& path);
};

COFFAPI FileProcessor* CoffeeLoader()
{
    return new AssimpProcessor;
}

void AssimpProcessor::process(Vector<VirtFS::VirtDesc> &files,
                              TerminalCursor& cursor)
{
    using MP = ASSIMP::MaterialParser;

    Vector<CString> targets;

    for(auto& file : files)
    {
        auto rscName = Path(file.filename);

        if(!supported(rscName.extension()))
            continue;

        targets.push_back(file.filename);
    }

    cursor.progress(PRESSURE_LIB "Found {0} resources",
                    targets.size());

    Vector<ASSIMP::MeshLoader::Attr> attributes =
    {
        {ASSIMP::MeshLoader::AttrType::Position, 0},
        {ASSIMP::MeshLoader::AttrType::TexCoord, 0},
        {ASSIMP::MeshLoader::AttrType::Normal, 0},
        {ASSIMP::MeshLoader::AttrType::Color, 0},
    };

    Array<aiTextureType, 2> textureTypes =
    {{
         aiTextureType_DIFFUSE,
         aiTextureType_SHININESS,
     }};

    Vector<MP::PropertyClass> materialProps =
    {
        MP::PDiffuse,
        MP::PSpecular,
        MP::PSurface,
    };

    ASSIMP::MeshLoader::DrawInfo dinfo;
    dinfo.quirks = ASSIMP::MeshLoader::QuirkCompressElements;

    for(auto file : targets)
    {
        auto filePath = Path(file);

        CResources::Resource sceneFile(MkUrl(filePath.internUrl.c_str()));

        {
            cursor.progress(PRESSURE_LIB "Processing {0}", filePath);

            ASSIMP::MeshLoader::BufferDescription<RHI::NullAPI> bdesc;
            ASSIMP::AssimpPtr scene;
            if(!ASSIMP::LoadScene(scene, C_OCAST<Bytes>(sceneFile),
                              Path(file).extension().c_str()))
                continue;

            ASSIMP::MeshLoader::ExtractDescriptors(
                        scene, attributes, dinfo,
                        bdesc);

            ASSIMP::MaterialParser::MaterialSerializer materials;

            ASSIMP::MaterialParser::ExtractDescriptors(
                        scene, materials, textureTypes,
                        materialProps);

            cursor.progress(PRESSURE_LIB "Processed {0} materials",
                            materials.header.num_materials);

            auto baseFname = filePath.removeExt();

            auto vertex = baseFname.addExtension("vertices");
            auto element = baseFname.addExtension("elements");
            auto draws = baseFname.addExtension("draws");
            auto attributes = baseFname.addExtension("attributes");
            auto drawcall = baseFname.addExtension("dcall");
            auto graph = baseFname.addExtension("graph");
            auto materialFn = baseFname.addExtension("materials");

            Bytes vertexBytes;
            vertexBytes.size = bdesc.vertexData.size();
            vertexBytes.data = C_FCAST<byte_t*>(
                        Calloc(vertexBytes.size, 1));
            Bytes::SetDestr(vertexBytes, [](Bytes& b)
            {
                CFree(b.data);
            });

            bdesc.vertexData.cpy(vertexBytes.data,  vertexBytes.size);

            files.push_back({
                                vertex.internUrl.c_str(),
                                std::move(vertexBytes),
                                (vertexBytes.size > 1_MB)
                                ? VirtFS::File_Compressed : u32(0)
                            });

            vertexBytes.size = bdesc.elementData.size();
            vertexBytes.data = C_FCAST<byte_t*>(
                        Calloc(vertexBytes.size, 1));
            Bytes::SetDestr(vertexBytes, [](Bytes& b)
            {
                CFree(b.data);
            });

            bdesc.elementData.cpy(vertexBytes.data, vertexBytes.size);
            files.push_back({
                                element.internUrl.c_str(),
                                std::move(vertexBytes),
                                (vertexBytes.size > 1_MB)
                                ? VirtFS::File_Compressed : u32(0)
                            });

            vertexBytes.size = bdesc.nodes.size();
            vertexBytes.data = C_FCAST<byte_t*>(
                        Calloc(vertexBytes.size, 1));
            Bytes::SetDestr(vertexBytes, [](Bytes& b)
            {
                CFree(b.data);
            });
            bdesc.nodes.cpy(vertexBytes.data, vertexBytes.size);
            files.push_back({
                                graph.internUrl.c_str(),
                                std::move(vertexBytes),
                                0
                            });

            files.push_back({
                                draws.internUrl.c_str(),
                                Bytes::CopyFrom(bdesc.draws),
                                0
                            });
            files.push_back({
                                attributes.internUrl.c_str(),
                                Bytes::CopyFrom(bdesc.attributes),
                                0
                            });

            files.push_back({
                                drawcall.internUrl.c_str(),
                                Bytes::Copy(bdesc.call),
                                0
                            });
            files.push_back({
                                materialFn.internUrl.c_str(),
                                Bytes::CopyFrom(materials.data),
                                0
                            });
        }
    }

    auto removePred = [&](VirtFS::VirtDesc const& file)
    {
        auto ext = Path(file.filename).extension();

        for(auto otherExt : assimpExtensions)
            if(StrICmp(ext.c_str(), otherExt))
                return true;
        return false;
    };

    auto removeIt = std::remove_if(files.begin(),
                                   files.end(),
                                   removePred);

    /* Listen... remove_if does some funky stuff. */
    for(auto it=removeIt; it!=files.end();it++)
        it->data = Bytes();

    files.erase(removeIt, files.end());
}

void AssimpProcessor::receiveAssetPath(const CString &path) {
    FileResourcePrefix(path.c_str());
}

