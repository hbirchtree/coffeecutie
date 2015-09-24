#include "cassimpimporters.h"

#include "coffee/cregex.h"
#include "coffee/cfunctional.h"
#include "coffee/cdebug.h"
#include "coffee/cfiles.h"

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
        cMsg("Assimp","Scene imported: cam=%i,lgt=%i,msh=%i,mat=%i,anm=%i,txt=%i",
             scene->mNumCameras,scene->mNumLights,
             scene->mNumMeshes,scene->mNumMaterials,
             scene->mNumAnimations,scene->mNumTextures);
    }

    CElapsedTimer timer;
    timer.start();

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
        data->meshes    = reinterpret_cast<CAssimpMesh**>(calloc(meshes.size(),sizeof(CAssimpMesh*)));
        i=0;
        for(CAssimpMesh* mesh : meshes){
            data->meshes[i] = mesh;
            i++;
        }
    }

    cMsg("Assimp","Elapsed time on import: %ld",timer.elapsed());

    importer.FreeScene();

    return data;
}

} // namespace CAssimp
} // namespace CResourceTypes
} // namespace Coffee

