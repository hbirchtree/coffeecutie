#include <coffee/assimp/assimp_animation_iterators.h>
#include <coffee/assimp/assimp_iterators.h>
#include <coffee/assimp/assimp_material_iterators.h>
#include <coffee/assimp/cassimpimporters.h>
#include <coffee/core/CFiles>
#include <peripherals/stl/types.h>
//#include <coffee/interfaces/cgraphics_api.h>
#include <coffee/interfaces/content_pipeline.h>
#include <coffee/interfaces/content_settings.h>
#include <coffee/strings/libc_types.h>
#include <coffee/strings/url_types.h>
#include <peripherals/libc/string_ops.h>
#include <peripherals/stl/string_casting.h>
#include <peripherals/stl/threads/job_system.h>

#include <coffee/core/CDebug>
#include <coffee/core/terminal/cursor.h>

#define PRESSURE_LIB "PressurizeModels::"

using namespace CoffeePipeline;
using namespace Coffee;

static Array<cstring, 2> assimpExtensions = {{"FBX", "DAE"}};

static Vector<std::string> const* baseDirs = nullptr;

bool supported(std::string const& otherExt)
{
    for(auto ext : assimpExtensions)
        if(ext && libc::str::cmp<libc::str::comp_nocase>(ext, otherExt.c_str()))
            return true;
    return false;
}

using at = ASSIMP::MeshLoader::AttrType;

struct model_settings_t : settings_visitor
{
    static const Map<std::string, aiTextureType>                texture_map;
    static const Map<std::string, ASSIMP::MeshLoader::AttrType> attribute_map;
    static const Map<std::string, ASSIMP::MaterialParser::PropertyClass> prop_map;

    ASSIMP::MeshLoader::Attr stringToAttribute(std::string const& v_)
    {
        auto attr = attribute_map.find(v_.substr(0, 3));

        if(attr == attribute_map.end())
            Throw(PRESSURE_LIB "failed to recognize attribute format: " + v_);

        return {attr->second, cast_string<u32>(v_.substr(3))};
    }

    virtual std::string type()
    {
        return "mesh";
    }
    virtual void visit(std::string const& name, json::Value const& value)
    {
        if(name == "attributes")
        {
            attributes.clear();

            for(auto const& m : value.GetArray())
                attributes.push_back(stringToAttribute(m.GetString()));

        } else if(name == "element_type")
        {
        } else if(name == "textures")
        {
            textures.clear();

            for(auto const& m : value.GetArray())
            {
                auto tex = texture_map.find(m.GetString());

                if(tex == texture_map.end())
                    Throw(implementation_error(
                        PRESSURE_LIB "unsupported texture type: " +
                        std::string(m.GetString())));

                textures.push_back(tex->second);
            }

        } else if(name == "material_properties")
        {
            properties.clear();

            for(auto const& m : value.GetArray())
            {
                auto prop = prop_map.find(m.GetString());

                if(prop == prop_map.end())
                    Throw(implementation_error(
                        PRESSURE_LIB "unsupported material property: " +
                        std::string(m.GetString())));

                properties.push_back(prop->second);
            }
        }
    }

    Vector<ASSIMP::MeshLoader::Attr>              attributes;
    Vector<aiTextureType>                         textures;
    Vector<ASSIMP::MaterialParser::PropertyClass> properties;
};

const Map<std::string, ASSIMP::MeshLoader::AttrType>
    model_settings_t::attribute_map = {
        {"POS", at::Position},
        {"TEX", at::TexCoord},
        {"COL", at::Color},
        {"NRM", at::Normal},
        {"BIT", at::Bitangent},
        {"TAN", at::Tangent},
};
const Map<std::string, aiTextureType> model_settings_t::texture_map = {
    {"SURFACE", aiTextureType_SHININESS},
    {"DIFFUSE", aiTextureType_DIFFUSE},
    {"EMISSIVE", aiTextureType_EMISSIVE},
    {"SPECULAR", aiTextureType_SPECULAR},
    {"AMBIENT", aiTextureType_AMBIENT},
    {"NORMALS", aiTextureType_NORMALS},
    {"HEIGHT", aiTextureType_HEIGHT},

    {"LIGHTMAP", aiTextureType_LIGHTMAP},
};
const Map<std::string, ASSIMP::MaterialParser::PropertyClass>
    model_settings_t::prop_map = {
        {"DIFFUSE", ASSIMP::MaterialParser::PDiffuse},
        {"AMBIENT", ASSIMP::MaterialParser::PAmbient},
        {"SPECULAR", ASSIMP::MaterialParser::PSpecular},
        {"EMISSIVE", ASSIMP::MaterialParser::PEmissive},
};

struct AssimpProcessor : FileProcessor
{
    virtual void process(
        Vector<VirtFS::VirtDesc>& files, TerminalCursor& cursor);

    virtual void setBaseDirectories(const Vector<std::string>& dirs);

    virtual cstring name() const
    {
        return "AssimpProcessor";
    }
};

COFFAPI FileProcessor* CoffeeLoader()
{
    return new AssimpProcessor;
}

void AssimpProcessor::process(
    Vector<VirtFS::VirtDesc>& files, TerminalCursor& cursor)
{
    using MP = ASSIMP::MaterialParser;

    Vector<std::string> targets;

    for(auto& file : files)
    {
        auto rscName = Path(file.filename);

        if(!supported(rscName.extension()))
            continue;

        targets.push_back(file.filename);
    }

    cursor.progress(PRESSURE_LIB "Found {0} resources", targets.size());

    Vector<ASSIMP::MeshLoader::Attr> attributes = {
        {ASSIMP::MeshLoader::AttrType::Position, 0},
        {ASSIMP::MeshLoader::AttrType::TexCoord, 0},
        {ASSIMP::MeshLoader::AttrType::Normal, 0},
        {ASSIMP::MeshLoader::AttrType::Color, 0},
    };

    Vector<aiTextureType> textureTypes = {{
        aiTextureType_DIFFUSE,
        aiTextureType_SPECULAR,
        aiTextureType_SHININESS,
        aiTextureType_NORMALS,
        aiTextureType_EMISSIVE,
    }};

    Vector<MP::PropertyClass> materialProps = {
        MP::PDiffuse,
        MP::PSpecular,
        MP::PSurface,
    };

    ASSIMP::MeshLoader::DrawInfo dinfo;
    dinfo.quirks = ASSIMP::MeshLoader::QuirkCompressElements;

    Mutex files_lock;

    //    for(auto file : targets)
    Function<void(std::string&)> fileWorker = [&](std::string& file) {
        DProfContext _(PRESSURE_LIB "Processing file");

        auto filePath = Path(file);

        Resource sceneFile(MkUrl(filePath.internUrl.c_str()));

        {
            cursor.progress(PRESSURE_LIB "Processing {0}", filePath);

            ASSIMP::MeshLoader::BufferDescription<RHI::NullAPI> bdesc;
            ASSIMP::AssimpPtr                                   scene;
            if(!ASSIMP::LoadScene(
                   scene,
                   C_OCAST<Bytes>(sceneFile),
                   Path(file).extension().c_str()))
                return;

            auto baseFname = filePath.removeExt();

            model_settings_t settings;

            settings.attributes = attributes;
            settings.properties = materialProps;
            settings.textures   = textureTypes;

            settings.parse(baseFname);

            ASSIMP::MeshLoader::ExtractDescriptors(
                scene, settings.attributes, dinfo, bdesc);

            ASSIMP::MaterialParser::MaterialSerializer materials = {};

            ASSIMP::MaterialParser::ExtractDescriptors(
                scene,
                materials,
                settings.textures,
                settings.properties,
                *baseDirs);

            ASSIMP::AnimationParser::AnimationHeap animations = {};

            ASSIMP::AnimationParser::Extract(scene, bdesc.nodes, animations);

            cursor.progress(
                PRESSURE_LIB "Processed {0} materials",
                materials.header.num_materials);

            cursor.progress(
                PRESSURE_LIB "Processed {0} animations",
                animations.header.animations.num);

            auto vertex     = baseFname.addExtension("vertices");
            auto element    = baseFname.addExtension("elements");
            auto draws      = baseFname.addExtension("draws");
            auto attributes = baseFname.addExtension("attributes");
            auto drawcall   = baseFname.addExtension("dcall");
            auto graph      = baseFname.addExtension("graph");
            auto materialFn = baseFname.addExtension("materials");
            auto animFn     = baseFname.addExtension("animations");

            Bytes vertexBytes;

            vertexBytes = Bytes::Alloc(bdesc.vertexData.size());

            bdesc.vertexData.cpy(vertexBytes.data, vertexBytes.size);

            files_lock.lock();

            files.push_back(
                {vertex.internUrl.c_str(),
                 std::move(vertexBytes),
                 (vertexBytes.size > 1_MB) ? VirtFS::File_Compressed : u32(0)});

            vertexBytes = Bytes::Alloc(bdesc.elementData.size());

            bdesc.elementData.cpy(vertexBytes.data, vertexBytes.size);
            files.push_back(
                {element.internUrl.c_str(),
                 std::move(vertexBytes),
                 (vertexBytes.size > 1_MB) ? VirtFS::File_Compressed : u32(0)});

            vertexBytes = Bytes::Alloc(bdesc.nodes.size());

            bdesc.nodes.cpy(vertexBytes.data, vertexBytes.size);
            files.push_back(
                {graph.internUrl.c_str(), std::move(vertexBytes), 0});

            files.push_back(
                {draws.internUrl.c_str(), Bytes::CopyFrom(bdesc.draws), 0});
            files.push_back({attributes.internUrl.c_str(),
                             Bytes::CopyFrom(bdesc.attributes),
                             0});

            files.push_back(
                {drawcall.internUrl.c_str(), Bytes::Copy(bdesc.call), 0});
            files.push_back({materialFn.internUrl.c_str(),
                             Bytes::CopyFrom(materials.data),
                             0});

            files_lock.unlock();
        }
    };

    threads::Parallel::ForEach(targets, std::move(fileWorker));

    auto removePred = [&](VirtFS::VirtDesc const& file) {
        auto ext = Path(file.filename).extension();

        for(auto otherExt : assimpExtensions)
            if(libc::str::cmp<libc::str::comp_nocase>(ext.c_str(), otherExt))
                return true;
        return false;
    };

    auto removeIt = std::remove_if(files.begin(), files.end(), removePred);

    /* Listen... remove_if does some funky stuff. */
    for(auto it = removeIt; it != files.end(); it++)
        it->data = Bytes();

    files.erase(removeIt, files.end());
}

void AssimpProcessor::setBaseDirectories(const Vector<std::string>& dirs)
{
    baseDirs = &dirs;
}
