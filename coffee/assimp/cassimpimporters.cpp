#include "cassimpimporters.h"

#include <coffee/core/base/cregex.h>
#include <coffee/core/base/cthreading.h>
#include <coffee/core/base/cdebug.h>
#include <coffee/core/base/cfiles.h>

#include "private/assimpfun.h"

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

    CElapsedTimer* timer = coffee_fun_alloc_timer();
    timer->start();

    std::vector<CAssimpMesh*> meshes;
#ifdef CASSIMP_MULTITHREAD
    std::vector<std::future<CAssimpMesh*> > meshes_future;
#endif
    szptr i;
    for(i=0;i<scene->mNumMeshes;i++){
#ifdef CASSIMP_MULTITHREAD
        meshes_future.push_back(CThreading::runAsync<CAssimpMesh*>([=](){
            return importMesh(scene->mMeshes[i]);
        }));
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
        data->meshes    = (CAssimpMesh**)c_calloc(
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
        c_free(data->meshes[i]);
    c_free(data->meshes);
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

    coffee_file_free(resource);

    resource->size = mesh->byteSize;
    resource->data = c_alloc(resource->size);

    c_memcpy(resource->data,mesh,resource->size);

    if(!coffee_file_commit(resource))
        cWarning("Failed to store mesh data");
    else success = true;

    coffee_file_free(resource);

    return success;
}

} // namespace CAssimp
} // namespace CResourceTypes
} // namespace Coffee

