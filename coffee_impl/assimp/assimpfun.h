#ifndef ASSIMPFUN
#define ASSIMPFUN

#include <thread>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/anim.h"
#include "assimp/matrix4x4.h"
#include "assimp/vector3.h"
#include "assimp/color4.h"

#include "cassimptypes.h"
#include "stdint.h"
#include "coffee.h"

namespace Coffee{
namespace CResourceTypes{
namespace CAssimp{

struct assimp_mesh_intermediate
{
    szptr bufferSize	= 0;
    szptr vertices	= 0;
    szptr faces		= 0;

    uint32 bufferCnt	= 0;
};

extern  CAssimpMesh*         importMesh(aiMesh* meshdata);
extern  CAssimpMaterial*     importMaterial(aiMaterial* matdata);
extern  CAssimpLight*        importLight(aiLight* ldata);
extern  CAssimpCamera*       importCamera(aiCamera* camdata);
extern  CAssimpTexture*      importTexture(aiTexture* texdata);
extern  CAssimpAnimation*    importAnimation(aiAnimation* anidata);
}

extern bool coffee_assimp_dump_mesh(
        CAssimp::CAssimpMesh* mesh, CResources::CResource* resource);

extern void coffee_assimp_free(CAssimp::CAssimpData* data);

}
}

#endif // ASSIMPFUN
