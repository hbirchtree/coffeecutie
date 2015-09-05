#include "cassimpimporters.h"

#include "coffee/cregex.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/anim.h"
#include "assimp/matrix4x4.h"
#include "assimp/vector3.h"

namespace Coffee {
namespace CResourceTypes {
namespace CAssimp {

using namespace Assimp;

CAssimpImporters::CAssimpImporters()
{
}

CAssimpData *CAssimpImporters::importResource(CResource *source, CString hint)
{
    Importer importer;

    const aiScene* scene = importer
            .ReadFileFromMemory(source->data,
                                source->size,
                                aiProcess_CalcTangentSpace|
                                aiProcess_Triangulate|
                                aiProcess_OptimizeMeshes|
                                aiProcess_SortByPType,
                                hint.c_str());
    if(!scene){
        cWarning("Failed to import scene \"%s\": %s",
               source->resource().c_str(),
               importer.GetErrorString());
    }else{
        cMsg("Assimp","Scene imported: cam=%i,lgt=%i,msh=%i,mat=%i,anm=%i,txt=%i",
             scene->mNumCameras,scene->mNumLights,
             scene->mNumMeshes,scene->mNumMaterials,
             scene->mNumAnimations,scene->mNumTextures);
    }

    importer.FreeScene();

    return nullptr;
}

} // namespace CAssimp
} // namespace CResourceTypes
} // namespace Coffee

