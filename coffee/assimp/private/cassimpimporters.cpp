#include <coffee/assimp/cassimpimporters.h>

#include <coffee/assimp/assimp_data.h>

#include <coffee/core/CRegex>
#include <coffee/core/CThreading>
#include <coffee/core/CDebug>
#include <coffee/core/CFiles>

#include <coffee/graphics/common/scene/cnode.h>
#include <coffee/graphics/common/SMesh>

#include "assimpfun.h"

#define CASSIMP_MULTITHREAD

namespace Coffee {
namespace CResourceTypes {
namespace CAssimp {

using namespace ::Assimp;

CAssimpImporters::CAssimpImporters()
{
}

CAssimpData *CAssimpImporters::importResource(Resource *source,
                                              cstring hint)
{
    ::Assimp::Importer importer;

    const aiScene* scene = importer
            .ReadFileFromMemory(source->data,
                                source->size,
                                aiProcess_CalcTangentSpace|
                                aiProcess_Triangulate|
                                aiProcess_OptimizeMeshes|
                                aiProcess_SortByPType,
                                hint);
    if(!scene){
        cWarning("Failed to import scene \"%s\": %s",
               source->resource(),
               importer.GetErrorString());
        return nullptr;
    }else{
//        cMsg("Assimp","Scene imported: cam=%i,lgt=%i,msh=%i,mat=%i,anm=%i,txt=%i",
//             scene->mNumCameras,scene->mNumLights,
//             scene->mNumMeshes,scene->mNumMaterials,
//             scene->mNumAnimations,scene->mNumTextures);
    }

    CElapsedTimer timer;
    timer.start();

    std::vector<CAssimpMesh*> meshes;
#ifdef CASSIMP_MULTITHREAD
    Function<CAssimpMesh*(aiMesh*)> fun = importMesh;
    Vector<std::future<CAssimpMesh*> > meshes_future;
#endif
    szptr i;
    for(i=0;i<scene->mNumMeshes;i++){
#ifdef CASSIMP_MULTITHREAD
        meshes_future.push_back(Threads::RunAsync(fun,scene->mMeshes[i]));
#else
        meshes.push_back(importMesh(scene->mMeshes[i]));
#endif
    }

#ifdef CASSIMP_MULTITHREAD
    for(Threads::Future<CAssimpMesh*>& future : meshes_future){
        meshes.push_back(future.get());
    }
#endif

    CAssimpData* data = new CAssimpData;

    {
        data->numMeshes = meshes.size();
        data->meshes    = (CAssimpMesh**)Calloc(
                    meshes.size(),
                    sizeof(CAssimpMesh*));
        i=0;
        for(CAssimpMesh* mesh : meshes){
            data->meshes[i] = mesh;
            i++;
        }
    }

//    cMsg("Assimp","Elapsed time on import: %ld",timer.elapsed());

    importer.FreeScene();

    return data;
}

void coffee_assimp_free(CAssimp::CAssimpData *data)
{
    szptr i;
    for(i=0;i<data->numMeshes;i++)
        CFree(data->meshes[i]);
    CFree(data->meshes);
}

byte_t *coffee_assimp_get_reflexive_ptr(void *baseptr, const assimp_reflexive *ref)
{
    return &((byte_t*)baseptr)[ref->offset];
}

cstring assimp_reflexive_string_get(const void* basePtr, const assimp_reflexive &ref)
{
    cstring b_ptr = (cstring)basePtr;
    return &b_ptr[ref.offset];
}

bool coffee_assimp_dump_mesh(CAssimpMesh *mesh, Resource *resource)
{
    bool success = false;

    FileFree(*resource);

    resource->size = mesh->byteSize;
    resource->data = Alloc(resource->size);

    MemCpy(resource->data,mesh,resource->size);

    if(!FileCommit(*resource))
        cWarning("Failed to store mesh data");
    else success = true;

    FileFree(*resource);

    return success;
}

} // namespace CAssimp
} // namespace CResourceTypes

namespace ASSIMP{

bool LoadScene(UqPtr<AssimpData>& target,Resource* source, cstring hint)
{
    auto& data = target;

    data = UqPtr<AssimpData>(new AssimpData);

    u32 aiFlags = aiProcess_CalcTangentSpace|
            aiProcess_Triangulate|
            aiProcess_OptimizeMeshes|
            aiProcess_SortByPType;

    data->scene = data->importer.ReadFileFromMemory(source->data, source->size,
                                                    aiFlags, hint);

    if(!data->scene)
    {
        target.reset();
        return false;
    }

    return true;
}

bool GetSceneObjects(const UqPtr<AssimpData> &scene, Vector<ObjectDesc> &objects)
{
    if(!scene || !scene->scene)
        return false;

    auto& mScene = *scene->scene;

#define ExtractDetail(typeName, enumName) \
    if(mScene.Has ## typeName ()) \
        for(u32 i=0;i<mScene.mNum ## typeName;i++) \
            objects.push_back({mScene.m ## typeName[i]->mName.C_Str(), ObjectDesc::enumName});

    ExtractDetail(Animations, Animation);
    ExtractDetail(Cameras, Camera);
    ExtractDetail(Lights, Light);
    ExtractDetail(Meshes, Mesh);

#undef ExtractDetail

    return objects.size() > 0;
}

FORCEDINLINE Matf4 convert_aiMatrix(aiMatrix4x4 const& mat)
{
    return (Matf4 const&)mat;
}

FORCEDINLINE Node* create_scene_node(aiNode* node, Node* parent, LinkList<Node>& node_storage)
{
    node_storage.push_back(Node(nullptr));

    auto& n_node = node_storage.back();
    n_node.setParent(parent);
    n_node.setObjectName(node->mName.C_Str());
    n_node.transform = transpose(convert_aiMatrix(node->mTransformation));

    if(node->mNumMeshes > 0)
        n_node.mesh = C_CAST<i32>(node->mMeshes[0]);
    else
        n_node.mesh = -1;

    return &node_storage.back();
}

DENYINLINE void get_scene_nodes(aiNode* node, Node* parent, LinkList<Node>& node_storage)
{
    for(u32 i=0;i<node->mNumChildren;i++)
    {
        auto c = node->mChildren[i];

        auto np = create_scene_node(c, parent, node_storage);

        /* Pre-multiplying the scene transform, much easier to deal with when doing MultiDraw* */
        np->transform = parent->transform * np->transform;

        get_scene_nodes(c, np, node_storage);
    }
}

bool GetSceneRoot(UqPtr<AssimpData> const& scene, Node **root, NodeList &nodes)
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

aiMesh* get_mesh(UqPtr<AssimpData> const& scene, i32 node)
{
    if(node < 0 || node >= C_CAST<i32>(scene->scene->mNumMeshes))
        return nullptr;
    return scene->scene->mMeshes[node];
}

bool GetMeshData(const UqPtr<AssimpData> &scene, i32 node, Mesh &output_mesh)
{
    auto mesh = get_mesh(scene, node);

    if(!mesh)
        return false;

    output_mesh.clearAttributes();

    if(mesh->HasPositions())
        output_mesh.addAttributeData(Mesh::Position,
                                     C_FCAST<Vecf3*>(mesh->mVertices),
                                     mesh->mNumVertices, 0);

    if(mesh->HasFaces())
    {
        Vector<u32> indices_cpy;
        for(u32 i=0;i<mesh->mNumFaces;i++)
        {
            auto const& face = mesh->mFaces[i];
            indices_cpy.insert(indices_cpy.end(), face.mIndices, face.mIndices + face.mNumIndices);
        }
        output_mesh.addAttributeData(Mesh::Indices,
                                     indices_cpy.data(),
                                     indices_cpy.size(), 0);
    }

    for(u32 i=0;i<mesh->GetNumUVChannels();i++)
    {
        if(mesh->mNumUVComponents[i] == 3)
            output_mesh.addAttributeData(Mesh::TexCoord,
                                         C_FCAST<Vecf3*>(mesh->mTextureCoords[i]),
                                         mesh->mNumVertices, i);
        else
        {
//            Vector<Vecf2> texcoords;
//            texcoords.resize(mesh->mNumVertices);
//            for(u32 j=0;j<mesh->mNumVertices;j++)
//                MemCpy(&texcoords[j], &mesh->mTextureCoords[i][j], sizeof(Vecf2));
//            output_mesh.addAttributeData(Mesh::TexCoord,
//                                         texcoords.data(),
//                                         texcoords.size(), i);
        }
    }

    for(u32 i=0;i<mesh->GetNumColorChannels();i++)
        if(mesh->HasVertexColors(i))
            output_mesh.addAttributeData(Mesh::Color, mesh->mColors[i],
                                         mesh->mNumVertices, i);

    if(mesh->HasNormals())
        output_mesh.addAttributeData(Mesh::Normal,
                                     C_FCAST<Vecf3*>(mesh->mNormals),
                                     mesh->mNumVertices, 0);

    if(mesh->HasTangentsAndBitangents())
    {
        output_mesh.addAttributeData(Mesh::Tangent,
                                     C_FCAST<Vecf3*>(mesh->mTangents),
                                     mesh->mNumVertices, 0);
        output_mesh.addAttributeData(Mesh::Bitangent,
                                     C_FCAST<Vecf3*>(mesh->mBitangents),
                                     mesh->mNumVertices, 0);
    }

    return true;
}

bool GetMeshCount(const UqPtr<AssimpData> &scene, i32 &count)
{
    if(!scene || !scene->scene)
        return false;

    count = C_CAST<i32>(scene->scene->mNumMeshes);
    return true;
}

}

} // namespace Coffee

