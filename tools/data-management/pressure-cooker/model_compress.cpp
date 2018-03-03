#include <coffee/core/coffee_mem_macros.h>
#include <coffee/interfaces/content_pipeline.h>
#include <coffee/core/CFiles>
#include <coffee/core/CDebug>
#include <coffee/assimp/cassimpimporters.h>
#include <coffee/assimp/assimp_iterators.h>
#include <coffee/interfaces/cgraphics_api.h>
#include <coffee/core/types/tdef/stltypes.h>

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
    virtual void process(Vector<VirtFS::VirtDesc> &files);
    
    virtual void receiveAssetPath(CString const& path);
};

COFFAPI FileProcessor* CoffeeLoader()
{
    return new AssimpProcessor;
}

void AssimpProcessor::process(Vector<VirtFS::VirtDesc> &files)
{
    Vector<CString> targets;

    for(auto& file : files)
    {
        auto rscName = Path(file.filename);

        if(!supported(rscName.extension()))
            continue;

        targets.push_back(file.filename);
    }

    cDebug(PRESSURE_LIB "Found {0} resources",
           targets.size());

    Vector<ASSIMP::MeshLoader::Attr> attributes =
    {
        {ASSIMP::MeshLoader::AttrType::Position, 0},
        {ASSIMP::MeshLoader::AttrType::TexCoord, 0},
        {ASSIMP::MeshLoader::AttrType::Normal, 0},
        {ASSIMP::MeshLoader::AttrType::Color, 0},
    };

    ASSIMP::MeshLoader::DrawInfo dinfo;
    dinfo.quirks = ASSIMP::MeshLoader::QuirkCompressElements;

    for(auto file : targets)
    {
        auto filePath = Path(file);

        CResources::Resource sceneFile(MkUrl(filePath.internUrl.c_str()));

        if(FileMap(sceneFile))
        {
            ASSIMP::MeshLoader::BufferDescription<RHI::NullAPI> bdesc;
            ASSIMP::AssimpPtr scene;
            ASSIMP::LoadScene(scene,
                              &sceneFile,
                              Path(file).extension().c_str());

            ASSIMP::MeshLoader::ExtractDescriptors(
                        scene, attributes, dinfo,
                        bdesc);

            auto baseFname = filePath.removeExt();

            auto vertex = baseFname.addExtension("vertices");
            auto element = baseFname.addExtension("elements");
            auto draws = baseFname.addExtension("draws");
            auto attributes = baseFname.addExtension("attributes");
            auto drawcall = baseFname.addExtension("dcall");
            auto graph = baseFname.addExtension("graph");

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
                                Bytes::CreateFrom(bdesc.draws),
                                0
                            });

            files.push_back({
                                drawcall.internUrl.c_str(),
                                Bytes::Create(bdesc.call),
                                0
                            });

            FileUnmap(sceneFile);
        }else
            cWarning(PRESSURE_LIB "Failed to map file: {0}",
                     sceneFile.resource());

        files.erase(std::remove_if(files.begin(), files.end(),
                       [&](VirtFS::VirtDesc& otherFile)
        {
            return file == otherFile.filename;
        }));
    }
}

void AssimpProcessor::receiveAssetPath(const CString &path) {
    FileResourcePrefix(path.c_str());
}

