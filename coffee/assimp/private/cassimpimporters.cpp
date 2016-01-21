#include <cassimpimporters.h>

#include <base/cregex.h>
#include <base/cthreading.h>
#include <base/cdebug.h>
#include <base/cfiles.h>

#include "assimpfun.h"

#define CASSIMP_MULTITHREAD

namespace Coffee {
namespace CResourceTypes {
namespace CAssimp {

using namespace Assimp;

CAssimpImporters::CAssimpImporters()
{
}

CAssimpData *CAssimpImporters::importResource(CResource *source,
                                              cstring hint)
{
    Importer importer;

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

    CElapsedTimer* timer = AllocTimer();
    timer->start();

    std::vector<CAssimpMesh*> meshes;
#ifdef CASSIMP_MULTITHREAD
    std::function<CAssimpMesh*(aiMesh*)> fun = importMesh;
    std::vector<std::future<CAssimpMesh*> > meshes_future;
#endif
    szptr i;
    for(i=0;i<scene->mNumMeshes;i++){
#ifdef CASSIMP_MULTITHREAD
        meshes_future.push_back(CThreading::RunAsync(fun,scene->mMeshes[i]));
#else
        meshes.push_back(importMesh(scene->mMeshes[i]));
#endif
    }

#ifdef CASSIMP_MULTITHREAD
    for(std::future<CAssimpMesh*>& future : meshes_future){
        meshes.push_back(future.get());
    }
#endif

    CAssimpData* data = new CAssimpData;

    {
        data->numMeshes = meshes.size();
        data->meshes    = (CAssimpMesh**)CCalloc(
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
    const byte_t* b_ptr = (const byte_t*)basePtr;
    return &b_ptr[ref.offset];
}

bool coffee_assimp_dump_mesh(CAssimpMesh *mesh, CResource *resource)
{
    bool success = false;

    FileFree(*resource);

    resource->size = mesh->byteSize;
    resource->data = Alloc(resource->size);

    CMemCpy(resource->data,mesh,resource->size);

    if(!FileCommit(*resource))
        cWarning("Failed to store mesh data");
    else success = true;

    FileFree(*resource);

    return success;
}

} // namespace CAssimp
} // namespace CResourceTypes
} // namespace Coffee

