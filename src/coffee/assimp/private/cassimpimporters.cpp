#include <coffee/assimp/cassimpimporters.h>

#include <coffee/assimp/assimp_data.h>

#include <coffee/core/CThreading>

#include <coffee/core/CDebug>
#include <coffee/core/CFiles>
#include <coffee/core/CRegex>

#include <coffee/graphics/common/SMesh>
#include <coffee/graphics/common/scene/cnode.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace Coffee {
namespace ASSIMP {

void AssimpData_deleter::operator()(AssimpData* imp)
{
    imp->importer->FreeScene();
}

void Importer_deleter::operator()(::Assimp::Importer* imp)
{
    delete imp;
}

bool LoadScene(UqPtr<AssimpData>& target, const Bytes& source, cstring hint)
{
    auto& data = target;

    data = MkUq<AssimpData>();

    u32 aiFlags = aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                  aiProcess_OptimizeMeshes | aiProcess_SortByPType;

    data->importer = MkUqDST<::Assimp::Importer, Importer_deleter>();

    data->scene = data->importer->ReadFileFromMemory(
            source.data, source.size, aiFlags, hint);

    if(!data->scene)
    {
        cWarning("Could not load scene: {0}", hint);
        target.reset();
        return false;
    }

    return true;
}

bool GetSceneObjects(
    const UqPtr<AssimpData>& scene, Vector<ObjectDesc>& objects)
{
    if(!scene || !scene->scene)
        return false;

    auto& mScene = *scene->scene;

#define ExtractDetail(typeName, enumName)              \
    if(mScene.Has##typeName())                         \
        for(u32 i = 0; i < mScene.mNum##typeName; i++) \
            objects.push_back(                         \
                {mScene.m##typeName[i]->mName.C_Str(), ObjectDesc::enumName});

    ExtractDetail(Animations, Animation);
    ExtractDetail(Cameras, Camera);
    ExtractDetail(Lights, Light);
    ExtractDetail(Meshes, Mesh);

#undef ExtractDetail

    return objects.size() > 0;
}

static Matf4 convert_aiMatrix(aiMatrix4x4 const& mat)
{
    static_assert(sizeof(Matf4) == sizeof(aiMatrix4x4), "Matrix sizes differ");

    Matf4 out;
    MemCpy(Bytes::Create(mat), Bytes::Create(out));
    //    MemCpy(&out.d, &mat, sizeof(Matf4));
    return out;
}

static Node* create_scene_node(
    aiNode* node, Node* parent, LinkList<Node>& node_storage)
{
    node_storage.push_back(Node(nullptr));

    auto& n_node = node_storage.back();
    if(parent)
        parent->addChild(&n_node);
    n_node.setObjectName(node->mName.C_Str());
    n_node.transform = transpose(convert_aiMatrix(node->mTransformation));
    n_node.flags     = 0;

    if(node->mNumMeshes > 0)
        n_node.mesh = C_CAST<i32>(node->mMeshes[0]);
    else
        n_node.mesh = -1;

    return &node_storage.back();
}

static void get_scene_nodes(
    aiNode*         node,
    Node*           parent,
    LinkList<Node>& node_storage,
    bool            accumulate = true)
{
    for(u32 i = 0; i < node->mNumChildren; i++)
    {
        auto c = node->mChildren[i];

        auto np = create_scene_node(c, parent, node_storage);

        /* Pre-multiplying the scene transform, much easier to deal
         *  with when doing MultiDraw* */
        if(accumulate)
            np->transform = parent->transform * np->transform;

        get_scene_nodes(c, np, node_storage, accumulate);
    }
}

bool GetSceneRoot(UqPtr<AssimpData> const& scene, Node** root, NodeList& nodes)
{
    if(!scene || !scene->scene)
        return false;

    auto mNode = scene->scene->mRootNode;

    auto parent = create_scene_node(mNode, nullptr, nodes);
    get_scene_nodes(mNode, parent, nodes);

    if(root)
        *root = parent;

    return nodes.size() > 0;
}

bool GetRawSceneRoot(
    const UqPtr<AssimpData>& scene, Node** root, NodeList& nodes)
{
    if(!scene || !scene->scene)
        return false;

    auto mNode = scene->scene->mRootNode;

    auto parent = create_scene_node(mNode, nullptr, nodes);
    get_scene_nodes(mNode, parent, nodes, false);

    if(root)
        *root = parent;

    return nodes.size() > 0;
}

static aiMesh* get_mesh(UqPtr<AssimpData> const& scene, i32 node)
{
    if(node < 0 || node >= C_CAST<i32>(scene->scene->mNumMeshes))
        return nullptr;
    return scene->scene->mMeshes[node];
}

bool GetMeshData(const UqPtr<AssimpData>& scene, i32 node, Mesh& output_mesh)
{
    auto mesh = get_mesh(scene, node);

    if(!mesh)
        return false;

    new(&output_mesh) Mesh(true);

    output_mesh.clearAttributes();

    if(mesh->HasPositions())
        output_mesh.addAttributeData(
            Mesh::Position, mesh->mVertices, mesh->mNumVertices, 0);

    if(mesh->HasFaces())
    {
        Vector<u32> indices_cpy;
        for(u32 i = 0; i < mesh->mNumFaces; i++)
        {
            auto const& face = mesh->mFaces[i];
            indices_cpy.insert(
                indices_cpy.end(),
                face.mIndices,
                face.mIndices + face.mNumIndices);
        }
        output_mesh.addAttributeData(
            Mesh::Indices, indices_cpy.data(), indices_cpy.size(), 0, true);
    }

    for(u32 i = 0; i < mesh->GetNumUVChannels(); i++)
    {
        if(!mesh->HasTextureCoords(i))
            continue;
        output_mesh.addAttributeData(
            Mesh::TexCoord, mesh->mTextureCoords[i], mesh->mNumVertices, i);
    }

    for(u32 i = 0; i < mesh->GetNumColorChannels(); i++)
    {
        if(!mesh->HasVertexColors(i))
            continue;
        output_mesh.addAttributeData(
            Mesh::Color, mesh->mColors[i], mesh->mNumVertices, i);
    }

    if(mesh->HasNormals())
        output_mesh.addAttributeData(
            Mesh::Normal,
            C_FCAST<Vecf3*>(mesh->mNormals),
            mesh->mNumVertices,
            0);

    if(mesh->HasTangentsAndBitangents())
    {
        output_mesh.addAttributeData(
            Mesh::Tangent, mesh->mTangents, mesh->mNumVertices, 0);
        output_mesh.addAttributeData(
            Mesh::Bitangent, mesh->mBitangents, mesh->mNumVertices, 0);
    }

    return true;
}

bool GetMeshCount(const UqPtr<AssimpData>& scene, i32& count)
{
    if(!scene || !scene->scene)
        return false;

    count = C_CAST<i32>(scene->scene->mNumMeshes);
    return true;
}

} // namespace ASSIMP

} // namespace Coffee
